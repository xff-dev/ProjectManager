#include "Console.hpp"
#include "consts.hpp"
#include <ostream>
#include <string_view>

Console::Console(std::ostream &output) : output(output) {}

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
