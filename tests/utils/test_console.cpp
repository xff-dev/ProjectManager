#include "../../src/utils/Console.hpp"
#include "../../src/utils/consts.hpp"
#include "catch2/catch_test_macros.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {

bool contains(const std::string &haystack, std::string_view needle) {
  return haystack.find(needle) != std::string::npos;
}

std::size_t count(const std::string &haystack, std::string_view needle) {
  std::size_t occurrences = 0;
  std::size_t from = 0;
  while ((from = haystack.find(needle, from)) != std::string::npos) {
    ++occurrences;
    from += needle.size();
  }
  return occurrences;
}

} // namespace

TEST_CASE("Console status messages contain the sent message", "[Console]") {
  std::ostringstream output;
  Console console(output, std::cin);

  console.step("running command");
  console.warn("Process exited with non-zero status code: 7");
  console.success("Build successful");
  console.error("Project not found");

  const std::string rendered = output.str();
  REQUIRE(contains(rendered, "running command"));
  REQUIRE(contains(rendered, "Process exited with non-zero status code: 7"));
  REQUIRE(contains(rendered, "Build successful"));
  REQUIRE(contains(rendered, "Project not found"));
}

// ---------------------------------------------------------------------------
// ask
// ---------------------------------------------------------------------------

TEST_CASE("ask prints the question and returns the typed answer",
          "[Console][ask]") {
  std::ostringstream output;
  std::istringstream input("my answer\n");
  Console console(output, input);

  std::string answer = console.ask("project name");

  REQUIRE(answer == "my answer");
  REQUIRE(contains(output.str(), "project name"));
}

TEST_CASE("ask returns an empty string on EOF", "[Console][ask]") {
  std::ostringstream output;
  std::istringstream input("");
  Console console(output, input);

  std::string answer = console.ask("prompt");

  REQUIRE(answer.empty());
}

TEST_CASE("ask returns an empty string for a blank line", "[Console][ask]") {
  std::ostringstream output;
  std::istringstream input("\n");
  Console console(output, input);

  std::string answer = console.ask("prompt");

  REQUIRE(answer.empty());
}

TEST_CASE("ask preserves surrounding whitespace in the answer",
          "[Console][ask]") {
  std::ostringstream output;
  std::istringstream input("  padded answer  \n");
  Console console(output, input);

  std::string answer = console.ask("prompt");

  REQUIRE(answer == "  padded answer  ");
}

TEST_CASE("ask keeps the part of the answer before the first newline",
          "[Console][ask]") {
  std::ostringstream output;
  std::istringstream input("first line\nsecond line\n");
  Console console(output, input);

  std::string answer = console.ask("prompt");

  REQUIRE(answer == "first line");
}

TEST_CASE("ask reads sequential answers from the same input stream",
          "[Console][ask]") {
  std::ostringstream output;
  std::istringstream input("one\ntwo\n");
  Console console(output, input);

  std::string first = console.ask("first?");
  std::string second = console.ask("second?");

  REQUIRE(first == "one");
  REQUIRE(second == "two");
  REQUIRE(count(output.str(), "first?") == 1);
  REQUIRE(count(output.str(), "second?") == 1);
}

// ---------------------------------------------------------------------------
// confirm
// ---------------------------------------------------------------------------

TEST_CASE("confirm returns the default on an empty answer", "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input("\n");
  Console console(output, input);

  REQUIRE(console.confirm("Really?", true));
  REQUIRE(contains(output.str(), "Really?"));
}

TEST_CASE("confirm returns the negative default on an empty answer",
          "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input("\n");
  Console console(output, input);

  REQUIRE_FALSE(console.confirm("Proceed?", false));
  REQUIRE(contains(output.str(), "Proceed?"));
}

TEST_CASE("confirm returns the default on EOF", "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input("");
  Console console(output, input);

  REQUIRE(console.confirm("Really?", true));
}

TEST_CASE("confirm treats the positive variants as yes", "[Console][confirm]") {
  for (std::string_view yes : {"y", "Y", "yes", "Yes"}) {
    std::ostringstream output;
    std::istringstream input(std::string(yes) + "\n");
    Console console(output, input);

    REQUIRE(console.confirm("Proceed?", false));
    REQUIRE(contains(output.str(), "Proceed?"));
  }
}

TEST_CASE("confirm treats the negative variants as no", "[Console][confirm]") {
  for (std::string_view no : {"n", "N", "no", "No"}) {
    std::ostringstream output;
    std::istringstream input(std::string(no) + "\n");
    Console console(output, input);

    REQUIRE_FALSE(console.confirm("Proceed?", true));
  }
}

TEST_CASE("confirm warns and re-prompts on invalid input, then honors the "
          "next valid answer",
          "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input("maybe\ny\n");
  Console console(output, input);

  REQUIRE(console.confirm("Proceed?", false));
  const std::string rendered = output.str();
  REQUIRE(contains(rendered, consts::warnings::InvalidConfirmationInput));
  REQUIRE(count(rendered, "Proceed?") == 2);
}

TEST_CASE("confirm returns the default when the re-prompt is answered "
          "blank",
          "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input("maybe\n\n");
  Console console(output, input);

  REQUIRE(console.confirm("Really?", true));
  const std::string rendered = output.str();
  REQUIRE(contains(rendered, consts::warnings::InvalidConfirmationInput));
  REQUIRE(count(rendered, "Really?") == 2);
}

TEST_CASE("confirm keeps re-prompting until it gets a valid answer",
          "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input("a\nb\nc\nY\n");
  Console console(output, input);

  REQUIRE(console.confirm("Proceed?", false));
  const std::string rendered = output.str();
  REQUIRE(count(rendered, consts::warnings::InvalidConfirmationInput) == 3);
  REQUIRE(count(rendered, "Proceed?") == 4);
}

TEST_CASE("confirm rejects an invalid answer even when the default is yes",
          "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input("maybe\nn\n");
  Console console(output, input);

  REQUIRE_FALSE(console.confirm("Proceed?", true));
  const std::string rendered = output.str();
  REQUIRE(contains(rendered, consts::warnings::InvalidConfirmationInput));
  REQUIRE(count(rendered, "Proceed?") == 2);
}

TEST_CASE("confirm requires an exact answer and does not trim whitespace",
          "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input(" y \nn\n");
  Console console(output, input);

  REQUIRE_FALSE(console.confirm("Proceed?", true));
  const std::string rendered = output.str();
  REQUIRE(contains(rendered, consts::warnings::InvalidConfirmationInput));
  REQUIRE(count(rendered, "Proceed?") == 2);
}

TEST_CASE("confirm calls consume answers line by line", "[Console][confirm]") {
  std::ostringstream output;
  std::istringstream input("y\nN\nyes\n");
  Console console(output, input);

  REQUIRE(console.confirm("one?", true));
  REQUIRE_FALSE(console.confirm("two?", true));
  REQUIRE(console.confirm("three?", false));
  REQUIRE(count(output.str(), "one?") == 1);
  REQUIRE(count(output.str(), "two?") == 1);
  REQUIRE(count(output.str(), "three?") == 1);
  REQUIRE_FALSE(contains(output.str(), consts::warnings::InvalidConfirmationInput));
}