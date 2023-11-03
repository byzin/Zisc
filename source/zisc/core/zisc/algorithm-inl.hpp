/*!
  \file algorithm-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALGORITHM_INL_HPP
#define ZISC_ALGORITHM_INL_HPP

#include "algorithm.hpp"
// Standard C++ library
#include <algorithm>
#include <array>
#include <concepts>
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
constexpr auto Algorithm::abs(const Arith x) noexcept -> Arith
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
constexpr auto Algorithm::clamp(const Type value, 
                                const LowerType lower, 
                                const UpperType upper) noexcept
    -> std::common_type_t<Type, LowerType, UpperType>
{
  using ResultT = std::common_type_t<Type, LowerType, UpperType>;
  const ResultT y = (min)((max)(value, lower), upper);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Algorithm::invert(const Float x) noexcept -> Float
{
  constexpr auto zero = cast<Float>(0.0);
  constexpr auto one = cast<Float>(1.0);
  constexpr Float m = (std::numeric_limits<Float>::max)();
  const Float y = (x != zero) ? one / x : m;
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
constexpr auto Algorithm::isInBounds(const Type& value, 
                                     const LowerType& lower, 
                                     const UpperType& upper) noexcept -> bool
{
  const bool result = ((kIsLeftClosed) ? !(value < lower) : (lower < value)) &&
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
constexpr auto Algorithm::isInOpenBounds(const Type& value, 
                                         const LowerType& lower, 
                                         const UpperType& upper) noexcept -> bool
{
  const bool result = isInBounds<false, false>(value, lower, upper);
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
constexpr auto Algorithm::isInClosedBounds(const Type& value, 
                                           const LowerType& lower, 
                                           const UpperType& upper) noexcept -> bool
{
  const bool result = isInBounds<true, true>(value, lower, upper);
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \param [in] x No description.
  \return No description
  */
template <Arithmetic Arith> inline
constexpr auto Algorithm::isNegative(const Arith x) noexcept -> bool
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
constexpr auto Algorithm::max(const Type1& a, const Type2& b) noexcept
    -> const std::common_type_t<Type1, Type2>&
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
constexpr auto Algorithm::min(const Type1& a, const Type2& b) noexcept
    -> const std::common_type_t<Type1, Type2>&
{
  const std::common_type_t<Type1, Type2>& y = (b < a) ? b : a;
  return y;
}

/*!
  \details No detailed description

  \tparam Int1 No description.
  \tparam Int2 No description.
  \tparam Int3 No description.
  \param [in] range No description.
  \param [in] num_of_division No description.
  \param [in] index No description.
  \return No description
  */
template <Integer Int1, Integer Int2, Integer Int3> inline
constexpr auto Algorithm::divideRange(const Int1 range,
                                      const Int2 num_of_division,
                                      const Int3 index) noexcept
    -> std::array<std::common_type_t<Int1, Int2, Int3>, 2>
{
  using Int = std::common_type_t<Int1, Int2, Int3>;

  std::array<Int, 2> result{{0, 0}};
  const auto r = cast<Int>(range);
  const auto num = cast<Int>(num_of_division);
  const auto i = cast<Int>(index);
  if (isInBounds(i, cast<Int>(0), num)) {
    const Int n = r / num;
    result[0] = n * i;
    result[1] = Algorithm::max(result[0] + n, (i + 1 == num) ? r : 0);
  }
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] x No description.
  \return No description
  */
template <Integer Int> inline
constexpr auto Algorithm::isOdd(const Int x) noexcept -> bool
{
  constexpr auto lsb = cast<Int>(0b01);
  const bool result = ((x & lsb) == lsb);
  return result;
}

/*!
  \details No detailed description

  \tparam Arith description.
  \param [in] x No description.
  \return No description
  */
template <Arithmetic Arith> inline
constexpr auto abs(const Arith x) noexcept -> Arith
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
constexpr auto clamp(const Type value, const LowerType lower, const UpperType upper) noexcept
    -> std::common_type_t<Type, LowerType, UpperType>
{
  using ResultT = std::common_type_t<Type, LowerType, UpperType>;
  const ResultT y = Algorithm::clamp(value, lower, upper);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto invert(const Float x) noexcept -> Float
{
  const Float y = Algorithm::invert(x);
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
constexpr auto max(const Type1& a, const Type2& b) noexcept
    -> const std::common_type_t<Type1, Type2>&
{
  const std::common_type_t<Type1, Type2>& y = (Algorithm::max)(a, b);
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
constexpr auto min(const Type1& a, const Type2& b) noexcept
    -> const std::common_type_t<Type1, Type2>&
{
  const std::common_type_t<Type1, Type2>& y = (Algorithm::min)(a, b);
  return y;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \param [in] x No description.
  \return No description
  */
template <Arithmetic Arith> inline
constexpr auto signbit(const Arith x) noexcept -> bool
{
  const bool result = Algorithm::isNegative(x);
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
constexpr auto isInBounds(const Type& value, 
                          const LowerType& lower, 
                          const UpperType& upper) noexcept -> bool
{
  const bool result = Algorithm::isInBounds<kIsLeftClosed,
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
constexpr auto isInOpenBounds(const Type& value, 
                              const LowerType& lower, 
                              const UpperType& upper) noexcept -> bool
{
  const bool result = isInBounds<false, false>(value, lower, upper);
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
constexpr auto isInClosedBounds(const Type& value, 
                                const LowerType& lower, 
                                const UpperType& upper) noexcept -> bool
{
  const bool result = isInBounds<true, true>(value, lower, upper);
  return result;
}

/*!
  \details No detailed description

  \tparam Int1 No description.
  \tparam Int2 No description.
  \tparam Int3 No description.
  \param [in] range No description.
  \param [in] num_of_division No description.
  \param [in] index No description.
  \return No description
  */
template <Integer Int1, Integer Int2, Integer Int3> inline
constexpr auto divideRange(const Int1 range,
                           const Int2 num_of_division,
                           const Int3 index) noexcept
    -> std::array<std::common_type_t<Int1, Int2, Int3>, 2>
{
  const std::array result = Algorithm::divideRange(range, num_of_division, index);
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] x No description.
  \return No description
  */
template <Integer Int> inline
constexpr auto isOdd(const Int x) noexcept -> bool
{
  const bool result = Algorithm::isOdd(x);
  return result;
}

} // namespace zisc

#endif // ZISC_ALGORITHM_INL_HPP
