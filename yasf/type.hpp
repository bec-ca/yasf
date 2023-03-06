#pragma once

#include <memory>
#include <optional>
#include <set>

namespace yasf {

struct Type {
 public:
  using ptr = std::shared_ptr<Type>;

  virtual ~Type();

  virtual const std::string& type_name() const = 0;
  virtual std::string parse_expr(const std::string& exp) const = 0;
  virtual std::string unparse_expr(const std::string& exp) const = 0;
  virtual std::string unparse_expr_optional(const std::string& exp) const = 0;

  virtual std::optional<std::string> default_value() const = 0;
  virtual bool is_container() const = 0;
  virtual bool has_optional_default() const = 0;

  virtual std::set<std::string> additional_headers() const = 0;
};

struct CustomType : public Type {
 public:
  using ptr = std::shared_ptr<CustomType>;

  virtual ~CustomType();
  virtual std::string gen_impl() const = 0;
  virtual std::string gen_decl() const = 0;

 private:
};

} // namespace yasf
