#pragma once

#include <memory>
#include <string>
#include <vector>

#include "serializer.hpp"
#include "value.hpp"

#include "bee/error.hpp"

namespace yasf {

struct ParserHelper {
  static bee::OrError<bool> to_bool(const Value::ptr& config_value);
  static Value::ptr of_bool(bool value);
  static std::optional<Value::ptr> of_bool_optional(bool value);

  static bee::OrError<int64_t> to_int(const Value::ptr& config_value);
  static Value::ptr of_int(int64_t value);
  static std::optional<Value::ptr> of_int_optional(int64_t value);

  static bee::OrError<double> to_float(const Value::ptr& config_value);
  static Value::ptr of_float(double value);
  static std::optional<Value::ptr> of_float_optional(double value);

  static void push_back_field(
    std::vector<Value::ptr>& v,
    std::optional<Value::ptr> value,
    std::string name);

  template <class T, class F>
  static std::optional<Value::ptr> of_vector_optional(
    const std::vector<T>& values, F of_t)
  {
    if (values.empty()) return std::nullopt;
    std::vector<Value::ptr> elements;
    for (const auto& value : values) { elements.push_back(of_t(value)); }
    return Value::create_list(std::move(elements), std::nullopt);
  }

  static std::string location(const Value::ptr& config_value);

  static bee::Error make_error_msg(
    const std::string& msg, const Value::ptr& value);

  template <class... Ts>
  static bee::Error err(const char* fmt, const Value::ptr& value, Ts&&... args)
  {
    return make_error_msg(F(fmt, std::forward<Ts>(args)...), value);
  }
};

} // namespace yasf
