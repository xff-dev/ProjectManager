#include "App/App.hpp"
#include "CLI/CLI.hpp"
#include "CommandRunner/CommandRunner.hpp"
#include "CommandRunner/TerminalLauncher.hpp"
#include "Project/ConfigLoader.hpp"
#include "Project/DatabaseHandler/DatabaseHandler.hpp"
#include "Project/Registry.hpp"
#include "utils/Console.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <pwd.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  const char *homedir;

  if ((homedir = getenv("HOME")) == nullptr) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  std::filesystem::path projectsDataPath =
      std::filesystem::path(homedir) / ".project-manager/";
  std::filesystem::path projectsDatabasePath = projectsDataPath / "data.toml";

  Console console(std::cout);

  std::string terminal = getenv("TERM");
  std::unique_ptr<TerminalLauncher> launcher = getTerminalLauncher(terminal);

  try {
    // if (dynamic_cast<Dummy *>(launcher.get()) != nullptr) {
    //   throw std::runtime_error(std::string(consts::errors::UnknownTerminal));
    // }

    ConfigLoader loader;
    CommandRunner runner(console, std::move(launcher));

    CLI cli(argc, argv);
    auto cliResult = cli.parse();

    std::unique_ptr<DatabaseHandler> databaseHandler =
        std::make_unique<TomlDatabase>(projectsDatabasePath);

    Registry registry(std::move(databaseHandler));
    registry.load();

    App app(cliResult.tasks, registry, loader, runner, console);
    app.run();
  } catch (std::runtime_error &e) {
    console.error(e.what());
  }

  return 0;
}
