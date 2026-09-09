#include "../../utils/consts.hpp"
#include "../App.hpp"
#include <format>

void App::handleTask(RunTask &task) {
  Project project = registry.findByPath(".");
  ProjectConfig config = loader.load(project);

  ProjectRun run = config.run;

  Command command{};

  if (!run.directory.empty()) {
    command.workingDirectory = run.directory;
  }

  command.command = run.command;

  int status = runner.run(command);
  if (status != 0) {
    console.warn(std::format(consts::warnings::NonZeroStatusCode, status));
  }
}
