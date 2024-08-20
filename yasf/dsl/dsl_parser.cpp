#include "dsl_parser.hpp"

#include <variant>

#include "dsl_token_kind.generated.hpp"

#include "bee/file_reader.hpp"
#include "tokenizer/token_reader.hpp"
#include "tokenizer/tokenizer.hpp"
#include "tokenizer/typed_token.hpp"

namespace yasf::dsl {
namespace {

using Token = tokenizer::TypedToken<TokenKind>;

auto create_tokenizer()
{
  const std::vector<std::pair<std::string, TokenKind>> operators = {
    {"{", TokenKind::BraceOpen},
    {"}", TokenKind::BraceClose},
    {";", TokenKind::Semicolon},
    {".", TokenKind::Period},
  };
  const std::map<std::string, TokenKind> keywords{
    {"ns", TokenKind::Namespace},
    {"record", TokenKind::Record},
    {"variant", TokenKind::Variant},
    {"optional", TokenKind::Optional},
    {"attr", TokenKind::Attribute},
    {"enum", TokenKind::Enum},
  };
  return tokenizer::TypedTokenizer(operators, keywords);
}

struct ParseError {
 public:
  ParseError(const Token& tok, std::string&& msg)
      : _line(tok.line), _col(tok.col), _msg(std::move(msg))
  {}

  bee::Error to_error(const bee::FilePath& path) const
  {
    return bee::Error::fmt("$:$:$: $", path, _line, _col, _msg);
  }

  int line() const { return _line; }
  int col() const { return _col; }
  const std::string& msg() const { return _msg; }

