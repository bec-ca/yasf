#include "yasf/time.hpp"

#include "bee/parse_string.hpp"

namespace yasf {

Value::ptr Serialize<Time>::serialize(const Time& time)
{
  return ser(time.to_string());
}

bee::OrError<Time> Serialize<Time>::deserialize(const Value::ptr& value)
{
  bail(str, des<std::string>(value));

  // Note that time used to be serialized as a single timestamp of nanos since
  // epoch. Check if the given value is all digits first, if so, assume it is
  // the old format.

  bool is_all_digits = [&str]() {
    for (char c : str) {
      if (!isdigit(c)) { return false; }
    }
    return true;
  }();

  if (is_all_digits) {
    bail(ts, bee::parse_string<int64_t>(str));
    return Time::of_nanos_since_epoch(ts);
  } else {
    return Time::of_string(str);
  }
}

} // namespace yasf
