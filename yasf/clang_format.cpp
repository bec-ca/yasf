#include "clang_format.hpp"

#include "bee/sub_process.hpp"

namespace yasf {

namespace {

std::optional<std::string> get_env(const std::string& variable_name)
{
  char* c_path = getenv(variable_name.data());
  if (c_path == nullptr) return std::nullopt;
  return c_path;
}

} // namespace

bee::OrError<> ClangFormat::format_file(const bee::FilePath& file)
{
  auto clang_format = get_env("CLANG_FORMAT").value_or("clang-format");

  return bee::SubProcess::run({
    .cmd = bee::FilePath::of_string(clang_format),
    .args = {"-i", file.to_string()},
  });
}

} // namespace yasf
