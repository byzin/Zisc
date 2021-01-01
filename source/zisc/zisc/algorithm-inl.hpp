/*!
  \file algorithm-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALGORITHM_INL_HPP
#define ZISC_ALGORITHM_INL_HPP

#include "algorithm.hpp"
// Standard C++ library
#include <algorithm>
#include <limits>
#include <type_traits>
#include <utility>
// Zisc
#include "concepts.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Arith No description.
  \param [in] x No description.
  \return No description
  */
template <Arithmetic Arith> inline
constexpr Arith Algorithm::abs(const Arith x) noexcept
{
  if constexpr (std::is_signed_v<Arith>) {
    const Arith y = signbit(x) ? -x : x;
    return y;
  }
  else {
    return x;
  }
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam LowerType No description.
  \tparam UpperType No description.
  \param [in] value No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr std::common_type_t<Type, LowerType, UpperType> Algorithm::clamp(
    const Type value, 
    const LowerType lower, 
    const UpperType upper) noexcept
{
  const auto y = min(max(value, lower), upper);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Algorithm::invert(const Float x) noexcept
{
  constexpr Float zero = cast<Float>(0.0);
  constexpr Float one = cast<Float>(1.0);
  constexpr Float m = (std::numeric_limits<Float>::max)();
  const auto y = (x != zero) ? one / x : m;
  return y;
}

/*!
  \details No detailed description

  \tparam kIsLeftClosed No description.
  \tparam kIsRightClosed No description.
  \tparam Type No description.
  \tparam LowerType No description.
  \tparam UpperType No description.
  \param [in] value No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <bool kIsLeftClosed,
          bool kIsRightClosed,
          typename Type,
          typename LowerType,
          typename UpperType> inline
constexpr bool Algorithm::isInBounds(const Type& value, 
                                     const LowerType& lower, 
                                     const UpperType& upper) noexcept
{
  const auto result = ((kIsLeftClosed) ? !(value < lower) : (lower < value)) &&
                      ((kIsRightClosed) ? !(upper < value) : (value < upper));
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam LowerType No description.
  \tparam UpperType No description.
  \param [in] value No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr bool Algorithm::isInOpenBounds(const Type& value, 
                                         const LowerType& lower, 
                                         const UpperType& upper) noexcept
{
  const auto result = isInBounds<false, false>(value, lower, upper);
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam LowerType No description.
  \tparam UpperType No description.
  \param [in] value No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr bool Algorithm::isInClosedBounds(const Type& value, 
                                           const LowerType& lower, 
                                           const UpperType& upper) noexcept
{
  const auto result = isInBounds<true, true>(value, lower, upper);
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \param [in] x No description.
  \return No description
  */
template <Arithmetic Arith> inline
constexpr bool Algorithm::isNegative(const Arith x) noexcept
{
  if constexpr (std::is_signed_v<Arith>) {
    constexpr auto zero = cast<Arith>(0);
    const bool result = x < zero;
    return result;
  }
  else {
    return false;
  }
}

/*!
  \details No detailed description

  \tparam Type1 No description.
  \tparam Type2 No description.
  \param [in] a No description.
  \param [in] b No description.
  \return No description
  */
template <typename Type1, typename Type2> inline
constexpr const std::common_type_t<Type1, Type2>& Algorithm::max(
    const Type1& a,
    const Type2& b) noexcept
{
  const std::common_type_t<Type1, Type2>& y = (b < a) ? a : b;
  return y;
}

/*!
  \details No detailed description

  \tparam Type1 No description.
  \tparam Type2 No description.
  \param [in] a No description.
  \param [in] b No description.
  \return No description
  */
template <typename Type1, typename Type2> inline
constexpr const std::common_type_t<Type1, Type2>& Algorithm::min(
    const Type1& a,
    const Type2& b) noexcept
{
  const std::common_type_t<Type1, Type2>& y = (b < a) ? b : a;
  return y;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] x No description.
  \return No description
  */
template <Integer Int> inline
constexpr bool Algorithm::isOdd(const Int x) noexcept
{
  constexpr auto lsb = cast<Int>(0b01);
  const auto result = ((x & lsb) == lsb);
  return result;
}

/*!
  \details No detailed description

  \tparam Arith description.
  \param [in] x No description.
  \return No description
  */
template <Arithmetic Arith> inline
constexpr Arith abs(const Arith x) noexcept
{
  const Arith y = Algorithm::abs(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam LowerType No description.
  \tparam UpperType No description.
  \param [in] value No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */

template <typename Type, typename LowerType, typename UpperType> inline
constexpr std::common_type_t<Type, LowerType, UpperType> clamp(
    const Type value, 
    const LowerType lower, 
    const UpperType upper) noexcept
{
  const auto y = Algorithm::clamp(value, lower, upper);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float invert(const Float x) noexcept
{
  const auto y = Algorithm::invert(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Type1 No description.
  \tparam Type2 No description.
  \param [in] a No description.
  \param [in] b No description.
  \return No description
  */
template <typename Type1, typename Type2> inline
constexpr const std::common_type_t<Type1, Type2>& max(const Type1& a,
                                                      const Type2& b) noexcept
{
  const std::common_type_t<Type1, Type2>& y = Algorithm::max(a, b);
  return y;
}

/*!
  \details No detailed description

  \tparam Type1 No description.
  \tparam Type2 No description.
  \param [in] a No description.
  \param [in] b No description.
  \return No description
  */
template <typename Type1, typename Type2> inline
constexpr const std::common_type_t<Type1, Type2>& min(const Type1& a,
                                                      const Type2& b) noexcept
{
  const std::common_type_t<Type1, Type2>& y = Algorithm::min(a, b);
  return y;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \param [in] x No description.
  \return No description
  */
template <Arithmetic Arith> inline
constexpr bool signbit(const Arith x) noexcept
{
  const auto result = Algorithm::isNegative(x);
  return result;
}

/*!
  \details No detailed description

  \tparam kIsLeftClosed No description.
  \tparam kIsRightClosed No description.
  \tparam Type No description.
  \tparam LowerType No description.
  \tparam UpperType No description.
  \param [in] value No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
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
  const auto result = Algorithm::isInBounds<kIsLeftClosed,
                                            kIsRightClosed,
                                            Type,
                                            LowerType,
                                            UpperType>(value, lower, upper);
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam LowerType No description.
  \tparam UpperType No description.
  \param [in] value No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr bool isInOpenBounds(const Type& value, 
                              const LowerType& lower, 
                              const UpperType& upper) noexcept
{
  const auto result = isInBounds<false, false>(value, lower, upper);
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam LowerType No description.
  \tparam UpperType No description.
  \param [in] value No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename Type, typename LowerType, typename UpperType> inline
constexpr bool isInClosedBounds(const Type& value, 
                                const LowerType& lower, 
                                const UpperType& upper) noexcept
{
  const auto result = isInBounds<true, true>(value, lower, upper);
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] x No description.
  \return No description
  */
template <Integer Int> inline
constexpr bool isOdd(const Int x) noexcept
{
  const auto result = Algorithm::isOdd(x);
  return result;
}

} // namespace zisc

#endif // ZISC_ALGORITHM_INL_HPP
