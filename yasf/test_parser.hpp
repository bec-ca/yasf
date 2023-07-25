#pragma once

#include <set>
#include <string>
#include <variant>
#include <vector>

#include "bee/error.hpp"
#include "yasf/serializer.hpp"
#include "yasf/to_stringable_mixin.hpp"

namespace test_parser {

struct int_or_str : public yasf::ToStringableMixin<int_or_str> {
  using value_type = std::variant<int64_t, std::string>;

  value_type value;

  int_or_str() noexcept = default;
  int_or_str(const value_type& value) noexcept;
  int_or_str(value_type&& value) noexcept;

  template <std::convertible_to<value_type> U>
  int_or_str(U&& value) noexcept : value(std::forward<U>(value))
  {}

  static bee::OrError<int_or_str> of_yasf_value(
    const yasf::Value::ptr& config_value);

  template <class F> auto visit(F&& f) const
  {
    return std::visit(std::forward<F>(f), value);
  }
  template <class F> auto visit(F&& f)
  {
    return std::visit(std::forward<F>(f), value);
  }
  yasf::Value::ptr to_yasf_value() const;
};

struct Color {
  enum Value {
    red,
    blue,
    green,
  };
  constexpr Color(Value v) : _value(v){};
  constexpr operator Value() const { return _value; };
  explicit operator bool() const = delete;
  yasf::Value::ptr to_yasf_value() const;
  static bee::OrError<Color> of_yasf_value(
    const yasf::Value::ptr& config_value);

  const char* to_string() const;

 private:
  Value _value;
};

struct foo : public yasf::ToStringableMixin<foo> {
  std::string bar;
  std::vector<std::string> eggs;
  bool bool_field = false;
  int64_t int_field = 0;
  std::optional<int64_t> opt_field = std::nullopt;
  std::vector<std::string> opt_vec_field = {};
  double float_field = 0;
  std::vector<int64_t> vector_int_field;
  int_or_str variant;
  std::optional<Color> color = std::nullopt;
  std::optional<yasf::Location> location;

  static bee::OrError<foo> of_yasf_value(const yasf::Value::ptr& config_value);

  yasf::Value::ptr to_yasf_value() const;
};

struct top : public yasf::ToStringableMixin<top> {
  std::vector<foo> foos = {};

  static bee::OrError<top> of_yasf_value(const yasf::Value::ptr& config_value);

  yasf::Value::ptr to_yasf_value() const;
};

} // namespace test_parser

// olint-allow: missing-package-namespace
