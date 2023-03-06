#pragma once

#include "type.hpp"

namespace yasf {

namespace details {

Type::ptr make_vec(const Type::ptr& element);

} // namespace details

template <class T> struct VecType {
 public:
  constexpr VecType(T element) : _element(element) {}

  operator Type::ptr() const { return details::make_vec(_element); }

 private:
  const T _element;
};

namespace types {

template <class E> constexpr VecType<E> vec(const E& element)
{
  return VecType<E>(element);
}

} // namespace types

} // namespace yasf
