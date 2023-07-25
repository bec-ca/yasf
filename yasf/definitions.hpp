#pragma once

#include <string>
#include <vector>

#include "type.hpp"

namespace yasf {

struct Definitions {
  std::vector<CustomType::ptr> types;

  std::string gen_decl(const std::string& base_name) const;
  std::string gen_impl(const std::string& base_name) const;
};

} // namespace yasf
