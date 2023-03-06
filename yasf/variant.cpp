#include "variant.hpp"

using bee::format;
using std::nullopt;
using std::optional;
using std::set;
using std::string;
using std::vector;

namespace yasf {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Variant
//

struct Variant final : public CustomType {
 public:
  using ptr = std::shared_ptr<Variant>;

  Variant(string&& name, vector<VariantLeg>&& legs)
      : name(std::move(name)), legs(std::move(legs))
  {}

  virtual ~Variant() {}

  virtual const std::string& type_name() const override { return name; }
  virtual std::string parse_expr(const std::string& value) const override;
  virtual std::string unparse_expr(const std::string& value) const override;
  virtual std::string unparse_expr_optional(
    const std::string& value) const override;
  virtual std::optional<std::string> default_value() const override;

  virtual bool is_container() const override { return false; }
  virtual bool has_optional_default() const override { return false; }

  virtual std::string gen_decl() const override;
  virtual std::string gen_impl() const override;

  std::string variant_type_name() const;
  std::string parse_func_name() const;
  std::string unparse_func_name() const;
  std::string gen_parse_code() const;
  std::string gen_unparse_code() const;
  std::string gen_constructor() const;

  virtual set<string> additional_headers() const override
  {
    set<string> headers;
    for (const auto& leg : legs) {
      bee::insert_many(headers, leg.type->additional_headers());
    }

    return headers;
  }

 private:
  const std::string name;
  const std::vector<VariantLeg> legs;
};

string Variant::variant_type_name() const
{
  string output = "std::variant<";
  bool first = true;
  for (const auto& leg : legs) {
    if (!first) {
      output += ", ";
    } else {
      first = false;
    }
    output += leg.type->type_name();
  }
  output += ">";

  return output;
}

string Variant::parse_func_name() const
{
  return format("$::of_yasf_value", name);
}

string Variant::unparse_func_name() const
{
  return format("$::to_yasf_value", name);
}

string Variant::parse_expr(const string& value) const
{
  return format("yasf::des<$>($)", type_name(), value);
}

string Variant::unparse_expr(const string& value) const
{
  return format("yasf::ser<$>($)", type_name(), value);
}

string Variant::unparse_expr_optional(const string& value) const
{
  return unparse_expr(value);
}

string Variant::gen_decl() const
{
  string output;
  output += format("struct $ {\n", name);
  output += format("using value_type = $;\n\n", variant_type_name());
  output += "value_type value;\n\n";
  output += format("$() noexcept = default;\n", name, name);
  output += format("$(const value_type& value) noexcept;\n", name);
  output += format("$(value_type&& value) noexcept;\n\n", name);
  output += format(
    "static bee::OrError<$> of_yasf_value(const yasf::Value::ptr& "
    "config_value);\n\n",
    name);

  output += "template <class F>";
  output += "auto visit(F&& f) const {";
  output += "  return std::visit(std::forward<F>(f), value);";
  output += "}";

  output += "template <class F>";
  output += "auto visit(F&& f) {";
  output += "  return std::visit(std::forward<F>(f), value);";
  output += "}";

  output += "yasf::Value::ptr to_yasf_value() const;\n";
  output += "};\n";
  return output;
}

string Variant::gen_constructor() const
{
  string output;
  output += format(
    "$::$(const value_type& value) noexcept : value(value) {}\n", name, name);
  output += format(
    "$::$(value_type&& value) noexcept : value(std::move(value)) {}\n",
    name,
    name);
  return output;
}

string Variant::gen_parse_code() const
{
  string output;
  output += format(
    "bee::OrError<$> $(const yasf::Value::ptr& arg_value) {\n",
    name,
    parse_func_name());
  output += "auto value = arg_value;";
  output += "if (value->is_list() && value->list().size() == 1) { value = "
            "value->list()[0]; }";
  output += "if (!value->is_key_value()) { return PH::err(\"Expected a key "
            "value as a variant element\", value); }\n\n";
  output += "const auto& kv = value->key_value();\n";
  output += "const std::string& name = kv.key;";
  bool first = true;
  for (const auto& leg : legs) {
    if (!first) {
      output += "else ";
    } else {
      first = false;
    }
    output += format("if (name == \"$\") {\n", leg.name);
    output += format("return $;\n", leg.type->parse_expr("kv.value"));
    output += "}\n";
  }
  output += "else {\n";
  output += "return PH::err(\"Unknown variant leg\", value);\n";
  output += "}\n";
  output += "}\n";
  return output;
}

string Variant::gen_unparse_code() const
{
  string output;

  output += format("yasf::Value::ptr $() const {\n", unparse_func_name());
  output += "return visit([](const auto& leg) {";
  output += "using T = std::decay_t<decltype(leg)>;";
  bool first = true;
  for (const auto& leg : legs) {
    if (!first) {
      output += "else ";
    } else {
      first = false;
    }
    output +=
      format("if constexpr (std::is_same_v<T, $>) {\n", leg.type->type_name());
    output += format(
      "return yasf::Value::create_key_value(\"$\", {$}, std::nullopt);",
      leg.name,
      leg.type->unparse_expr("leg"));
    output += "}\n";
  }
  output += "}\n";
  output += ");\n";

  output += "}\n";
  return output;
}

string Variant::gen_impl() const
{
  return gen_constructor() + "\n\n" + gen_parse_code() + "\n\n" +
         gen_unparse_code();
}

optional<string> Variant::default_value() const { return nullopt; }

} // namespace

namespace details {

CustomType::ptr make_variant(const char* name, std::vector<VariantLeg>&& legs)
{
  return make_shared<Variant>(name, std::move(legs));
}

} // namespace details

} // namespace yasf
