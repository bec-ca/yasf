#pragma once

#include "bee/file_path.hpp"
#include "bee/or_error.hpp"

namespace yasf::dsl {

struct Formatter {
 public:
  static bee::OrError<> format(const bee::FilePath& src);
};

} // namespace yasf::dsl
