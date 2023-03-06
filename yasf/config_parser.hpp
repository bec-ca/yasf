#pragma once

#include "value.hpp"

#include "bee/error.hpp"

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace yasf {

struct ConfigParser {
  static bee::OrError<Value::ptr> parse_from_file(const std::string& filename);

  template <class T>
  static bee::OrError<T> parse_file(const std::string& filename)
  {
    bail(config_parsed, ConfigParser::parse_from_file(filename));
    return des<T>(config_parsed);
  }

  static bee::OrError<Value::ptr> parse_from_string(
    const std::string& filename, const std::string& content);
};

} // namespace yasf
