#include "parser_helpers.hpp"

#include "cof.hpp"

using std::nullopt;
using std::optional;
using std::string;
using std::vector;

namespace yasf {

bee::OrError<bool> ParserHelper::to_bool(const Value::ptr& config_value)
{
  if (config_value->is_list()) {
    const auto& elements = config_value->list();
    if (elements.size() != 1) {
      return bee::Error::fmt(
        "Expected a single value list to convert to a bool, got $",
        elements.size());
    }
    return to_bool(elements[0]);
  } else if (config_value->is_atom()) {
    const string& value = config_value->atom();
    if (value == "true") {
      return true;
    } else if (value == "false") {
      return false;
    } else {
      return bee::Error::fmt("Unexpected value $ for bool type", value);
    }
  } else {
    return bee::Error("Cannot convert a key value into a bool");
  }
}

Value::ptr ParserHelper::of_bool(bool value)
{
  return Value::create_atom(value ? "true" : "false", nullopt);
}

optional<Value::ptr> ParserHelper::of_bool_optional(bool value)
{
  if (!value) {
    return nullopt;
  } else {
    return of_bool(value);
  }
}

bee::OrError<int64_t> ParserHelper::to_int(const Value::ptr& config_value)
{
  if (config_value->is_list()) {
    const auto& elements = config_value->list();
    if (elements.size() != 1) {
      return bee::Error::fmt(
        "Expected a single value list to convert to a int, got $",
        elements.size());
    }
    return to_int(elements[0]);
  } else if (config_value->is_atom()) {
    const string& value = config_value->atom();
    return stoll(value);
  } else {
    return bee::Error("Cannot convert a key value into an int");
  }
}

Value::ptr ParserHelper::of_int(int64_t value)
{
  return Value::create_atom(std::to_string(value), nullopt);
}

optional<Value::ptr> ParserHelper::of_int_optional(int64_t value)
{
  if (value == 0) {
    return nullopt;
  } else {
    return of_int(value);
  }
}

bee::OrError<double> ParserHelper::to_float(const Value::ptr& config_value)
{
  if (config_value->is_list()) {
    const auto& elements = config_value->list();
    if (elements.size() != 1) {
      return bee::Error::fmt(
        "Expected a single value list to convert to a double, got $",
        elements.size());
    }
    return to_float(elements[0]);
  } else {
    const string& value = config_value->atom();
    return stof(value);
  }
}

Value::ptr ParserHelper::of_float(double value)
{
  return Value::create_atom(std::to_string(value), nullopt);
}

optional<Value::ptr> ParserHelper::of_float_optional(double value)
{
  if (value == 0) return nullopt;
  return of_float(value);
}

void ParserHelper::push_back_field(
  vector<Value::ptr>& v, optional<Value::ptr> value, string name)
{
  if (value.has_value()) {
    v.push_back(Value::create_key_value(
      std::move(name), std::move(value.value()), nullopt));
  }
}

string ParserHelper::location(const Value::ptr& v)
{
  if (const auto& loc = v->location()) {
    return loc->hum();
  } else {
    return "<unknown>";
  }
}

bee::Error ParserHelper::make_error_msg(
  const std::string& msg, const Value::ptr& value)
{
  return bee::Error::fmt(
    "$: $ in '$'", location(value), msg, Cof::serialize(value));
}

} // namespace yasf
