#include "cof.hpp"

#include "bee/file_reader.hpp"
#include "bee/file_writer.hpp"
#include "yasf/value.hpp"

using std::nullopt;
using std::string;
using std::vector;

namespace yasf {
namespace {

string escape(const string& value)
{
  if (value.empty()) { return "''"; }
  bool need_quoting = false;
  for (char c : value) {
    if (c == ':' || c == ' ' || c == ']') {
      need_quoting = true;
      break;
    }
  }

  string output;

  if (need_quoting) output += '\'';
  for (char c : value) {
    if (c == '\'' || c == '\\') { output += '\\'; }
    output += c;
  }
  if (need_quoting) output += '\'';
  return output;
}

struct StringReader {
 public:
  StringReader(const string& str) : _str(str) {}

  char peek() const
  {
    assert(!eof());
    return _str[_position];
  }

  char pop()
  {
    char c = peek();
    _position++;
    return c;
  }

  bool eof() const { return _position >= _str.size(); }

  void skip_spaces()
  {
    while (!eof() && peek() == ' ') { pop(); }
  }

 private:
  const string& _str;
  size_t _position = 0;
};

bee::OrError<Value::ptr> parse_element(StringReader& reader);

bee::OrError<Value::ptr> parse_list(StringReader& reader)
{
  if (reader.eof() || reader.peek() != '[') {
    return bee::Error("Malformed list");
  }
  reader.pop();

  vector<Value::ptr> elements;
  while (true) {
    if (reader.eof()) { return bee::Error("Malformed list"); }
    char c = reader.peek();
    if (c == ']') {
      reader.pop();
      break;
    }
    if (c == ' ') {
      reader.pop();
      continue;
    }
    bail(el, parse_element(reader));
    elements.push_back(std::move(el));
  }

  return Value::create_list(elements, nullopt);
}

bee::OrError<Value::ptr> parse_atom_or_key(StringReader& reader)
{
  reader.skip_spaces();
  if (reader.eof()) { return bee::Error("Malformed element"); }
  bool quoted = false;
  if (reader.peek() == '\'') {
    quoted = true;
    reader.pop();
  }

  string atom;
  while (true) {
    if (reader.eof() && quoted) return bee::Error("Malformed atom");
    if (reader.eof()) break;
    char c = reader.peek();
    if (c == '\\') {
      reader.pop();
      if (reader.eof()) { return bee::Error("Malformed atom"); }
    } else if ((c == ' ' || c == ':' || c == ']') && !quoted) {
      break;
    } else if (c == '\'' && quoted) {
      reader.pop();
      break;
    }
    atom += reader.pop();
  }

  reader.skip_spaces();
  if (!reader.eof() && reader.peek() == ':') {
    reader.pop();
    bail(value, parse_element(reader));
    return Value::create_key_value(atom, value, nullopt);
  } else {
    return Value::create_atom(atom, nullopt);
  }
}

bee::OrError<Value::ptr> parse_element(StringReader& reader)
{
  reader.skip_spaces();
  if (reader.eof()) { return bee::Error("Malformed element"); }
  if (reader.peek() == '[') {
    return parse_list(reader);
  } else {
    return parse_atom_or_key(reader);
  }
}

} // namespace

bee::OrError<Value::ptr> Cof::raw_parse_string(const string& str)
{
  StringReader reader(str);
  bail(output, parse_element(reader));
  reader.skip_spaces();
  // if (!reader.eof()) { return bee::Error("Unexpected trailing data"); }
  return output;
}

bee::OrError<Value::ptr> Cof::raw_parse_file(const bee::FilePath& filename)
{
  bail(content, bee::FileReader::read_file(filename));
  return raw_parse_string(content);
}

bee::OrError<> Cof::raw_to_file(
  const bee::FilePath& filename, const Value::ptr& value)
{
  return bee::FileWriter::write_file(filename, to_string(value));
}

string Cof::to_string(const Value::ptr& value)
{
  if (value->is_atom()) {
    return escape(value->atom());
  } else if (value->is_list()) {
    string output;
    output += '[';
    for (const auto& el : value->list()) {
      if (output.size() > 1) output += ' ';
      output += to_string(el);
    }
    output += ']';
    return output;
  } else if (value->is_key_value()) {
    string output;
    output += escape(value->key_value().key);
    output += ':';
    output += to_string(value->key_value().value);
    return output;
  } else {
    assert(false);
  }
}

} // namespace yasf

std::string bee::to_string_t<yasf::Value::ptr>::convert(
  const yasf::Value::ptr& value)
{
  return yasf::Cof::serialize(value);
}
