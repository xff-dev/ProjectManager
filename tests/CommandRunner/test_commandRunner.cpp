#include "../../src/CommandRunner/CommandRunner.hpp"
#include "catch2/catch_test_macros.hpp"

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

namespace {
namespace fs = std::filesystem;

struct CommandOptions {
  bool workingDirectory;
  bool environment;
  bool background;
};

bool waitForFile(const fs::path &path) {
  constexpr auto timeout = std::chrono::seconds(2);
  const auto deadline = std::chrono::steady_clock::now() + timeout;

  while (std::chrono::steady_clock::now() < deadline) {
    if (fs::exists(path)) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return fs::exists(path);
}

std::string readFile(const fs::path &path) {
  std::ifstream file(path);
  std::stringstream content;
  content << file.rdbuf();
  return content.str();
}

CommandRunner createRunner(Console &console) {
  return CommandRunner(console, std::make_unique<Dummy>());
}
} // namespace

TEST_CASE("runs every combination of Command use cases", "[CommandRunner]") {
  // Commands run in child processes, so their stdout/stderr remains outside
  // the injected Console. Verify their observable filesystem effects instead.
  std::ostringstream consoleOutput;
  Console console(consoleOutput, std::cin);
  const std::array cases{
      CommandOptions{false, false, false}, CommandOptions{true, false, false},
      CommandOptions{false, true, false},  CommandOptions{true, true, false},
      CommandOptions{false, false, true},  CommandOptions{true, false, true},
      CommandOptions{false, true, true},   CommandOptions{true, true, true},
  };

  const fs::path base = fs::temp_directory_path() /
                        ("command-runner-" + std::to_string(getpid()));
  fs::remove_all(base);
  fs::create_directories(base);

  for (std::size_t index = 0; index < cases.size(); ++index) {
    const auto options = cases[index];
    const fs::path workingDirectory = base / ("work-" + std::to_string(index));
    const fs::path output = base / ("result-" + std::to_string(index));
    fs::create_directories(workingDirectory);

    Command command;
    command.command =
        "test \"$COMMAND_RUNNER_TEST_ENV\" = \"enabled\" && pwd > '" +
        output.string() + "'";
    command.isBackground = options.background;
    if (options.workingDirectory) {
      command.workingDirectory = workingDirectory;
    }
    if (options.environment) {
      command.envCommand = "export COMMAND_RUNNER_TEST_ENV=enabled";
    } else {
      command.command = "pwd > '" + output.string() + "'";
    }

    CommandRunner runner = createRunner(console);
    const int result = runner.run(command);

    if (options.background) {
      REQUIRE(result > 0);
      REQUIRE(waitForFile(output));
    } else {
      REQUIRE(result == 0);
    }

    REQUIRE(readFile(output) == fs::absolute(options.workingDirectory
                                                 ? workingDirectory
                                                 : fs::current_path())
                                        .string() +
                                    "\n");
  }

  fs::remove_all(base);
}

TEST_CASE("returns the command exit status", "[CommandRunner]") {
  std::ostringstream consoleOutput;
  Console console(consoleOutput, std::cin);
  CommandRunner runner = createRunner(console);

  REQUIRE(runner.run(Command{.command = "exit 42"}) == 42);
}

TEST_CASE("reports an invalid working directory as a command failure",
          "[CommandRunner]") {
  std::ostringstream consoleOutput;
  Console console(consoleOutput, std::cin);
  CommandRunner runner = createRunner(console);
  Command command{.command = "true"};
  command.workingDirectory = "/path/that/does/not/exist";

  REQUIRE(runner.run(command) == 127);
}

TEST_CASE("reports commands through Console", "[CommandRunner][Console]") {
  std::ostringstream output;
  Console console(output, std::cin);
  CommandRunner runner = createRunner(console);

  REQUIRE(runner.run(Command{.command = "true"}) == 0);
  REQUIRE(runner.run(Command{.command = "true", .envCommand = "export X=1"}) ==
          0);

  REQUIRE(output.str().find("export X=1") != std::string::npos);
  REQUIRE(output.str().find("true") != std::string::npos);
}

TEST_CASE("returns a signal-based command status", "[CommandRunner]") {
  std::ostringstream output;
  Console console(output, std::cin);
  CommandRunner runner = createRunner(console);

  REQUIRE(runner.run(Command{.command = "kill -TERM $$"}) == 143);
}

TEST_CASE("envCommand affects the shell environment", "[CommandRunner]") {
  std::ostringstream consoleOutput;
  Console console(consoleOutput, std::cin);
  CommandRunner runner = createRunner(console);
  const char *var = "PROJECT_MANAGER_TEST_FOO";
  unsetenv(var);

  const std::string checkEnv = "test \"$" + std::string(var) + "\" = \"1\"";

  REQUIRE(runner.run(Command{.command = checkEnv}) != 0);

  REQUIRE(runner.run(
              Command{.command = checkEnv,
                      .envCommand = "export PROJECT_MANAGER_TEST_FOO=1"}) == 0);

  REQUIRE(getenv(var) == nullptr);
}
