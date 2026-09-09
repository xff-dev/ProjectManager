#pragma once
#include <string_view>

namespace consts {

namespace ansi {
inline constexpr std::string_view black = "\033[0;30m";
inline constexpr std::string_view red = "\033[0;31m";
inline constexpr std::string_view green = "\033[0;32m";
inline constexpr std::string_view yellow = "\033[0;33m";
inline constexpr std::string_view blue = "\033[0;34m";
inline constexpr std::string_view purple = "\033[0;35m";
inline constexpr std::string_view cyan = "\033[0;36m";
inline constexpr std::string_view white = "\033[0;37m";
inline constexpr std::string_view reset = "\033[0m";
} // namespace ansi

namespace icons {
inline constexpr std::string_view c = "  ";
inline constexpr std::string_view cpp = "  ";
inline constexpr std::string_view csharp = " 󰌛 ";
inline constexpr std::string_view python = "  ";
inline constexpr std::string_view java = "  ";
inline constexpr std::string_view javascript = "  ";
inline constexpr std::string_view typescript = "  ";
inline constexpr std::string_view rust = "  ";
inline constexpr std::string_view go = "  ";
inline constexpr std::string_view php = " 🐘 ";
inline constexpr std::string_view ruby = "  ";
inline constexpr std::string_view swift = "  ";
inline constexpr std::string_view kotlin = "  ";
inline constexpr std::string_view lua = "  ";
inline constexpr std::string_view zig = "  ";
inline constexpr std::string_view nim = " 👑 ";
inline constexpr std::string_view html = "  ";
inline constexpr std::string_view css = "  ";
inline constexpr std::string_view sql = "  ";
inline constexpr std::string_view bash = "  ";

inline constexpr std::string_view exists = "●";
inline constexpr std::string_view missing = "○";
} // namespace icons

namespace errors {
inline constexpr std::string_view ProjectNotFound = "Project not found";
inline constexpr std::string_view ScriptNotFound = "Script not found";
inline constexpr std::string_view ProjectConfigNotFound =
    "Project config not found";

inline constexpr std::string_view UnknownCommand =
    "Unknown command: '{}'. Run '{} help' for more information";

inline constexpr std::string_view MissingProjectName = "Missing project name";
inline constexpr std::string_view MissingProjectPath = "Missing project path";
inline constexpr std::string_view MissingScriptName = "Missing script name";

inline constexpr std::string_view BuildPreparationError =
    "Error preparing build";
inline constexpr std::string_view BuildError = "Error building";

inline constexpr std::string_view ProjectAlreadyExists =
    "Project already exists";

inline constexpr std::string_view InvalidProjectPath =
    "Project directory doesn't exist(disk not mounted?)";
} // namespace errors

namespace success {
inline constexpr std::string_view BuildSuccessful = "Build successful";
inline constexpr std::string_view RunSuccessful = "Run successful";

inline constexpr std::string_view ScriptSuccessful =
    "Script \"{}\" ran successfuly";
} // namespace success

namespace warnings {
inline constexpr std::string_view NonZeroStatusCode =
    "Process exited with non-zero status code: {}";
} // namespace warnings

inline constexpr std::string_view HelpMessage = R"(
Usage: {0} [command_1] [args...] [command_2] [args...] ...

Commands:
  build             Build the current project.
  run               Run the project.
  list              List all projects.
  list-names        List the names of all projects.
  script <name>     Execute a specified script by its name.
  list-scripts      List all available scripts.
  remove <name>     Remove a project by its name.
  add <name> <path> Add a new project with the given name and directory path.
  open <name>       Open a project by its name.

Chaining Commands:
  Multiple commands can be passed sequentially in a single invocation. 
  They will be parsed and executed in the order provided.

Examples:
  {0} build run
  {0} add my_project ./path/to/project script test_script
  {0} list-names open my_project
)";

} // namespace consts
