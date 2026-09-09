#pragma once
#include "../utils/Console.hpp"
#include "Command.hpp"
#include <vector>

class CommandRunner {
public:
  CommandRunner(Console &console);

  int run(Command command);
  void launchTerminal(Command command);

  ~CommandRunner();

private:
  int runCommand(Command command);
  void printCommand(Command command);

private:
  bool debugCommands = false;
  std::vector<int> backgroundJobs;
  Console &console;
};
