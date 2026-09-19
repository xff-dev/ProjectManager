#include "DatabaseHandler.hpp"
#include <fstream>
#include <vector>

std::vector<Project> LegacyParser::load() {
  std::vector<Project> projects;

  std::ifstream file;
  file.open(std::filesystem::absolute(databasePath), std::ios::in);

  std::string line;

  projects.erase(projects.begin(), projects.end());

  while (std::getline(file, line)) {
    auto pos = line.find('|');

    std::string name = line.substr(0, pos);
    std::string path = line.substr(pos + 1);

    Project project = Project(path, name);

    projects.push_back(project);
  }

  return projects;
}

void LegacyParser::save(std::vector<Project> &projects) {
  std::ofstream file;
  file.open(databasePath);

  for (auto &project : projects) {
    file << project.name << "|" << project.path.string() << std::endl;
  }

  file.close();
}
