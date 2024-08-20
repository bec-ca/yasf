#include "code.hpp"

#include "bee/format.hpp"
#include "bee/string_util.hpp"

namespace yasf::dsl {

////////////////////////////////////////////////////////////////////////////////
// Type
//

std::string Type::to_string() const
{
  if (parameter.has_value()) {
    return F("$ $", *parameter, name);
  } else {
    return name;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Record
//

std::string Record::to_string() const
{
  std::string out = F("record $\n", name);
  if (location) { out += "  attr location\n"; }
  for (const auto& field : fields) {
    out +=
      F("  $ $ $\n",
        field.name,
        field.type,
        field.optional ? "optional" : "required");
  }
  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Enum
//

std::string Enum::to_string() const
{
  std::string out = F("enum $\n", name);
  for (const auto& leg : legs) { out += F("  $\n", leg); }
  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Variant
//

std::string Variant::to_string() const
{
  std::string out;
  out += F("variant $\n", name);
  for (const auto& field : legs) {
    out += F("  $ $\n", field.name, field.type);
  }
  return out;
}

////////////////////////////////////////////////////////////////////////////////
// Namespace
//

std::string Namespace::to_string() const { return bee::join(parts, "."); }

std::string Namespace::to_cpp() const { return bee::join(parts, "::"); }

////////////////////////////////////////////////////////////////////////////////
// Code
//

std::string Code::to_string() const
{
  std::string out = F("ns $\n", ns);
  for (const auto& type : types) {
    out += std::visit([&]<class T>(const T& v) { return v.to_string(); }, type);
  }
  return out;
}

} // namespace yasf::dsl
