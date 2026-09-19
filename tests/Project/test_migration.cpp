#include "../../src/Project/DatabaseHandler/DatabaseHandler.hpp"
#include "catch2/catch_test_macros.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

namespace {
namespace fs = std::filesystem;

fs::path migrationPath(std::string_view suffix) {
  return fs::temp_directory_path() /
         ("project-manager-migration-" + std::to_string(getpid()) + "-" +
          std::string(suffix));
}

void writeFile(const fs::path &path, std::string_view content) {
  std::ofstream(path) << content;
}

bool equalProjects(const std::vector<Project> &a,
                   const std::vector<Project> &b) {
  if (a.size() != b.size()) {
    return false;
  }
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i].name != b[i].name || a[i].path != b[i].path) {
      return false;
    }
  }
  return true;
}
} // namespace

TEST_CASE("toml database holds the same projects the legacy database did",
          "[Migration]") {
  const auto legacyFile = migrationPath("legacy");
  const auto tomlFile = migrationPath("toml");
  writeFile(legacyFile, "project-manager|/home/xff/projects/project-manager\n"
                        "my projects|/home/xff/my projects/проект\n"
                        "gamepad-shell|/home/xff/projects/gamepad-shell\n");

  LegacyDatabase legacyDatabase(legacyFile);
  auto projects = legacyDatabase.load();

  TomlDatabase tomlDatabase(tomlFile);
  tomlDatabase.save(projects);
  const auto tomlProjects = tomlDatabase.load();

  REQUIRE(equalProjects(tomlProjects, projects));

  fs::remove(legacyFile);
  fs::remove(tomlFile);
}