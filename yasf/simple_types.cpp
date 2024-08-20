#include "simple_types.hpp"

#include <string>

#include "bee/format.hpp"

using std::make_shared;
using std::nullopt;
using std::optional;
using std::set;
using std::string;

namespace yasf {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Float
//

struct Float final : public Type {
  virtual ~Float() {}

  virtual const string& type_name() const override
  {
    static string name = "double";
    return name;
  }
  virtual string parse_expr(const string& value) const override;
  virtual string unparse_expr(const string& value) const override;
  virtual string unparse_expr_optional(const string& value) const override;
  virtual optional<string> default_value() const override;
  virtual bool is_container() const override { return false; }
  virtual bool has_optional_default() const override { return false; }

  virtual set<string> additional_headers() const override { return {}; }
};

string Float::parse_expr(const string& value) const
{
  return F("PH::to_float($)", value);
}

string Float::unparse_expr(const string& value) const
{
  return F("PH::of_float($)", value);
}

string Float::unparse_expr_optional(const string& value) const
{
  return F("PH::of_float_optional($)", value);
}

optional<string> Float::default_value() const { return "0"; }

////////////////////////////////////////////////////////////////////////////////
// Int
//

struct Int final : public Type {
  virtual ~Int() {}

  virtual const string& type_name() const override
  {
    static string name = "int64_t";
    return name;
  }
  virtual string parse_expr(const string& value) const override;
  virtual string unparse_expr(const string& value) const override;
  virtual string unparse_expr_optional(const string& value) const override;
  virtual optional<string> default_value() const override;
  virtual bool is_container() const override { return false; }
  virtual bool has_optional_default() const override { return false; }

  virtual set<string> additional_headers() const override { return {}; }
};

string Int::parse_expr(const string& value) const
{
  return F("PH::to_int($)", value);
}

string Int::unparse_expr(const string& value) const
{
  return F("PH::of_int($)", value);
}

string Int::unparse_expr_optional(const string& value) const
{
  return F("PH::of_int_optional($)", value);
}

optional<string> Int::default_value() const { return "0"; }

////////////////////////////////////////////////////////////////////////////////
// Bool
//

struct Bool final : public Type {
  virtual ~Bool() {}

  virtual const string& type_name() const override
  {
    static string name = "bool";
    return name;
  }
  virtual string parse_expr(const string& value) const override;
  virtual string unparse_expr(const string& value) const override;
  virtual string unparse_expr_optional(const string& value) const override;
  virtual optional<string> default_value() const override;
  virtual bool is_container() const override { return false; }
  virtual bool has_optional_default() const override { return true; }

  virtual set<string> additional_headers() const override { return {}; }
};

string Bool::parse_expr(const string& value) const
{
  return F("PH::to_bool($)", value);
}

string Bool::unparse_expr(const string& value) const
{
  return F("PH::of_bool($)", value);
}

string Bool::unparse_expr_optional(const string& value) const
{
  return F("PH::of_bool_optional($)", value);
}

optional<string> Bool::default_value() const { return "false"; }

////////////////////////////////////////////////////////////////////////////////
// String
//

struct String final : public Type {
  virtual ~String() {}

  virtual const string& type_name() const override
  {
    static string name = "std::string";
    return name;
  }
  virtual string parse_expr(const string& value) const override;
  virtual string unparse_expr(const string& value) const override;
  virtual string unparse_expr_optional(const string& value) const override;
  virtual optional<string> default_value() const override;
  virtual bool is_container() const override { return false; }
  virtual bool has_optional_default() const override { return false; }

  virtual set<string> additional_headers() const override { return {}; }

 private:
};

string String::parse_expr(const string& value) const
{
  return F("yasf::des<$>($)", type_name(), value);
}

string String::unparse_expr(const string& value) const
{
  return F("yasf::ser($)", value);
}

string String::unparse_expr_optional(const string& value) const
{
  return unparse_expr(value);
}

optional<string> String::default_value() const { return nullopt; }

} // namespace

////////////////////////////////////////////////////////////////////////////////
// simple_type_t
//

SimpleType::operator Type::ptr() const
{
  switch (_kind) {
  case Kind::Int:
    return make_shared<Int>();
  case Kind::Float:
    return make_shared<Float>();
  case Kind::Bool:
    return make_shared<Bool>();
  case Kind::Str:
    return make_shared<String>();
  }
  assert(false);
}

} // namespace yasf
