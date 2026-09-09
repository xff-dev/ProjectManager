#include "../../src/Project/ConfigLoader.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

ProjectConfig loadStringConfig(std::string content, std::string suffix) {
  std::string filename =
      "/tmp/project-manager-test-" + std::to_string(getpid()) + "-" + suffix;

  std::ofstream file(filename);
  file << content;
  file.close();

  ConfigLoader loader;
  ProjectConfig config = loader.load(filename);

  std::filesystem::remove(filename);

  return config;
}

TEST_CASE("test Data block", "[ConfigLoader]") {
  std::string iniContent = R"(
[data]
lang = cpp
editorCommand = nvim .
envCommand = export DUMMY=1
)";

  std::string suffix = "test_parseConfig";
  ProjectConfig config = loadStringConfig(iniContent, suffix);

  REQUIRE(config.lang == "cpp");
  REQUIRE(config.editorCommand == "nvim .");
  REQUIRE(config.envCommand == "export DUMMY=1");
}

TEST_CASE("test terminal block", "[ConfigLoader]") {
  std::string iniContent = R"(
[terminal]
command = echo "Hello, World!")";

  std::string suffix = "test_parseConfig_terminal";
  ProjectConfig config = loadStringConfig(iniContent, suffix);

  REQUIRE(config.terminalCommands.size() == 1);
  REQUIRE(config.terminalCommands[0] == "echo \"Hello, World!\"");
}

TEST_CASE("test terminal blocks", "[ConfigLoader]") {
  std::string iniContent = R"(
[terminal]
command = echo "Hello, World!"
[terminal_2]
command = echo "Second command"
[terminal3]
command = echo "Third command"
)";

  std::string suffix = "test_parseConfig_terminal_multiple";
  ProjectConfig config = loadStringConfig(iniContent, suffix);

  std::sort(config.terminalCommands.begin(), config.terminalCommands.end());

  REQUIRE(config.terminalCommands.size() == 3);
  REQUIRE(config.terminalCommands[0] == "echo \"Hello, World!\"");
  REQUIRE(config.terminalCommands[1] == "echo \"Second command\"");
  REQUIRE(config.terminalCommands[2] == "echo \"Third command\"");
}

TEST_CASE("test run block", "[ConfigLoader]") {
  std::string iniContent = R"(
[run]
command = ./run.sh
directory = ./run_dir)";

  std::string suffix = "test_parseConfig_run";
  ProjectConfig config = loadStringConfig(iniContent, suffix);

  REQUIRE(config.run.command == "./run.sh");
  REQUIRE(config.run.directory == "./run_dir");
}

TEST_CASE("test build block", "[ConfigLoader]") {
  std::string iniContent = R"(
[build]
command = make
directory = ./build_dir
prepare = ./prepare.sh)";

  std::string suffix = "test_parseConfig_build";
  ProjectConfig config = loadStringConfig(iniContent, suffix);

  REQUIRE(config.build.buildCommand == "make");
  REQUIRE(config.build.directory == "./build_dir");
  REQUIRE(config.build.prepareCommand == "./prepare.sh");
}

TEST_CASE("scripts and terminal declarations are loaded together",
          "[ConfigLoader]") {
  const std::string iniContent = R"(
[data]
terminalCommand = echo data-terminal
[terminal_extra]
command = echo extra-terminal
[script_format]
directory = tools
command = ./format.sh
[script_lint]
directory = .
command = ./lint.sh
)";

  const ProjectConfig config =
      loadStringConfig(iniContent, "scripts-and-terminals");

  REQUIRE(config.terminalCommands.size() == 2);
  REQUIRE(std::find(config.terminalCommands.begin(), config.terminalCommands.end(),
                    "echo data-terminal") != config.terminalCommands.end());
  REQUIRE(std::find(config.terminalCommands.begin(), config.terminalCommands.end(),
                    "echo extra-terminal") != config.terminalCommands.end());
  REQUIRE(config.scripts.at("format").directory == "tools");
  REQUIRE(config.scripts.at("format").command == "./format.sh");
  REQUIRE(config.scripts.at("lint").directory == ".");
  REQUIRE(config.scripts.at("lint").command == "./lint.sh");
}

TEST_CASE("loads a project config through Project", "[ConfigLoader]") {
  const auto directory = std::filesystem::temp_directory_path() /
                         ("project-config-" + std::to_string(getpid()));
  std::filesystem::remove_all(directory);
  std::filesystem::create_directories(directory);
  std::ofstream(directory / "project.ini") << "[data]\nlang = rust\n";
  ConfigLoader loader;
  Project project{directory, "demo"};

  const ProjectConfig config = loader.load(project);

  REQUIRE(config.lang == "rust");
  std::filesystem::remove_all(directory);
}

TEST_CASE("missing project config reports a clear error",
          "[ConfigLoader][error]") {
  const auto filename = std::filesystem::temp_directory_path() /
                        ("missing-project-config-" + std::to_string(getpid()));
  std::filesystem::remove(filename);
  ConfigLoader loader;

  REQUIRE_THROWS_WITH(loader.load(filename),
                      Catch::Matchers::ContainsSubstring("config"));
}
