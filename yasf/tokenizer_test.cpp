#include "tokenizer.hpp"

#include "bee/format_vector.hpp"
#include "bee/or_error.hpp"
#include "bee/testing.hpp"

using std::string;

namespace yasf {
namespace {

void run_test(const string& document)
{
  must(output, Tokenizer::tokenize(bee::FilePath(""), document));

  P("document:\n$*", document);
  P("------");
  P("Tokens:\n$", output);
  P("----------------------------------------");
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
