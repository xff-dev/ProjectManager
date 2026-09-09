#pragma once

#include "Project.hpp"
#include <filesystem>
#include <string>
#include <vector>

class Registry {
public:
  Registry(std::filesystem::path databasePath);

  Project &find(const std::string &name);
  Project &findByPath(const std::filesystem::path &path);

  void add(Project &project);
  void remove(const std::string &name);

  void load();
  void save();

  const std::vector<Project> &getProjects();

private:
  std::vector<Project> projects;
  std::filesystem::path databasePath;
};
