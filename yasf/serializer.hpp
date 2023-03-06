#pragma once

#include "bee/span.hpp"
#include "bee/time.hpp"
#include "value.hpp"

#include <map>
#include <set>

namespace yasf {

template <class T> struct Serialize;

////////////////////////////////////////////////////////////////////////////////
// Helper functions
//

template <class T> Value::ptr ser(const T& value)
{
  return Serialize<std::decay_t<T>>::serialize(value);
}

template <class T, class... Args>
bee::OrError<T> des(const Value::ptr& value, Args&&... args)
{
  return Serialize<std::decay_t<T>>::deserialize(
    value, std::forward<Args>(args)...);
}

////////////////////////////////////////////////////////////////////////////////
// Value
//

template <> struct Serialize<Value::ptr> {
  static Value::ptr serialize(const Value::ptr& value) { return value; }

  static bee::OrError<Value::ptr> deserialize(const Value::ptr& value)
  {
    return value;
  }
};

////////////////////////////////////////////////////////////////////////////////
// bee::OrError
//

template <class T> struct Serialize<bee::OrError<T>> {
  static Value::ptr serialize(const bee::OrError<T>& value)
  {
    if (value.is_error()) {
      return Value::create_list(
        {Value::create_atom("<bee::Error", std::nullopt),
         Value::create_atom(value.error().msg(), std::nullopt)},
        std::nullopt);
    } else {
      return Value::create_list(
        {Value::create_atom("Ok", std::nullopt), ser(value.value())},
        std::nullopt);
    }
  }

  static bee::OrError<bee::OrError<T>> deserialize(const Value::ptr& value)
  {
    if (!value->is_list() || value->list().size() != 2) {
      return bee::Error("Expected list of size 2");
    }
    auto fst = value->list()[0];
    auto snd = value->list()[1];

    if (!fst->is_atom()) {
      return bee::Error("Expected first element to be an atom");
    }

    if (fst->atom() == "<bee::Error" && snd->is_atom()) {
      return bee::OrError<bee::OrError<T>>(bee::Error(snd->atom()));
    } else if (fst->atom() == "Ok") {
      return des<T>(snd);
    } else {
      return bee::Error("Expected first element to either bee::Error or Ok");
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
// string
//

template <> struct Serialize<std::string> {
  static Value::ptr serialize(const std::string& value);
  static bee::OrError<std::string> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// optional
//

template <class T> struct Serialize<std::optional<T>> {
  static Value::ptr serialize(const std::optional<T>& value)
  {
    if (!value.has_value()) {
      return ser<std::vector<T>>({});
    } else {
      return ser<std::vector<T>>({*value});
    }
  }

  static bee::OrError<std::optional<T>> deserialize(const Value::ptr& value)
  {
    bail(l, des<std::vector<T>>(value));
    if (l.empty()) {
      return std::nullopt;
    } else if (l.size() == 1) {
      return std::move(l[0]);
    } else {
      shot("Optional expect list of one or zero items, got $", l.size());
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
// vector
//

template <class T> struct Serialize<std::vector<T>> {
  static Value::ptr serialize(const std::vector<T>& values)
  {
    std::vector<Value::ptr> elements;
    for (const auto& value : values) { elements.push_back(ser<T>(value)); }
    return Value::create_list(std::move(elements), std::nullopt);
  }

  template <class... Args>
  static bee::OrError<std::vector<T>> deserialize(
    const Value::ptr& value, Args&&... args)
  {
    if (!value->is_list()) { return bee::Error("vector expects a list"); }
    std::vector<T> values;
    for (const auto& element : value->list()) {
      bail(parsed, des<T>(element, std::forward<Args>(args)...));
      values.push_back(std::move(parsed));
    }
    return values;
  }
};

////////////////////////////////////////////////////////////////////////////////
// set
//

template <class T> struct Serialize<std::set<T>> {
  static Value::ptr serialize(const std::set<T>& values)
  {
    std::vector<Value::ptr> elements;
    for (const auto& value : values) { elements.push_back(ser<T>(value)); }
    return Value::create_list(std::move(elements), std::nullopt);
  }

  template <class... Args>
  static bee::OrError<std::set<T>> deserialize(
    const Value::ptr& value, Args&&... args)
  {
    if (!value->is_list()) { return bee::Error("set expects a list"); }
    std::set<T> values;
    for (const auto& element : value->list()) {
      bail(parsed, des<T>(element, std::forward<Args>(args)...));
      values.insert(std::move(parsed));
    }
    return values;
  }
};

////////////////////////////////////////////////////////////////////////////////
// map
//

template <class K, class V> struct Serialize<std::map<K, V>> {
  static Value::ptr serialize(const std::map<K, V>& value)
  {
    std::vector<Value::ptr> elements;
    elements.reserve(value.size());
    for (const auto& el : value) {
      elements.emplace_back(ser<std::pair<K, V>>(el));
    }
    return Value::create_list(std::move(elements), std::nullopt);
  }

  static bee::OrError<std::map<K, V>> deserialize(const Value::ptr& value)
  {
    bail(els, (des<std::vector<std::pair<K, V>>>(value)));
    std::map<K, V> output;
    for (auto& el : els) { output.emplace(std::move(el)); }
    return output;
  }
};

////////////////////////////////////////////////////////////////////////////////
// pair
//

template <class T, class U> struct Serialize<std::pair<T, U>> {
  static Value::ptr serialize(const std::pair<T, U>& value)
  {
    if constexpr (std::is_same_v<T, std::string>) {
      return Value::create_key_value(
        value.first, ser<U>(value.second), std::nullopt);
    } else {
      return Value::create_list(
        {ser<T>(value.first), ser<U>(value.second)}, std::nullopt);
    }
  }

  template <class... Args>
  static bee::OrError<std::pair<T, U>> deserialize(
    const Value::ptr& value, Args&&... args)
  {
    if constexpr (std::is_same_v<T, std::string>) {
      if (!value->is_key_value()) {
        return bee::Error("Expected a key value pair");
      }
      auto& kv = value->key_value();
      bail(second, des<U>(kv.value, std::forward<Args>(args)...));
      return make_pair(kv.key, std::move(second));
    } else {
      bail(els, des<std::vector<Value::ptr>>(value));
      if (els.size() != 2) {
        return bee::Error("Pair expected a list of size 2");
      }
      bail(first, des<T>(els[0], args...));
      bail(second, des<U>(els[1], std::forward<Args>(args)...));
      return std::make_pair(std::move(first), std::move(second));
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
// tuple
//

template <class... Ts> struct Serialize<std::tuple<Ts...>> {
  using value_type = std::tuple<Ts...>;

  static Value::ptr serialize(const value_type& value)
  {
    return _ser(value, std::make_index_sequence<sizeof...(Ts)>());
  }

  static bee::OrError<value_type> deserialize(const Value::ptr& value)
  {
    return _des(value, std::make_index_sequence<sizeof...(Ts)>());
  }

 private:
  template <size_t... I>
  static Value::ptr _ser(const value_type& tup, std::index_sequence<I...>)
  {
    return Value::create_list({ser(std::get<I>(tup))...}, std::nullopt);
  }

  template <class T> static T _parse_one(const Value::ptr& v)
  {
    auto r = des<T>(v);
    if (r.is_error()) { throw r.error(); }
    return std::move(r.value());
  }

  template <size_t... I>
  static bee::OrError<value_type> _des(
    const Value::ptr& v, std::index_sequence<I...>)
  {
    auto lst = v->list();
    try {
      return value_type{_parse_one<Ts>(lst[I])...};
    } catch (const bee::Error& e) {
      return e;
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
// long long
//

template <> struct Serialize<long long> {
  static Value::ptr serialize(const long long value);
  static bee::OrError<long long> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// long int
//

template <> struct Serialize<long int> {
  static Value::ptr serialize(const long int value);
  static bee::OrError<long int> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// int
//

template <> struct Serialize<int> {
  static Value::ptr serialize(const int value);
  static bee::OrError<int> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// long unsigned int
//

template <> struct Serialize<long unsigned int> {
  static Value::ptr serialize(const long unsigned int value);
  static bee::OrError<long unsigned int> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// double
//

template <> struct Serialize<double> {
  static Value::ptr serialize(const double value);
  static bee::OrError<double> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// float
//

template <> struct Serialize<float> {
  static Value::ptr serialize(const float value);
  static bee::OrError<float> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// Time
//

template <> struct Serialize<bee::Time> {
  static Value::ptr serialize(const bee::Time& value);
  static bee::OrError<bee::Time> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// Span
//

template <> struct Serialize<bee::Span> {
  static Value::ptr serialize(const bee::Span& value);
  static bee::OrError<bee::Span> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// bee::Unit
//

template <> struct Serialize<bee::Unit> {
  static Value::ptr serialize(bee::Unit value);
  static bee::OrError<bee::Unit> deserialize(const Value::ptr& value);
};

////////////////////////////////////////////////////////////////////////////////
// Fallback to methods
//

template <class T> struct Serialize<std::shared_ptr<T>> {
  static Value::ptr serialize(const std::shared_ptr<T>& value)
  {
    return value->to_yasf_value();
  }

  template <class... Args>
  static bee::OrError<std::shared_ptr<T>> deserialize(
    const Value::ptr& value, Args&&... args)
  {
    return T::of_yasf_value(value, std::forward<Args>(args)...);
  }
};

template <class T> struct Serialize {
  static Value::ptr serialize(const T& value) { return value.to_yasf_value(); }
  template <class... Args>
  static bee::OrError<T> deserialize(const Value::ptr& value, Args&&... args)
  {
    return T::of_yasf_value(value, std::forward<Args>(args)...);
  }
};

} // namespace yasf
