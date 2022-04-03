/*!
  \file utility-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UTILITY_INL_HPP
#define ZISC_UTILITY_INL_HPP

#include "utility.hpp"
// Standard C++ library
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam T No description.
  \param [in] value No description.
  \return No description
  */
template <typename Type, typename T>
requires std::is_nothrow_convertible_v<T, Type> ||
         std::is_nothrow_constructible_v<Type, T> inline
constexpr Type cast(T&& value) noexcept
{
  if constexpr (std::same_as<Type, T>)
    return value;
  else
    return static_cast<Type>(value);
}

/*!
  \details No detailed description

  \tparam Type1 No description.
  \tparam Type2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Type1, typename Type2>
requires std::equality_comparable_with<Type1, Type2> inline
constexpr bool equal(const Type1& lhs, const Type2& rhs) noexcept
{
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif // Z_GCC || Z_CLANG
  const bool result = lhs == rhs;
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG
  return result;
}

/*!
  \details Please read "Generating uniform doubles in the unit interval" section
  in the following link:
  <a href="http://prng.di.unimi.it/">xoshiro / xoroshiro generators and the PRNG shootout</a> for more details.

  \tparam Float No description.
  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, std::unsigned_integral Integer> inline
constexpr Float mapTo01(const Integer x) noexcept
{
  constexpr std::size_t int_digits = std::numeric_limits<Integer>::digits;
  constexpr std::size_t mant_digits = std::numeric_limits<Float>::digits;
  using ValueType = std::conditional_t<sizeof(Float) == 4, uint32b, uint64b>;
  ValueType v = 0;
  if constexpr (mant_digits < int_digits) {
    constexpr std::size_t offset = int_digits - mant_digits;
    v = cast<ValueType>(x >> offset);
  }
  else {
    constexpr std::size_t offset = mant_digits - int_digits;
    v = cast<ValueType>(x) << offset;
  }
  constexpr Float norm = cast<Float>(1.0) / cast<Float>(1ull << mant_digits);
  const Float y = cast<Float>(v) * norm;
  return y;
}

/*!
  \details No detailed description

  \tparam NewType No description.
  \tparam Type No description.
  \param [in] object No description.
  \return No description
  */
template <typename NewType, typename Type> inline
NewType reinterp(Type object) noexcept
{
  if constexpr (std::same_as<Type, NewType>)
    return object;
  else
    return reinterpret_cast<NewType>(object);
}

} // namespace zisc

#endif // ZISC_UTILITY_INL_HPP
