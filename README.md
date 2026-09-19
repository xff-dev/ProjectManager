# project-manager

A personal project management CLI written in C++23. Tracks your local projects,
builds and runs them, opens them in your editor and terminals, and executes
project-specific scripts — all from a single command.

## Features

- **Project registry** — add, remove, list, and search projects by name or path,
  persisted as toml in `~/.project-manager/data.toml`.
- **Per-project config** — each project carries a `project.ini` describing its
  language, editor command, environment command, build/run commands, terminals,
  and named scripts.
- **Build & run** — run the configured build and run commands for the current
  project, creating the build directory and running an optional prepare step
  automatically.
- **Terminal launching** — opens configured terminals (Ghostty, Kitty) in the
  project directory with the environment command applied; implemented behind a
  small `TerminalLauncher` interface so new terminals are easy to add.
- **Scripts** — execute named scripts defined in `project.ini`
  (`script_<name>` sections).
- **Command chaining** — multiple commands in a single invocation, executed in
  order: `pm build run`.
- **Rich output** — colored output with language icons and status indicators.
- **zsh autocomplete** — ships with a completion file and installer.

## Requirements

- C++23 compiler (GCC/Clang)
- CMake >= 3.12
- make
- zsh (used by terminal launchers and autocomplete)
- One of the supported terminals: Ghostty or Kitty

## Building

```bash
cmake -S . -B build
make -C build -j$(nproc)
```

Dependencies (inipp and Catch2) are fetched automatically via CMake's
`FetchContent`.

## Usage

```
Usage: pm [command_1] [args...] [command_2] [args...] ...
```

| Command                | Description                                        |
| ---------------------- | -------------------------------------------------- |
| `build`                | Build the current project.                         |
| `run`                  | Run the current project.                           |
| `list`                 | List all projects (with language icons).           |
| `list-names`           | List just the names of all projects.               |
| `script <name>`        | Execute a named script from `project.ini`.         |
| `list-scripts`         | List the available scripts for the current project.|
| `add <name> <path>`    | Add a new project.                                 |
| `remove <name>`        | Remove a project.                                  |
| `open <name>`          | Open the project in its terminals and editor.      |
| `migrate <src> <dst>`  | Convert the legacy registry database to toml.      |
| `help`                 | Show this help.                                    |

### Examples

```bash
pm add gamepad-shell $(pwd)   # register the current directory as a project
pm list                       # list all registered projects
pm open gamepad-shell         # open it in the configured terminals and editor
pm build                      # build the current project
pm build run                  # build, then run
pm script clean build         # chain two named scripts
pm build script test          # build, then run the test script
pm remove test                # remove a project
```

### Data storage

The registry is stored as toml in `~/.project-manager/data.toml`
(`[[projects]]` entries with `name`/`path` fields). Older versions kept it in a
legacy `name|path` file at `~/.project-manager/data`; convert it once with:

```bash
pm migrate ~/.project-manager/data ~/.project-manager/data.toml
```

`migrate` reads the legacy file and writes a toml database; the source file is
left untouched. The registry reader is swappable behind a small
`DatabaseHandler` interface (legacy and toml backends).

## Configuration

Project settings live in a `project.ini` at the project root:

```ini
[data]
editorCommand = nvim .
lang = cpp
envCommand =

[terminal]
command = pm build

[run]
command = ./build/app
directory = build

[build]
prepare = cmake ..
directory = build
command = make -j12

[script_test]
command = ./build/unit_tests "~[terminal]"

[script_dump]
command = python scripts/merge.py
```

- `[data]` — `editorCommand`, `lang` (used for the icon in `pm list`), and
  `envCommand` (prepended to every command run for this project).
- Any section starting with `terminal` opens a terminal in the project
  directory on `pm open` (multiple allowed).
- `[build]` — `prepare`, `command`, and build `directory`.
- `[run]` — `command` and working `directory`.
- `script_<name>` — named scripts with an optional `directory`.

To register a project with `open`/`build`/`run`/`script`, add it first:

```bash
pm add myproj $(pwd)
```

## Testing

Build and run the test suite:

```bash
cmake -S . -B build
make -C build -j$(nproc)
./build/unit_tests "~[terminal]"
```

The default run excludes real-terminal integration tests, which spawn actual
Ghostty/Kitty windows and require a graphical session, zsh, and the terminals
to be installed. To run everything (e.g. from a desktop session):

```bash
./build/unit_tests
```

## Autocomplete

Install zsh completion:

```bash
cd zsh
./install_autocomplete.sh
```

## Project layout

```
src/
  App/            Command dispatch and task handlers (build, run, open, ...)
  CLI/            Argument parsing and task definitions
  CommandRunner/  Shell command execution and terminal launching
  Project/        Registry (DatabaseHandler backends) and project.ini loading
  utils/          Console helpers, ANSI colors, icons, constants
tests/
  ...             Catch2 test suite
zsh/              Autocomplete file and installer
```

## AI usage

This project is developed with AI assistance. See [ai.md](ai.md) for details.