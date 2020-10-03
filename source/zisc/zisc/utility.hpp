/*!
  \file utility.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UTILITY_HPP
#define ZISC_UTILITY_HPP

// Standard C++ library
#include <type_traits>
// Zisc
#include "concepts.hpp"

namespace zisc {

//! Convert type from T to Type
template <typename Type, typename T>
constexpr Type cast(T&& value) noexcept;

//! Map an integer value into a [0, 1) floating point value
template <FloatingPoint Float, UnsignedInteger Integer>
constexpr Float mapTo01(const Integer x) noexcept;

//! Swap the values
template <typename Type1, typename Type2>
constexpr void swap(Type1& a, Type2& b) noexcept;

//! Treat T* as Type*
template <typename Type, typename T>
Type treatAs(T* object) noexcept;

//! Convert between types by reinterpreting the underlying bit pattern
template <typename NewType, typename Type>
NewType reinterp(Type object) noexcept;

} // namespace zisc

#include "utility-inl.hpp"

#endif // ZISC_UTILITY_HPP
