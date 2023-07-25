#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "location.hpp"

#include "bee/error.hpp"

namespace yasf {

struct Value {
 public:
  using ptr = std::shared_ptr<const Value>;

  static_assert(std::is_copy_constructible_v<Location>);
  static_assert(std::is_move_assignable_v<Location>);

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

  Value(Value&& other) = default;

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

  static ptr create_atom(std::string value, std::optional<Location> loc);
  static ptr create_list(std::vector<ptr> values, std::optional<Location> loc);
  static ptr create_key_value(
    std::string key, ptr value, std::optional<Location> loc);

  Value(std::string value, std::optional<Location> loc);
  Value(std::vector<ptr> values, std::optional<Location> loc);
  Value(std::string key, ptr value, std::optional<Location> loc);

 private:
  std::variant<Atom, List, KeyValue> _variant;
  const std::optional<Location> _loc;
};

} // namespace yasf
