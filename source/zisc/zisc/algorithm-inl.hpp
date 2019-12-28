/*!
  \file algorithm-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALGORITHM_INL_HPP
#define ZISC_ALGORITHM_INL_HPP

#include "algorithm.hpp"
// Standard C++ library
#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
// Microsoft
#if defined(Z_MSVC)
#include <intrin.h>
#endif // Z_MSVC
// Zisc
#include "error.hpp"
#include "std_memory_resource.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
 \details
 No detailed.
 */
template <typename Arithmetic> inline
constexpr Arithmetic Algorithm::abs(const Arithmetic x) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  if constexpr (std::is_signed_v<Arithmetic>) {
    const auto y = isNegative(x) ? -x : x;
    return y;
  }
  else {
    return x;
  }
}

/*!
 \details
 No detailed.
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
  */
template <typename Float> inline
constexpr Float Algorithm::invert(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point.");
  constexpr Float zero = cast<Float>(0.0);
  constexpr Float one = cast<Float>(1.0);
  constexpr Float m = std::numeric_limits<Float>::max();
  const auto y = (x != zero) ? one / x : m;
  return y;
}

/*!
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
 \details
 No detailed.
 */
template <typename Arithmetic> inline
constexpr bool Algorithm::isNegative(const Arithmetic n) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  if constexpr (std::is_signed_v<Arithmetic>) {
    constexpr auto zero = cast<Arithmetic>(0);
    const bool result = n < zero;
    return result;
  }
  else {
    return false;
  }
}

/*!
  \details
  No detailed.
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
  \details
  No detailed.
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
  \param[in] x \a x should be greater than 0
  \return The exponent corresponds the \a x in the base-2 if \a x is positive, 
          otherwize \a x 
  */
template <typename Integer> inline
constexpr Integer Algorithm::getExponent(Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr auto zero = cast<Integer>(0);
  Integer exponent = 0;
  if (zero < x) {
    constexpr std::size_t half_bits = sizeof(Integer) * 4;
    for (Integer bits = half_bits; bits > 0; bits = bits >> 1) {
      const Integer upper_bits = x >> bits;
      exponent += (upper_bits != 0) ? bits : 0;
      x = (upper_bits != 0) ? upper_bits : x; 
    }
  }
  return exponent;
}

/*!
  */
template <typename Integer> inline
constexpr bool Algorithm::isOdd(const Integer n) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr auto lsb = cast<Integer>(0b01);
  const auto result = ((n & lsb) == lsb);
  return result;
}

/*!
  */
template <typename Integer> inline
constexpr bool Algorithm::isPowerOf2(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  const auto result = (x & (x - 1)) == 0;
  return result;
}

/*!
  */
template <typename Integer> inline
Integer Algorithm::popcount(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  const auto y = popcountImpl<Config::implType()>(x);
  return y;
}

/*!
  \param[in] x The \a x should be positive.
  \return The smallest power of 2 that is greater or equal to \a x
          if \a x is positive, otherwize \a x
  */
