#pragma once
#include "../CLI/CLI.hpp"
#include "../CommandRunner/CommandRunner.hpp"
#include "../Project/ConfigLoader.hpp"
#include "../Project/Registry.hpp"
#include "../utils/Console.hpp"
#include <vector>

class App {
public:
  App(std::vector<Task> tasks, Registry &registry, ConfigLoader &loader,
      CommandRunner &runner, Console &console);

  void run();

private:
  void handleTask(BuildTask &task);
  void handleTask(RunTask &task);
  void handleTask(ListTask &task);
  void handleTask(ListNamesTask &task);
  void handleTask(ScriptTask &task);
  void handleTask(ListScriptsTask &task);
  void handleTask(RemoveTask &task);
  void handleTask(AddTask &task);
  void handleTask(OpenTask &task);
  void handleTask(HelpTask &task);
  void handleTask(MigrateTask &task);

private:
  std::vector<Task> tasks;
  Registry &registry;
  ConfigLoader &loader;
  CommandRunner &runner;
  Console &console;
};
