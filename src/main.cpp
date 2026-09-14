#include "App/App.hpp"
#include "CLI/CLI.hpp"
#include "CommandRunner/CommandRunner.hpp"
#include "CommandRunner/Terminal/Ghostty.cpp"
#include "Project/ConfigLoader.hpp"
#include "Project/Registry.hpp"
#include "utils/Console.hpp"
#include <filesystem>
#include <iostream>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  const char *homedir;

  if ((homedir = getenv("HOME")) == nullptr) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  std::filesystem::path projectsDataPath =
      std::filesystem::path(homedir) / ".project-manager/";
  std::filesystem::path projectsDatabasePath = projectsDataPath / "data";

  Console console(std::cout);

  try {
    ConfigLoader loader;
    GhosttyLauncher launcher;
    CommandRunner runner(console, launcher);

    CLI cli(argc, argv);
    auto cliResult = cli.parse();

    Registry registry(projectsDatabasePath);
    registry.load();

    App app(cliResult.tasks, registry, loader, runner, console);
    app.run();
  } catch (std::runtime_error e) {
    console.error(e.what());
  }

  return 0;
}
