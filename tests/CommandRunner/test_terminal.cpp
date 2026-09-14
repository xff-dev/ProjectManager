#include "../../src/CommandRunner/CommandRunner.hpp"
#include "terminal_launchers.hpp"

using namespace termlaunchers;

TEMPLATE_LIST_TEST_CASE(
    "real terminal runs a command in the working directory without envCommand",
    "[terminal][CommandRunner]", TestedLaunchers) {
  requireLaunchEnvironment<TestType>();

  const fs::path base = uniqueBaseDir("plain");
  const fs::path work = base / "work";
  const fs::path output = base / "probe.txt";
  fs::create_directories(work);
  unsetenv("PM_TERM_PLAIN");

  Command command;
  command.command = probeCommand(output, "PM_TERM_PLAIN");
  command.workingDirectory = work;

  std::ostringstream consoleOutput;
  Console console(consoleOutput);
  TestType launcher;
  CommandRunner runner(console, launcher);
  ScopedTerminal terminal(runner.launchTerminal(command));

  REQUIRE(waitForFile(output));
  const auto lines = readLines(output);
  REQUIRE(lines.size() == 2);
  REQUIRE(lines[0] == fs::absolute(work).string());
  REQUIRE(lines[1] == "unset");

  fs::remove_all(base);
}

TEMPLATE_LIST_TEST_CASE("real terminal applies envCommand before running the "
                        "command",
                        "[terminal][CommandRunner]", TestedLaunchers) {
  requireLaunchEnvironment<TestType>();

  const fs::path base = uniqueBaseDir("env");
  const fs::path work = base / "work";
  const fs::path output = base / "probe.txt";
  fs::create_directories(work);
  unsetenv("PM_TERM_ENV");

  Command command;
  command.command = probeCommand(output, "PM_TERM_ENV");
  command.workingDirectory = work;
  command.envCommand = "export PM_TERM_ENV=enabled";

  std::ostringstream consoleOutput;
  Console console(consoleOutput);
  TestType launcher;
  CommandRunner runner(console, launcher);
  ScopedTerminal terminal(runner.launchTerminal(command));

  REQUIRE(waitForFile(output));
  const auto lines = readLines(output);
  REQUIRE(lines.size() == 2);
  REQUIRE(lines[0] == fs::absolute(work).string());
  REQUIRE(lines[1] == "enabled");
  REQUIRE(getenv("PM_TERM_ENV") == nullptr);

  fs::remove_all(base);
}

TEMPLATE_LIST_TEST_CASE("real terminal without envCommand inherits the parent "
                        "environment",
                        "[terminal][CommandRunner]", TestedLaunchers) {
  requireLaunchEnvironment<TestType>();

  const fs::path base = uniqueBaseDir("inherit");
  const fs::path work = base / "work";
  const fs::path output = base / "probe.txt";
  fs::create_directories(work);
  unsetenv("PM_TERM_INHERITED");
  REQUIRE(setenv("PM_TERM_INHERITED", "passed-down", 1) == 0);

  Command command;
  command.command = probeCommand(output, "PM_TERM_INHERITED");
  command.workingDirectory = work;

  std::ostringstream consoleOutput;
  Console console(consoleOutput);
  TestType launcher;
  CommandRunner runner(console, launcher);
  ScopedTerminal terminal(runner.launchTerminal(command));

  REQUIRE(waitForFile(output));
  const auto lines = readLines(output);
  REQUIRE(lines.size() == 2);
  REQUIRE(lines[0] == fs::absolute(work).string());
  REQUIRE(lines[1] == "passed-down");

  unsetenv("PM_TERM_INHERITED");
  fs::remove_all(base);
}

TEMPLATE_LIST_TEST_CASE("real terminal envCommand steers the executed command",
                        "[terminal][CommandRunner]", TestedLaunchers) {
  requireLaunchEnvironment<TestType>();

  const fs::path base = uniqueBaseDir("steer");
  const fs::path work = base / "work";
  const fs::path flagOn = base / "flag-on.txt";
  const fs::path flagOff = base / "flag-off.txt";
  fs::create_directories(work);

  Command command;
  command.command = "if [ \"$PM_TERM_STEER\" = \"on\" ]; then touch '" +
                    flagOn.string() + "'; else touch '" + flagOff.string() +
                    "'; fi";
  command.envCommand = "export PM_TERM_STEER=on";

  std::ostringstream consoleOutput;
  Console console(consoleOutput);
  TestType launcher;
  CommandRunner runner(console, launcher);
  ScopedTerminal terminal(runner.launchTerminal(command));

  REQUIRE(waitForFile(flagOn));
  REQUIRE(!fs::exists(flagOff));

  fs::remove_all(base);
}

TEMPLATE_LIST_TEST_CASE("real terminal executes a command without a "
                        "workingDirectory",
                        "[terminal][CommandRunner]", TestedLaunchers) {
  requireLaunchEnvironment<TestType>();

  const fs::path base = uniqueBaseDir("noworkdir");
  const fs::path output = base / "probe.txt";

  Command command;
  command.command = probeCommand(output, "PM_TERM_NOWORKDIR");

  std::ostringstream consoleOutput;
  Console console(consoleOutput);
  TestType launcher;
  CommandRunner runner(console, launcher);
  ScopedTerminal terminal(runner.launchTerminal(command));

  REQUIRE(waitForFile(output));
  const auto lines = readLines(output);
  REQUIRE(lines.size() == 2);
  INFO("terminal ran in: " << lines[0]);
  REQUIRE(lines[1] == "unset");

  fs::remove_all(base);
}