#pragma once
#include "../utils/Console.hpp"
#include "Command.hpp"

class TerminalLauncher {
public:
  virtual ~TerminalLauncher() = default;

  virtual int launch(const Command &command) = 0;
};
