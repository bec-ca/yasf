#pragma once

#include "type.hpp"

namespace yasf {

struct SimpleType {
 public:
  enum class Kind {
    Int,
    Float,
    Bool,
    Str,
  };

  constexpr SimpleType(Kind k) : _kind(k) {}

  operator Type::ptr() const;

 private:
  const Kind _kind;
};

namespace types {

constexpr SimpleType int_type(SimpleType::Kind::Int);
constexpr SimpleType float_type(SimpleType::Kind::Float);
constexpr SimpleType bool_type(SimpleType::Kind::Bool);
constexpr SimpleType str(SimpleType::Kind::Str);

} // namespace types

} // namespace yasf
