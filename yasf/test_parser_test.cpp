#include "config_parser.hpp"
#include "test_parser.hpp"

#include "bee/format_optional.hpp"
#include "bee/testing.hpp"

using namespace test_parser;
using bee::print_line;
using std::string;

namespace yasf {
namespace {

void run_test(const string& doc)
{
  must(config, ConfigParser::parse_from_string("test.txt", doc));
  print_line("original:");
  print_line(config->to_string_hum());
  must(parsed, top::of_yasf_value(config));
  auto back = parsed.to_yasf_value();
  print_line("round trip:");
  print_line(back->to_string_hum());
  print_line("round trip once more:");
  must(parsed_again, top::of_yasf_value(back));
  print_line(parsed_again.to_yasf_value()->to_string_hum());
  print_line("location: $", config->location());
};

void run_error_test(const string& doc)
{
  must(config, ConfigParser::parse_from_string("foobar.txt", doc));
  auto err = top::of_yasf_value(config);
  if (err.is_error()) {
    print_line(err.error());
  } else {
    print_line("No error!!");
  }
};

TEST(parse)
{
  string example = R"(
foos:
  * 
    bar:
      str
    eggs:
    bool_field: false
    int_field: 5
    float_field: 5.4
    vector_int_field:
    variant:
      int:
        5
    color: red
)";

  run_test(example);
}

TEST(parse2)
{
  string example = R"(
foos:
  * 
    bar:
      str
    eggs:
    bool_field: false
    int_field: 5
    opt_field: 8
    opt_vec_field: foo
    float_field: 5.4
    vector_int_field:
    variant:
      int:
        5
    color: red
)";

  run_test(example);
}

TEST(parse3)
{
  string example = R"(
foos:
  * 
    bar:
      str
    eggs:
    bool_field: false
    int_field: 5
    opt_field: 8
    opt_vec_field:
    float_field: 5.4
    vector_int_field:
    variant:
      int:
        5
    color: red
)";

  run_test(example);
}

TEST(error_missing_field)
{
  string example = R"(
foos:
  * 
    bar:
      str
    eggs:
    bool_field: false
    opt_field: 8
    float_field: 5.4
    vector_int_field:
    variant:
      int:
        5
    color: red
)";

  run_error_test(example);
}

TEST(error_missing_field2)
{
  string example = R"(
foos:
  * 
    bar:
      str
    bool_field: false
    int_field: 5
    opt_field: 8
    float_field: 5.4
    vector_int_field:
    variant:
      int:
        5
    color: red
)";

  run_error_test(example);
}

TEST(error_repeated)
{
  string example = R"(
foos:
  * 
    bar:
      str
    bool_field: false
    int_field: 5
    int_field: 6
    opt_field: 8
    float_field: 5.4
    vector_int_field:
    variant:
      int:
        5
    color: red
)";

  run_error_test(example);
}

} // namespace
} // namespace yasf
