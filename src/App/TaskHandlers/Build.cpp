#include "../../utils/consts.hpp"
#include "../App.hpp"
#include <filesystem>
#include <stdexcept>

void App::handleTask(BuildTask &task) {
  (void)task;
  Project project = registry.findByPath(".");
  ProjectConfig config = loader.load(project);

  ProjectBuild build = config.build;

  if (!build.directory.empty() && !std::filesystem::exists(build.directory)) {
    std::filesystem::create_directories(build.directory);

    Command prepareCommand{.command = build.prepareCommand,
                           .workingDirectory = build.directory};

    int status = runner.run(prepareCommand);

    if (status != 0) {
      throw std::runtime_error(
          std::string(consts::errors::BuildPreparationError));
    }
  }

  Command command{build.buildCommand};

  if (!build.directory.empty()) {
    command.workingDirectory = build.directory;
  }

  int status = runner.run(command);

  if (status != 0) {
    throw std::runtime_error(std::string(consts::errors::BuildError));
  }
  console.success(consts::success::BuildSuccessful);
}
