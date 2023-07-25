#pragma once

#include <string>

#include "type.hpp"

#include "bee/util.hpp"

namespace yasf {

struct Field {
  Field(std::string&& name, const Type::ptr& type, bool required);

  const std::string name;
  const Type::ptr type;
  const bool is_required;

  std::string gen_decl() const;
  std::string gen_variable_name() const;
};

namespace details {

CustomType::ptr make_record(
  const char* name, std::vector<Field>&& fields, bool include_location);

} // namespace details

template <class T> struct GenericField {
 public:
  constexpr GenericField(const char* name, const T type, bool required)
      : _name(name), _type(type), _is_required(required)
  {}

  operator Field() const { return {_name, _type, _is_required}; }

 private:
  const char* _name;
  const T _type;
  const bool _is_required;
};

template <class... T> struct GenericRecord {
 public:
  constexpr GenericRecord(
    const char* name,
    std::tuple<GenericField<T>...> fields,
    bool include_location = false)
      : _name(name), _fields(fields), _include_location(include_location)
  {}

  operator CustomType::ptr() const
  {
    return details::make_record(
      _name, bee::tuple_to_vector<Field>(_fields), _include_location);
  }

  operator Type::ptr() const { return static_cast<CustomType::ptr>(*this); }

 private:
  const char* _name;
  std::tuple<GenericField<T>...> _fields;
  const bool _include_location;
};

template <class T>
constexpr GenericField<T> required_field(const char* name, const T type)
{
  return {name, type, true};
}

template <class T>
constexpr GenericField<T> optional_field(const char* name, const T type)
{
  return {name, type, false};
}

namespace types {

template <class... T>
constexpr std::tuple<GenericField<T>...> fields(GenericField<T>... fields)
{
  return {fields...};
}

template <class... T>
constexpr GenericRecord<T...> record(
  const char* name, std::tuple<GenericField<T>...> fields)
{
  return {name, fields};
}

template <class... T>
constexpr GenericRecord<T...> record_with_location(
  const char* name, std::tuple<GenericField<T>...> fields)
{
  return {name, fields, true};
}

} // namespace types

} // namespace yasf
