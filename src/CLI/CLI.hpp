#pragma once
#include <filesystem>
#include <string>
#include <variant>
#include <vector>

struct BuildTask {};
struct RunTask {};

struct ListTask {};
struct ListNamesTask {};

struct ScriptTask {
  std::string name;
};

struct ListScriptsTask {};

struct RemoveTask {
  std::string name;
};
struct AddTask {
  std::string name;
  std::filesystem::path path;
};

struct OpenTask {
  std::string name;
};

struct HelpTask {
  std::string appName;
};

struct MigrateTask {
  std::filesystem::path from;
  std::filesystem::path to;
};

using Task = std::variant<BuildTask, RunTask, ListTask, ListNamesTask,
                          ScriptTask, ListScriptsTask, RemoveTask, AddTask,
                          OpenTask, HelpTask, MigrateTask>;

struct CLIResult {
  std::vector<Task> tasks;
};

class CLI {
public:
  CLI(int argc, char **argv);

  CLIResult parse();

private:
  BuildTask parseBuildTask();
  RunTask parseRunTask();
  ListTask parseListTask();
  ListNamesTask parseListNamesTask();
  ScriptTask parseScriptTask();
  ListScriptsTask parseListScriptsTask();
  RemoveTask parseRemoveTask();
  AddTask parseAddTask();
  OpenTask parseOpenTask();
  HelpTask parseHelpTask();
  MigrateTask parseMigrateTask();

private:
  int argn = 1;

  int argc;
  char **argv;
};
