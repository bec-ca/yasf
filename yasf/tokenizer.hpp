#pragma once

#include <string>
#include <variant>
#include <vector>

#include "location.hpp"

#include "bee/file_path.hpp"
#include "bee/or_error.hpp"

namespace yasf {

struct TokenBase {
 public:
  TokenBase(std::string value);
  const std::string& value() const;

 private:
  std::string _value;
};

struct KeyToken : public TokenBase {
  KeyToken(std::string value);
  std::string to_string() const;
};

struct AtomToken : public TokenBase {
  AtomToken(std::string value);
  std::string to_string() const;
};

struct IndentToken : public TokenBase {
  IndentToken(std::string value);
  std::string to_string() const;

  int indent() const;
};

struct BulletToken : public TokenBase {
  BulletToken(std::string value);
  std::string to_string() const;

  int indent() const;
};

struct Token {
 public:
  using value_type =
    std::variant<KeyToken, AtomToken, IndentToken, BulletToken>;
  const Location& location() const;
  const value_type& variant() const;

  const KeyToken& key_token() const;
  const AtomToken& atom_token() const;
  const IndentToken& indent_token() const;
  const BulletToken& bullet_token() const;

  bool is_key() const;
  bool is_atom() const;
  bool is_indent() const;
  bool is_bullet() const;

  const std::string& value() const;

  std::string to_string() const;

  Token(const value_type& value, Location loc);

 private:
  Location _location;
  value_type _variant;
};

struct Tokenizer {
  static bee::OrError<std::vector<Token>> tokenize(
    const bee::FilePath& filename, const std::string& conent);
};

} // namespace yasf
