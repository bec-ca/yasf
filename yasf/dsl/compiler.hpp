#pragma once

#include "bee/file_path.hpp"
#include "bee/or_error.hpp"

namespace yasf::dsl {

struct Compiler {
 public:
  static bee::OrError<> compile(const bee::FilePath& src);
};

} // namespace yasf::dsl
