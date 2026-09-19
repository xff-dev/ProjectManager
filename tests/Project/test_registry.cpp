#include "../../src/Project/Registry.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <unistd.h>

namespace {
namespace fs = std::filesystem;

fs::path testDatabasePath(std::string_view suffix) {
  return fs::temp_directory_path() /
         ("project-manager-registry-" + std::to_string(getpid()) + "-" +
          std::string(suffix));
}

std::vector<std::string> readLines(const fs::path &path) {
  std::ifstream file(path);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }
  return lines;
}
} // namespace

TEST_CASE("registry reports missing projects", "[Registry][error]") {
  const auto database = testDatabasePath("missing");
  std::ofstream(database).close();
  Registry registry(std::make_unique<LegacyDatabase>(database));
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
  Registry registry(std::make_unique<LegacyDatabase>(database));
  registry.load();
  Project first{fs::temp_directory_path() / "first", "project"};
  Project duplicate{fs::temp_directory_path() / "second", "project"};
  registry.add(first);

  REQUIRE_THROWS_WITH(registry.add(duplicate),
                      Catch::Matchers::ContainsSubstring("already"));
  fs::remove(database);
}

TEST_CASE("registry persists projects and finds equivalent paths",
          "[Registry]") {
  const auto database = testDatabasePath("round-trip");
  const auto projectDirectory =
      fs::temp_directory_path() /
      ("registry-project-" + std::to_string(getpid()));
  fs::remove_all(projectDirectory);
  fs::create_directories(projectDirectory);
  std::ofstream(database).close();

  Registry registry(std::make_unique<LegacyDatabase>(database));
  registry.load();
  Project project{projectDirectory, "project"};
  registry.add(project);

  Registry reloaded(std::make_unique<LegacyDatabase>(database));
  reloaded.load();
  REQUIRE(reloaded.getProjects().size() == 1);
  REQUIRE(std::filesystem::weakly_canonical(reloaded.find("project").path) ==
          std::filesystem::weakly_canonical(projectDirectory));
  REQUIRE(reloaded.findByPath(projectDirectory / ".").name == "project");

  fs::remove(database);
  fs::remove_all(projectDirectory);
}

TEST_CASE("registry stays consistent after loading the database twice",
          "[Registry]") {
  const auto database = testDatabasePath("reload");
  const auto projectDirectory =
      fs::temp_directory_path() /
      ("registry-reload-project-" + std::to_string(getpid()));
  fs::remove_all(projectDirectory);
  fs::create_directories(projectDirectory);
  std::ofstream(database) << "project|" << projectDirectory.string() << "\n";

  Registry registry(std::make_unique<LegacyDatabase>(database));
  registry.load();
  registry.load();

  REQUIRE(registry.find("project").path == projectDirectory);
  REQUIRE(registry.findByPath(projectDirectory).name == "project");
  Project duplicate{projectDirectory / "other", "project"};
  REQUIRE_THROWS_WITH(registry.add(duplicate),
                      Catch::Matchers::ContainsSubstring("already"));

  fs::remove(database);
  fs::remove_all(projectDirectory);
}

TEST_CASE("loading an empty database twice stays empty", "[Registry]") {
  const auto database = testDatabasePath("empty-reload");
  std::ofstream(database).close();
  Registry registry(std::make_unique<LegacyDatabase>(database));
  registry.load();
  registry.load();
  REQUIRE(registry.getProjects().empty());
  registry.save();
  REQUIRE(readLines(database).empty());
  fs::remove(database);
}

TEST_CASE("loading the database twice does not duplicate projects in memory",
          "[Registry]") {
  const auto database = testDatabasePath("memory-dup");
  const auto projectDirectory =
      fs::temp_directory_path() /
      ("registry-memory-dup-" + std::to_string(getpid()));
  fs::remove_all(projectDirectory);
  fs::create_directories(projectDirectory);
  std::ofstream(database) << "project|" << projectDirectory.string() << "\n";

  Registry registry(std::make_unique<LegacyDatabase>(database));
  registry.load();
  registry.load();

  REQUIRE(registry.getProjects().size() == 1);

  fs::remove(database);
  fs::remove_all(projectDirectory);
}

TEST_CASE("saving after a double load writes each project once", "[Registry]") {
  const auto database = testDatabasePath("disk-dup");
  const auto projectDirectory =
      fs::temp_directory_path() /
      ("registry-disk-dup-" + std::to_string(getpid()));
  fs::remove_all(projectDirectory);
  fs::create_directories(projectDirectory);
  std::ofstream(database) << "project|" << projectDirectory.string() << "\n";

  Registry registry(std::make_unique<LegacyDatabase>(database));
  registry.load();
  registry.load();
  registry.save();

  const auto lines = readLines(database);
  REQUIRE(lines.size() == 1);
  REQUIRE(lines ==
          std::vector<std::string>{"project|" + projectDirectory.string()});

  fs::remove(database);
  fs::remove_all(projectDirectory);
}

TEST_CASE("removing a project after a double load leaves no duplicates on disk",
          "[Registry]") {
  const auto database = testDatabasePath("remove-dup");
  const auto projectDirectory =
      fs::temp_directory_path() /
      ("registry-remove-dup-" + std::to_string(getpid()));
  fs::remove_all(projectDirectory);
  fs::create_directories(projectDirectory);
  std::ofstream(database) << "project|" << projectDirectory.string() << "\n";

  Registry registry(std::make_unique<LegacyDatabase>(database));
  registry.load();
  registry.load();
  registry.remove("project");

  REQUIRE(readLines(database).empty());

  fs::remove(database);
  fs::remove_all(projectDirectory);
}

TEST_CASE("registry reports a path that belongs to no project",
          "[Registry][error]") {
  const auto database = testDatabasePath("missing-path");
  std::ofstream(database).close();
  Registry registry(std::make_unique<LegacyDatabase>(database));
  registry.load();

  REQUIRE_THROWS_WITH(
      registry.findByPath(fs::temp_directory_path() / "unregistered-project"),
      Catch::Matchers::ContainsSubstring("Project"));
  fs::remove(database);
}
