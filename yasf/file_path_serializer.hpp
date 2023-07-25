#pragma once

#include "serializer.hpp"

#include "bee/file_path.hpp"

namespace yasf {

template <> struct Serialize<bee::FilePath> {
  static Value::ptr serialize(const bee::FilePath& value);
  static bee::OrError<bee::FilePath> deserialize(const Value::ptr& value);
};

} // namespace yasf
