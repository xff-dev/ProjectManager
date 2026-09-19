#include "Registry.hpp"
#include "../utils/consts.hpp"
#include "DatabaseHandler/DatabaseHandler.hpp"
#include "Project.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

Registry::Registry(std::unique_ptr<DatabaseHandler> databaseHandler)
    : databaseHandler(std::move(databaseHandler)) {}

Project &Registry::find(const std::string &name) {
  for (auto &project : projects) {
    if (project.name == name) {
      return project;
    }
  }
  throw std::runtime_error(std::string(consts::errors::ProjectNotFound));
}

Project &Registry::findByPath(const std::filesystem::path &path) {
  for (auto &project : projects) {
    if (std::filesystem::weakly_canonical(path) ==
        std::filesystem::weakly_canonical(project.path)) {
      return project;
    }
  }
  throw std::runtime_error(std::string(consts::errors::ProjectNotFound));
}

void Registry::load() { projects = databaseHandler->load(); }

void Registry::save() { databaseHandler->save(projects); }

void Registry::remove(const std::string &name) {
  auto it =
      std::find_if(projects.begin(), projects.end(),
                   [&](Project &project) { return project.name == name; });

  if (it == projects.end()) {
    throw std::runtime_error(std::string(consts::errors::ProjectNotFound));
  }

  projects.erase(it);
  save();
}

void Registry::add(Project &project) {
  auto it = std::find_if(projects.begin(), projects.end(),
                         [&](Project &pr) { return pr.name == project.name; });

  if (it != projects.end()) {
    throw std::runtime_error(std::string(consts::errors::ProjectAlreadyExists));
  }

  projects.push_back(project);
  save();
}

const std::vector<Project> &Registry::getProjects() { return projects; }
