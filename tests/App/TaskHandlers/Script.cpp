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

TEST_CASE("test script open", "[script_open][app]") {
  const auto env = createEnvironment("script");
  AppFixture fixture(env);

  std::ostringstream output;
  runAppWithCwd({ScriptTask{"open"}}, env.projPath, fixture.registry, output);

  REQUIRE(fs::exists(fs::path(env.projPath) / "created_by_script.txt"));
  REQUIRE(output.str().ends_with(std::string(consts::ansi::green) + "==> " +
                                 std::string(consts::ansi::reset) +
                                 "Script \"open\" ran successfuly\n"));
  cleanupEnv(env);
}

TEST_CASE("script warns when its command fails", "[script][app][warning]") {
  const auto env = createEnvironment("script-failure", "exit 9");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({ScriptTask{"open"}}, env.projPath, fixture.registry, output);

  REQUIRE(
      output.str().ends_with(std::string(consts::ansi::yellow) + "==> " +
                             std::string(consts::ansi::reset) +
                             "Process exited with non-zero status code: 9\n"));
  REQUIRE(output.str().find(consts::success::ScriptSuccessful) ==
          std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("script reports an unknown script", "[script][app][error]") {
  const auto env = createEnvironment("unknown-script");
  AppFixture fixture(env);
  std::ostringstream output;

  REQUIRE_THROWS_WITH(runAppWithCwd({ScriptTask{"missing"}}, env.projPath,
                                    fixture.registry, output),
                      Catch::Matchers::ContainsSubstring("Script"));
  cleanupEnv(env);
}

TEST_CASE("test list-scripts", "[list-scripts][app]") {
  const auto env = createEnvironment("list-scripts");
  AppFixture fixture(env);
  std::ostringstream output;
  runAppWithCwd({ListScriptsTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(output.str().find("open") != std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("list-scripts prints every configured script", "[list-scripts][app]") {
  const auto env = createEnvironment("list-scripts-all");
  configureScript(env, "lint", ".", "true");
  std::ofstream ini(std::filesystem::path(env.projPath) / "project.ini",
                    std::ios::app);
  ini << "[script_format]\n"
      << "directory = .\n"
      << "command = true\n";
  ini.close();
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({ListScriptsTask{}}, env.projPath, fixture.registry, output);

  REQUIRE(output.str().find("lint\n") != std::string::npos);
  REQUIRE(output.str().find("format\n") != std::string::npos);
  cleanupEnv(env);
}

TEST_CASE("script runs in its configured directory", "[script][app]") {
  const auto env = createEnvironment("script-directory");
  const auto scriptDirectory = std::filesystem::path(env.projPath) / "tools";
  std::filesystem::create_directories(scriptDirectory);
  configureScript(env, "format", "tools", "touch ran-here");
  AppFixture fixture(env);
  std::ostringstream output;

  runAppWithCwd({ScriptTask{"format"}}, env.projPath, fixture.registry,
                output);

  REQUIRE(std::filesystem::exists(scriptDirectory / "ran-here"));
  cleanupEnv(env);
}
