#include "../TerminalLauncher.hpp"
#include <fcntl.h>
#include <optional>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int KittyLauncher::launch(const Command &command) {
  pid_t pid = fork();

  if (pid == 0) {
    int devNull = open("/dev/null", O_WRONLY);
    if (devNull != -1) {
      dup2(devNull, STDOUT_FILENO);
      dup2(devNull, STDERR_FILENO);
      close(devNull);
    }

    std::string workdir = ".";
    if (command.workingDirectory.has_value()) {
      workdir = *command.workingDirectory;
    }

    std::string envCommand = "";
    if (command.envCommand.has_value()) {
      envCommand = *command.envCommand;
    }

    std::string workingDirArg = "-d=" + workdir;
    std::string zshPayload = envCommand + "; " + command.command + "; zsh";

    char *const args[] = {const_cast<char *>("kitty"),
                          const_cast<char *>(workingDirArg.c_str()),
                          const_cast<char *>("zsh"),
                          const_cast<char *>("-i"),
                          const_cast<char *>("-c"),
                          const_cast<char *>(zshPayload.c_str()),
                          nullptr};

    execvp(args[0], args);
    _exit(127);
  }

  return pid;
}
