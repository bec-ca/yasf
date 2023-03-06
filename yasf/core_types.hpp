#pragma once

#include "external_type.hpp"

namespace yasf {

namespace types {

constexpr auto Time = ext("bee::Time", "bee/time.hpp");
constexpr auto Span = ext("bee::Span", "bee/span.hpp");
constexpr auto yasf_value = ext("yasf::Value::ptr", "yasf/value.hpp");

} // namespace types

} // namespace yasf
