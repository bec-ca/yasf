#pragma once

#include <array>
#include <vector>

#include "type.hpp"

namespace yasf {

namespace details {

CustomType::ptr make_enum_type(
  const std::string& name, const std::vector<std::string>& values);

}

template <size_t K> struct EnumType {
 public:
  constexpr EnumType(const char* name, std::array<const char*, K> legs)
      : _name(name), _legs(legs)
  {}

  operator CustomType::ptr() const
  {
    return details::make_enum_type(_name, {_legs.begin(), _legs.end()});
  }

  operator Type::ptr() const { return static_cast<CustomType::ptr>(*this); }

 private:
  const char* _name;
  std::array<const char*, K> _legs;
};

namespace types {

template <class... K>
constexpr std::array<const char*, sizeof...(K)> enum_legs(K... legs)
{
  return {legs...};
}

template <size_t K>
constexpr EnumType<K> enum_type(
  const char* name, std::array<const char*, K> legs)
{
  return {name, legs};
}

} // namespace types

} // namespace yasf
