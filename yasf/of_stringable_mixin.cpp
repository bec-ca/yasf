#include "of_stringable_mixin.hpp"

#include "yasf/serializer.hpp"

using std::string;

namespace yasf {

namespace detail {

bee::OrError<std::string> string_of_yasf_value(const Value::ptr& value)
{
  return yasf::des<string>(value);
}

} // namespace detail

} // namespace yasf
