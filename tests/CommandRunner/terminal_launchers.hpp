#pragma once

#include "../../src/CommandRunner/Command.hpp"
#include "../../src/CommandRunner/TerminalLauncher.hpp"

#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_tostring.hpp"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <tuple>
#include <unistd.h>
#include <vector>

namespace termlaunchers {
namespace fs = std::filesystem;

template <class T> const char *terminalBinaryName();

template <> inline const char *terminalBinaryName<GhosttyLauncher>() {
  return "ghostty";
}
template <> inline const char *terminalBinaryName<KittyLauncher>() {
  return "kitty";
}

// Single place listing the terminal launchers to exercise in the
// real-launch tests. Add or remove an entry here to include/exclude it.
using TestedLaunchers = std::tuple<GhosttyLauncher, KittyLauncher>;

using Clock = std::chrono::steady_clock;

inline bool commandAvailable(const std::string &name) {
  const char *pathEnv = getenv("PATH");
  if (pathEnv != nullptr) {
    std::stringstream paths(pathEnv);
    std::string dir;
    while (std::getline(paths, dir, ':')) {
      const fs::path candidate = fs::path(dir.empty() ? "." : dir) / name;
      if (fs::exists(candidate)) {
        return true;
      }
    }
  }
  for (const char *dir :
       {"/usr/bin", "/usr/local/bin", "/usr/sbin", "/bin", "/sbin"}) {
    if (fs::exists(fs::path(dir) / name)) {
      return true;
    }
  }
  return false;
}

inline bool hasDisplay() {
  return getenv("DISPLAY") != nullptr || getenv("WAYLAND_DISPLAY") != nullptr;
}

template <class Launcher> void requireLaunchEnvironment() {
  if (!commandAvailable(terminalBinaryName<Launcher>())) {
    SKIP(std::string("terminal '") + terminalBinaryName<Launcher>() +
         "' is not installed; real terminal tests are skipped");
  }
  if (!commandAvailable("zsh")) {
    SKIP("zsh is not installed; real terminal tests are skipped");
  }
  if (!hasDisplay()) {
    SKIP("no graphical session; real terminal tests are skipped");
  }
}

inline fs::path uniqueBaseDir(const std::string &tag) {
  static std::atomic<unsigned> counter{0};
  const fs::path base =
      fs::temp_directory_path() /
      ("pm-terminal-test-" + tag + "-" + std::to_string(getpid()) + "-" +
       std::to_string(counter.fetch_add(1)));
  fs::remove_all(base);
  fs::create_directories(base);
  return base;
}

inline bool
waitForFile(const fs::path &path,
            std::chrono::seconds timeout = std::chrono::seconds(2)) {
  const auto deadline = Clock::now() + timeout;
  while (Clock::now() < deadline) {
    if (fs::exists(path)) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  return fs::exists(path);
}

inline std::vector<std::string> readLines(const fs::path &path) {
  std::ifstream file(path);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }
  return lines;
}

inline std::string probeCommand(const fs::path &output,
                                const std::string &var) {
  return "printf '%s\\n%s\\n' \"$PWD\" \"${" + var + ":-unset}\" > '" +
         output.string() + "'";
}

class ScopedTerminal {
public:
  explicit ScopedTerminal(pid_t pid) : pid_(pid) {}

  ~ScopedTerminal() {
    if (pid_ > 0) {
      kill(pid_, SIGTERM);
      waitpid(pid_, nullptr, 0);
    }
  }

  ScopedTerminal(const ScopedTerminal &) = delete;
  ScopedTerminal &operator=(const ScopedTerminal &) = delete;

private:
  pid_t pid_;
};

} // namespace termlaunchers

namespace Catch {
template <> struct StringMaker<GhosttyLauncher> {
  static std::string convert(const GhosttyLauncher &) { return "Ghostty"; }
};
template <> struct StringMaker<KittyLauncher> {
  static std::string convert(const KittyLauncher &) { return "Kitty"; }
};
} // namespace Catch
