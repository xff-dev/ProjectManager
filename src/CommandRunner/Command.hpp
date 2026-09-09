#pragma once

#include <filesystem>
#include <optional>
#include <string>

struct Command {
  std::string command;
  std::optional<std::filesystem::path> workingDirectory;
  std::optional<std::string> envCommand;

  bool isBackground = false;
};
