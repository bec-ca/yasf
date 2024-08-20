#include "codegen.hpp"

#include <unordered_map>

#include "bee/concepts.hpp"
#include "bee/format.hpp"
#include "clang_format/clang_format.hpp"
#include "yasf/core_types.hpp"
#include "yasf/definitions.hpp"
#include "yasf/dsl/code.hpp"
#include "yasf/enum.hpp"
#include "yasf/record.hpp"
#include "yasf/simple_types.hpp"
#include "yasf/type.hpp"
#include "yasf/variant.hpp"
#include "yasf/vec.hpp"

namespace yasf::dsl {
namespace {

std::unordered_map<std::string, yasf::Type::ptr> make_builtin_types()
{
  return {
    {"int", yasf::types::int_type},
    {"str", yasf::types::str},
    {"bool", yasf::types::bool_type},
    {"float", yasf::types::float_type},
    {"time", yasf::types::time},
    {"span", yasf::types::span},
    {"file_path", yasf::types::file_path},
    {"yasf", yasf::types::yasf_value},
  };
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// GeneratedFile
//

std::string GeneratedFile::to_string() const
{
  return F("$\n$\n", path, content);
}

////////////////////////////////////////////////////////////////////////////////
// GeneratedCode
//

std::string GeneratedCode::to_string() const
{
  return F("$\n$\n", header, src);
}

////////////////////////////////////////////////////////////////////////////////
// Codegen
//

bee::OrError<GeneratedCode> Codegen::gen(const Code& code)
{
  bee::FilePath output_dir = code.src.parent();
  auto stem = code.src.stem();

  auto types_map = make_builtin_types();

  auto get_type =
    [&](const std::string& name) -> bee::OrError<yasf::Type::ptr> {
    auto it = types_map.find(name);
    if (it != types_map.end()) { return it->second; }
    return EF("Unknown type: $", name);
  };

  auto make_type = [&](const Type& type) -> bee::OrError<yasf::Type::ptr> {
    if (type.parameter.has_value()) {
      bail(sub_type, get_type(*type.parameter));
      if (type.name == "vector") {
        return yasf::details::make_vec(sub_type);
      } else {
        return EF("Unknown parametric type: $", type.name);
      }
    } else {
      return get_type(type.name);
    }
  };

  Definitions defs;

  auto add_type = [&](const yasf::CustomType::ptr& t) -> bee::OrError<> {
    const auto& name = t->type_name();
    if (types_map.contains(name)) {
      return EF("Type already declared: $", name);
    }
    types_map.emplace(name, t);
    defs.types.push_back(t);
    return bee::ok();
  };

  for (const auto& type : code.types) {
    bail(
      custom_type,
      std::visit(
        [&]<class T>(const T& v) -> bee::OrError<yasf::CustomType::ptr> {
          if constexpr (std::is_same_v<T, Record>) {
            std::vector<yasf::Field> fields;
            for (const auto& f : v.fields) {
              bail(type, make_type(f.type));
              fields.push_back(yasf::Field{f.name, type, !f.optional});
            }
            return yasf::details::make_record(
              v.name, std::move(fields), v.location);
          } else if constexpr (std::is_same_v<T, Enum>) {
            return yasf::details::make_enum_type(v.name, v.legs);
          } else if constexpr (std::is_same_v<T, Variant>) {
            std::vector<VariantLeg> legs;
            for (const auto& leg : v.legs) {
              bail(type, make_type(leg.type));
              legs.push_back(yasf::VariantLeg{.name = leg.name, .type = type});
            }
            return yasf::details::make_variant(v.name, std::move(legs));
          } else {
            static_assert(bee::always_false<T> && "non exhaustive visit");
          }
        },
        type));
    bail_unit(add_type(custom_type));
  }

  auto header_path = output_dir / F("$.generated.hpp", stem);
  auto src_path = output_dir / F("$.generated.cpp", stem);

  const auto& ns = code.ns.to_cpp();
  auto unformatted_header = defs.gen_decl(ns, false);
  auto unformatted_src = defs.gen_impl(ns, stem, false);

  bail(
    header,
    clang_format::ClangFormat::format_code(unformatted_header, header_path));
  bail(src, clang_format::ClangFormat::format_code(unformatted_src, src_path));

  return GeneratedCode{
    .header = {.path = header_path, .content = header},
    .src = {.path = src_path, .content = src},
  };
}

} // namespace yasf::dsl
