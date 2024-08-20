#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "value.hpp"

#include "bee/file_path.hpp"
#include "bee/or_error.hpp"

namespace yasf {

struct ConfigParser {
  static bee::OrError<Value::ptr> parse_from_file(
    const bee::FilePath& filename);

  template <class T>
  static bee::OrError<T> parse_file(const bee::FilePath& filename)
  {
    bail(config_parsed, parse_from_file(filename));
    return des<T>(config_parsed);
  }

  static bee::OrError<Value::ptr> parse_from_string(
    const bee::FilePath& filename, const std::string& content);
};

} // namespace yasf
