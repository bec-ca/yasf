#include "value.hpp"

#include <cassert>

using std::optional;
using std::string;
using std::vector;

namespace yasf {
namespace {

string indent(int level)
{
  string output;
  for (int i = 0; i < level; i++) { output += "  "; }
  return output;
}

string pretty_print(const Value& value, int level, bool in_list)
{
  string output;

  if (value.is_list()) {
    if (in_list) {
      output += indent(level);
      output += "*\n";
      level++;
    }
    for (const auto& element : value.list()) {
      output += pretty_print(*element, level, true);
      if (!in_list && level == 0) { output += "\n"; }
    }
  } else if (value.is_key_value()) {
    const auto& kv = value.key_value();
    output += indent(level);
    output += kv.key;
    output += ":";
    if (
      kv.value->is_list() && kv.value->list().size() == 1 &&
      kv.value->list()[0]->is_atom()) {
      output += " ";
      output += kv.value->list()[0]->atom();
      output += "\n";
    } else if (kv.value->is_atom()) {
      output += " ";
      output += kv.value->atom();
      output += "\n";
    } else {
      output += "\n";
      output += pretty_print(*kv.value, level + 1, false);
    }
  } else if (value.is_atom()) {
    output += indent(level);
    output += value.atom();
    output += "\n";
  } else {
    assert(false);
  }

  return output;
}

} // namespace

bool Value::is_list() const { return holds_alternative<List>(_variant); }
bool Value::is_atom() const { return holds_alternative<Atom>(_variant); }
bool Value::is_key_value() const
{
  return holds_alternative<KeyValue>(_variant);
}

const vector<Value::ptr>& Value::list() const
{
  assert((is_list()) && "Called list on value that is not a list");
  return get<List>(_variant).values;
}

const string& Value::atom() const
{
  assert((is_atom()) && "Called atom on value that is not an atom");
  return get<Atom>(_variant).value;
}

const Value::KeyValue& Value::key_value() const
{
  assert(
    (is_key_value()) && "Called key_value on value that is not a key_value");
  return get<KeyValue>(_variant);
}

Value::ptr Value::find_key_in_list(const string& key) const
{
  for (const auto& element : list()) {
    if (element->is_key_value() && element->key_value().key == key) {
      return element->key_value().value;
    }
  }
  return nullptr;
}

string Value::to_string_hum() const { return pretty_print(*this, 0, false); }

const optional<Location>& Value::location() const { return _loc; }

} // namespace yasf
