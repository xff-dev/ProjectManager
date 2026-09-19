#pragma once

#include "../../src/App/App.hpp"
#include "../test_helpers.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace app_test {

namespace fs = std::filesystem;
using testhelpers::TestEnv;

class CurrentPathGuard {
public:
  explicit CurrentPathGuard(const std::filesystem::path &path)
      : oldPath(fs::current_path()) {
    fs::current_path(path);
  }

  ~CurrentPathGuard() { fs::current_path(oldPath); }

private:
  std::filesystem::path oldPath;
};

inline void runAppWithCwd(const std::vector<Task> &tasks,
                          const std::string &cwd, Registry &registry,
                          std::ostream &output) {
  CurrentPathGuard cwdGuard(cwd);

  Console console(output);
  ConfigLoader loader;
  CommandRunner runner(console, std::make_unique<Dummy>());
  App app(tasks, registry, loader, runner, console);
  app.run();
}

inline void
configureBuild(const TestEnv &env, const std::optional<std::string> &directory,
               const std::string &prepareCommand = "touch prepared.txt",
               const std::string &buildCommand = "touch built.txt",
               const std::string &scriptCommand = "touch scripted.txt") {
  std::ofstream ini(fs::path(env.projPath) / "project.ini");
  ini << "[data]\n"
      << "editorCommand = echo editor\n"
      << "envCommand = export DUMMY=1\n"
      << "lang = cpp\n"
      << "[build]\n";
  if (directory) {
    ini << "directory = " << *directory << "\n";
  }
  ini << "command = " << buildCommand << "\n"
      << "prepare = " << prepareCommand << "\n"
      << "[run]\n"
      << "command = true\n"
      << "directory = .\n"
      << "[script_open]\n"
      << "directory = .\n"
      << "command = " << scriptCommand << "\n";
}

inline void configureRun(const TestEnv &env, const std::string &command) {
  std::ofstream ini(fs::path(env.projPath) / "project.ini");
  ini << "[data]\n"
      << "editorCommand = echo editor\n"
      << "envCommand = export DUMMY=1\n"
      << "lang = cpp\n"
      << "[build]\n"
      << "command = true\n"
      << "[run]\n"
      << "command = " << command << "\n"
      << "directory = .\n"
      << "[script_open]\n"
      << "directory = .\n"
      << "command = true\n";
}

inline void configureScript(const TestEnv &env, const std::string &name,
                            const std::string &directory,
                            const std::string &command) {
  std::ofstream ini(fs::path(env.projPath) / "project.ini");
  ini << "[data]\n"
      << "editorCommand = echo editor\n"
      << "envCommand = export DUMMY=1\n"
      << "lang = cpp\n"
      << "[build]\n"
      << "command = true\n"
      << "[run]\n"
      << "command = true\n"
      << "directory = .\n"
      << "[script_" << name << "]\n"
      << "directory = " << directory << "\n"
      << "command = " << command << "\n";
}

struct AppFixture {
  explicit AppFixture(const TestEnv &env)
      : registry(std::make_unique<LegacyParser>(env.dbPath)) {
    registry.load();
  }

  Registry registry;
};

} // namespace app_test
