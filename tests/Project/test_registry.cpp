#include "../../src/Project/Registry.hpp"
#include "../../src/utils/consts.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

namespace {
namespace fs = std::filesystem;

fs::path testDatabasePath(std::string_view suffix) {
  return fs::temp_directory_path() /
         ("project-manager-registry-" + std::to_string(getpid()) + "-" +
          std::string(suffix));
}
} // namespace

TEST_CASE("registry reports missing projects", "[Registry][error]") {
  const auto database = testDatabasePath("missing");
  std::ofstream(database).close();
  Registry registry(database);
  registry.load();

  REQUIRE_THROWS_WITH(registry.find("unknown"),
                      Catch::Matchers::ContainsSubstring("Project"));
  REQUIRE_THROWS_WITH(registry.remove("unknown"),
                      Catch::Matchers::ContainsSubstring("Project"));
  fs::remove(database);
}

TEST_CASE("registry reports duplicate project names", "[Registry][error]") {
  const auto database = testDatabasePath("duplicate");
  std::ofstream(database).close();
  Registry registry(database);
  registry.load();
  Project first{fs::temp_directory_path() / "first", "project"};
  Project duplicate{fs::temp_directory_path() / "second", "project"};
  registry.add(first);

  REQUIRE_THROWS_WITH(registry.add(duplicate),
                      Catch::Matchers::ContainsSubstring("already"));
  fs::remove(database);
}

TEST_CASE("registry persists projects and finds equivalent paths", "[Registry]") {
  const auto database = testDatabasePath("round-trip");
  const auto projectDirectory =
      fs::temp_directory_path() / ("registry-project-" + std::to_string(getpid()));
  fs::remove_all(projectDirectory);
  fs::create_directories(projectDirectory);
  std::ofstream(database).close();

  Registry registry(database);
  registry.load();
  Project project{projectDirectory, "project"};
  registry.add(project);

  Registry reloaded(database);
  reloaded.load();
  REQUIRE(reloaded.getProjects().size() == 1);
  REQUIRE(std::filesystem::weakly_canonical(reloaded.find("project").path) == std::filesystem::weakly_canonical(projectDirectory));
  REQUIRE(reloaded.findByPath(projectDirectory / ".").name == "project");

  fs::remove(database);
  fs::remove_all(projectDirectory);
}

TEST_CASE("registry reports a path that belongs to no project", "[Registry][error]") {
  const auto database = testDatabasePath("missing-path");
  std::ofstream(database).close();
  Registry registry(database);
  registry.load();

  REQUIRE_THROWS_WITH(registry.findByPath(fs::temp_directory_path() /
                                          "unregistered-project"),
                      Catch::Matchers::ContainsSubstring("Project"));
  fs::remove(database);
}
