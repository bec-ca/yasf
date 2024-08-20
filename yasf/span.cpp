#include "span.hpp"

#include "bee/parse_string.hpp"

namespace yasf {
namespace {

const bee::Error invalid_format_error("Invalid format");

}

Value::ptr Serialize<bee::Span>::serialize(const bee::Span& value)
{
  return ser(value.to_string({.decimal_places = 20}));
}

bee::OrError<bee::Span> Serialize<bee::Span>::deserialize(
  const Value::ptr& value)
{
  bail(str, des<std::string>(value));
  if (str.empty()) { return invalid_format_error; }
  if (isdigit(str.back())) {
    bail(ts, bee::parse_string<double>(str));
    return bee::Span::of_seconds(ts);
  } else {
    return bee::Span::of_string(str);
  }
}

} // namespace yasf
