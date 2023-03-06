#include "serializer.hpp"

#include <optional>
#include <sstream>

using std::nullopt;
using std::string;

using bee::Span;
using bee::Time;

namespace yasf {

////////////////////////////////////////////////////////////////////////////////
// string
//

Value::ptr Serialize<string>::serialize(const std::string& value)
{
  return Value::create_atom(value, std::nullopt);
}

bee::OrError<std::string> Serialize<string>::deserialize(
  const Value::ptr& value)
{
  if (value->is_list()) {
    const auto& elements = value->list();
    if (elements.size() != 1) {
      return bee::Error::format(
        "Expected a single value list to convert to a string, got $",
        elements.size());
    }
    return deserialize(elements[0]);
  } else if (value->is_atom()) {
    return value->atom();
  } else {
    return bee::Error::format(
      "Cannot convert a key value into a string: '$'", value->to_string_hum());
  }
}

////////////////////////////////////////////////////////////////////////////////
// long long
//

Value::ptr Serialize<long long>::serialize(const long long value)
{
  return ser<std::string>(std::to_string(value));
}

bee::OrError<long long> Serialize<long long>::deserialize(
  const Value::ptr& value)
{
  bail(b, des<std::string>(value));
  return stol(b);
}

////////////////////////////////////////////////////////////////////////////////
// long int
//

Value::ptr Serialize<long int>::serialize(const long int value)
{
  return ser<std::string>(std::to_string(value));
}

bee::OrError<long int> Serialize<long int>::deserialize(const Value::ptr& value)
{
  bail(b, des<std::string>(value));
  return stol(b);
}

////////////////////////////////////////////////////////////////////////////////
// int
//

Value::ptr Serialize<int>::serialize(const int value)
{
  return ser<std::string>(std::to_string(value));
}

bee::OrError<int> Serialize<int>::deserialize(const Value::ptr& value)
{
  bail(b, des<std::string>(value));
  return stol(b);
}

////////////////////////////////////////////////////////////////////////////////
// long unsigned int
//

Value::ptr Serialize<long unsigned int>::serialize(
  const long unsigned int value)
{
  return ser<std::string>(std::to_string(value));
}

bee::OrError<long unsigned int> Serialize<long unsigned int>::deserialize(
  const Value::ptr& value)
{
  bail(b, des<std::string>(value));
  return stoul(b);
}

////////////////////////////////////////////////////////////////////////////////
// double
//

Value::ptr Serialize<double>::serialize(const double value)
{
  std::ostringstream s;
  s << value;
  return ser(std::move(s.str()));
}

bee::OrError<double> Serialize<double>::deserialize(const Value::ptr& value)
{
  bail(b, des<std::string>(value));
  return std::stod(b);
}

////////////////////////////////////////////////////////////////////////////////
// float
//

Value::ptr Serialize<float>::serialize(const float value)
{
  std::ostringstream s;
  s << value;
  return ser(std::move(s.str()));
}

bee::OrError<float> Serialize<float>::deserialize(const Value::ptr& value)
{
  bail(b, des<std::string>(value));
  return std::stof(b);
}

////////////////////////////////////////////////////////////////////////////////
// Time
//

Value::ptr Serialize<Time>::serialize(const Time& value)
{
  return ser<int64_t>(value.to_nanos_since_epoch());
}

bee::OrError<Time> Serialize<Time>::deserialize(const Value::ptr& value)
{
  bail(ts, des<int64_t>(value));
  return Time::of_nanos_since_epoch(ts);
}

////////////////////////////////////////////////////////////////////////////////
// Span
//

Value::ptr Serialize<Span>::serialize(const Span& value)
{
  return ser<double>(value.to_float_seconds());
}

bee::OrError<Span> Serialize<Span>::deserialize(const Value::ptr& value)
{
  bail(ts, des<double>(value));
  return Span::of_seconds(ts);
}

////////////////////////////////////////////////////////////////////////////////
// bee::Unit
//

Value::ptr Serialize<bee::Unit>::serialize(bee::Unit)
{
  return Value::create_list({}, nullopt);
}

bee::OrError<bee::Unit> Serialize<bee::Unit>::deserialize(
  const Value::ptr& value)
{
  if (!value->is_list() || value->list().size() > 0) {
    return bee::Error("Unit expected an empty list");
  }
  return bee::unit;
}

} // namespace yasf
