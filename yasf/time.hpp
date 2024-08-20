#pragma once

#include "serializer.hpp"

#include "bee/time.hpp"

namespace yasf {

using Time = bee::Time;

template <> struct Serialize<Time> {
  static Value::ptr serialize(const Time& value);
  static bee::OrError<Time> deserialize(const Value::ptr& value);
};

} // namespace yasf
