#include "utils.hpp"
#include "consts.hpp"
#include <string>

std::string_view getLanguageIcon(std::string_view name) {
  std::string lower;
  lower.reserve(name.size());

  for (unsigned char ch : name) {
    lower.push_back(static_cast<char>(std::tolower(ch)));
  }

  while (!lower.empty() &&
         std::isspace(static_cast<unsigned char>(lower.front()))) {
    lower.erase(lower.begin());
  }
  while (!lower.empty() &&
         std::isspace(static_cast<unsigned char>(lower.back()))) {
    lower.pop_back();
  }

  if (lower == "c")
    return consts::icons::c;
  if (lower == "cpp" || lower == "c++" || lower == "cc" || lower == "cxx")
    return consts::icons::cpp;
  if (lower == "csharp" || lower == "c#" || lower == "cs")
    return consts::icons::csharp;
  if (lower == "python" || lower == "py" || lower == "py3" ||
      lower == "python3")
    return consts::icons::python;
  if (lower == "java" || lower == "jar")
    return consts::icons::java;
  if (lower == "javascript" || lower == "js" || lower == "jsx")
    return consts::icons::javascript;
  if (lower == "typescript" || lower == "ts" || lower == "tsx")
    return consts::icons::typescript;
  if (lower == "rust" || lower == "rs")
    return consts::icons::rust;
  if (lower == "go" || lower == "golang")
    return consts::icons::go;
  if (lower == "php" || lower == "php7" || lower == "php8")
    return consts::icons::php;
  if (lower == "ruby" || lower == "rb")
    return consts::icons::ruby;
  if (lower == "swift")
    return consts::icons::swift;
  if (lower == "kotlin" || lower == "kt")
    return consts::icons::kotlin;
  if (lower == "lua" || lower == "luau")
    return consts::icons::lua;
  if (lower == "zig")
    return consts::icons::zig;
  if (lower == "nim" || lower == "nimble")
    return consts::icons::nim;
  if (lower == "html" || lower == "htm" || lower == "xhtml")
    return consts::icons::html;
  if (lower == "css" || lower == "scss" || lower == "sass")
    return consts::icons::css;
  if (lower == "sql" || lower == "mysql" || lower == "postgresql")
    return consts::icons::sql;
  if (lower == "bash" || lower == "sh" || lower == "shell" || lower == "zsh")
    return consts::icons::bash;

  return {};
}
