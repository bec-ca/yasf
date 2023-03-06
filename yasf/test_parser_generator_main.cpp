#include "generator.hpp"
#include "generator_main_lib.hpp"

namespace yasf {
namespace {

using namespace yasf::types;

constexpr auto color = enum_type("Color", enum_legs("red", "blue", "green"));

constexpr auto vector_str = vec(str);
constexpr auto vector_int = vec(int_type);

constexpr auto v =
  variant("int_or_str", vlegs(vleg("int", int_type), vleg("str", str)));

constexpr auto foo_record = record_with_location(
  "foo",
  fields(
    required_field("bar", str),
    required_field("eggs", vector_str),
    required_field("bool_field", bool_type),
    required_field("int_field", int_type),
    optional_field("opt_field", int_type),
    optional_field("opt_vec_field", vector_str),
    required_field("float_field", float_type),
    required_field("vector_int_field", vector_int),
    required_field("variant", v),
    optional_field("color", color)));

constexpr auto foo_vector = vec(foo_record);

constexpr auto top_record =
  record("top", fields(optional_field("foos", foo_vector)));

} // namespace
} // namespace yasf

yasf::Definitions create_def()
{
  return {
    .types =
      {
        yasf::v,
        yasf::color,
        yasf::foo_record,
        yasf::top_record,
      },
  };
}
