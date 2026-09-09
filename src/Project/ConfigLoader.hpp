#pragma once

#include "Project.hpp"
#include "inipp.h"
#include <filesystem>
#include <map>
#include <vector>

struct ProjectBuild {
  std::string directory = ".";
  std::string prepareCommand;
  std::string buildCommand;
};

struct ProjectScript {
  std::string name;
  std::string command;
  std::string directory = ".";
};

struct ProjectRun {
  std::string command;
  std::string directory = ".";
};

struct ProjectConfig {
  std::string editorCommand = "nvim .";
  std::string lang = "unknown";
  std::string envCommand;
  std::vector<std::string> terminalCommands;
  ProjectBuild build;
  ProjectRun run;
  std::map<std::string, ProjectScript> scripts;
};

class ConfigLoader {
public:
  ProjectConfig load(const Project &project);
  ProjectConfig load(const std::filesystem::path filename);

private:
  void parseScripts(ProjectConfig &config, inipp::Ini<char> &ini);
  void parseTerminals(ProjectConfig &config, inipp::Ini<char> &ini);
};
