#include "cof.hpp"
#include "config_parser.hpp"
#include "example.generated.hpp"

#include "bee/format_optional.hpp"
#include "bee/testing.hpp"

using std::string;

namespace yasf {
namespace {

void run_test(const string& doc)
{
  must(config, ConfigParser::parse_from_string(bee::FilePath("test.txt"), doc));
  P("original:");
  P(config->to_string_hum());
  must(parsed, example::top::of_yasf_value(config));
  auto back = parsed.to_yasf_value();
  P("round trip:");
  P(back->to_string_hum());
  P("round trip once more:");
  must(parsed_again, example::top::of_yasf_value(back));
  P(parsed_again.to_yasf_value()->to_string_hum());
  P("location: $", config->location());
};

void run_error_test(const string& doc)
{
  must(
    config, ConfigParser::parse_from_string(bee::FilePath("foobar.txt"), doc));
  auto err = example::top::of_yasf_value(config);
  if (err.is_error()) {
    P(err.error());
  } else {
    P("No error!!");
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
    variant_field:
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
    variant_field:
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
    variant_field:
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
    variant_field:
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
    variant_field:
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
    variant_field:
      int:
        5
    color: red
)";

  run_error_test(example);
}

TEST(time)
{
  example::foo f;
  f.time = Time::of_string("2023-01-01 12:00:00").value();
  example::top data;
  data.foos.push_back(f);
  auto str = Cof::serialize(data);
  P(str);
  must(parsed, Cof::deserialize<example::top>(str));
  P(parsed);
}

} // namespace
} // namespace yasf
