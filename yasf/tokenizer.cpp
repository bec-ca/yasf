#include "tokenizer.hpp"

using std::string;
using std::vector;

namespace yasf {

////////////////////////////////////////////////////////////////////////////////
// TokenBase class
//

const string& TokenBase::value() const { return _value; }

TokenBase::TokenBase(string value) : _value(std::move(value)) {}

////////////////////////////////////////////////////////////////////////////////
// KeyToken
//

KeyToken::KeyToken(string value) : TokenBase(std::move(value)) {}

string KeyToken::to_string() const { return F("KeyToken($)", value()); }

////////////////////////////////////////////////////////////////////////////////
// AtomToken
//

AtomToken::AtomToken(string value) : TokenBase(std::move(value)) {}

string AtomToken::to_string() const { return F("AtomToken($)", value()); }

////////////////////////////////////////////////////////////////////////////////
// IndentToken
//

IndentToken::IndentToken(string value) : TokenBase(std::move(value)) {}

string IndentToken::to_string() const { return F("IndentToken($)", indent()); }

int IndentToken::indent() const { return value().size(); }

////////////////////////////////////////////////////////////////////////////////
// BulletToken
//

BulletToken::BulletToken(string value) : TokenBase(std::move(value)) {}

string BulletToken::to_string() const { return F("BulletToken()"); }

////////////////////////////////////////////////////////////////////////////////
// Token
//

Token::Token(const value_type& value, Location loc)
    : _location(loc), _variant(value)
{}

const Location& Token::location() const { return _location; }

const Token::value_type& Token::variant() const { return _variant; }

const string& Token::value() const
{
  return visit(
    [](const auto& value) -> const string& { return value.value(); },
    variant());
}

bool Token::is_key() const { return holds_alternative<KeyToken>(_variant); }
bool Token::is_atom() const { return holds_alternative<AtomToken>(_variant); }
bool Token::is_indent() const
{
  return holds_alternative<IndentToken>(_variant);
}
bool Token::is_bullet() const
{
  return holds_alternative<BulletToken>(_variant);
}

const KeyToken& Token::key_token() const { return get<KeyToken>(_variant); }
const AtomToken& Token::atom_token() const { return get<AtomToken>(_variant); }
const IndentToken& Token::indent_token() const
{
  return get<IndentToken>(_variant);
}

const BulletToken& Token::bullet_token() const
{
  return get<BulletToken>(_variant);
}

string Token::to_string() const
{
  return visit(
    [&](const auto& t) -> string { return F("Token($ $)", t, location()); },
    variant());
}

////////////////////////////////////////////////////////////////////////////////
// Tokenizer
//

struct CharInfo {
  char ch;
  int line;
  int col;
};

struct DocumentReader {
 public:
  DocumentReader(const string& document) : _document(document) {}

  CharInfo peek() const
  {
    assert(!eod());
    return CharInfo{
      .ch = _document[_position],
      .line = _line,
      .col = _col,
    };
  }

  CharInfo pop()
  {
    CharInfo ret = peek();
    if (ret.ch == '\n') {
      _col = 1;
      _line++;
    } else {
      _col++;
    }
    _position++;
    return ret;
  }

  bool eod() const { return _position >= _document.size(); }

 private:
  const string& _document;
  size_t _position = 0;

  int _line = 1;
  int _col = 1;
};

enum class State {
  text_token,
  indent,
  space,
};

Token parse_token(State state, const string& content, Location loc)
{
  switch (state) {
  case State::indent:
    return Token(IndentToken(content), loc);
  case State::text_token:
    if (content.back() == ':') {
      return Token(KeyToken(content.substr(0, content.size() - 1)), loc);
    } else if (content == "*") {
      return Token(BulletToken(content), loc);
    } else {
      return Token(AtomToken(content), loc);
    }
  case State::space:
    assert(false);
  }
  assert(false);
}

enum class CharType {
  text,
  space,
  new_line,
};

CharType get_char_type(char c)
{
  if (c == '\n') {
    return CharType::new_line;
  } else if (isspace(c)) {
    return CharType::space;
  } else {
    return CharType::text;
  }
}

bee::OrError<vector<Token>> Tokenizer::tokenize(
  const bee::FilePath& filename, const string& content)
{
  vector<Token> output;
  State state = State::indent;
  DocumentReader reader(content);
  string current_token;
  int current_token_line = 1;
  int current_token_col = 1;
  bool has_current_token = true;

  State new_state = state;

  auto maybe_push_current_token = [&]() {
    if (!has_current_token) return;
    output.push_back(parse_token(
      state,
      current_token,
      Location{
        .line = current_token_line,
        .col = current_token_col,
        .filename = filename}));
    current_token_line = -1;
    current_token_col = -1;
    has_current_token = false;
    new_state = State::space;
    current_token.clear();
  };

  while (!reader.eod()) {
    CharInfo info = reader.pop();
    auto char_type = get_char_type(info.ch);
    new_state = state;

    auto start_indent = [&]() {
      maybe_push_current_token();
      if (!reader.eod()) {
        current_token.clear();
        new_state = State::indent;
        current_token_line = reader.peek().line;
        current_token_col = reader.peek().col;
        has_current_token = true;
      }
    };

    auto start_text_token = [&]() {
      maybe_push_current_token();
      current_token.clear();
      new_state = State::text_token;
      current_token += info.ch;
      current_token_line = info.line;
      current_token_col = info.col;
      has_current_token = true;
    };

    auto add_to_current_token = [&]() { current_token += info.ch; };

    switch (state) {
    case State::indent:
      switch (char_type) {
      case CharType::space:
        add_to_current_token();
        break;
      case CharType::new_line:
        start_indent();
        break;
      case CharType::text:
        start_text_token();
        break;
      }
      break;
    case State::space:
      switch (char_type) {
      case CharType::space:
        break;
      case CharType::new_line:
        start_indent();
        break;
      case CharType::text:
        start_text_token();
        break;
      }
      break;
    case State::text_token:
      switch (char_type) {
      case CharType::space:
        maybe_push_current_token();
        break;
      case CharType::new_line:
        start_indent();
        break;
      case CharType::text:
        add_to_current_token();
        break;
      }
      break;
    }

    state = new_state;
  }

  maybe_push_current_token();

  return output;
}

} // namespace yasf
