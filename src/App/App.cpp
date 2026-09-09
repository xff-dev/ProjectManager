#include "App.hpp"
#include "../utils/consts.hpp"
#include <format>
#include <variant>
#include <vector>

App::App(std::vector<Task> tasks, Registry &registry,
         ConfigLoader &configLoader, CommandRunner &runner, Console &console)
    : tasks(tasks), registry(registry), loader(configLoader), runner(runner),
      console(console) {}

void App::run() {
  for (auto &task : tasks) {
    std::visit([this](auto &task) { handleTask(task); }, task);
  }
}

void App::handleTask(HelpTask &task) {
  console << std::format(consts::HelpMessage, task.appName) << std::endl;
}
