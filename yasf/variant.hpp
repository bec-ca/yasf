#pragma once

#include "type.hpp"

#include "bee/util.hpp"

#include <string>

namespace yasf {

struct VariantLeg {
  const std::string name;
  const Type::ptr type;
};

namespace details {

CustomType::ptr make_variant(const char* name, std::vector<VariantLeg>&& legs);

} // namespace details

template <class T> struct GenericVariantLeg {
  constexpr GenericVariantLeg(const char* name, const T type)
      : name(name), type(type)
  {}

  operator VariantLeg() const
  {
    return VariantLeg{
      .name = name,
      .type = type,
    };
  }

  const char* name;
  const T type;
};

template <class... T> struct GenericVariantType {
  constexpr GenericVariantType(
    const char* name, const std::tuple<GenericVariantLeg<T>...> legs)
      : _name(name), _legs(legs)
  {}

  operator Type::ptr() const { return static_cast<CustomType::ptr>(*this); }

  operator CustomType::ptr() const
  {
    return details::make_variant(
      _name, bee::tuple_to_vector<VariantLeg>(_legs));
  }

 private:
  const char* _name;
  const std::tuple<GenericVariantLeg<T>...> _legs;
};

namespace types {

template <class T>
constexpr GenericVariantLeg<T> vleg(const char* name, T leg_type)
{
  return {name, leg_type};
}

template <class... T>
constexpr std::tuple<GenericVariantLeg<T>...> vlegs(GenericVariantLeg<T>... leg)
{
  return {leg...};
}

template <class... T>
constexpr GenericVariantType<T...> variant(
  const char* name, std::tuple<GenericVariantLeg<T>...> legs)
{
  return {name, legs};
}

} // namespace types

} // namespace yasf
