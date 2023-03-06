#pragma once

#include "of_stringable_mixin.hpp"

#include "bee/string_mixin.hpp"

#include <compare>
#include <string>

namespace yasf {

template <class T>
struct StringMixin : public bee::StringMixin<T>, public OfStringableMixin<T> {
 public:
  using bee::StringMixin<T>::StringMixin;
};

}; // namespace yasf
