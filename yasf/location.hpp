#pragma once

#include <string>

#include "bee/file_path.hpp"

namespace yasf {

struct Location {
  int line;
  int col;
  bee::FilePath filename;

  std::string hum() const;

  std::string to_string() const;
};

} // namespace yasf
