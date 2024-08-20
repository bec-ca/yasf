#include "compiler.hpp"

#include "bee/file_writer.hpp"
#include "clang_format/clang_format.hpp"
#include "yasf/dsl/codegen.hpp"
#include "yasf/dsl/dsl_parser.hpp"

namespace yasf::dsl {
namespace {

bee::OrError<> write_file(const GeneratedFile& gf)
{
  return bee::FileWriter::write_file(gf.path, gf.content);
}

} // namespace

bee::OrError<> Compiler::compile(const bee::FilePath& src)
{
  bail(code, DslParser::parse(src));
  bail(generated, Codegen::gen(code));
  bail_unit(write_file(generated.header));
  bail_unit(write_file(generated.src));
  return bee::ok();
}

} // namespace yasf::dsl
