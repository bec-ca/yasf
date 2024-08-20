#include "command/command_builder.hpp"
#include "command/file_path.hpp"
#include "command/group_builder.hpp"
#include "yasf/dsl/compiler.hpp"
#include "yasf/dsl/formatter.hpp"

namespace yasf {

command::Cmd compile_cmd()
{
  using namespace command::flags;
  auto builder = command::CommandBuilder("Compile yasf");
  auto src_path = builder.required_anon(command::flags::FilePath, "SOURCE");
  return builder.run([=]() -> bee::OrError<> {
    return yasf::dsl::Compiler::compile(*src_path);
  });
}

command::Cmd format_cmd()
{
  using namespace command::flags;
  auto builder = command::CommandBuilder("Format yasf");
  auto src_path = builder.required_anon(command::flags::FilePath, "SOURCE");
  return builder.run([=]() -> bee::OrError<> {
    return yasf::dsl::Formatter::format(*src_path);
  });
}

int main(int argc, char* argv[])
{
  return command::GroupBuilder("Ant")
    .cmd("compile", compile_cmd())
    .cmd("format", format_cmd())
    .build()
    .main(argc, argv);
}

} // namespace yasf

int main(int argc, char* argv[]) { return yasf::main(argc, argv); }
