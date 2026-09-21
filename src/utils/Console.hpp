#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <string_view>

class Console {
public:
  explicit Console(std::ostream &output, std::istream &input);

  void step(std::string_view message);
  void warn(std::string_view message);
  void success(std::string_view message);
  void error(std::string_view message);

  std::string ask(std::string_view question);
  bool confirm(std::string_view question, bool defaultValue);

  template <typename T> Console &operator<<(const T &value) {
    output << value;
    return *this;
  }

  Console &operator<<(std::ostream &(*manip)(std::ostream &)) {
    manip(output);
    return *this;
  }

private:
  void print(std::string_view message, std::string_view ansi_color);

private:
  std::ostream &output;
  std::istream &input;
};
