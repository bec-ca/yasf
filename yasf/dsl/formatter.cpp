#include "formatter.hpp"

#include "bee/print.hpp"

namespace yasf::dsl {

bee::OrError<> Formatter::format(const bee::FilePath& src)
{
  P(src);
  return EF("Not implemented");
}

} // namespace yasf::dsl
