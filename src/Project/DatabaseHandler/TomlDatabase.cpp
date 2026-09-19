#include "../../utils/consts.hpp"
#include "DatabaseHandler.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <toml++/impl/forward_declarations.hpp>
#include <toml++/impl/parse_error.hpp>
#include <toml++/toml.hpp>
#include <vector>

std::vector<Project> TomlDatabase::load() {
  std::vector<Project> projects;

  toml::parse_result config;

  if (!std::filesystem::exists(databasePath))
    return projects;

  try {
    config = toml::parse_file(databasePath.string());
  } catch (const toml::parse_error error) {
    throw std::runtime_error(std::string(consts::errors::DatabaseParseError));
  }

  auto arr = config["projects"].as_array();

  if (!arr)
    return projects;

  for (const auto &project : *arr) {
    auto table = *project.as_table();

    auto name = table["name"];
    auto path = table["path"];

    if (!name.is_string() || !path.is_string() ||
        name.value<std::string>()->empty() ||
        path.value<std::string>()->empty()) {
      throw std::runtime_error(
          std::string(consts::errors::MalformedDatabaseFile));
    }

    Project p{
        .path = path.value_or(""),
        .name = name.value_or(""),
    };

    projects.push_back(p);
  }

  return projects;
}
void TomlDatabase::save(std::vector<Project> &projects) {
  toml::array array;

  for (const auto &project : projects) {
    array.push_back(toml::table{
        {"name", project.name},
        {"path", project.path.string()},
    });
  }

  toml::table config;
  config.insert("projects", array);

  std::ofstream file(databasePath);
  file << config;
}
