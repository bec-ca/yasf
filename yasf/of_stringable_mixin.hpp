#pragma once

#include "yasf/serializer.hpp"
#include "yasf/value.hpp"

namespace yasf {

namespace detail {

bee::OrError<std::string> string_of_yasf_value(const Value::ptr& value);

} // namespace detail

template <class T> struct OfStringableMixin {
 public:
  Value::ptr to_yasf_value() const { return ser(parent().to_string()); }

  static bee::OrError<T> of_yasf_value(const Value::ptr& value)
  {
    bail(str, detail::string_of_yasf_value(value));
    return T::of_string(std::move(str));
  }

 private:
  T& parent() { return static_cast<T&>(*this); }
  const T& parent() const { return static_cast<const T&>(*this); }
};

} // namespace yasf
