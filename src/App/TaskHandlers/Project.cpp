#include "../../Project/Project.hpp"
#include "../../utils/consts.hpp"
#include "../../utils/utils.hpp"
#include "../App.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

void App::handleTask(ListTask &task) {
  (void)task;

  for (const auto &project : registry.getProjects()) {
    bool existsOnDisk = std::filesystem::exists(project.path);
    std::string lang = "";

    try {
      ProjectConfig config = loader.load(project);
      lang = getLanguageIcon(config.lang);
    } catch (std::runtime_error) {
    };

    console << (existsOnDisk ? consts::icons::exists : consts::icons::missing);
    console << " " << project.name;

    console << lang;

    if (!existsOnDisk) {
      console << " (" << project.path << ")";
    }
    console << std::endl;
  }
}

void App::handleTask(ListNamesTask &task) {
  (void)task;
  for (const auto &project : registry.getProjects()) {
    console << project.name << std::endl;
  }
}

void App::handleTask(RemoveTask &task) { registry.remove(task.name); }

void App::handleTask(AddTask &task) {
  Project project{task.path, task.name};
  registry.add(project);
}

void App::handleTask(OpenTask &task) {
  Project project = registry.find(task.name);
  ProjectConfig config;

  if (!std::filesystem::exists(project.path)) {
    throw std::runtime_error(std::string(consts::errors::InvalidProjectPath));
  }

  try {
    config = loader.load(project);
  } catch (std::runtime_error) {
  }

  for (auto terminalCommand : config.terminalCommands) {
    Command command{
        .command = terminalCommand,
        .workingDirectory = project.path,
        .envCommand = config.envCommand,
    };

    runner.launchTerminal(command);
  }

  Command command{
      .command = config.editorCommand,
      .workingDirectory = project.path,
  };
  if (!config.envCommand.empty() &&
      config.envCommand.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
    command.envCommand = config.envCommand;
  }

  runner.run(command);
}
