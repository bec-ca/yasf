#include "definitions.hpp"

#include <string>

#include "bee/format.hpp"
#include "bee/util.hpp"

using std::set;
using std::string;

namespace yasf {

////////////////////////////////////////////////////////////////////////////////
// Definitions
//

const string olint_allow = "\n// olint-allow: missing-package-namespace\n";

string Definitions::gen_decl(const string& ns, bool ignore_olint_ns) const
{
  string output;
  output += "#pragma once\n";
  output += "\n";
  output += "#include \"yasf/serializer.hpp\"\n";
  output += "#include \"bee/or_error.hpp\"\n";

  set<string> additional_headers;
  for (const auto& type : types) {
    bee::insert_many(additional_headers, type->additional_headers());
  }
  for (const auto& header : additional_headers) {
    output += F("#include \"$\"\n", header);
  }

  output += "\n";
  output += "#include <set>\n";
  output += "#include <string>\n";
  output += "#include <vector>\n";
  output += "#include <variant>\n";
  output += "\n";
  output += F("namespace $ {{\n", ns);
  output += "\n";
  for (const auto& type : types) {
    output += type->gen_decl();
    output += "\n";
  }
  output += "\n";
  output += "}\n\n";
  if (ignore_olint_ns) { output += olint_allow; }
  return output;
}

string Definitions::gen_impl(
  const string& ns, const string& base_name, bool ignore_olint_ns) const
{
  string output;
  output += F("#include \"$.generated.hpp\"\n", base_name);
  output += "\n";
  output += "#include <type_traits>\n";
  output += "\n";
  output += "#include \"bee/format.hpp\"\n";
  output += "#include \"bee/util.hpp\"\n";
  output += "#include \"yasf/parser_helpers.hpp\"\n";
  output += "#include \"yasf/serializer.hpp\"\n";
  output += "\n";
  output += "using PH = yasf::ParserHelper;\n";
  output += "\n";
  output += F("namespace $ {{\n", ns);
  output += "\n";
  for (const auto& type : types) {
    output += "////////////////////////////////////////////////////////////////"
              "////////////////\n";
    output += F("// $\n", type->type_name());
    output += "//\n\n";
    output += type->gen_impl();
    output += "\n";
  }
  output += "\n";
  output += "}\n\n";
  if (ignore_olint_ns) { output += olint_allow; }
  return output;
}

} // namespace yasf
