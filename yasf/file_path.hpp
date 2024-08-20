#pragma once

#include "serializer.hpp"

#include "bee/file_path.hpp"

namespace yasf {

using FilePath = bee::FilePath;

template <> struct Serialize<FilePath> {
  static Value::ptr serialize(const FilePath& value);
  static bee::OrError<FilePath> deserialize(const Value::ptr& value);
};

} // namespace yasf
