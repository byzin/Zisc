/*!
  \file utility-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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
template <typename Arithmetic> inline
constexpr Arithmetic abs(const Arithmetic x) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  if constexpr (std::is_signed_v<Arithmetic>)
    return isNegative(x) ? -x : x;
  else
    return x;
}

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
 \details
 No detailed.
*/
template <typename Type, typename LowerType, typename UpperType> inline
constexpr std::common_type_t<Type, LowerType, UpperType> clamp(
    const Type value, 
    const LowerType lower, 
    const UpperType upper) noexcept
{
  return min(max(value, lower), upper);
}

/*!
  */
template <bool kIsLeftClosed,
          bool kIsRightClosed,
          typename Type,
          typename LowerType,
          typename UpperType> inline
constexpr bool isInBounds(const Type& value, 
                          const LowerType& lower, 
                          const UpperType& upper) noexcept
{
  return ((kIsLeftClosed) ? !(value < lower) : (lower < value)) &&
         ((kIsRightClosed) ? !(upper < value) : (value < upper));
}

/*!
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr bool isInOpenBounds(const Type& value, 
                              const LowerType& lower, 
                              const UpperType& upper) noexcept
{
  return isInBounds<false, false>(value, lower, upper);
}

/*!
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr bool isInLClosedROpenBounds(const Type& value, 
                                      const LowerType& lower, 
                                      const UpperType& upper) noexcept
{
  return isInBounds<true, false>(value, lower, upper);
}

/*!
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr bool isInLOpenRClosedBounds(const Type& value, 
                                      const LowerType& lower, 
                                      const UpperType& upper) noexcept
{
  return isInBounds<false, true>(value, lower, upper);
}

/*!
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr bool isInClosedBounds(const Type& value, 
                                const LowerType& lower, 
                                const UpperType& upper) noexcept
{
  return isInBounds<true, true>(value, lower, upper);
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic> inline
constexpr bool isNegative(const Arithmetic n) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  bool result = false;
  if constexpr (std::is_signed_v<Arithmetic>) {
    constexpr auto zero = cast<Arithmetic>(0);
    result = n < zero;
  }
  return result;
}

/*!
  */
template <typename Integer> inline
constexpr bool isOdd(const Integer n) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr auto lsb = cast<Integer>(0b01);
  return ((n & lsb) == lsb);
}

/*!
  */
template <typename Integer> inline
constexpr bool isPowerOf2(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  return (x & (x - 1)) == 0;
}

/*!
  \details
  No detailed.
  */
template <typename Type1, typename Type2> inline
constexpr const std::common_type_t<Type1, Type2>& max(const Type1& a,
                                                      const Type2& b) noexcept
{
  return (b < a) ? a : b;
}

/*!
  \details
  No detailed.
  */
template <typename Type1, typename Type2> inline
constexpr const std::common_type_t<Type1, Type2>& min(const Type1& a,
                                                      const Type2& b) noexcept
{
  return (b < a) ? b : a;
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
