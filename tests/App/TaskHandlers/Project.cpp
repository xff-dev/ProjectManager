#include "../../../src/utils/consts.hpp"
#include "../test_app_helpers.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include <fstream>
#include <sstream>

using namespace app_test;
using testhelpers::cleanupEnv;
using testhelpers::createEnvironment;

TEST_CASE("test list", "[list][app]") {
  const auto env = createEnvironment("list");
  AppFixture fixture(env);
  std::ostringstream output;
  runAppWithCwd({ListTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(output.str().find("myproj") != std::string::npos);
  REQUIRE(output.str().find(consts::icons::cpp) != std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("test list-names", "[list-names][app]") {
  const auto env = createEnvironment("list-names");
  AppFixture fixture(env);
  std::ostringstream output;
  runAppWithCwd({ListNamesTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(output.str().find("myproj") != std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("list marks an unavailable project and shows its path",
          "[list][app]") {
  const auto env = createEnvironment("list-unavailable");
  AppFixture fixture(env);
  const auto missingPath = std::filesystem::path(env.base) / "unavailable";
  Project unavailable{missingPath, "offline"};
  fixture.registry.add(unavailable);
  std::ostringstream output;

  runAppWithCwd({ListTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(output.str().find(std::string(consts::icons::missing)) != std::string::npos);
  REQUIRE(output.str().find(missingPath.string()) != std::string::npos);

  // REQUIRE(output.str().find(std::string(consts::icons::missing) +
  //                           " offline (" + missingPath.string() + ")\n") !=
  //         std::string::npos);
  
  cleanupEnv(env);
}

TEST_CASE("list remains useful when a project config is unavailable",
          "[list][app]") {
  const auto env = createEnvironment("list-missing-config");
  AppFixture fixture(env);
  const auto projectPath = std::filesystem::path(env.base) / "no-config";
  std::filesystem::create_directories(projectPath);
  Project project{projectPath, "no-config"};
  fixture.registry.add(project);
  std::ostringstream output;

  runAppWithCwd({ListTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(output.str().find("● no-config\n") != std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("list-names prints every registered project", "[list-names][app]") {
  const auto env = createEnvironment("list-names-all");
  AppFixture fixture(env);
  Project second{std::filesystem::path(env.projPath), "second"};
  fixture.registry.add(second);
  std::ostringstream output;

  runAppWithCwd({ListNamesTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(output.str().find("myproj\n") != std::string::npos);
  REQUIRE(output.str().find("second\n") != std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("test remove", "[remove][app]") {
  const auto env = createEnvironment("remove");
  AppFixture fixture(env);

  std::ostringstream output;
  runAppWithCwd({RemoveTask{"myproj"}}, env.projPath, fixture.registry, output);

  fixture.registry.save();

  std::ifstream db(env.dbPath);
  std::string dbContent;
  std::getline(db, dbContent);
  REQUIRE(dbContent.empty());
  cleanupEnv(env);
}

TEST_CASE("test add", "[add][app]") {
  const auto env = createEnvironment("add");
  AppFixture fixture(env);
  const std::string newProjectPath = env.base + "/newproj";
  std::filesystem::create_directories(newProjectPath);

  std::ostringstream output;
  runAppWithCwd({AddTask{"newproj", newProjectPath}}, env.projPath,
                fixture.registry, output);

  fixture.registry.save();

  std::ifstream db(env.dbPath);
  std::string dbContent((std::istreambuf_iterator<char>(db)), {});
  REQUIRE(dbContent.find("newproj") != std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("open reports a project directory that is unavailable",
          "[open][app][error]") {
  const auto env = createEnvironment("open-missing-directory");
  AppFixture fixture(env);
  const auto missingPath = std::filesystem::path(env.base) / "unavailable";
  Project project{missingPath, "missing"};
  fixture.registry.add(project);
  std::ostringstream output;

  REQUIRE_THROWS_WITH(runAppWithCwd({OpenTask{"missing"}}, env.projPath,
                                    fixture.registry, output),
                      Catch::Matchers::ContainsSubstring("directory"));
  cleanupEnv(env);
}

TEST_CASE("open runs the configured editor with its environment",
          "[open][app]") {
  const auto env = createEnvironment("open-editor");
  std::ofstream ini(std::filesystem::path(env.projPath) / "project.ini");
  ini << "[data]\n"
      << "editorCommand = test \"$DUMMY\" = \"1\" && touch opened.txt\n"
      << "envCommand = export DUMMY=1\n";
  ini.close();
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({OpenTask{"myproj"}}, env.projPath, fixture.registry, output);

  REQUIRE(std::filesystem::exists(std::filesystem::path(env.projPath) /
                                  "opened.txt"));
  cleanupEnv(env);
}
