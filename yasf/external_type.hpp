#pragma once

#include "type.hpp"

namespace yasf {

struct GenericExternalType {
 public:
  constexpr GenericExternalType(
    const char* name, std::optional<const char*> header)
      : _name(name), _header(header)
  {}

  operator Type::ptr() const;

 private:
  const char* _name;
  std::optional<const char*> _header;
};

namespace types {

constexpr GenericExternalType ext(
  const char* name, std::optional<const char*> header)
{
  return GenericExternalType(name, header);
}

} // namespace types

} // namespace yasf
