#include "config_parser.hpp"

#include <cassert>
#include <fstream>
#include <string>
#include <variant>
#include <vector>

#include "bee/file_reader.hpp"
#include "bee/util.hpp"
#include "tokenizer.hpp"

using std::nullopt;
using std::optional;
using std::string;
using std::vector;

namespace yasf {

struct TokenReader {
 public:
  TokenReader(const vector<Token>& tokens) : _tokens(tokens), _position(0) {}

  const Token& peek() const
  {
    _assert_not_empty();
    return _tokens[_position];
  }

  const Token& pop()
  {
    _assert_not_empty();
    const Token& out = peek();
    _position++;
    return out;
  }

  bool empty() const { return _position >= _tokens.size(); }

  optional<Location> location() const
  {
    if (empty()) { return nullopt; }
    return peek().location();
  }

  void skip_useless_indents()
  {
    while (_position + 1 < _tokens.size() && _tokens[_position].is_indent() &&
           _tokens[_position + 1].is_indent()) {
      _position++;
    }
  }

 private:
  void _assert_not_empty() const
  {
    assert(_position < _tokens.size() && "past end of document");
  }
  const vector<Token>& _tokens;
  size_t _position;
};

bee::OrError<Value::ptr> parse_list(TokenReader& reader, int parent_indent);

bee::OrError<Value::ptr> parse_key_value(TokenReader& reader, int parent_indent)
{
  reader.skip_useless_indents();

  int current_indent = parent_indent;
  if (reader.peek().is_indent()) {
    const Token& token = reader.pop();
    current_indent = token.indent_token().indent();
    if (current_indent <= parent_indent) {
      return bee::Error::format(
        "$: Key should be either on the same line as parent or it should "
        "be "
        "more indented than parent",
        token.location().hum());
    }
  } // if there are no indent token, the key is on the same line as parent

  const Token& token = reader.pop();
  if (!token.is_key()) {
    return bee::Error::format(
      "$: Expected key token", reader.peek().location().hum());
  }
  const KeyToken& key_token = token.key_token();
  string key = key_token.value();

  bail(value, parse_list(reader, current_indent));
  return Value::create_key_value(
    std::move(key), std::move(value), token.location());
}

bee::OrError<Value::ptr> parse_list(TokenReader& reader, int parent_indent)
{
  vector<Value::ptr> values;
  int current_indent = parent_indent;

  bool on_same_line = true;

  auto loc = reader.location();

  while (!reader.empty()) {
    reader.skip_useless_indents();
    if (reader.peek().is_indent()) {
      const Token& token = reader.peek();
      int new_indent = token.indent_token().indent();
      if (current_indent == -1) {
        if (new_indent <= parent_indent) { break; }
        current_indent = new_indent;
        on_same_line = false;
      } else if (on_same_line) {
        on_same_line = false;
        if (new_indent <= parent_indent) break;
        current_indent = new_indent;
      } else {
        if (new_indent < current_indent) {
          break;
        } else if (new_indent > current_indent) {
          return bee::Error::format(
            "$: Unexpected indentation, expected $ spaces, got $",
            token.location().hum(),
            current_indent,
            new_indent);
        }
      }
      reader.pop();
    } else if (reader.peek().is_key()) {
      if (current_indent == -1) {
        on_same_line = true;
        current_indent = parent_indent;
      }
      bail(result, parse_key_value(reader, current_indent));
      values.push_back(std::move(result));
    } else if (reader.peek().is_atom()) {
      const Token& token = reader.pop();
      values.push_back(Value::create_atom(token.value(), token.location()));
    } else if (reader.peek().is_bullet()) {
      reader.pop();
      bail(result, parse_list(reader, current_indent));
      values.push_back(std::move(result));
    } else {
      assert(false && "Unexpect token");
    }
  }

  return Value::create_list(std::move(values), loc);
}

bee::OrError<Value::ptr> parse_from_tokens(const vector<Token>& tokens)
{
  TokenReader reader(tokens);
  return parse_list(reader, -1);
}

bee::OrError<Value::ptr> ConfigParser::parse_from_string(
  const string& filename, const string& content)
{
  bail(tokens, Tokenizer::tokenize(filename, content));
  return parse_from_tokens(tokens);
}

bee::OrError<Value::ptr> ConfigParser::parse_from_file(const string& filename)
{
  bail(content, bee::FileReader::read_file(bee::FilePath::of_string(filename)));
  return parse_from_string(filename, content);
}

} // namespace yasf
