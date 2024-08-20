#include "generator_main_lib.hpp"

#include <optional>
#include <string>

#include "definitions.hpp"

#include "bee/file_writer.hpp"
#include "clang_format/clang_format.hpp"
#include "command/command_builder.hpp"
#include "command/file_path.hpp"
#include "command/group_builder.hpp"

using bee::FilePath;
using command::Cmd;
using command::CommandBuilder;
using command::GroupBuilder;
using std::optional;
using std::string;

namespace yasf {
namespace {

bee::OrError<> write_to_file(const FilePath& filename, const string& content)
{
  bail_unit(bee::FileWriter::write_file(filename, content));
  return clang_format::ClangFormat::format_file(filename);
}

bee::OrError<> write_code(
  const FilePath& output_dir,
  const string& base_name,
  const CreateDefs& create_defs)
{
  const auto& ns = base_name;
  auto defs = create_defs();
  bail_unit(write_to_file(
    output_dir / F("$.generated.hpp", base_name), defs.gen_decl(ns, true)));
  bail_unit(write_to_file(
    output_dir / F("$.generated.cpp", base_name),
    defs.gen_impl(ns, base_name, true)));

  return bee::ok();
}

bee::OrError<> generate_code(
  const optional<FilePath>& output_dir_opt,
  const optional<string>& base_name_opt,
  const CreateDefs& create_defs)
{
  FilePath output_dir = output_dir_opt.value_or(FilePath("."));

  if (!base_name_opt) { return bee::Error("No basename provided"); }
  string base_name = base_name_opt.value();

  auto ret = write_code(output_dir, base_name, create_defs);
  if (ret.is_error()) {
    return bee::Error::fmt("Failed to generated code: $", ret.error());
  }
  return bee::ok();
}

Cmd gen_command(const CreateDefs& create_defs)
{
  namespace f = command::flags;
  auto builder = CommandBuilder("Generate code");
  auto base_name = builder.optional("--base-name", f::String);
  auto output_dir = builder.optional("--output-dir", f::FilePath);
  return builder.run(
    [=]() { return generate_code(*output_dir, *base_name, create_defs); });
}

} // namespace

int GeneratorMainLib::run_main(
  int argc, char* argv[], const CreateDefs& create_defs)
{
  return GroupBuilder("Yasf generator")
    .cmd("gen", yasf::gen_command(create_defs))
    .build()
    .main(argc, argv);
}

} // namespace yasf
