#include "external_type.hpp"

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
// ExternalType
//

struct ExternalType final : public Type {
 public:
  ExternalType(
    string&& name,
    optional<string>&& header,
    optional<string>&& serialize_header)
      : _name(std::move(name)),
        _header(header),
        _serialize_header(serialize_header)
  {}
  virtual ~ExternalType() {}

  virtual const string& type_name() const override { return _name; }
  virtual string parse_expr(const string& value) const override;
  virtual string unparse_expr(const string& value) const override;
  virtual string unparse_expr_optional(const string& value) const override;
  virtual optional<string> default_value() const override;
  virtual bool is_container() const override { return false; }
  virtual bool has_optional_default() const override { return false; }

  virtual set<string> additional_headers() const override
  {
    if (_header.has_value()) {
      return {*_header};
    } else {
      return {};
    }
  }

  virtual set<string> additional_serialize_headers() const override
  {
    if (_serialize_header.has_value()) {
      return {*_serialize_header};
    } else {
      return {};
    }
  }

 private:
  const string _name;
  const std::optional<string> _header;
  const std::optional<string> _serialize_header;
};

string ExternalType::parse_expr(const string& value) const
{
  return F("yasf::des<$>($)", _name, value);
}

string ExternalType::unparse_expr(const string& value) const
{
  return F("yasf::ser<$>($)", _name, value);
}

string ExternalType::unparse_expr_optional(const string& value) const
{
  return unparse_expr(value);
}

optional<string> ExternalType::default_value() const { return nullopt; }

} // namespace

GenericExternalType::operator Type::ptr() const
{
  return make_shared<ExternalType>(_name, _header, _serialize_header);
}

} // namespace yasf
