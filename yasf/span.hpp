
#pragma once

#include "serializer.hpp"

#include "bee/span.hpp"

namespace yasf {

using Span = bee::Span;

template <> struct Serialize<Span> {
  static Value::ptr serialize(const Span& value);
  static bee::OrError<Span> deserialize(const Value::ptr& value);
};

} // namespace yasf
