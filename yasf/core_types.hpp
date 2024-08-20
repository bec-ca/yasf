#pragma once

#include "external_type.hpp"

namespace yasf {
namespace types {

constexpr auto yasf_value = ext("yasf::Value::ptr", "yasf/value.hpp");

constexpr auto time = ext("yasf::Time", "yasf/time.hpp");
constexpr auto span = ext("yasf::Span", "yasf/span.hpp");
constexpr auto file_path = ext("yasf::FilePath", "yasf/file_path.hpp");

} // namespace types
} // namespace yasf
