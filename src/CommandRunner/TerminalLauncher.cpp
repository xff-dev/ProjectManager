#include "TerminalLauncher.hpp"
#include <algorithm>
#include <memory>
#include <string>

std::unique_ptr<TerminalLauncher> getTerminalLauncher(std::string name) {
  std::unique_ptr<TerminalLauncher> launcher = std::make_unique<Dummy>();

  std::string term_lower(name.size(), ' ');

  std::transform(name.begin(), name.end(), term_lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (term_lower == "ghostty" || term_lower == "xterm-ghostty")
    launcher = std::make_unique<GhosttyLauncher>();
  if (term_lower == "kitty" || term_lower == "xterm-kitty")
    launcher = std::make_unique<KittyLauncher>();

  return launcher;
}
