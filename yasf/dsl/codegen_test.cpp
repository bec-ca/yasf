#include "codegen.hpp"

#include "bee/testing.hpp"

namespace yasf::dsl {
namespace {

TEST(gen)
{
  const auto g = Codegen::gen({
    .src = bee::FilePath("foobar/source/file.yasf"),
    .ns = {.parts = {"carrot"}},
    .types =
      {
        Record{
          .name = "hello",
          .fields =
            {{.name = "name", .type = {.name = "str"}, .optional = false}}},
      },
  });
  P(g);
}

} // namespace

} // namespace yasf::dsl
