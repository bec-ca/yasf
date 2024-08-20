#pragma once

#include <optional>
#include <vector>

#include "code.hpp"

#include "bee/file_path.hpp"
#include "bee/or_error.hpp"

namespace yasf::dsl {

struct DslParser {
 public:
  static bee::OrError<Code> parse(
    const bee::FilePath& src_path,
    const std::optional<std::string_view>& content = std::nullopt);
};

} // namespace yasf::dsl
