#pragma once
#include "../utils/Console.hpp"
#include "Command.hpp"
#include <string>
#include <string_view>

class TerminalLauncher {
public:
  virtual ~TerminalLauncher() = default;

  virtual int launch(const Command &command) = 0;
};

std::unique_ptr<TerminalLauncher> getTerminalLauncher(std::string name);

class Dummy : public TerminalLauncher {
public:
  int launch(const Command &command);
};

class KittyLauncher : public TerminalLauncher {
public:
  int launch(const Command &command);
};

class GhosttyLauncher : public TerminalLauncher {
public:
  int launch(const Command &command);
};
