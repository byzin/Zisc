/*!
  \file utility-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
#include <functional>
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
    v = static_cast<ValueType>(x >> offset);
  }
  else {
    constexpr std::size_t offset = mant_digits - int_digits;
    v = static_cast<ValueType>(x) << offset;
  }
  constexpr Float norm = static_cast<Float>(1.0) / static_cast<Float>(1ull << mant_digits);
  const Float y = static_cast<Float>(v) * norm;
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

/*!
  \details No detailed description

  \tparam Lhs No description.
  \tparam Rhs No description.
  \param [in] flag No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Lhs, typename Rhs> requires std::is_nothrow_assignable_v<Lhs, Rhs>
inline
bool updateIfTrue(const bool flag, Lhs&& lhs, Rhs&& rhs) noexcept
{
  if (flag)
    lhs = std::forward<Rhs>(rhs);
  return flag;
}

/*!
  \details No detailed description

  \tparam ReturnT No description.
  \tparam Func No description.
  \tparam ArgTypes No description.
  \param [in] flag No description.
  \param [out] result No description.
  \param [in] func No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename Func, typename ...ArgTypes>
    requires std::is_nothrow_invocable_r_v<ReturnT, Func, ArgTypes...>
inline
bool invokeIfTrue(const bool flag, ReturnT& result, Func&& func, ArgTypes&&... args) noexcept
{
  if (flag)
    result = std::invoke(func, std::forward<ArgTypes>(args)...);
  return flag;
}

/*!
  \details No detailed description

  \tparam Func No description.
  \tparam ArgTypes No description.
  \param [in] flag No description.
  \param [in] func No description.
  \param [in] args No description.
  \return No description
  */
template <typename Func, typename ...ArgTypes>
    requires std::is_nothrow_invocable_v<Func, ArgTypes...>
inline
bool invokeIfTrue(const bool flag, Func&& func, ArgTypes&&... args) noexcept
{
  if (flag)
    std::invoke(func, std::forward<ArgTypes>(args)...);
  return flag;
}

} // namespace zisc

#endif // ZISC_UTILITY_INL_HPP