template <typename Integer> inline
constexpr Integer Algorithm::roundUpToPowerOf2(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr auto zero = cast<Integer>(0);
  Integer y = x;
  if (zero < x) {
    --y;
    constexpr int bits = sizeof(Integer) * 8;
    for (int shift = 1; shift < bits; shift = shift << 1)
      y = y | (y >> shift);
    ++y;
  }
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator Algorithm::searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value) noexcept
{
  ZISC_ASSERT(0 < std::distance(begin, end), "The end is in advance of the begin.");
  const auto result = Config::isStlBinaryTreeUsed()
    ? Stl::searchBinaryTree(begin, end, value)
    : Zisc::searchBinaryTree(begin, end, value);
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename RandomAccessIterator> inline
void Algorithm::toBinaryTree(
    RandomAccessIterator begin, 
    RandomAccessIterator end,
    std::pmr::memory_resource* mem_resource) noexcept
{
  ZISC_ASSERT(std::is_sorted(begin, end), "The array isn't sorted.");
  if (Config::isStlBinaryTreeUsed())
    Stl::toBinaryTree(begin, end, mem_resource);
  else
    Zisc::toBinaryTree(begin, end, mem_resource);
}

/*!
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator Algorithm::Stl::searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value) noexcept
{
  ZISC_ASSERT(0 < std::distance(begin, end), "The end is in advance of the begin.");
  auto result = std::lower_bound(begin, end, value);
  result = (*result == value) ? result : result - 1;
  return result;
}

/*!
  */
template <typename RandomAccessIterator> inline
void Algorithm::Stl::toBinaryTree(
    RandomAccessIterator /* begin */, 
    RandomAccessIterator /* end */,
    std::pmr::memory_resource* /* mem_resource */) noexcept
{
}

/*!
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator Algorithm::Zisc::searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value) noexcept
{
  ZISC_ASSERT(0 < std::distance(begin, end), "The end is in advance of the begin.");
  const std::size_t n = cast<std::size_t>(std::distance(begin, end));
  std::size_t index = 0;
  while (index < n) {
    const auto node = begin + index;
    index = (index << 1) + ((value < *node) ? 1 : 2);
  }
  while (isOdd(index))
    index = index >> 1;
  ZISC_ASSERT(2 <= index, "The index is invalid.");
  index = (index >> 1) - 1;
  return begin + index;
}

/*!
  \details
  No detailed.
  */
template <typename RandomAccessIterator> inline
void Algorithm::Zisc::toBinaryTree(
    RandomAccessIterator begin, 
    RandomAccessIterator end,
    std::pmr::memory_resource* mem_resource) noexcept
{
  ZISC_ASSERT(std::is_sorted(begin, end), "The array isn't sorted.");
  const auto size = std::distance(begin, end);
  ZISC_ASSERT(0 < size, "The end is in advance of the begin.");
  if (1 < size) {
    // Create a temp array
    using Type = typename std::iterator_traits<RandomAccessIterator>::value_type;
    pmr::vector<Type> array{
        typename decltype(array)::allocator_type{mem_resource}};
    array.reserve(cast<std::size_t>(size));
    for (auto iterator = begin; iterator != end; ++iterator)
      array.emplace_back(std::move(*iterator));
    // Transform an array to a binary tree
    toBinaryTreeImpl(array.begin(), array.end(), 0, begin);
  }
}

/*!
  */
template <typename RandomAccessIterator, typename OutputIterator> inline
void Algorithm::Zisc::toBinaryTreeImpl(RandomAccessIterator begin, 
                                       RandomAccessIterator end,
                                       const std::size_t index,
                                       OutputIterator first) noexcept
{
  const std::size_t n = cast<std::size_t>(std::distance(begin, end));
  if (n == 1) {
    auto& leaf = begin;
    *(first + index) = std::move(*leaf);
  }
  else if (1 < n) {
    std::size_t s = 1;
    for (; s <= n; s = s << 1);
    s = s >> 1;
    // Find the center node
    const auto c = ((s >> 1) - 1) +
                   ((3 * (s >> 1) <= n + 1) ? (s >> 1) : (n + 1 - s));
    ZISC_ASSERT(isInBounds(c, 0ul, n),
                "The center index is out of range. n:", n, " i:", index, " c:", c);
    auto center = begin + c; 
    *(first + index) = std::move(*center);
    // Left child
    {
      const std::size_t left_index = (index << 1) + 1;
      toBinaryTreeImpl(begin, center, left_index, first);
    }
    // Right child
    {
      const std::size_t right_index = (index << 1) + 2;
      toBinaryTreeImpl(center + 1, end, right_index, first);
    }
  }
}

/*!
  */
template <typename Integer> inline
constexpr Integer Algorithm::makePopcountMask(const std::size_t stage) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr std::size_t bit_size = 8 * sizeof(Integer);
  Integer mask = cast<Integer>(0b1);
  std::size_t i = 0;
  for (; i < stage; ++i)
    mask = cast<Integer>(mask << (1 << i)) | mask;
  for (; cast<std::size_t>(2 << i) < bit_size; ++i)
    mask = cast<Integer>(mask << (2 << i)) | mask;
  return mask;
}

/*!
  */
template <std::size_t kStage, typename Integer> inline
Integer Algorithm::popcountFallback(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr Integer l_mask = makePopcountMask<Integer>(kStage);
  constexpr Integer h_mask = cast<Integer>(~l_mask);
  const Integer l = x & l_mask;
  const Integer h = x & h_mask;
  Integer y = l + cast<Integer>(h >> (1 << kStage));
  constexpr std::size_t bit_size = 8 * sizeof(Integer);
  if constexpr ((2 << kStage) < bit_size)
    y = popcountFallback<kStage + 1, Integer>(y);
  return y;
}

/*!
  */
template <Config::ImplType kImpl, typename Integer> inline
Integer Algorithm::popcountImpl(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr std::size_t size = sizeof(Integer);
  Integer y = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kClang) ||
                (kImpl == Config::ImplType::kGcc)) {
    if constexpr ((size == 8) && (sizeof(long) == 8)) {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned long, long>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(__builtin_popcountl(v));
    }
    else if constexpr ((size == 8) && (sizeof(long long) == 8)) {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned long long, long long>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(__builtin_popcountll(v));
    }
    else  {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, uint, int>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(__builtin_popcount(v));
    }
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    if constexpr (size == 2) {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned short, short>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(__popcnt16(v));
    }
    else if constexpr (size == 8) {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned long long, long long>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(__popcnt64(v));
    }
    else {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned int, int>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(__popcnt(v));
    }
  }
  else {
    y = popcountFallback<0>(x);
  }
  return y;
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic> inline
constexpr Arithmetic abs(const Arithmetic x) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  const auto y = Algorithm::abs(x);
  return y;
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
  const auto y = Algorithm::clamp(value, lower, upper);
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float invert(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point.");
  const auto y = Algorithm::invert(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Type1, typename Type2> inline
constexpr const std::common_type_t<Type1, Type2>& max(const Type1& a,
                                                      const Type2& b) noexcept
{
  const std::common_type_t<Type1, Type2>& y = Algorithm::max(a, b);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Type1, typename Type2> inline
constexpr const std::common_type_t<Type1, Type2>& min(const Type1& a,
                                                      const Type2& b) noexcept
{
  const std::common_type_t<Type1, Type2>& y = Algorithm::min(a, b);
  return y;
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
  const auto result = Algorithm::isInBounds<kIsLeftClosed,
                                            kIsRightClosed,
                                            Type,
                                            LowerType,
                                            UpperType>(value, lower, upper);
  return result;
}

/*!
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
 \details
 No detailed.
 */
template <typename Arithmetic> inline
constexpr bool isNegative(const Arithmetic n) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  const auto result = Algorithm::isNegative(n);
  return result;
}

/*!
  */
template <typename Integer> inline
constexpr bool isOdd(const Integer n) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  const auto result = Algorithm::isOdd(n);
  return result;
}

} // namespace zisc

#endif // ZISC_ALGORITHM_INL_HPP
