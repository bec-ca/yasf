#include "cof.hpp"
#include "config_parser.hpp"
#include "test_parser.hpp"

#include "bee/format_memory.hpp"
#include "bee/testing.hpp"

using std::string;

namespace yasf {
namespace {

void run_test(string doc)
{
  must(config, ConfigParser::parse_from_string("", doc));
  P("");
  P(config->to_string_hum());
  P(Cof::to_string(config));
};

TEST(basic)
{
  string example = R"(
bar:
  str
  next
  other
eggs:
bool_field: false
int_field: 5
float_field: 5.4
vector_int_field:
)";

  run_test(example);
}

TEST(basic_escaping)
{
  string example = R"""(
bar:
  str
  ne'xt
  ot:her
  ot\her
)""";

  run_test(example);
}

void run_round_trip(string doc)
{
  P(doc);
  must(config, ConfigParser::parse_from_string("", doc));
  auto cof = Cof::to_string(config);
  P(cof);
  P("");
  must(parsed, Cof::raw_parse_string(cof));
  P(parsed->to_string_hum());
};

TEST(basic_round_trip)
{
  string example = R"""(
bar:
  str
  ne'xt
  ot:her
  ot\her
  foo]bar
  list:
    1
    2
    3
)""";

  run_round_trip(example);
}

TEST(top_round_trip)
{
  auto run_round_trip = [](test_parser::top doc) {
    P("Original: $", doc.to_yasf_value()->to_string_hum());
    auto str = Cof::serialize(doc);
    P("Cof serizlied: $", str);
    must(parsed, Cof::raw_parse_string(str));
    P("Config value from cof: $", parsed->to_string_hum());
    must(config, Cof::deserialize<test_parser::top>(str));
    P("Final back: $", config.to_yasf_value()->to_string_hum());
    P("-----------------------------");
  };
  auto example = test_parser::top{.foos = {test_parser::foo{}}};
  run_round_trip(example);
}

} // namespace
} // namespace yasf
