#include "../../src/CLI/CLI.hpp"
#include "../../src/utils/consts.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include <string>
#include <vector>

std::vector<Task> parseArgs(std::vector<const char *> args) {
  std::vector<char *> argv;
  argv.reserve(args.size());
  for (auto arg : args)
    argv.push_back(const_cast<char *>(arg));

  CLI cli(static_cast<int>(argv.size()), argv.data());
  return cli.parse().tasks;
}

TEST_CASE("BuildTask ", "[CLI]") {
  auto tasks = parseArgs({"pm", "build"});

  REQUIRE(tasks.size() == 1);

  CHECK(std::holds_alternative<BuildTask>(tasks[0]));
}

TEST_CASE("RunTask ", "[CLI]") {
  auto tasks = parseArgs({"pm", "run"});

  REQUIRE(tasks.size() == 1);

  CHECK(std::holds_alternative<RunTask>(tasks[0]));
}

TEST_CASE("ListTask ", "[CLI]") {
  auto tasks = parseArgs({"pm", "list"});

  REQUIRE(tasks.size() == 1);

  CHECK(std::holds_alternative<ListTask>(tasks[0]));
}

TEST_CASE("ListNamesTask ", "[CLI]") {
  auto tasks = parseArgs({"pm", "list-names"});

  REQUIRE(tasks.size() == 1);

  CHECK(std::holds_alternative<ListNamesTask>(tasks[0]));
}

TEST_CASE("ScriptTask", "[CLI]") {
  auto tasks = parseArgs({"pm", "script", "build"});

  REQUIRE(tasks.size() == 1);
  REQUIRE(std::holds_alternative<ScriptTask>(tasks[0]));

  const auto &script = std::get<ScriptTask>(tasks[0]);
  CHECK(script.name == "build");
}

TEST_CASE("ListScriptsTask", "[CLI]") {
  auto tasks = parseArgs({"pm", "list-scripts"});

  REQUIRE(tasks.size() == 1);
  CHECK(std::holds_alternative<ListScriptsTask>(tasks[0]));
}

TEST_CASE("RemoveTask", "[CLI]") {
  auto tasks = parseArgs({"pm", "remove", "demo"});

  REQUIRE(tasks.size() == 1);
  REQUIRE(std::holds_alternative<RemoveTask>(tasks[0]));

  const auto &remove = std::get<RemoveTask>(tasks[0]);
  CHECK(remove.name == "demo");
}

TEST_CASE("AddTask", "[CLI]") {
  auto tasks = parseArgs({"pm", "add", "demo", "/tmp/demo-project"});

  REQUIRE(tasks.size() == 1);
  REQUIRE(std::holds_alternative<AddTask>(tasks[0]));

  const auto &add = std::get<AddTask>(tasks[0]);
  CHECK(add.name == "demo");
  CHECK(add.path == std::filesystem::path("/tmp/demo-project"));
}

TEST_CASE("OpenTask", "[CLI]") {
  auto tasks = parseArgs({"pm", "open", "demo"});

  REQUIRE(tasks.size() == 1);
  REQUIRE(std::holds_alternative<OpenTask>(tasks[0]));

  const auto &open = std::get<OpenTask>(tasks[0]);
  CHECK(open.name == "demo");
}

TEST_CASE("ScriptTask missing name throws", "[CLI]") {
  REQUIRE_THROWS_WITH(parseArgs({"pm", "script"}),
                      Catch::Matchers::ContainsSubstring(std::string(consts::errors::MissingScriptName)));
}

TEST_CASE("RemoveTask missing name throws", "[CLI]") {
  REQUIRE_THROWS_WITH(parseArgs({"pm", "remove"}),
                      Catch::Matchers::ContainsSubstring(std::string(consts::errors::MissingProjectName)));
}

TEST_CASE("AddTask missing name throws", "[CLI]") {
  REQUIRE_THROWS_WITH(parseArgs({"pm", "add"}),
                      Catch::Matchers::ContainsSubstring(std::string(consts::errors::MissingProjectName)));
}

TEST_CASE("AddTask missing path throws", "[CLI]") {
  REQUIRE_THROWS_WITH(parseArgs({"pm", "add", "demo"}),
                      Catch::Matchers::ContainsSubstring(std::string(consts::errors::MissingProjectPath)));
}

TEST_CASE("OpenTask missing name throws", "[CLI]") {
  REQUIRE_THROWS_WITH(parseArgs({"pm", "open"}),
                      Catch::Matchers::ContainsSubstring(std::string(consts::errors::MissingProjectName)));
}

TEST_CASE("Unknown command includes the command name in its error", "[CLI]") {
  REQUIRE_THROWS_WITH(parseArgs({"pm", "unknown"}),
                      Catch::Matchers::ContainsSubstring(std::format(consts::errors::UnknownCommand, "unknown", "pm")));
}

TEST_CASE("HelpTask keeps the invoked application name", "[CLI]") {
  const auto tasks = parseArgs({"custom-pm", "help"});

  REQUIRE(tasks.size() == 1);
  REQUIRE(std::holds_alternative<HelpTask>(tasks.front()));
  CHECK(std::get<HelpTask>(tasks.front()).appName == "custom-pm");
}

TEST_CASE("MigrateTask", "[CLI]") {
  auto tasks = parseArgs({"pm", "migrate", "/data/legacy", "/data/data.toml"});

  REQUIRE(tasks.size() == 1);
  REQUIRE(std::holds_alternative<MigrateTask>(tasks[0]));

  const auto &migrate = std::get<MigrateTask>(tasks[0]);
  CHECK(migrate.from == std::filesystem::path("/data/legacy"));
  CHECK(migrate.to == std::filesystem::path("/data/data.toml"));
}

TEST_CASE("MigrateTask missing source throws", "[CLI]") {
  REQUIRE_THROWS_WITH(parseArgs({"pm", "migrate"}),
                      Catch::Matchers::ContainsSubstring(std::string(consts::errors::MissingMigrationSource)));
}

TEST_CASE("MigrateTask missing destination throws", "[CLI]") {
  REQUIRE_THROWS_WITH(parseArgs({"pm", "migrate", "/data/legacy"}),
                      Catch::Matchers::ContainsSubstring(std::string(consts::errors::MissingMigrationDestination)));
}
