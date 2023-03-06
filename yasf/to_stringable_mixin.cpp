#include "to_stringable_mixin.hpp"
#include "cof.hpp"

using std::string;

namespace yasf {

bee::OrError<Value::ptr> DetailToStringableMixin::cof_to_yasf_value(
  const std::string& str)
{
  return Cof::raw_parse_string(str);
}

std::string DetailToStringableMixin::yasf_value_to_cof(const Value::ptr& value)
{
  return Cof::to_string(value);
}

} // namespace yasf
