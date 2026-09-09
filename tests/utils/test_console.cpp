#include "../../src/utils/Console.hpp"
#include "../../src/utils/consts.hpp"
#include "catch2/catch_test_macros.hpp"

#include <sstream>

TEST_CASE("Console formats status messages consistently", "[Console]") {
  std::ostringstream output;
  Console console(output);

  console.step("running command");
  console.warn("Process exited with non-zero status code: 7");
  console.success("Build successful");
  console.error("Project not found");

  REQUIRE(output.str() ==
          std::string(consts::ansi::blue) + "==> " +
              std::string(consts::ansi::reset) + "running command\n" +
              std::string(consts::ansi::yellow) + "==> " +
              std::string(consts::ansi::reset) +
              "Process exited with non-zero status code: 7\n" +
              std::string(consts::ansi::green) + "==> " +
              std::string(consts::ansi::reset) + "Build successful\n" +
              std::string(consts::ansi::red) + "==> " +
              std::string(consts::ansi::reset) + "Project not found\n");
}
