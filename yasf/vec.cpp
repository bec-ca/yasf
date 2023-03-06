#include "vec.hpp"

#include "bee/format.hpp"

using bee::format;
using std::nullopt;
using std::optional;
using std::set;
using std::string;

namespace yasf {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Vector
//

struct Vector final : public Type {
 public:
  Vector(const Type::ptr& element_type)
      : element_type(element_type),
        name(format("std::vector<$>", element_type->type_name()))
  {}

  virtual ~Vector() {}

  virtual const string& type_name() const override { return name; }
  virtual string parse_expr(const string& value) const override;
  virtual string unparse_expr(const string& value) const override;
  virtual string unparse_expr_optional(const string& value) const override;
  virtual optional<string> default_value() const override;
  virtual bool is_container() const override { return true; }
  virtual bool has_optional_default() const override { return false; }

  virtual set<string> additional_headers() const override
  {
    return element_type->additional_headers();
  }

 private:
  const Type::ptr element_type;
  const string name;
};

string Vector::parse_expr(const string& value) const
{
  return format("yasf::des<$>($)", type_name(), value);
}

string Vector::unparse_expr(const string& value) const
{
  return format("yasf::ser<$>($)", type_name(), value);
}

string Vector::unparse_expr_optional(const string& value) const
{
  return format(
    "PH::of_vector_optional($, [](const auto& value){ return "
    "$; })",
    value,
    element_type->unparse_expr("value"));
}

optional<string> Vector::default_value() const { return nullopt; }

} // namespace

namespace details {

Type::ptr make_vec(const Type::ptr& element)
{
  return make_shared<Vector>(element);
}

} // namespace details

} // namespace yasf
