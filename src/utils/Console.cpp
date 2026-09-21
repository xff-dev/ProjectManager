#include "Console.hpp"
#include "consts.hpp"
#include <ostream>
#include <string>
#include <string_view>

Console::Console(std::ostream &output, std::istream &input)
    : output(output), input(input) {}

void Console::print(std::string_view message, std::string_view ansi_color) {
  output << ansi_color << "==> " << consts::ansi::reset << message << std::endl;
}

void Console::step(std::string_view message) {
  print(message, consts::ansi::blue);
}

void Console::warn(std::string_view message) {
  print(message, consts::ansi::yellow);
}

void Console::success(std::string_view message) {
  print(message, consts::ansi::green);
}

void Console::error(std::string_view message) {
  print(message, consts::ansi::red);
}

std::string Console::ask(std::string_view question) {
  output << consts::ansi::cyan << "? " << consts::ansi::reset << question
         << ": ";

  std::string answer;
  std::getline(input, answer);

  return answer;
}

bool Console::confirm(std::string_view question, bool defaultValue) {
  output << consts::ansi::cyan << "? " << consts::ansi::reset << question
         << (defaultValue ? "[Y/n]" : "[y/N]") << " ";

  std::string answer;
  std::getline(input, answer);

  if (answer.empty())
    return defaultValue;

  if (answer == "y" || answer == "Y" || answer == "yes" || answer == "Yes") {
    return 1;
  } else if (answer == "n" || answer == "N" || answer == "no" ||
             answer == "No") {
    return 0;
  } else {
    output << consts::warnings::InvalidConfirmationInput << std::endl;
    return confirm(question, defaultValue);
  }
}
