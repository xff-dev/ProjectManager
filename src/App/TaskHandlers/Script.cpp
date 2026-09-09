#include "../../CommandRunner/Command.hpp"
#include "../../CommandRunner/CommandRunner.hpp"
#include "../../utils/consts.hpp"
#include "../App.hpp"
#include <filesystem>
#include <format>
#include <stdexcept>
#include <string>

void App::handleTask(ScriptTask &task) {
  Project project = registry.findByPath(".");
  ProjectConfig config = loader.load(project);

  if (config.scripts.find(task.name) == config.scripts.end())
    throw std::runtime_error(std::string(consts::errors::ScriptNotFound));

  ProjectScript &script = config.scripts[task.name];

  Command command{};

  if (!script.directory.empty()) {
    command.workingDirectory = script.directory;
  }

  command.command = script.command;

  int status = runner.run(command);

  if (status == 0) {
    console.success(std::format(consts::success::ScriptSuccessful, task.name));
  } else {
    console.warn(std::format(consts::warnings::NonZeroStatusCode, status));
  }
}

void App::handleTask(ListScriptsTask &task) {
  Project project = registry.findByPath(".");
  ProjectConfig config = loader.load(project);

  for (auto [name, script] : config.scripts) {
    console << name << std::endl;
  }
}
