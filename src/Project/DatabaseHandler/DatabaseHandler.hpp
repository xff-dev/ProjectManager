#pragma once

#include "../Project.hpp"
#include <filesystem>
#include <vector>

class DatabaseHandler {
public:
  virtual ~DatabaseHandler() = default;
  DatabaseHandler(std::filesystem::path databasePath)
      : databasePath(databasePath) {};

  virtual std::vector<Project> load() = 0;
  virtual void save(std::vector<Project> &projects) = 0;

protected:
  std::filesystem::path databasePath;
};

class LegacyDatabase : public DatabaseHandler {
public:
  LegacyDatabase(std::filesystem::path databasePath)
      : DatabaseHandler(databasePath) {};

  std::vector<Project> load();
  void save(std::vector<Project> &projects);
};
