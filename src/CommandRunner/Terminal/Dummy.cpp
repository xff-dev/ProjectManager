#include "../TerminalLauncher.hpp"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int Dummy::launch(const Command &command) { return 0; }
