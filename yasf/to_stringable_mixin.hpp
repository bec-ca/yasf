#pragma once

#include "value.hpp"

namespace yasf {

struct DetailToStringableMixin {
  static bee::OrError<Value::ptr> cof_to_yasf_value(const std::string& str);
  static std::string yasf_value_to_cof(const Value::ptr& value);
};

template <class T> struct ToStringableMixin {
 public:
  std::string to_string() const
  {
    return DetailToStringableMixin::yasf_value_to_cof(parent().to_yasf_value());
  }

  static bee::OrError<T> of_string(const std::string& value)
  {
    return DetailToStringableMixin::cof_to_yasf_value(value).bind(
      T::of_yasf_value);
  }

 private:
  const T& parent() const { return static_cast<const T&>(*this); }
};

} // namespace yasf
