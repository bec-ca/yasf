#include "config_parser.hpp"

#include "bee/format_memory.hpp"
#include "bee/testing.hpp"

using bee::print_line;
using std::string;

namespace yasf {
namespace {

const string example = R"(
foo:
  bar:
    yo
    here
    list
  there:
    ah
    haha
  empty_list:

nested_records:
  *
    foo:
      bar
      drink
  atom_no_change
  *
    key1:
      1
    key2:
      2
  foo:
    this_is_also_a_valid_syntax
    and this too

other:
  thing
  thang
empty_list_at_the_end:)";

TEST(parse)
{
  must(config, ConfigParser::parse_from_string("", example));

  print_line(config->to_string_hum());
}

const string new_example = R"(
foo: bar
  next
other: a b c
list:
  * key: 1
  * key: 2
not_list:
  key: 1
  key: 2
super_nested: key1: key2: key3: a b foo: a * key1: 4 key2: 6
end:
)";

TEST(parse_new_syntax)
{
  must(config, ConfigParser::parse_from_string("", new_example));

  print_line("output:");
  print_line(config->to_string_hum());
}

const string error_example = R"(
foo:
  bar:
    yo:
  there:
    ah
     haha

other:
  thing
  thang
)";

TEST(parse_error)
{
  auto resp = ConfigParser::parse_from_string("", error_example);

  if (resp.is_error()) {
    print_line(resp.error());
  } else {
    print_line("Expected error, but didn't get, parsed doc is:");
    print_line(resp.value()->to_string_hum());
  }
}

} // namespace
} // namespace yasf
