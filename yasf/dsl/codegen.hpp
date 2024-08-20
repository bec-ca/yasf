#pragma once

#include <string>

#include "code.hpp"

#include "bee/file_path.hpp"
#include "bee/or_error.hpp"

namespace yasf::dsl {

struct GeneratedFile {
  bee::FilePath path;
  std::string content;
  std::string to_string() const;
};

struct GeneratedCode {
  GeneratedFile header;
  GeneratedFile src;

  std::string to_string() const;
};

struct Codegen {
  static bee::OrError<GeneratedCode> gen(const Code& code);
};

} // namespace yasf::dsl
