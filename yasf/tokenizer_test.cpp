#include "tokenizer.hpp"

#include "bee/error.hpp"
#include "bee/format_vector.hpp"
#include "bee/testing.hpp"

using bee::print_line;
using std::string;

namespace yasf {
namespace {

void run_test(const string& document)
{
  must(output, Tokenizer::tokenize("", document));

  print_line("document:\n$*", document);
  print_line("------");
  print_line("Tokens:\n$", output);
  print_line("----------------------------------------");
}

TEST(tokenize1)
{
  run_test("foo bar bla");
  run_test("foo bar: bla");
  run_test("foo: bar: bla:");
  run_test("foo bar bla   ");
  run_test("   foo bar bla");
  run_test("   foo bar bla   ");
  run_test("foo\nbar: bla");
  run_test("   foo\n   bar\n    \n    bla\n   \n      ");
}

} // namespace
} // namespace yasf
