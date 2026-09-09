#include "../../../src/utils/consts.hpp"
#include "../test_app_helpers.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;
using namespace app_test;
using testhelpers::cleanupEnv;
using testhelpers::createEnvironment;

TEST_CASE("test build", "[build][app]") {
  const auto env = createEnvironment("build");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({BuildTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(fs::exists(fs::path(env.projPath) / "build"));
  REQUIRE(output.str().ends_with(std::string(consts::ansi::green) + "==> " +
                                 std::string(consts::ansi::reset) +
                                 std::string(consts::success::BuildSuccessful) +
                                 "\n"));
  cleanupEnv(env);
}

TEST_CASE("build reports its preparation failure", "[build][app][error]") {
  const auto env = createEnvironment("build-preparation-failure");
  configureBuild(env, "build", "exit 17");
  AppFixture fixture(env);
  std::ostringstream output;

  REQUIRE_THROWS_WITH(
      runAppWithCwd({BuildTask{}}, env.projPath, fixture.registry, output),
      Catch::Matchers::ContainsSubstring("prepar"));
  REQUIRE(output.str().find(consts::success::BuildSuccessful) ==
          std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("build reports its command failure", "[build][app][error]") {
  const auto env = createEnvironment("build-command-failure");
  configureBuild(env, ".", "true", "exit 23");
  AppFixture fixture(env);
  std::ostringstream output;

  REQUIRE_THROWS_WITH(
      runAppWithCwd({BuildTask{}}, env.projPath, fixture.registry, output),
      Catch::Matchers::ContainsSubstring("build"));
  REQUIRE(output.str().find(consts::success::BuildSuccessful) ==
          std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("build prepares and builds when configured directory is absent",
          "[build][app]") {
  const auto env = createEnvironment("build-configured-directory-absent");
  configureBuild(env, "build");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({BuildTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(fs::exists(fs::path(env.projPath) / "build" / "prepared.txt"));
  REQUIRE(fs::exists(fs::path(env.projPath) / "build" / "built.txt"));
  cleanupEnv(env);
}

TEST_CASE("build skips preparation when configured directory already exists",
          "[build][app]") {
  const auto env = createEnvironment("build-configured-directory-present");
  configureBuild(env, "build");
  fs::create_directories(fs::path(env.projPath) / "build");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({BuildTask{}}, env.projPath, fixture.registry, output);

  REQUIRE_FALSE(fs::exists(fs::path(env.projPath) / "build" / "prepared.txt"));
  REQUIRE(fs::exists(fs::path(env.projPath) / "build" / "built.txt"));
  cleanupEnv(env);
}

TEST_CASE("build uses project directory when build directory is not configured",
          "[build][app]") {
  const auto env = createEnvironment("build-no-directory");
  configureBuild(env, std::nullopt);
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({BuildTask{}}, env.projPath, fixture.registry, output);

  REQUIRE_FALSE(fs::exists(fs::path(env.projPath) / "prepared.txt"));
  REQUIRE(fs::exists(fs::path(env.projPath) / "built.txt"));
  cleanupEnv(env);
}

TEST_CASE("build and script run in the order supplied to App",
          "[app][multi-task]") {
  const auto env = createEnvironment("build-then-script");
  configureBuild(env, "build", "echo prepare >> ../task-order.txt",
                 "echo build >> ../task-order.txt",
                 "echo script >> task-order.txt");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({BuildTask{}, ScriptTask{"open"}}, env.projPath,
                fixture.registry, output);

  std::ifstream order(fs::path(env.projPath) / "task-order.txt");
  std::string first, second, third;
  std::getline(order, first);
  std::getline(order, second);
  std::getline(order, third);
  REQUIRE(first == "prepare");
  REQUIRE(second == "build");
  REQUIRE(third == "script");
  REQUIRE(output.str().find(consts::success::BuildSuccessful) <
          output.str().find("Script \"open\" ran successfuly"));
  cleanupEnv(env);
}

TEST_CASE("script and build preserve their reversed task order",
          "[app][multi-task]") {
  const auto env = createEnvironment("script-then-build");
  configureBuild(env, "build", "echo prepare >> ../task-order.txt",
                 "echo build >> ../task-order.txt",
                 "echo script >> task-order.txt");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({ScriptTask{"open"}, BuildTask{}}, env.projPath,
                fixture.registry, output);

  std::ifstream order(fs::path(env.projPath) / "task-order.txt");
  std::string first, second, third;
  std::getline(order, first);
  std::getline(order, second);
  std::getline(order, third);
  REQUIRE(first == "script");
  REQUIRE(second == "prepare");
  REQUIRE(third == "build");
  REQUIRE(output.str().find("Script \"open\" ran successfuly") <
          output.str().find(consts::success::BuildSuccessful));
  cleanupEnv(env);
}
