#include "../../src/utils/consts.hpp"
#include "test_app_helpers.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include <format>
#include <iterator>
#include <sstream>

using namespace app_test;
using testhelpers::cleanupEnv;
using testhelpers::createEnvironment;

TEST_CASE("local tasks work without registering the project",
          "[app][project-context]") {
  const auto env = createEnvironment("unregistered-project");
  Registry registry(std::make_unique<LegacyDatabase>(env.dbPath));
  std::ostringstream output;

  SECTION("build prepares and builds in its configured directory") {
    configureBuild(env, "build");
    runAppWithCwd({BuildTask{}}, env.projPath, registry, output);
    REQUIRE(fs::exists(fs::path(env.projPath) / "build/prepared.txt"));
    REQUIRE(fs::exists(fs::path(env.projPath) / "build/built.txt"));
    REQUIRE(output.str().find(consts::success::BuildSuccessful) !=
            std::string::npos);
  }

  SECTION("run preserves arguments and its configured directory") {
    fs::create_directory(fs::path(env.projPath) / "output");
    std::ofstream ini(fs::path(env.projPath) / "project.ini");
    ini << "[run]\ncommand = touch\ndirectory = output\n";
    ini.close();
    runAppWithCwd({RunTask{{"one", "two"}}}, env.projPath, registry, output);
    REQUIRE(fs::exists(fs::path(env.projPath) / "output/one"));
    REQUIRE(fs::exists(fs::path(env.projPath) / "output/two"));
    REQUIRE_FALSE(fs::exists(fs::path(env.projPath) / "one"));
  }

  SECTION("script uses its configured directory") {
    fs::create_directory(fs::path(env.projPath) / "tools");
    configureScript(env, "lint", "tools", "touch ran-here");
    runAppWithCwd({ScriptTask{"lint"}}, env.projPath, registry, output);
    REQUIRE(fs::exists(fs::path(env.projPath) / "tools/ran-here"));
    REQUIRE(output.str().find(
                std::format(consts::success::ScriptSuccessful, "lint")) !=
            std::string::npos);
  }

  SECTION("list-scripts reads the local configuration") {
    runAppWithCwd({ListScriptsTask{}}, env.projPath, registry, output);
    REQUIRE(output.str().find("open\n") != std::string::npos);
  }

  SECTION("chained tasks preserve their order") {
    configureBuild(env, "build", "echo prepare >> ../order.txt",
                   "echo build >> ../order.txt", "echo script >> order.txt");
    runAppWithCwd({BuildTask{}, ScriptTask{"open"}}, env.projPath, registry,
                  output);
    std::ifstream order(fs::path(env.projPath) / "order.txt");
    std::string contents((std::istreambuf_iterator<char>(order)), {});
    REQUIRE(contents == "prepare\nbuild\nscript\n");
  }

  REQUIRE(output.str().find(consts::errors::ProjectNotFound) !=
          std::string::npos);
  REQUIRE(registry.getProjects().empty());
  cleanupEnv(env);
}

TEST_CASE("local tasks report a missing configuration outside the registry",
          "[app][project-context][error]") {
  const auto env = createEnvironment("unregistered-missing-config");
  fs::remove(fs::path(env.projPath) / "project.ini");
  Registry registry(std::make_unique<LegacyDatabase>(env.dbPath));
  std::ostringstream output;
  std::vector<Task> tasks;

  SECTION("build") { tasks = {BuildTask{}}; }
  SECTION("run") { tasks = {RunTask{}}; }
  SECTION("script") { tasks = {ScriptTask{"open"}}; }
  SECTION("list-scripts") { tasks = {ListScriptsTask{}}; }

  REQUIRE_THROWS_WITH(runAppWithCwd(tasks, env.projPath, registry, output),
                      std::string(consts::errors::ProjectConfigNotFound));
  REQUIRE(output.str().find(consts::errors::ProjectNotFound) !=
          std::string::npos);
  REQUIRE_FALSE(fs::exists(fs::path(env.projPath) / "build"));
  REQUIRE_FALSE(fs::exists(fs::path(env.projPath) / "run"));
  REQUIRE_FALSE(fs::exists(fs::path(env.projPath) / "created_by_script.txt"));
  cleanupEnv(env);
}

TEST_CASE("registered local tasks do not warn about a missing project",
          "[app][project-context]") {
  const auto env = createEnvironment("registered-project-context");
  AppFixture fixture(env);
  std::ostringstream output;
  runAppWithCwd({BuildTask{}, RunTask{}, ScriptTask{"open"}, ListScriptsTask{}},
                env.projPath, fixture.registry, output);
  REQUIRE(output.str().find(consts::errors::ProjectNotFound) ==
          std::string::npos);
  REQUIRE(fs::exists(fs::path(env.projPath) / "build/build"));
  REQUIRE(fs::exists(fs::path(env.projPath) / "run"));
  REQUIRE(fs::exists(fs::path(env.projPath) / "created_by_script.txt"));
  REQUIRE(output.str().find("open\n") != std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("registry filesystem errors are warned before using a local config",
          "[app][project-context][warning]") {
  const auto env = createEnvironment("project-context-filesystem-error");
  const auto loop = fs::path(env.base) / "loop";
  fs::create_symlink("loop", loop);
  Registry registry(std::make_unique<LegacyDatabase>(env.dbPath));
  Project broken{loop, "broken"};
  registry.add(broken);
  std::string warning;
  try {
    registry.findByPath(env.projPath);
    FAIL("Expected a filesystem error from the symlink loop");
  } catch (const fs::filesystem_error &error) {
    warning = error.what();
  }

  std::ostringstream output;
  runAppWithCwd({RunTask{}}, env.projPath, registry, output);
  REQUIRE(output.str().find(warning) != std::string::npos);
  REQUIRE(output.str().find(warning) < output.str().find("touch run"));
  REQUIRE(fs::exists(fs::path(env.projPath) / "run"));
  cleanupEnv(env);
}
