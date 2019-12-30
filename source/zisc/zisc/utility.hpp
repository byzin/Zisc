/*!
  \file utility.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UTILITY_HPP
#define ZISC_UTILITY_HPP

// Standard C++ library
#include <type_traits>

namespace zisc {

//! Convert type from T to Type
template <typename Type, typename T>
constexpr Type cast(T&& value) noexcept;

//! Swap the values
template <typename Type>
constexpr void swapValue(Type& a, Type& b) noexcept;

//! Treat T* as Type*
template <typename Type, typename T>
Type treatAs(T* object) noexcept;

} // namespace zisc

#include "utility-inl.hpp"

#endif // ZISC_UTILITY_HPP
