#include "Registry.hpp"
#include "../utils/consts.hpp"
#include "Project.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

Registry::Registry(std::filesystem::path databasePath)
    : databasePath(databasePath) {}

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

void Registry::load() {
  std::ifstream file;
  file.open(std::filesystem::absolute(databasePath), std::ios::in);

  std::string line;

  while (std::getline(file, line)) {
    auto pos = line.find('|');

    std::string name = line.substr(0, pos);
    std::string path = line.substr(pos + 1);

    Project project = Project(path, name);

    projects.push_back(project);
  }
}

void Registry::save() {
  std::ofstream file;
  file.open(databasePath);

  for (auto &project : projects) {
    file << project.name << "|" << project.path.string() << std::endl;
  }

  file.close();
}

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
