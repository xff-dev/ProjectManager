#include "../../../src/Project/DatabaseHandler/DatabaseHandler.hpp"
#include "../test_app_helpers.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

using namespace app_test;
using testhelpers::cleanupEnv;
using testhelpers::createEnvironment;

namespace {
namespace fs = std::filesystem;

void writeFile(const fs::path &path, const std::string &content) {
  std::ofstream(path) << content;
}

std::string readFile(const fs::path &path) {
  std::ifstream file(path);
  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}
} // namespace

TEST_CASE("migrate converts a legacy database into a toml database",
          "[migrate][app]") {
  const auto env = createEnvironment("migrate-convert");
  const fs::path from = fs::path(env.base) / "data-legacy";
  writeFile(from, "myproj|" + env.projPath + "\n");
  const fs::path to = fs::path(env.base) / "data-toml";

  AppFixture fixture(env);
  std::ostringstream output;
  runAppWithCwd({MigrateTask{from, to}}, env.projPath, fixture.registry,
                output);

  REQUIRE(fs::exists(to));
  const auto content = readFile(to);
  REQUIRE(content.find("[[projects]]") != std::string::npos);
  REQUIRE(content.find("myproj") != std::string::npos);
  REQUIRE(content.find(env.projPath) != std::string::npos);

  REQUIRE(readFile(from) == "myproj|" + env.projPath + "\n");
  cleanupEnv(env);
}

TEST_CASE("migrate converts an empty legacy database", "[migrate][app]") {
  const auto env = createEnvironment("migrate-empty");
  const fs::path from = fs::path(env.base) / "data-legacy";
  writeFile(from, "");
  const fs::path to = fs::path(env.base) / "data-toml";

  AppFixture fixture(env);
  std::ostringstream output;
  runAppWithCwd({MigrateTask{from, to}}, env.projPath, fixture.registry,
                output);

  REQUIRE(fs::exists(to));

  TomlDatabase tomlDatabase(to);
  REQUIRE(tomlDatabase.load().empty());
  cleanupEnv(env);
}

TEST_CASE("migrate produces a database the app serves in the next run",
          "[migrate][app]") {
  const auto env = createEnvironment("migrate-next-run");
  const fs::path from = fs::path(env.base) / "data-legacy";
  writeFile(from, "myproj|" + env.projPath + "\n");
  const fs::path to = fs::path(env.base) / "data-toml";

  {
    AppFixture fixture(env);
    std::ostringstream output;
    runAppWithCwd({MigrateTask{from, to}}, env.projPath, fixture.registry,
                  output);
  }

  Registry registry(std::make_unique<TomlDatabase>(to));
  registry.load();
  std::ostringstream output;
  runAppWithCwd({ListNamesTask{}}, env.projPath, registry, output);

  REQUIRE(output.str().find("myproj\n") != std::string::npos);
  cleanupEnv(env);
}