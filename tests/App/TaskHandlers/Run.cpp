#include "../../../src/utils/consts.hpp"
#include "../test_app_helpers.hpp"
#include "catch2/catch_test_macros.hpp"
#include <sstream>

namespace fs = std::filesystem;
using namespace app_test;
using testhelpers::cleanupEnv;
using testhelpers::createEnvironment;

TEST_CASE("test run", "[run][app]") {
  const auto env = createEnvironment("run");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({RunTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(fs::exists(fs::path(env.projPath) / "run"));
  REQUIRE(output.str().find(consts::success::RunSuccessful) ==
          std::string::npos);
  REQUIRE(output.str().find(consts::warnings::NonZeroStatusCode) ==
          std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("run passes args from -- to the configured command", "[run][app]") {
  const auto env = createEnvironment("run-args");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({RunTask{{"one", "two"}}}, env.projPath, fixture.registry,
                output);

  REQUIRE(fs::exists(fs::path(env.projPath) / "run"));
  REQUIRE(fs::exists(fs::path(env.projPath) / "one"));
  REQUIRE(fs::exists(fs::path(env.projPath) / "two"));
  cleanupEnv(env);
}

TEST_CASE("run warns when its command fails", "[run][app][warning]") {
  const auto env = createEnvironment("run-failure");
  configureRun(env, "exit 12");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({RunTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(output.str().find(std::format(consts::warnings::NonZeroStatusCode, 12)) !=
        std::string::npos);
  REQUIRE(output.str().find(consts::success::RunSuccessful) ==
          std::string::npos);
  cleanupEnv(env);
}
