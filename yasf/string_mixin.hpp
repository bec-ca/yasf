#pragma once

#include <compare>
#include <string>

#include "of_stringable_mixin.hpp"

#include "bee/string_mixin.hpp"

namespace yasf {

template <class T>
struct StringMixin : public bee::StringMixin<T>, public OfStringableMixin<T> {
 public:
  using bee::StringMixin<T>::StringMixin;
};

}; // namespace yasf
