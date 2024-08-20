
#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "bee/file_path.hpp"

namespace yasf::dsl {

struct Type {
  std::string name;
  std::optional<std::string> parameter{};
  std::string to_string() const;
};

struct Field {
  std::string name;
  Type type;
  bool optional;
};

struct Record {
  std::string name;
  std::vector<Field> fields;

  bool location = false;

  std::string to_string() const;
};

struct Enum {
  std::string name;
  std::vector<std::string> legs;
  std::string to_string() const;
};

struct Variant {
  struct Leg {
    std::string name;
    Type type;
  };

  std::string name;
  std::vector<Leg> legs;

  std::string to_string() const;
};

using TypeDecl = std::variant<Enum, Record, Variant>;

struct Namespace {
  std::vector<std::string> parts;

  std::string to_string() const;
  std::string to_cpp() const;
};

struct Code {
  bee::FilePath src;
  Namespace ns;

  std::vector<TypeDecl> types;

  std::string to_string() const;
};

} // namespace yasf::dsl
