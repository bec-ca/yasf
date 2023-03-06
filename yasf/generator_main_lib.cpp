#include "generator_main_lib.hpp"

#include "bee/file_writer.hpp"
#include "bee/sub_process.hpp"
#include "command/command_builder.hpp"
#include "command/group_builder.hpp"

#include <filesystem>
#include <optional>
#include <string>

using bee::SubProcess;
using command::Cmd;
using command::CommandBuilder;
using command::GroupBuilder;
using std::optional;
using std::string;

namespace fs = std::filesystem;

namespace yasf {
namespace {

bee::OrError<bee::Unit> write_to_file(
  const string& filename, const string& content)
{
  bail_unit(
    bee::FileWriter::save_file(bee::FilePath::of_string(filename), content));

  return SubProcess::run({.cmd = "clang-format", .args = {"-i", filename}});
}

bee::OrError<bee::Unit> write_code(
  const fs::path& output_dir, const string& base_name, const Definitions defs)
{
  bail_unit(write_to_file(
    output_dir / bee::format("$.hpp", base_name), defs.gen_decl(base_name)));
  bail_unit(write_to_file(
    output_dir / bee::format("$.cpp", base_name), defs.gen_impl(base_name)));

  return bee::unit;
}

bee::OrError<bee::Unit> generate_code(
  const optional<string>& output_dir_opt, const optional<string>& base_name_opt)
{
  string output_dir = output_dir_opt.value_or(".");

  if (!base_name_opt) { return bee::Error("No basename provided"); }
  string base_name = base_name_opt.value();

  auto defs = create_def();

  auto ret = write_code(output_dir, base_name, defs);
  if (ret.is_error()) {
    return bee::Error::format("Failed to generated code: $", ret.error());
  }
  return bee::unit;
}

Cmd gen_command()
{
  using namespace command::flags;
  auto builder = CommandBuilder("Generate code");
  auto base_name = builder.optional("--base-name", string_flag);
  auto output_dir = builder.optional("--output-dir", string_flag);
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
