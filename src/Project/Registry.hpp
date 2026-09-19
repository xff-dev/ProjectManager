#pragma once

#include "DatabaseHandler/DatabaseHandler.hpp"
#include "Project.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class Registry {
public:
  Registry(std::unique_ptr<DatabaseHandler> databaseHandler);

  Project &find(const std::string &name);
  Project &findByPath(const std::filesystem::path &path);

  void add(Project &project);
  void remove(const std::string &name);

  void load();
  void save();

  const std::vector<Project> &getProjects();

private:
  std::vector<Project> projects;

  std::unique_ptr<DatabaseHandler> databaseHandler;
};
