#include "../../src/Project/DatabaseHandler/DatabaseHandler.hpp"
#include "catch2/catch_test_macros.hpp"
#include <toml++/toml.hpp>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

namespace {
namespace fs = std::filesystem;

fs::path tomlDatabasePath(std::string_view suffix) {
  return fs::temp_directory_path() /
         ("project-manager-toml-" + std::to_string(getpid()) + "-" +
          std::string(suffix));
}

std::string readFile(const fs::path &path) {
  std::ifstream file(path);
  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

void writeFile(const fs::path &path, std::string_view content) {
  std::ofstream(path) << content;
}

std::vector<Project> parseWithTomlpp(const fs::path &path) {
  const auto table = toml::parse_file(path.string());
  std::vector<Project> projects;
  if (const auto *entries = table["projects"].as_array()) {
    for (const auto &entry : *entries) {
      const auto &project = *entry.as_table();
      const std::string name = project["name"].value_or("");
      const std::string projectPath = project["path"].value_or("");
      projects.push_back(Project{projectPath, name});
    }
  }
  return projects;
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

TEST_CASE("TomlDatabase loads projects from a toml file", "[TomlDatabase]") {
  const auto database = tomlDatabasePath("load");
  writeFile(database, R"([[projects]]
name = "project-manager"
path = "/home/xff/projects/project-manager"

[[projects]]
name = "gamepad-shell"
path = "/home/xff/projects/gamepad-shell"
)");

  TomlDatabase tomlDatabase(database);
  const auto projects = tomlDatabase.load();

  REQUIRE(projects.size() == 2);
  REQUIRE(projects[0].name == "project-manager");
  REQUIRE(projects[0].path == "/home/xff/projects/project-manager");
  REQUIRE(projects[1].name == "gamepad-shell");
  REQUIRE(projects[1].path == "/home/xff/projects/gamepad-shell");

  fs::remove(database);
}

TEST_CASE("TomlDatabase loads a single project", "[TomlDatabase]") {
  const auto database = tomlDatabasePath("single");
  writeFile(database, R"([[projects]]
name = "project-manager"
path = "/home/xff/projects/project-manager"
)");

  TomlDatabase tomlDatabase(database);
  const auto projects = tomlDatabase.load();

  REQUIRE(projects.size() == 1);
  REQUIRE(projects[0].name == "project-manager");
  REQUIRE(projects[0].path == "/home/xff/projects/project-manager");

  fs::remove(database);
}

TEST_CASE("TomlDatabase returns no projects when the database file does not exist",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("missing");

  TomlDatabase tomlDatabase(database);
  REQUIRE(tomlDatabase.load().empty());
}

TEST_CASE("TomlDatabase returns no projects when the database file is empty",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("empty-file");
  writeFile(database, "");

  TomlDatabase tomlDatabase(database);
  REQUIRE(tomlDatabase.load().empty());

  fs::remove(database);
}

TEST_CASE("TomlDatabase returns no projects when the projects array has no entries",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("empty-array");
  writeFile(database, "projects = []\n");

  TomlDatabase tomlDatabase(database);
  REQUIRE(tomlDatabase.load().empty());

  fs::remove(database);
}

TEST_CASE("TomlDatabase keeps names and paths with spaces and non-ascii",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("spaces");
  writeFile(database, R"([[projects]]
name = "my cool project"
path = "/home/xff/my projects/проект"
)");

  TomlDatabase tomlDatabase(database);
  const auto projects = tomlDatabase.load();

  REQUIRE(projects.size() == 1);
  REQUIRE(projects[0].name == "my cool project");
  REQUIRE(projects[0].path == "/home/xff/my projects/проект");

  fs::remove(database);
}

TEST_CASE("TomlDatabase ignores extra fields inside a project entry",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("extra-fields");
  writeFile(database, R"([[projects]]
name = "project-manager"
path = "/home/xff/projects/project-manager"
lang = "cpp"
editorCommand = "nvim ."
)");

  TomlDatabase tomlDatabase(database);
  const auto projects = tomlDatabase.load();

  REQUIRE(projects.size() == 1);
  REQUIRE(projects[0].name == "project-manager");
  REQUIRE(projects[0].path == "/home/xff/projects/project-manager");

  fs::remove(database);
}

TEST_CASE("TomlDatabase returns duplicate names as they appear in the file",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("duplicates");
  writeFile(database, R"([[projects]]
name = "dup"
path = "/home/xff/projects/one"

[[projects]]
name = "dup"
path = "/home/xff/projects/two"
)");

  TomlDatabase tomlDatabase(database);
  const auto projects = tomlDatabase.load();

  REQUIRE(projects.size() == 2);
  REQUIRE(projects[0].name == "dup");
  REQUIRE(projects[0].path == "/home/xff/projects/one");
  REQUIRE(projects[1].name == "dup");
  REQUIRE(projects[1].path == "/home/xff/projects/two");

  fs::remove(database);
}

TEST_CASE("TomlDatabase throws when the database file is not valid toml",
          "[TomlDatabase][error]") {
  const auto database = tomlDatabasePath("malformed");
  writeFile(database, "[[projects]\nname = \"broken\"\n");

  TomlDatabase tomlDatabase(database);
  REQUIRE_THROWS(tomlDatabase.load());

  fs::remove(database);
}

TEST_CASE("TomlDatabase throws when a project entry has no name",
          "[TomlDatabase][error]") {
  const auto database = tomlDatabasePath("no-name");
  writeFile(database, "[[projects]]\npath = \"/home/xff/projects/p\"\n");

  TomlDatabase tomlDatabase(database);
  REQUIRE_THROWS(tomlDatabase.load());

  fs::remove(database);
}

TEST_CASE("TomlDatabase throws when a project entry has no path",
          "[TomlDatabase][error]") {
  const auto database = tomlDatabasePath("no-path");
  writeFile(database, "[[projects]]\nname = \"project-manager\"\n");

  TomlDatabase tomlDatabase(database);
  REQUIRE_THROWS(tomlDatabase.load());

  fs::remove(database);
}

TEST_CASE("TomlDatabase throws when a project field is not a string",
          "[TomlDatabase][error]") {
  const auto database = tomlDatabasePath("wrong-type");
  writeFile(database, "[[projects]]\nname = 123\npath = \"/home/xff/projects/p\"\n");

  TomlDatabase tomlDatabase(database);
  REQUIRE_THROWS(tomlDatabase.load());

  fs::remove(database);
}

TEST_CASE("TomlDatabase saves projects to a toml file", "[TomlDatabase]") {
  const auto database = tomlDatabasePath("save");
  writeFile(database, "");

  TomlDatabase tomlDatabase(database);
  std::vector<Project> projects = {
      Project{"/home/xff/projects/project-manager", "project-manager"},
      Project{"/home/xff/projects/gamepad-shell", "gamepad-shell"},
  };
  tomlDatabase.save(projects);

  const auto content = readFile(database);
  REQUIRE(content.find("project-manager") != std::string::npos);
  REQUIRE(content.find("/home/xff/projects/project-manager") !=
          std::string::npos);
  REQUIRE(content.find("gamepad-shell") != std::string::npos);
  REQUIRE(content.find("/home/xff/projects/gamepad-shell") != std::string::npos);

  fs::remove(database);
}

TEST_CASE("saved file parses as valid toml with the project data",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("valid-save");
  writeFile(database, "");

  TomlDatabase tomlDatabase(database);
  std::vector<Project> projects = {
      Project{"/home/xff/projects/project-manager", "project-manager"},
      Project{"/home/xff/projects/gamepad-shell", "gamepad-shell"},
  };
  tomlDatabase.save(projects);

  REQUIRE(equalProjects(parseWithTomlpp(database), projects));

  fs::remove(database);
}

TEST_CASE("saving an empty project list round-trips to an empty load",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("save-empty");
  writeFile(database, "");

  TomlDatabase tomlDatabase(database);
  std::vector<Project> projects;
  tomlDatabase.save(projects);

  REQUIRE(tomlDatabase.load().empty());
  REQUIRE(parseWithTomlpp(database).empty());

  fs::remove(database);
}

TEST_CASE("save replaces the previous database content", "[TomlDatabase]") {
  const auto database = tomlDatabasePath("overwrite");
  writeFile(database, R"([[projects]]
name = "stale"
path = "/home/xff/projects/stale"
)");

  TomlDatabase tomlDatabase(database);
  std::vector<Project> projects = {
      Project{"/home/xff/projects/current", "current"},
  };
  tomlDatabase.save(projects);

  const auto loaded = tomlDatabase.load();
  REQUIRE(loaded.size() == 1);
  REQUIRE(loaded[0].name == "current");
  REQUIRE(loaded[0].path == "/home/xff/projects/current");

  fs::remove(database);
}

TEST_CASE("names and paths survive a save and load round-trip",
          "[TomlDatabase]") {
  const auto database = tomlDatabasePath("roundtrip");
  writeFile(database, "");

  TomlDatabase tomlDatabase(database);
  std::vector<Project> projects = {
      Project{"/odd \"path\"\t/\\back", "na\"me\\"},
      Project{"/home/xff/my projects/проект", "project-manager"},
  };
  tomlDatabase.save(projects);

  REQUIRE(equalProjects(tomlDatabase.load(), projects));
  REQUIRE(equalProjects(parseWithTomlpp(database), projects));

  fs::remove(database);
}