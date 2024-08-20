
#pragma once

#include <array>
#include <cstdint>

#include "exc/enum.hpp"

namespace yasf::dsl {

enum class TokenKind {
  Comment,
  Empty,
  Eof,
  Identifier,
  Number,
  String,
  BraceOpen,
  BraceClose,
  Semicolon,
  Period,
  Record,
  Variant,
  Enum,
  Namespace,
  Optional,
  Attribute,

};

}

template <> struct exc::Enum<yasf::dsl::TokenKind> {
 public:
  using TokenKind = yasf::dsl::TokenKind;

  static const char* to_string(TokenKind t);

  static constexpr size_t count = 16;

  static constexpr std::array<TokenKind, count> all = {
    TokenKind::Comment,
    TokenKind::Empty,
    TokenKind::Eof,
    TokenKind::Identifier,
    TokenKind::Number,
    TokenKind::String,
    TokenKind::BraceOpen,
    TokenKind::BraceClose,
    TokenKind::Semicolon,
    TokenKind::Period,
    TokenKind::Record,
    TokenKind::Variant,
    TokenKind::Enum,
    TokenKind::Namespace,
    TokenKind::Optional,
    TokenKind::Attribute,

  };
};

static_assert(exc::is_ex_enum<yasf::dsl::TokenKind>);
