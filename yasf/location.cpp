#include "location.hpp"

#include "bee/format.hpp"

using std::string;

namespace yasf {

string Location::hum() const { return F("$:$", filename, line); }

string Location::to_string() const
{
  return F("Location(line:$ col:$ filename:$)", line, col, filename);
}

} // namespace yasf
