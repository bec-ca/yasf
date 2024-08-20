#include "variant.hpp"

#include "bee/format.hpp"

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

  Variant(const std::string_view& name, vector<VariantLeg>&& legs)
      : name(name), legs(std::move(legs))
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
    headers.insert("yasf/to_stringable_mixin.hpp");
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

string Variant::parse_func_name() const { return F("$::of_yasf_value", name); }

string Variant::unparse_func_name() const
{
  return F("$::to_yasf_value", name);
}

string Variant::parse_expr(const string& value) const
{
  return F("yasf::des<$>($)", type_name(), value);
}

string Variant::unparse_expr(const string& value) const
{
  return F("yasf::ser($)", value);
}

string Variant::unparse_expr_optional(const string& value) const
{
  return unparse_expr(value);
}

string Variant::gen_decl() const
{
  string output;
  output += F("struct $ : public yasf::ToStringableMixin<$> {{\n", name, name);
  output += F("using value_type = $;\n\n", variant_type_name());
  output += "value_type value;\n\n";
  output += F("$() noexcept = default;\n", name);
  output += F("$(const value_type& value) noexcept;\n", name);
  output += F("$(value_type&& value) noexcept;\n\n", name);
  output +=
    F("template <std::convertible_to<value_type> U> $(U&& value) noexcept : "
      "value(std::forward<U>(value)) {{}\n\n",
      name);
  output +=
    F("static bee::OrError<$> of_yasf_value(const yasf::Value::ptr& "
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
  output += F(
    "$::$(const value_type& value) noexcept : value(value) {{}\n", name, name);
  output +=
    F("$::$(value_type&& value) noexcept : value(std::move(value)) {{}\n",
      name,
      name);
  return output;
}

string Variant::gen_parse_code() const
{
  string output;
  output +=
    F("bee::OrError<$> $(const yasf::Value::ptr& arg_value) {{\n",
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
    output += F("if (name == \"$\") {{\n", leg.name);
    output += F("return $;\n", leg.type->parse_expr("kv.value"));
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

  output += F("yasf::Value::ptr $() const {{\n", unparse_func_name());
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
      F("if constexpr (std::is_same_v<T, $>) {{\n", leg.type->type_name());
    output +=
      F("return yasf::Value::create_key_value(\"$\", {{$}, std::nullopt);",
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

CustomType::ptr make_variant(
  const std::string_view& name, std::vector<VariantLeg>&& legs)
{
  return make_shared<Variant>(name, std::move(legs));
}

} // namespace details

} // namespace yasf
