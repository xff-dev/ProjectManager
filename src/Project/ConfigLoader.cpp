#include "ConfigLoader.hpp"
#include "../utils/consts.hpp"
#include "inipp.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

ProjectConfig ConfigLoader::load(const Project &project) {
  return load(project.path / "project.ini");
}

ProjectConfig ConfigLoader::load(const std::filesystem::path filename) {
  ProjectConfig config;

  std::ifstream file(filename);
  if (file.is_open()) {
    std::string terminalCommand;
    inipp::Ini<char> ini;
    ini.parse(file);

    // data
    inipp::get_value(ini.sections["data"], "editorCommand",
                     config.editorCommand);
    inipp::get_value(ini.sections["data"], "lang", config.lang);
    inipp::get_value(ini.sections["data"], "envCommand", config.envCommand);
    inipp::get_value(ini.sections["data"], "terminalCommand", terminalCommand);

    // build
    inipp::get_value(ini.sections["build"], "directory",
                     config.build.directory);
    inipp::get_value(ini.sections["build"], "command",
                     config.build.buildCommand);
    inipp::get_value(ini.sections["build"], "prepare",
                     config.build.prepareCommand);

    // run
    inipp::get_value(ini.sections["run"], "command", config.run.command);
    inipp::get_value(ini.sections["run"], "directory", config.run.directory);

    if (terminalCommand != "") {
      config.terminalCommands.push_back(terminalCommand);
    }

    parseScripts(config, ini);
    parseTerminals(config, ini);
  } else {
    throw std::runtime_error(
        std::string(consts::errors::ProjectConfigNotFound));
  }

  return config;
}

void ConfigLoader::parseTerminals(ProjectConfig &config,
                                  inipp::Ini<char> &ini) {
  for (auto &section : ini.sections) {
    if (section.first.rfind("terminal", 0) == 0) {
      std::string command = section.second["command"];
      config.terminalCommands.push_back(command);
    }
  }
}

void ConfigLoader::parseScripts(ProjectConfig &config, inipp::Ini<char> &ini) {
  for (auto &section : ini.sections) {
    std::string prefix = "script_";
    if (section.first.rfind(prefix, 0) == 0) {
      std::string name = section.first.substr(prefix.size());

      config.scripts[name].name = name;

      inipp::get_value(section.second, "directory",
                       config.scripts[name].directory);

      inipp::get_value(section.second, "command", config.scripts[name].command);
    }
  }
}
