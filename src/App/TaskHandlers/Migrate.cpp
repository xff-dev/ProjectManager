#include "../../Project/DatabaseHandler/DatabaseHandler.hpp"
#include "../App.hpp"

void App::handleTask(MigrateTask &task) {
  TomlDatabase tomlDatabase(task.to);
  LegacyDatabase legacyDatabase(task.from);

  auto projects = legacyDatabase.load();
  tomlDatabase.save(projects);
}