 private:
  int _line;
  int _col;
  std::string _msg;
};

#define ME(tok, args...) ParseError(tok, (F(args)))

template <class T> using OrParseError = std::variant<T, ParseError>;

#define bail_parse(var, or_error)                                              \
  auto __var##var = (or_error);                                                \
  if (auto v = std::get_if<ParseError>(&__var##var)) [[unlikely]] {            \
    return std::move(*v);                                                      \
  }                                                                            \
  auto& var = std::get<0>(__var##var)

Type parse_type(
  const std::string& first_id, tokenizer::TokenReader<TokenKind>& reader)
{
  const auto& next = reader.peek();
  switch (next.kind) {
  case TokenKind::Identifier: {
    // Parametric type
    reader.skip();
    return {.name = next.content, .parameter = first_id};
  } break;
  default: {
    // Non parametric type
    return {.name = first_id};
  } break;
  }
}

OrParseError<Variant> parse_variant(tokenizer::TokenReader<TokenKind>& reader)
{
  Variant out;
  const auto& name = reader.read();
  if (name.kind != TokenKind::Identifier) {
    return ME(name, "Expected identifier after variant, got $", name.kind);
  }
  out.name = name.content;
  const auto& open_brac = reader.read();
  if (open_brac.kind != TokenKind::BraceOpen) {
    return ME(
      open_brac, "Expected open braces after enum name, got $", open_brac.kind);
  }
  std::vector<Field> fields;
  bool closed = false;
  while (!closed) {
    const auto& token = reader.read();
    switch (token.kind) {
    case TokenKind::BraceClose: {
      closed = true;
    } break;
    case TokenKind::Identifier: {
      int state = 0;
      std::optional<Type> type;
      while (true) {
        const auto& token = reader.read();
        if (state == 0) {
          if (token.kind == TokenKind::Identifier) {
            type = parse_type(token.content, reader);
            state++;
          } else {
            return ME(
              token, "Expected type after field name, got $", token.kind);
          }
        } else if (state == 1) {
          if (token.kind == TokenKind::Semicolon) {
            break;
          } else {
            return ME(
              token,
              "Expected semicolon after end of variant leg declaration, "
              "got $",
              token.kind);
          }
          state++;
        }
      }
      out.legs.push_back({.name = token.content, .type = type.value()});
    } break;
    default: {
      return ME(
        token, "Unexpected token inside variant declaration: $", token.kind);
    }
    }
  }
  return out;
}

OrParseError<Record> parse_record(tokenizer::TokenReader<TokenKind>& reader)
{
  Record out;
  const auto& name = reader.read();
  if (name.kind != TokenKind::Identifier) {
    return ME(name, "Expected identifier after record, got $", name.kind);
  }
  out.name = name.content;
  const auto& open_brac = reader.read();
  if (open_brac.kind != TokenKind::BraceOpen) {
    return ME(
      open_brac, "Expected open braces after enum name, got $", open_brac.kind);
  }
  std::vector<Field> fields;
  bool closed = false;
  while (!closed) {
    const auto& token = reader.read();
    switch (token.kind) {
    case TokenKind::BraceClose: {
      closed = true;
    } break;
    case TokenKind::Attribute: {
      const auto& attr = reader.read();
      const auto& semi = reader.read();
      if (attr.kind != TokenKind::Identifier) {
        return ME(attr, "Expected identifier after attribute");
      }
      if (semi.kind != TokenKind::Semicolon) {
        return ME(semi, "Expected semicolon after attribute");
      }
      const auto& attr_name = attr.content;
      if (attr_name == "location") {
        out.location = true;
      } else {
        return ME(attr, "Unknown attribute '$'", attr_name);
      }
    } break;
    case TokenKind::Identifier: {
      int state = 0;
      std::optional<Type> type;
      bool optional = false;
      while (true) {
        const auto& token = reader.read();
        if (state == 0) {
          if (token.kind == TokenKind::Identifier) {
            type = parse_type(token.content, reader);
            state++;
          } else {
            return ME(
              token, "Expected type after field name, got $", token.kind);
          }
        } else if (state == 1) {
          if (token.kind == TokenKind::Optional) {
            optional = true;
            state++;
          } else if (token.kind == TokenKind::Semicolon) {
            break;
          } else {
            return ME(
              token,
              "Expected optional or semicolon after field type, got $",
              token.kind);
          }
        } else if (state == 2) {
          if (token.kind == TokenKind::Semicolon) {
            break;
          } else {
            return ME(
              token,
              "Expected semicolon after end of field declaration, got $",
              token.kind);
          }
          state++;
        }
      }
      out.fields.push_back(
        {.name = token.content, .type = type.value(), .optional = optional});
    } break;
    default: {
      return ME(
        token, "Unexpected token inside record declaration: $", token.kind);
    }
    }
  }
  return out;
}

OrParseError<Enum> parse_enum(tokenizer::TokenReader<TokenKind>& reader)
{
  Enum out;
  const auto& name = reader.read();
  if (name.kind != TokenKind::Identifier) {
    return ME(name, "Expected identifier after enum, got $", name.kind);
  }
  out.name = name.content;
  const auto& open_brac = reader.read();
  if (open_brac.kind != TokenKind::BraceOpen) {
    return ME(
      open_brac, "Expected open braces after enum name, got $", open_brac.kind);
  }
  bool closed = false;
  while (!closed) {
    const auto& token = reader.read();
    switch (token.kind) {
    case TokenKind::BraceClose: {
      closed = true;
    } break;
    case TokenKind::Identifier: {
      out.legs.push_back(token.content);
      const auto& tok1 = reader.read();
      if (tok1.kind != TokenKind::Semicolon) {
        return ME(tok1, "Expected semicolon after enum leg: $", tok1.kind);
      }
    } break;
    default: {
      return ME(
        token, "Unexpected token inside enum declaration: $", token.kind);
    }
    }
  }
  return out;
}

OrParseError<TypeDecl> parse_type(
  TokenKind kind, tokenizer::TokenReader<TokenKind>& reader)
{
  switch (kind) {
  case TokenKind::Enum: {
    bail_parse(out, parse_enum(reader));
    return out;
  } break;
  case TokenKind::Record: {
    bail_parse(out, parse_record(reader));
    return out;
  } break;
  case TokenKind::Variant: {
    bail_parse(out, parse_variant(reader));
    return out;
  } break;
  default:
    raise_error("Not a type");
  }
}

OrParseError<Code> compile_impl(
  const bee::FilePath& src_path, tokenizer::TokenReader<TokenKind>& reader)
{
  bool has_ns = false;
  Namespace ns;
  std::vector<TypeDecl> types;
  while (!reader.eof()) {
    const auto& tok = reader.read();
    switch (tok.kind) {
    case TokenKind::Namespace: {
      if (has_ns) { return ME(tok, "Namespace declared twice"); }
      while (true) {
        const auto& tok2 = reader.read();
        if (tok2.kind != TokenKind::Identifier) {
          return ME(tok2, "Expected Identifier after 'ns'");
        }
        ns.parts.push_back(tok2.content);
        const auto next = reader.peek();
        if (next.kind != TokenKind::Period) { break; }
        reader.skip();
      }
      has_ns = true;
    } break;
    case TokenKind::Enum:
    case TokenKind::Record:
    case TokenKind::Variant: {
      bail_parse(out, parse_type(tok.kind, reader));
      types.push_back(std::move(out));
    } break;
    default:
      return ME(tok, "Unexpected token: $", tok);
    }
  }
  if (!has_ns) { return ME(reader.peek(), "Namespace not declared"); }
  return Code{.src = src_path, .ns = std::move(ns), .types = std::move(types)};
}

bee::OrError<Code> compile_wrapper(
  const bee::FilePath& src_path, const std::string_view& content)
{
  auto tok = create_tokenizer();
  bail(tokens, tok.tokenize(content, src_path));
  tokenizer::TokenReader reader(tokens, true);
  auto ret = compile_impl(src_path, reader);
  if (auto err = std::get_if<ParseError>(&ret)) {
    return err->to_error(src_path);
  } else {
    return std::get<Code>(std::move(ret));
  }
}

} // namespace

bee::OrError<Code> DslParser::parse(
  const bee::FilePath& src_path,
  const std::optional<std::string_view>& content_opt)
{
  if (content_opt.has_value()) {
    return compile_wrapper(src_path, *content_opt);
  } else {
    bail(content, bee::FileReader::read_file(src_path));
    return compile_wrapper(src_path, content);
  }
}

} // namespace yasf::dsl
