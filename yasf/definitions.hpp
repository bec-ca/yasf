#pragma once

#include <string>
#include <vector>

#include "type.hpp"

namespace yasf {

struct Definitions {
  std::vector<CustomType::ptr> types;

  std::string gen_decl(
    const std::string& base_name, bool ignore_olint_ns) const;
  std::string gen_impl(
    const std::string& ns,
    const std::string& base_name,
    bool ignore_olint_ns) const;
};

} // namespace yasf
