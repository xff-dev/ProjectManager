#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

namespace testhelpers {
namespace fs = std::filesystem;

struct TestEnv {
  std::string base;
  std::string projPath;
  std::string dbPath;
};

inline TestEnv
createEnvironment(const std::string &suffix,
                  const std::string &scriptCmd = "touch created_by_script.txt",
                  const std::string &scriptFile = "created_by_script.txt") {
  TestEnv env;
  env.base =
      "/tmp/project-manager-test-" + std::to_string(getpid()) + "-" + suffix;

  if (fs::exists(env.base)) {
    fs::remove_all(env.base);
  }

  fs::create_directories(env.base);
  env.projPath = env.base + "/myproj";
  fs::create_directories(env.projPath);

  const std::string iniPath = env.projPath + "/project.ini";
  std::ofstream ini(iniPath);
  ini << "[data]\n";
  ini << "editorCommand = echo editor\n";
  ini << "envCommand = export DUMMY=1\n";
  ini << "lang = cpp\n";
  ini << "[build]\n";
  ini << "directory = build\n";
  ini << "command = touch build\n";
  ini << "prepare = touch prepare\n";
  ini << "[run]\n";
  ini << "command = touch run\n";
  ini << "directory = .\n";
  ini << "[script_open]\n";
  ini << "directory = .\n";
  ini << "command = " << scriptCmd << "\n";
  ini.close();

  env.dbPath = env.base + "/data";
  std::ofstream db(env.dbPath);
  db << "myproj|" << env.projPath << std::endl;
  db.close();

  return env;
}

inline void cleanupEnv(const TestEnv &env) {
  if (fs::exists(env.base)) {
    fs::remove_all(env.base);
  }
}

} // namespace testhelpers
