#include "test_parser.hpp"

#include <type_traits>

#include "bee/format.hpp"
#include "bee/util.hpp"
#include "yasf/parser_helpers.hpp"
#include "yasf/serializer.hpp"

using PH = yasf::ParserHelper;

namespace test_parser {

////////////////////////////////////////////////////////////////////////////////
// int_or_str
//

int_or_str::int_or_str(const value_type& value) noexcept : value(value) {}
int_or_str::int_or_str(value_type&& value) noexcept : value(std::move(value)) {}

bee::OrError<int_or_str> int_or_str::of_yasf_value(
  const yasf::Value::ptr& arg_value)
{
  auto value = arg_value;
  if (value->is_list() && value->list().size() == 1) {
    value = value->list()[0];
  }
  if (!value->is_key_value()) {
    return PH::err("Expected a key value as a variant element", value);
  }

  const auto& kv = value->key_value();
  const std::string& name = kv.key;
  if (name == "int") {
    return PH::to_int(kv.value);
  } else if (name == "str") {
    return yasf::des<std::string>(kv.value);
  } else {
    return PH::err("Unknown variant leg", value);
  }
}

yasf::Value::ptr int_or_str::to_yasf_value() const
{
  return visit([](const auto& leg) {
    using T = std::decay_t<decltype(leg)>;
    if constexpr (std::is_same_v<T, int64_t>) {
      return yasf::Value::create_key_value(
        "int", {PH::of_int(leg)}, std::nullopt);
    } else if constexpr (std::is_same_v<T, std::string>) {
      return yasf::Value::create_key_value(
        "str", {yasf::ser<std::string>(leg)}, std::nullopt);
    }
  });
}

////////////////////////////////////////////////////////////////////////////////
// Color
//

bee::OrError<Color> Color::of_yasf_value(const yasf::Value::ptr& input_value)
{
  bail(str_value, yasf::des<std::string>(input_value));
  if (str_value == "red") {
    return red;
  } else if (str_value == "blue") {
    return blue;
  } else if (str_value == "green") {
    return green;
  } else {
    return PH::err("Unknown enum value", input_value);
  }
}

const char* Color::to_string() const
{
  switch (_value) {
  case red:
    return "red";
  case blue:
    return "blue";
  case green:
    return "green";
  }
}

yasf::Value::ptr Color::to_yasf_value() const
{
  return yasf::ser<std::string>(to_string());
}

////////////////////////////////////////////////////////////////////////////////
// foo
//

bee::OrError<foo> foo::of_yasf_value(const yasf::Value::ptr& value)
{
  if (!value->is_list()) {
    return PH::err("$: Expected list for record", (value));
  }

  std::optional<std::string> output_bar;
  std::optional<std::vector<std::string>> output_eggs;
  std::optional<bool> output_bool_field;
  std::optional<int64_t> output_int_field;
  std::optional<int64_t> output_opt_field;
  std::optional<std::vector<std::string>> output_opt_vec_field;
  std::optional<double> output_float_field;
  std::optional<std::vector<int64_t>> output_vector_int_field;
  std::optional<int_or_str> output_variant;
  std::optional<Color> output_color;

  for (const auto& element : value->list()) {
    if (!element->is_key_value()) {
      return PH::err("Expected a key value as a record element", element);
    }

    const auto& kv = element->key_value();
    const std::string& name = kv.key;
    if (name == "bar") {
      if (output_bar.has_value()) {
        return PH::err("Field 'bar' is defined more than once", element);
      }
      bail_assign(output_bar, yasf::des<std::string>(kv.value));
    } else if (name == "eggs") {
      if (output_eggs.has_value()) {
        return PH::err("Field 'eggs' is defined more than once", element);
      }
      bail_assign(output_eggs, yasf::des<std::vector<std::string>>(kv.value));
    } else if (name == "bool_field") {
      if (output_bool_field.has_value()) {
        return PH::err("Field 'bool_field' is defined more than once", element);
      }
      bail_assign(output_bool_field, PH::to_bool(kv.value));
    } else if (name == "int_field") {
      if (output_int_field.has_value()) {
        return PH::err("Field 'int_field' is defined more than once", element);
      }
      bail_assign(output_int_field, PH::to_int(kv.value));
    } else if (name == "opt_field") {
      if (output_opt_field.has_value()) {
        return PH::err("Field 'opt_field' is defined more than once", element);
      }
      bail_assign(output_opt_field, PH::to_int(kv.value));
    } else if (name == "opt_vec_field") {
      if (output_opt_vec_field.has_value()) {
        return PH::err(
          "Field 'opt_vec_field' is defined more than once", element);
      }
      bail_assign(
        output_opt_vec_field, yasf::des<std::vector<std::string>>(kv.value));
    } else if (name == "float_field") {
      if (output_float_field.has_value()) {
        return PH::err(
          "Field 'float_field' is defined more than once", element);
      }
      bail_assign(output_float_field, PH::to_float(kv.value));
    } else if (name == "vector_int_field") {
      if (output_vector_int_field.has_value()) {
        return PH::err(
          "Field 'vector_int_field' is defined more than once", element);
      }
      bail_assign(
        output_vector_int_field, yasf::des<std::vector<int64_t>>(kv.value));
    } else if (name == "variant") {
      if (output_variant.has_value()) {
        return PH::err("Field 'variant' is defined more than once", element);
      }
      bail_assign(output_variant, yasf::des<int_or_str>(kv.value));
    } else if (name == "color") {
      if (output_color.has_value()) {
        return PH::err("Field 'color' is defined more than once", element);
      }
      bail_assign(output_color, yasf::des<Color>(kv.value));
    } else {
      return PH::err("No such field in record of type foo", element);
    }
  }

  if (!output_bar.has_value()) {
    return PH::err("Field 'bar' not defined", value);
  }
  if (!output_eggs.has_value()) {
    return PH::err("Field 'eggs' not defined", value);
  }
  if (!output_bool_field.has_value()) {
    return PH::err("Field 'bool_field' not defined", value);
  }
  if (!output_int_field.has_value()) {
    return PH::err("Field 'int_field' not defined", value);
  }
  if (!output_opt_vec_field.has_value()) { output_opt_vec_field.emplace(); }
  if (!output_float_field.has_value()) {
    return PH::err("Field 'float_field' not defined", value);
  }
  if (!output_vector_int_field.has_value()) {
    return PH::err("Field 'vector_int_field' not defined", value);
  }
  if (!output_variant.has_value()) {
    return PH::err("Field 'variant' not defined", value);
  }

  return foo{
    .bar = std::move(*output_bar),
    .eggs = std::move(*output_eggs),
    .bool_field = std::move(*output_bool_field),
    .int_field = std::move(*output_int_field),
    .opt_field = std::move(output_opt_field),
    .opt_vec_field = std::move(*output_opt_vec_field),
    .float_field = std::move(*output_float_field),
    .vector_int_field = std::move(*output_vector_int_field),
    .variant = std::move(*output_variant),
    .color = std::move(output_color),
    .location = value->location(),
  };
}

yasf::Value::ptr foo::to_yasf_value() const
{
  std::vector<yasf::Value::ptr> fields;
  PH::push_back_field(fields, yasf::ser<std::string>(bar), "bar");
  PH::push_back_field(
    fields, yasf::ser<std::vector<std::string>>(eggs), "eggs");
  PH::push_back_field(fields, PH::of_bool(bool_field), "bool_field");
  PH::push_back_field(fields, PH::of_int(int_field), "int_field");
  if (opt_field.has_value()) {
    PH::push_back_field(fields, PH::of_int(*opt_field), "opt_field");
  }
  if (!opt_vec_field.empty()) {
    PH::push_back_field(
      fields,
      yasf::ser<std::vector<std::string>>(opt_vec_field),
      "opt_vec_field");
  }
  PH::push_back_field(fields, PH::of_float(float_field), "float_field");
  PH::push_back_field(
    fields,
    yasf::ser<std::vector<int64_t>>(vector_int_field),
    "vector_int_field");
  PH::push_back_field(fields, yasf::ser<int_or_str>(variant), "variant");
  if (color.has_value()) {
    PH::push_back_field(fields, yasf::ser<Color>(*color), "color");
  }
  return yasf::Value::create_list(std::move(fields), std::nullopt);
}

////////////////////////////////////////////////////////////////////////////////
// top
//

bee::OrError<top> top::of_yasf_value(const yasf::Value::ptr& value)
{
  if (!value->is_list()) {
    return PH::err("$: Expected list for record", (value));
  }

  std::optional<std::vector<foo>> output_foos;

  for (const auto& element : value->list()) {
    if (!element->is_key_value()) {
      return PH::err("Expected a key value as a record element", element);
    }

    const auto& kv = element->key_value();
    const std::string& name = kv.key;
    if (name == "foos") {
      if (output_foos.has_value()) {
        return PH::err("Field 'foos' is defined more than once", element);
      }
      bail_assign(output_foos, yasf::des<std::vector<foo>>(kv.value));
    } else {
      return PH::err("No such field in record of type top", element);
    }
  }

  if (!output_foos.has_value()) { output_foos.emplace(); }
  return top{
    .foos = std::move(*output_foos),
  };
}

yasf::Value::ptr top::to_yasf_value() const
{
  std::vector<yasf::Value::ptr> fields;
  if (!foos.empty()) {
    PH::push_back_field(fields, yasf::ser<std::vector<foo>>(foos), "foos");
  }
  return yasf::Value::create_list(std::move(fields), std::nullopt);
}

} // namespace test_parser

// olint-allow: missing-package-namespace
