#pragma once

#include "bee/error.hpp"
#include "bee/file_path.hpp"

namespace yasf {

struct ClangFormat {
 public:
  static bee::OrError<> format_file(const bee::FilePath& file);
};

} // namespace yasf
