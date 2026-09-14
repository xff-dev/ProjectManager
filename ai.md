# AI usage in this project

This project is developed with help from AI coding assistants (LLMs). AI is
used in several distinct ways:

## Tests

The vast majority of the test suite (`tests/`) is written by AI, based on
discussions of the expected behaviour and edge cases. AI generated the test
cases, the test doubles (such as the `Dummy` terminal launcher), and the
integration-test helpers. The tests are manually reviewed and run in CI-like
fashion (`make` + the Catch2 suite) before changes are accepted.

## Documentation

Documentation, including the `README.md` and this file, is written by AI from
descriptions of the project, its commands, and its configuration format.

## Application code (`src/`)

The code under `src/` is written manually. AI is used as a consultant for:

- architecture questions (module boundaries, where a feature belongs,
  interface design such as the `TerminalLauncher` abstraction);
- implementation questions (idiomatic C++23 usage, standard-library/facility
  choices, error handling patterns);
- review feedback on proposed code.

AI does not generate the `src/` code wholesale; it is written and maintained by
hand, with AI acting on specific questions rather than producing the code.