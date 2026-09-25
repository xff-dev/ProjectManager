#include "CLI.hpp"
#include "../utils/consts.hpp"
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
CLI::CLI(int argc, char **argv) : argc(argc), argv(argv) {}

CLIResult CLI::parse() {
  CLIResult result;

  while (argn < argc) {
    std::string arg = argv[argn];
    if (arg == "build") {
      result.tasks.push_back(parseBuildTask());
    } else if (arg == "run") {
      result.tasks.push_back(parseRunTask());
    } else if (arg == "list") {
      result.tasks.push_back(parseListTask());
    } else if (arg == "list-names") {
      result.tasks.push_back(parseListNamesTask());
    } else if (arg == "script") {
      result.tasks.push_back(parseScriptTask());
    } else if (arg == "list-scripts") {
      result.tasks.push_back(parseListScriptsTask());
    } else if (arg == "remove") {
      result.tasks.push_back(parseRemoveTask());
    } else if (arg == "add") {
      result.tasks.push_back(parseAddTask());
    } else if (arg == "open") {
      result.tasks.push_back(parseOpenTask());
    } else if (arg == "help") {
      result.tasks.push_back(parseHelpTask());
    } else if (arg == "migrate") {
      result.tasks.push_back(parseMigrateTask());
    } else if (arg == "--") {
      auto args = parseArgs();
      for (auto &task : result.tasks) {
        if (std::holds_alternative<RunTask>(task)) {
          std::get<RunTask>(task).args = args;
        }
      }
    } else {
      throw std::runtime_error(
          std::format(consts::errors::UnknownCommand, arg, argv[0]));
    }
  }

  return result;
}

BuildTask CLI::parseBuildTask() {
  argn++;
  return BuildTask{};
}

RunTask CLI::parseRunTask() {
  argn++;
  return RunTask{};
}

ListTask CLI::parseListTask() {
  argn++;
  return ListTask{};
}

ListNamesTask CLI::parseListNamesTask() {
  argn++;
  return ListNamesTask{};
}

ScriptTask CLI::parseScriptTask() {
  argn++;
  if (argn >= argc) {
    throw std::runtime_error(std::string(consts::errors::MissingScriptName));
  }
  std::string name = argv[argn++];
  return ScriptTask{name};
}

ListScriptsTask CLI::parseListScriptsTask() {
  argn++;
  return ListScriptsTask{};
}

RemoveTask CLI::parseRemoveTask() {
  argn++;
  if (argn >= argc) {
    throw std::runtime_error(std::string(consts::errors::MissingProjectName));
  }
  std::string name = argv[argn++];
  return RemoveTask{name};
}

AddTask CLI::parseAddTask() {
  argn++;
  if (argn >= argc) {
    throw std::runtime_error(std::string(consts::errors::MissingProjectName));
  }
  std::string name = argv[argn++];
  if (argn >= argc) {
    throw std::runtime_error(std::string(consts::errors::MissingProjectPath));
  }
  std::filesystem::path path = argv[argn++];
  return AddTask{name, path};
}

OpenTask CLI::parseOpenTask() {
  argn++;
  if (argn >= argc) {
    throw std::runtime_error(std::string(consts::errors::MissingProjectName));
  }
  std::string name = argv[argn++];
  return OpenTask{name};
}

HelpTask CLI::parseHelpTask() {
  argn++;
  return HelpTask{argv[0]};
}

MigrateTask CLI::parseMigrateTask() {
  argn++;
  if (argn >= argc) {
    throw std::runtime_error(
        std::string(consts::errors::MissingMigrationSource));
  }
  std::string from = argv[argn++];
  if (argn >= argc) {
    throw std::runtime_error(
        std::string(consts::errors::MissingMigrationDestination));
  }
  std::string to = argv[argn++];

  return MigrateTask{from, to};
}

std::vector<std::string> CLI::parseArgs() {
  std::vector<std::string> args;

  while (++argn < argc) {
    args.push_back(argv[argn]);
  }

  return args;
}
