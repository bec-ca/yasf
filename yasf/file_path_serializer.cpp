#include "file_path_serializer.hpp"

#include "bee/file_path.hpp"

using bee::FilePath;

namespace yasf {

Value::ptr Serialize<FilePath>::serialize(const FilePath& value)
{
  return ser<std::string>(value.to_string());
}

bee::OrError<FilePath> Serialize<FilePath>::deserialize(const Value::ptr& value)
{
  bail(b, des<std::string>(value));
  return FilePath::of_string(b);
}

} // namespace yasf
