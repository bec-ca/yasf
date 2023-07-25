#include "generator_main_lib.hpp"

#include <filesystem>
#include <optional>
#include <string>

#include "clang_format.hpp"

#include "bee/file_writer.hpp"
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
  bail_unit(bee::FileWriter::save_file(filename, content));

  return ClangFormat::format_file(filename);
}

bee::OrError<> write_code(
  const FilePath& output_dir, const string& base_name, const Definitions defs)
{
  bail_unit(write_to_file(
    output_dir / F("$.hpp", base_name), defs.gen_decl(base_name)));
  bail_unit(write_to_file(
    output_dir / F("$.cpp", base_name), defs.gen_impl(base_name)));

  return bee::ok();
}

bee::OrError<> generate_code(
  const optional<FilePath>& output_dir_opt,
  const optional<string>& base_name_opt)
{
  FilePath output_dir = output_dir_opt.value_or(FilePath::of_string("."));

  if (!base_name_opt) { return bee::Error("No basename provided"); }
  string base_name = base_name_opt.value();

  auto defs = create_def();

  auto ret = write_code(output_dir, base_name, defs);
  if (ret.is_error()) {
    return bee::Error::fmt("Failed to generated code: $", ret.error());
  }
  return bee::ok();
}

Cmd gen_command()
{
  using namespace command::flags;
  auto builder = CommandBuilder("Generate code");
  auto base_name = builder.optional("--base-name", string_flag);
  auto output_dir = builder.optional("--output-dir", file_path);
  return builder.run([=]() { return generate_code(*output_dir, *base_name); });
}

} // namespace
} // namespace yasf

int main(int argc, char* argv[])
{
  return GroupBuilder("Yasf generator")
    .cmd("gen", yasf::gen_command())
    .build()
    .main(argc, argv);
}
