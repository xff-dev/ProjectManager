#pragma once
#include "Command.hpp"

class TerminalLauncher {
public:
  virtual ~TerminalLauncher() = default;

  virtual int launch(const Command &command) = 0;
};

std::unique_ptr<TerminalLauncher> getTerminalLauncher(std::string_view name);

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
