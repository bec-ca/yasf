#include "dsl_parser.hpp"

#include "bee/testing.hpp"

namespace yasf::dsl {
namespace {

TEST(parse)
{
  constexpr auto code = R"(
ns yasf


record Foobar {
  attr location;

  foo int;
  bar string;
  bar_optional string optional;
}

variant SumType {
  apples int;
  oranges string;
}
  )";

  must(out, DslParser::parse(bee::FilePath("test.yasf"), code));
  P(out);
}

} // namespace
} // namespace yasf::dsl
