#include "enum.hpp"

#include <string>

#include "bee/format.hpp"

using std::make_shared;
using std::nullopt;
using std::optional;
using std::set;
using std::string;
using std::vector;

namespace yasf {

////////////////////////////////////////////////////////////////////////////////
// Enum
//

struct Enum final : public CustomType {
 public:
  Enum(const string& name, const vector<string>& values)
      : name(name), values(values)
  {}

  virtual ~Enum() {}

  virtual const string& type_name() const override { return name; }
  virtual string parse_expr(const string& value) const override;
  virtual string unparse_expr(const string& value) const override;
  virtual string unparse_expr_optional(const string& value) const override;
  virtual optional<string> default_value() const override;
  virtual bool is_container() const override { return false; }
  virtual bool has_optional_default() const override { return false; }

  virtual string gen_decl() const override;
  virtual string gen_impl() const override;

  string variant_type_name() const;
  string gen_parse_code() const;
  string gen_unparse_code() const;

  virtual set<string> additional_headers() const override { return {}; }

 private:
  const string name;
  const vector<string> values;
};

string Enum::parse_expr(const string& value) const
{
  return F("yasf::des<$>($)", type_name(), value);
}

string Enum::unparse_expr(const string& value) const
{
  return F("yasf::ser($)", value);
}

string Enum::unparse_expr_optional(const string& value) const
{
  return unparse_expr(value);
}

string Enum::gen_decl() const
{
  string output;
  output += F("struct $ {{\n", name);
  output += "enum Value {\n";
  for (const auto& value : values) { output += F("$,\n", value); }
  output += "};\n";
  output += F("constexpr $(Value v) : _value(v) {{}\n", name);
  output += F("constexpr operator Value() const {{ return _value; }");
  output += F("explicit operator bool() const = delete;");
  output += "yasf::Value::ptr to_yasf_value() const;\n";
  output +=
    F("static bee::OrError<$> of_yasf_value(const yasf::Value::ptr& "
      "config_value);\n\n",
      name);
  output += "const char* to_string() const;\n";
  output += "private:\n";
  output += "Value _value;\n";
  output += "};\n";
  return output;
}

string Enum::gen_parse_code() const
{
  string output;
  output +=
    F("bee::OrError<$> $::of_yasf_value(const yasf::Value::ptr& input_value) "
      "{{\n",
      name,
      name);
  output += "bail(str_value, yasf::des<std::string>(input_value));\n";
  bool first = true;
  for (const auto& value : values) {
    if (!first) {
      output += "else ";
    } else {
      first = false;
    }
    output += F("if (str_value == \"$\") {{\n", value);
    output += F("return $;\n", value);
    output += "}\n";
  }
  output += "else {\n";
  output += "return PH::err(\"Unknown enum value\", input_value);\n";
  output += "}\n";
  output += "}\n";
  return output;
}

string Enum::gen_unparse_code() const
{
  string output;

  output += F("const char* $::to_string() const {{\n", name);
  output += "switch(_value) {";
  for (const auto& value : values) {
    output += F("case $: return \"$\";", value, value);
  }
  output += "}\n";
  output += "}\n\n";
  output += F("yasf::Value::ptr $::to_yasf_value() const {{\n", name);
  output += "return yasf::ser(to_string());";
  output += "}\n";
  return output;
}

string Enum::gen_impl() const
{
  return gen_parse_code() + "\n\n" + gen_unparse_code();
}

optional<string> Enum::default_value() const { return nullopt; }

namespace details {
CustomType::ptr make_enum_type(
  const std::string& name, const std::vector<std::string>& legs)
{
  return make_shared<Enum>(name, legs);
}

} // namespace details

} // namespace yasf
