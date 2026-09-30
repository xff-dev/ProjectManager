#include "../../utils/consts.hpp"
#include "../App.hpp"
#include <format>

void App::handleTask(RunTask &task) {
  ProjectConfig config = getCurrentProjectContext().config;

  ProjectRun run = config.run;

  Command command{};

  if (!run.directory.empty()) {
    command.workingDirectory = run.directory;
  }

  command.command = run.command;
  for (auto arg : task.args) {
    command.command += " ";
    command.command += arg;
  }

  int status = runner.run(command);
  if (status != 0) {
    console.warn(std::format(consts::warnings::NonZeroStatusCode, status));
  }
}
