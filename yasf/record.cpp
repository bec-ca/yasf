#include "record.hpp"

#include "bee/format.hpp"
#include "bee/util.hpp"

using std::make_shared;
using std::nullopt;
using std::optional;
using std::set;
using std::string;
using std::vector;

namespace yasf {

////////////////////////////////////////////////////////////////////////////////
// Field
//

Field::Field(const std::string_view& name, const Type::ptr& type, bool required)
    : name(name), type(std::move(type)), is_required(required)
{}

string Field::gen_decl() const
{
  auto type_name = type->type_name();
  bool has_optional_default = type->has_optional_default();
  bool is_container = type->is_container();
  auto use_std_opt = !is_required && !is_container && !has_optional_default;
  if (use_std_opt) { type_name = F("std::optional<$>", type_name); }
  auto def = is_required ? type->default_value() : "";
  return F("$ $ $;\n", type_name, name, def ? F("{{$}", *def) : "");
}

string Field::gen_variable_name() const { return F("output_$", name); }

Field required_field(string&& name, const Type::ptr& type)
{
  return Field(std::move(name), type, true);
}

Field optional_field(string&& name, const Type::ptr& type)
{
  return Field(std::move(name), type, false);
}

////////////////////////////////////////////////////////////////////////////////
// Record
//

struct Record final : public CustomType {
 public:
  Record(
    const std::string_view& name, vector<Field>&& fields, bool include_location)
      : name(name),
        fields(std::move(fields)),
        include_location(include_location)
  {}

  virtual ~Record() {}

  virtual const string& type_name() const override { return name; }
  virtual string parse_expr(const string& value) const override;
  virtual string unparse_expr(const string& value) const override;
  virtual string unparse_expr_optional(const string& value) const override;
  virtual optional<string> default_value() const override;
  virtual bool is_container() const override { return false; }
  virtual bool has_optional_default() const override { return false; }

  virtual string gen_decl() const override;
  virtual string gen_impl() const override;

  string gen_parse_code() const;
  string gen_unparse_code() const;
  string parse_func_name() const;
  string unparse_func_name() const;

  virtual set<string> additional_headers() const override
  {
    set<string> headers;
    headers.insert("yasf/to_stringable_mixin.hpp");
    for (const auto& field : fields) {
      bee::insert_many(headers, field.type->additional_headers());
    }

    return headers;
  }

