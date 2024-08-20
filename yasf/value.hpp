#pragma once

#include <concepts>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "location.hpp"

namespace yasf {

struct Value {
 public:
  using ptr = std::shared_ptr<const Value>;

  struct Atom {
    const std::string value;
  };

  struct List {
    const std::vector<ptr> values;
  };

  struct KeyValue {
    const std::string key;
    const ptr value;
  };

  template <
    std::convertible_to<std::string> T,
    std::convertible_to<std::optional<Location>> U>
  Value(T&& value, U&& loc)
      : _variant(Atom{std::forward<T>(value)}), _loc(std::forward<U>(loc))
  {}

  template <
    std::convertible_to<std::vector<ptr>> T,
    std::convertible_to<std::optional<Location>> U>
  Value(T&& values, U&& loc)
      : _variant(List{std::forward<T>(values)}), _loc(std::forward<U>(loc))
  {}

  template <
    std::convertible_to<std::string> T,
    std::convertible_to<std::optional<Location>> U>
  Value(T&& key, const ptr& value, U&& loc)
      : _variant(KeyValue{std::forward<T>(key), value}),
        _loc(std::forward<U>(loc))
  {}

  Value(Value&& other) noexcept = default;

  Value(const Value& other) = delete;
  Value& operator=(const Value& other) = delete;

  bool is_list() const;
  bool is_atom() const;
  bool is_key_value() const;

  const std::vector<ptr>& list() const;
  const std::string& atom() const;
  const KeyValue& key_value() const;

  std::string to_string_hum() const;

  const std::optional<Location>& location() const;

  ptr find_key_in_list(const std::string& name) const;

  template <
    std::convertible_to<std::string> T,
    std::convertible_to<std::optional<Location>> U>
  static ptr create_atom(T&& value, U&& loc)
  {
    return std::make_shared<Value>(
      std::forward<T>(value), std::forward<U>(loc));
  }

  template <
    std::convertible_to<std::vector<ptr>> T,
    std::convertible_to<std::optional<Location>> U>
  static ptr create_list(T&& values, U&& loc)
  {
    return make_shared<Value>(std::forward<T>(values), std::forward<U>(loc));
  }

  template <
    std::convertible_to<std::string> T,
    std::convertible_to<std::optional<Location>> U>
  static ptr create_key_value(T&& key, const ptr& value, U&& loc)
  {
    return make_shared<Value>(
      std::forward<T>(key), value, std::forward<U>(loc));
  }

  static_assert(std::is_copy_constructible_v<Location>);
  static_assert(std::is_move_assignable_v<Location>);

 private:
  std::variant<Atom, List, KeyValue> _variant;
  const std::optional<Location> _loc;
};

} // namespace yasf
