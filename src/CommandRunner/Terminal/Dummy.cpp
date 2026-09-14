#include "../TerminalLauncher.hpp"
#include <fcntl.h>
#include <optional>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class Dummy final : public TerminalLauncher {
public:
  int launch(const Command &command) { return -1; }
};
