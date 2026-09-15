#include "../../src/CommandRunner/TerminalLauncher.hpp"
#include "catch2/catch_test_macros.hpp"

#include <memory>

namespace {

template <class Launcher>
bool isLauncher(const std::unique_ptr<TerminalLauncher> &launcher) {
  return dynamic_cast<Launcher *>(launcher.get()) != nullptr;
}

} // namespace

TEST_CASE("getTerminalLauncher maps known terminal names",
          "[TerminalLauncher]") {
  REQUIRE(isLauncher<GhosttyLauncher>(getTerminalLauncher("ghostty")));
  REQUIRE(isLauncher<GhosttyLauncher>(getTerminalLauncher("xterm-ghostty")));
  REQUIRE(isLauncher<KittyLauncher>(getTerminalLauncher("kitty")));
  REQUIRE(isLauncher<KittyLauncher>(getTerminalLauncher("xterm-kitty")));
}

TEST_CASE("getTerminalLauncher falls back to Dummy for unknown names",
          "[TerminalLauncher]") {
  REQUIRE(isLauncher<Dummy>(getTerminalLauncher("gnome-terminal")));
  REQUIRE(isLauncher<Dummy>(getTerminalLauncher("")));
}

TEST_CASE("getTerminalLauncher matches names case-insensitively",
          "[TerminalLauncher]") {
  REQUIRE(isLauncher<GhosttyLauncher>(getTerminalLauncher("Ghostty")));
  REQUIRE(isLauncher<GhosttyLauncher>(getTerminalLauncher("XTERM-GHOSTTY")));
  REQUIRE(isLauncher<KittyLauncher>(getTerminalLauncher("KITTY")));
  REQUIRE(isLauncher<Dummy>(getTerminalLauncher("Unknown")));
}

TEST_CASE("getTerminalLauncher always returns a usable launcher",
          "[TerminalLauncher]") {
  REQUIRE(getTerminalLauncher("ghostty") != nullptr);
  REQUIRE(getTerminalLauncher("unknown") != nullptr);
}
