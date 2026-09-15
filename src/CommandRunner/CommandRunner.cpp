#include "CommandRunner.hpp"
#include "Command.hpp"
#include "TerminalLauncher.hpp"
#include <algorithm>
#include <fcntl.h>
#include <filesystem>
#include <optional>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>
#include <vector>

CommandRunner::CommandRunner(Console &console,
                             std::unique_ptr<TerminalLauncher> launcher)
    : console(console), launcher(std::move(launcher)) {}

CommandRunner::~CommandRunner() {
  for (pid_t pid : backgroundJobs) {
    // Проверка, жив ли ещё процесс
    if (waitpid(pid, nullptr, WNOHANG) == 0) {
      kill(pid, SIGTERM); // Мягкое завершение
    }
  }

  // Сбор статусов выходов
  for (pid_t pid : backgroundJobs) {
    int status;
    waitpid(pid, &status, 0);
  }
}

int CommandRunner::run(Command command) {
  printCommand(command);

  if (command.isBackground) {
    int pid = runCommand(command);
    backgroundJobs.push_back(pid);
    return pid;
  } else {
    return runCommand(command);
  }
}

int CommandRunner::runCommand(Command command) {
  // TODO: implement safer command execution
  // TODO: rewrite the function myself
  pid_t pid = fork();

  if (pid < 0) {
    throw std::system_error(errno, std::generic_category(),
                            "Failed to fork process");
  }

  if (pid == 0) {
    if (command.workingDirectory.has_value()) {
      std::error_code ec;
      std::filesystem::current_path(*command.workingDirectory, ec);
      if (ec) {
        _exit(127);
      }
    }
    std::string cmd;
    if (command.envCommand.has_value()) {
      cmd = std::format("{}; {}", *command.envCommand, command.command);
    } else {
      cmd = command.command;
    }

    std::vector<char *> argv = {(char *)"/bin/bash", (char *)"-c",
                                (char *)cmd.c_str(), nullptr};

    execv("/bin/bash", argv.data());

    _exit(127);
  }

  if (command.isBackground) {
    return pid;
  }

  int status = 0;

  if (waitpid(pid, &status, 0) < 0) {
    throw std::system_error(errno, std::generic_category(),
                            "Failed to wait for child process");
  }

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  if (WIFSIGNALED(status)) {
    return 128 + WTERMSIG(status);
  }

  return -1;
}

int CommandRunner::launchTerminal(Command command) {
  return launcher->launch(command);
}

void CommandRunner::printCommand(Command command) {
  std::stringstream ss;
  if (command.envCommand.has_value()) {
    ss << *command.envCommand << "; ";
  }

  ss << command.command << " ";

  console.step(ss.str());
}
