#include "serializer.hpp"

#include <optional>
#include <sstream>

#include "bee/parse_string.hpp"
#include "bee/to_string.hpp"

using std::nullopt;
using std::string;

namespace yasf {

////////////////////////////////////////////////////////////////////////////////
// std::string
//

bee::OrError<std::string> Serialize<string>::deserialize(
  const Value::ptr& value)
{
  if (value->is_list()) {
    const auto& elements = value->list();
    if (elements.size() != 1) {
      return bee::Error::fmt(
        "Expected a single value list to convert to a string, got $",
        elements.size());
    }
    return deserialize(elements[0]);
  } else if (value->is_atom()) {
    return value->atom();
  } else {
    return bee::Error::fmt(
      "Cannot convert a key value into a string: '$'", value->to_string_hum());
  }
}

////////////////////////////////////////////////////////////////////////////////
// Numerical
//

#define IMPLEMENT_STRING_PARSER_SERIALIZER(T)                                  \
  Value::ptr Serialize<T>::serialize(T value)                                  \
  {                                                                            \
    return ser(bee::to_string(value, {.decimal_places = 100}));                \
  }                                                                            \
                                                                               \
  bee::OrError<T> Serialize<T>::deserialize(const Value::ptr& value)           \
  {                                                                            \
    bail(b, des<std::string>(value));                                          \
    return bee::parse_string<T>(b);                                            \
  }

IMPLEMENT_STRING_PARSER_SERIALIZER(int);
IMPLEMENT_STRING_PARSER_SERIALIZER(unsigned);
IMPLEMENT_STRING_PARSER_SERIALIZER(long);
IMPLEMENT_STRING_PARSER_SERIALIZER(unsigned long);
IMPLEMENT_STRING_PARSER_SERIALIZER(long long);
IMPLEMENT_STRING_PARSER_SERIALIZER(unsigned long long);
IMPLEMENT_STRING_PARSER_SERIALIZER(float);
IMPLEMENT_STRING_PARSER_SERIALIZER(double);

////////////////////////////////////////////////////////////////////////////////
// bee::Unit
//

Value::ptr Serialize<bee::Unit>::serialize(bee::Unit)
{
  return Value::create_list(std::vector<Value::ptr>(), nullopt);
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
