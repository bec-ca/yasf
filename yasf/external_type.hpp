#pragma once

#include "type.hpp"

namespace yasf {

struct GenericExternalType {
 public:
  constexpr GenericExternalType(
    const char* name,
    std::optional<const char*> header,
    std::optional<const char*> serialize_header)
      : _name(name), _header(header), _serialize_header(serialize_header)
  {}

  operator Type::ptr() const;

 private:
  const char* _name;
  std::optional<const char*> _header;
  std::optional<const char*> _serialize_header;
};

namespace types {
constexpr GenericExternalType ext(
  const char* name,
  std::optional<const char*> header,
  std::optional<const char*> serialize_header = std::nullopt)
{
  return GenericExternalType(name, header, serialize_header);
}

} // namespace types

} // namespace yasf
