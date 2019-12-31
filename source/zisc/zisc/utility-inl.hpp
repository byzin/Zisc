/*!
  \file utility-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UTILITY_INL_HPP
#define ZISC_UTILITY_INL_HPP

#include "utility.hpp"
// Standard C++ library
#include <cstdint>
#include <type_traits>
#include <utility>

namespace zisc {

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam T No description.
  \param [in] value No description.
  \return No description
  */
template <typename Type, typename T> inline
constexpr Type cast(T&& value) noexcept
{
  if constexpr (std::is_same_v<Type, T>)
    return value;
  else
    return static_cast<Type>(value);
}

/*!
  \details No detailed description

  \tparam Type1 No description.
  \tparam Type2 No description.
  \param [in] a No description.
  \param [in] b No description.
  */
template <typename Type1, typename Type2> inline
constexpr void swap(Type1& a, Type2& b) noexcept
{
  static_assert(std::is_convertible_v<Type1, Type2>,
                "Type1 isn't able to be converted to Type2.");
  static_assert(std::is_convertible_v<Type2, Type1>,
                "Type2 isn't able to be converted to Type1.");
  auto tmp = std::move(b);
  b = std::move(a);
  a = std::move(tmp);
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam T No description.
  \param [in] object No description.
  \return No description
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
