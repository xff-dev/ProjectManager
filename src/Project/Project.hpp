#pragma once
#include <filesystem>
#include <string>

struct Project {
  std::filesystem::path path;
  std::string name;
};
