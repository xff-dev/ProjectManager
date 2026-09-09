#include "../test_app_helpers.hpp"
#include "catch2/catch_test_macros.hpp"

#include <sstream>

using namespace app_test;
using testhelpers::cleanupEnv;
using testhelpers::createEnvironment;

TEST_CASE("help prints usage for the invoked application", "[help][app]") {
  const auto env = createEnvironment("help");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({HelpTask{"custom-pm"}}, env.projPath, fixture.registry,
                output);

  REQUIRE(output.str().find("Usage: custom-pm") != std::string::npos);
  REQUIRE(output.str().find("build") != std::string::npos);
  REQUIRE(output.str().find("script <name>") != std::string::npos);
  cleanupEnv(env);
}
