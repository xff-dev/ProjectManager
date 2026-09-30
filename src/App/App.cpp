#include "App.hpp"
#include "../utils/consts.hpp"
#include <filesystem>
#include <format>
#include <stdexcept>
#include <string>
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

ProjectContext App::getCurrentProjectContext() {
  ProjectContext context;
  std::filesystem::path currentPath = ".";

  try {
    context.project = registry.findByPath(currentPath);
  } catch (std::runtime_error &e) {
    console.warn(e.what());
  }
  if (context.project.has_value()) {
    context.config = loader.load(*context.project);
  } else {
    context.config = loader.load(currentPath / "project.ini");
  }

  return context;
}
