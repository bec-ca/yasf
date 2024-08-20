
#include "dsl_token_kind.generated.hpp"

namespace yasf::dsl {}

const char* exc::Enum<yasf::dsl::TokenKind>::to_string(yasf::dsl::TokenKind e)
{
  switch (e) {
  case TokenKind::Comment:
    return "Comment";
  case TokenKind::Empty:
    return "Empty";
  case TokenKind::Eof:
    return "Eof";
  case TokenKind::Identifier:
    return "Identifier";
  case TokenKind::Number:
    return "Number";
  case TokenKind::String:
    return "String";
  case TokenKind::BraceOpen:
    return "BraceOpen";
  case TokenKind::BraceClose:
    return "BraceClose";
  case TokenKind::Semicolon:
    return "Semicolon";
  case TokenKind::Period:
    return "Period";
  case TokenKind::Record:
    return "Record";
  case TokenKind::Variant:
    return "Variant";
  case TokenKind::Enum:
    return "Enum";
  case TokenKind::Namespace:
    return "Namespace";
  case TokenKind::Optional:
    return "Optional";
  case TokenKind::Attribute:
    return "Attribute";
  }
}
