#pragma once

#include "bee/to_string.hpp"
#include "serializer.hpp"
#include "value.hpp"

namespace yasf {

struct Cof {
 public:
  static bee::OrError<Value::ptr> raw_parse_string(const std::string& content);

  static bee::OrError<Value::ptr> raw_parse_file(const std::string& filename);

  static bee::OrError<bee::Unit> raw_to_file(
    const std::string& filename, const Value::ptr& value);

  static std::string to_string(const Value::ptr& value);

  template <class T, class... Args>
  static bee::OrError<T> deserialize(const std::string& content, Args&&... args)
  {
    bail(parsed, raw_parse_string(content));
    return des<T>(parsed, std::forward<Args>(args)...);
  }

  template <class T, class... Args>
  static bee::OrError<T> deserialize_file(
    const std::string& filename, Args&&... args)
  {
    bail(parsed, raw_parse_file(filename));
    return des<T>(parsed, std::forward<Args>(args)...);
  }

  template <class T> static std::string serialize(const T& content)
  {
    return to_string(ser<T>(content));
  }

  template <class T>
  static bee::OrError<bee::Unit> serialize_file(
    const std::string& filename, const T& content)
  {
    return raw_to_file(filename, ser<T>(content));
  }
};

} // namespace yasf

namespace bee {
template <> struct to_string<yasf::Value::ptr> {
  static std::string convert(const yasf::Value::ptr& value);
};
} // namespace bee