 private:
  const string name;
  const vector<Field> fields;
  const bool include_location;
};

string Record::gen_decl() const
{
  string output;
  output += F("struct $ : public yasf::ToStringableMixin<$> {{\n", name, name);
  for (const auto& field : fields) { output += field.gen_decl(); }
  if (include_location) {
    output += "std::optional<yasf::Location> location{};\n";
  }
  output += "\n";
  output +=
    F("static bee::OrError<$> of_yasf_value(const yasf::Value::ptr& "
      "config_value);\n\n",
      name);
  output += "yasf::Value::ptr to_yasf_value() const;\n";
  output += "};\n\n";
  return output;
}

string Record::gen_parse_code() const
{
  string output;
  output +=
    F("bee::OrError<$> $(const yasf::Value::ptr& value) {{\n",
      name,
      parse_func_name());
  output += "if (!value->is_list()) { return PH::err(\"Record expected a "
            "list, but got something else\", value); }\n\n";
  for (const auto& field : fields) {
    output +=
      F("std::optional<$> $;\n",
        field.type->type_name(),
        field.gen_variable_name());
  }
  output += "\n";
  if (!fields.empty()) {
    output += "for (const auto& element : value->list()) {\n";
    output +=
      "if (!element->is_key_value()) { return "
      "PH::err(\"Expected a key value as a record element\", element); }\n\n";
    output += "const auto& kv = element->key_value();\n";
    output += "const std::string& name = kv.key;\n";
    bool first = true;
    for (const auto& field : fields) {
      auto var = field.gen_variable_name();
      if (!first) {
        output += "else ";
      } else {
        first = false;
      }
      output += F("if (name == \"$\") {{\n", field.name);
      output += F(
        "if ($.has_value()) {{ return PH::err(\"Field '$' is defined more than "
        "once\", element); }\n",
        var,
        field.name);
      output +=
        F("bail_assign($, $);\n", var, field.type->parse_expr("kv.value"));
      output += "}\n";
    }
    output += "else {\n";
    output +=
      F("return PH::err(\"No such field in record of type $\", element);\n",
        name);
    output += "}\n";
    output += "}\n\n";
  } else {
    output += "if (!value->list().empty()) { return PH::err(\"This record "
              "should be empty\", value); }";
  }

  for (const auto& field : fields) {
    auto var = field.gen_variable_name();
    if (field.is_required) {
      output +=
        F("if (!$.has_value()) {{ return PH::err(\"Field '$' not defined\", "
          "value); }\n",
          var,
          field.name);
    } else if (field.type->is_container()) {
      output += F("if (!$.has_value()) {{ $.emplace(); } ", var, var);
    } else if (field.type->has_optional_default()) {
      output +=
        F("if (!$.has_value()) {{ $ = $; } ",
          var,
          var,
          *field.type->default_value());
    }
  }

  output += F("\nreturn ${{\n", name);

  for (const auto& field : fields) {
    if (
      field.is_required || field.type->is_container() ||
      field.type->has_optional_default()) {
      output += F(".$ = std::move(*$),", field.name, field.gen_variable_name());
    } else {
      output += F(".$ = std::move($),", field.name, field.gen_variable_name());
    }
  }

  if (include_location) { output += ".location = value->location(),"; }

  output += "};\n";
  output += "}\n";
  return output;
}

string Record::gen_unparse_code() const
{
  string output;
  output += F("yasf::Value::ptr $() const {{\n", unparse_func_name());
  output += "std::vector<yasf::Value::ptr> fields;\n";
  // TODO: there is too much code duplication here
  for (const auto& field : fields) {
    if (field.is_required) {
      output +=
        F("PH::push_back_field(fields, $, \"$\");\n",
          field.type->unparse_expr(field.name),
          field.name);
    } else if (field.type->is_container()) {
      output += F("if (!$.empty()) {{\n", field.name);
      output +=
        F("PH::push_back_field(fields, $, \"$\");\n",
          field.type->unparse_expr(field.name),
          field.name);
      output += "}\n";
    } else if (field.type->has_optional_default()) {
      output += F("if ($ != $) {{\n", field.name, *field.type->default_value());
      output +=
        F("PH::push_back_field(fields, $, \"$\");\n",
          field.type->unparse_expr(field.name),
          field.name);
      output += "}\n";
    } else {
      output += F("if ($.has_value()) {{\n", field.name);
      output +=
        F("PH::push_back_field(fields, $, \"$\");\n",
          field.type->unparse_expr(F("*$", field.name)),
          field.name);
      output += "}\n";
    }
  }
  output +=
    F("return yasf::Value::create_list(std::move(fields), std::nullopt);\n");

  output += "}\n";
  return output;
}

string Record::gen_impl() const
{
  return gen_parse_code() + "\n\n" + gen_unparse_code();
}

string Record::parse_func_name() const { return F("$::of_yasf_value", name); }

string Record::unparse_func_name() const { return F("$::to_yasf_value", name); }

string Record::parse_expr(const string& value) const
{
  return F("$($)", parse_func_name(), value);
}

string Record::unparse_expr(const string& value) const
{
  return F("($).to_yasf_value()", value);
}

string Record::unparse_expr_optional(const string& value) const
{
  return unparse_expr(value);
}

optional<string> Record::default_value() const { return nullopt; }

namespace details {

CustomType::ptr make_record(
  const std::string_view& name,
  std::vector<Field>&& fields,
  bool include_location)
{
  return make_shared<Record>(Record(name, std::move(fields), include_location));
}

} // namespace details
} // namespace yasf
