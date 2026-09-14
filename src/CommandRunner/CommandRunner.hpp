#pragma once
#include "../utils/Console.hpp"
#include "Command.hpp"
#include "TerminalLauncher.hpp"
#include <vector>

class CommandRunner {
public:
  CommandRunner(Console &console, TerminalLauncher &launcher);

  int run(Command command);
  int launchTerminal(Command command);

  ~CommandRunner();

private:
  int runCommand(Command command);
  void printCommand(Command command);

private:
  bool debugCommands = false;
  std::vector<int> backgroundJobs;
  Console &console;
  TerminalLauncher &launcher;
};
