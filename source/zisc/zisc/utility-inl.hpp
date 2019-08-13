/*!
  \file utility-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UTILITY_INL_HPP
#define ZISC_UTILITY_INL_HPP

#include "utility.hpp"
// Standard C++ library
#include <cstdint>
#include <type_traits>

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Type, typename T> inline
constexpr Type cast(T&& value) noexcept
{
  return static_cast<Type>(value);
}

/*!
  */
template <typename Type> inline
constexpr void swapValue(Type& a, Type& b) noexcept
{
  const Type tmp{b};
  b = a;
  a = tmp;
}

/*!
  \details
  No detailed.
  */
template <typename Type, typename T> inline
Type treatAs(T* object) noexcept
{
  if constexpr (std::is_same_v<Type, T*>)
    return object;
  else
    return reinterpret_cast<Type>(object);
}

} // namespace zisc

#endif // ZISC_UTILITY_INL_HPP
