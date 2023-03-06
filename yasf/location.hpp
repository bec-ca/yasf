#pragma once

#include <string>

namespace yasf {

struct Location {
  int line;
  int col;
  std::string filename;

  std::string hum() const;

  std::string to_string() const;
};

} // namespace yasf
