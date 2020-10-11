/*!
  \file algorithm-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
// Zisc
#include "error.hpp"
#include "memory/std_memory_resource.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Arithmetic No description.
  \param [in] x No description.
  \return No description
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

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
Integer Algorithm::clz(const Integer x) noexcept
{
  const auto y = clzImpl<Config::implType()>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Algorithm::invert(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point.");
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

  \tparam Arithmetic No description.
  \param [in] x No description.
  \return No description
  */
template <typename Arithmetic> inline
constexpr bool Algorithm::isNegative(const Arithmetic x) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  if constexpr (std::is_signed_v<Arithmetic>) {
    constexpr auto zero = cast<Arithmetic>(0);
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

  \tparam Integer No description.
  \param [in] x \a x should be greater than 0
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
    constexpr std::size_t half_bit_size = sizeof(Integer) * 4;
    for (Integer bits = half_bit_size; 0 < bits; bits = bits >> 1) {
      const Integer upper_bits = x >> bits;
      exponent += (upper_bits != 0) ? bits : 0;
      x = (upper_bits != 0) ? upper_bits : x; 
    }
  }
  return exponent;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
constexpr bool Algorithm::isOdd(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr auto lsb = cast<Integer>(0b01);
  const auto result = ((x & lsb) == lsb);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
constexpr bool Algorithm::isPowerOf2(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  const auto result = (x & (x - 1)) == 0;
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
Integer Algorithm::popcount(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  const auto y = popcountImpl<Config::implType()>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x The \a x should be positive.
  \return The smallest power of 2 that is greater or equal to \a x
          if \a x is positive, otherwize \a x
  */
template <typename Integer> inline
constexpr Integer Algorithm::roundUpToPowOf2(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  const auto e = getExponent(x) + (isPowerOf2(x) ? 0 : 1);
  const Integer y = cast<Integer>(Integer{0b1} << e);
  return y;
}

/*!
  \details No detailed description

  \tparam RandomAccessIterator No description.
  \tparam Type No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] value No description.
  \return No description
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator Algorithm::searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value) noexcept
{
  //! \todo exception check
  const auto result = Config::isStlBinaryTreeUsed()
    ? Stl::searchBinaryTree(begin, end, value)
    : Zisc::searchBinaryTree(begin, end, value);
  return result;
}

/*!
  \details No detailed description

  \tparam RandomAccessIterator No description.
  \param [in,out] begin No description.
  \param [in,out] end No description.
  */
template <typename RandomAccessIterator> inline
void Algorithm::toBinaryTree(RandomAccessIterator begin, 
                             RandomAccessIterator end) noexcept
{
  //! \todo exception check
  if (Config::isStlBinaryTreeUsed())
    Stl::toBinaryTree(begin, end);
  else
    Zisc::toBinaryTree(begin, end);
}

/*!
  \details No detailed description

  \tparam RandomAccessIterator No description.
  \tparam Type No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] value No description.
  \return No description
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator Algorithm::Stl::searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value) noexcept
{
  //! \todo exception check
  auto result = std::lower_bound(begin, end, value);
  result = (*result == value) ? result : result - 1;
  return result;
}

/*!
  \details No detailed description

  \tparam RandomAccessIterator No description.
  \param [in,out] begin No description.
  \param [in,out] end No description.
  */
template <typename RandomAccessIterator> inline
void Algorithm::Stl::toBinaryTree(
    RandomAccessIterator begin, 
    RandomAccessIterator end) noexcept
{
  //! \todo exception check
  std::sort(begin, end);
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
constexpr Integer Algorithm::Zisc::clz(const Integer x) noexcept
{
  constexpr Integer size = 8 * sizeof(Integer);
  const Integer expt = Algorithm::getExponent(x);
  const Integer y = (x == 0) ? size : (size - 1) - expt;
  return y;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
constexpr Integer Algorithm::Zisc::popcount(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  const Integer y = popcountImpl<Integer, 0>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam RandomAccessIterator No description.
  \tparam Type No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] value No description.
  \return No description
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator Algorithm::Zisc::searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value) noexcept
{
  //! \todo exception check
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
  \details No detailed description

  \tparam RandomAccessIterator No description.
  \param [in,out] begin No description.
  \param [in,out] end No description.
  */
template <typename RandomAccessIterator> inline
void Algorithm::Zisc::toBinaryTree(
    RandomAccessIterator begin, 
    RandomAccessIterator end) noexcept
{
  //! \todo exception check
  std::sort(begin, end);
  const auto size = std::distance(begin, end);
  ZISC_ASSERT(0 < size, "The end is in advance of the begin.");
  if (1 < size) {
    // Create a temp array
    using Type = typename std::iterator_traits<RandomAccessIterator>::value_type;
    //! \todo Remove temporary array. Optimize memory usage
    std::vector<Type> array;
    array.reserve(cast<std::size_t>(size));
    for (auto iterator = begin; iterator != end; ++iterator)
      array.emplace_back(std::move(*iterator));
    // Transform an array to a binary tree
    toBinaryTreeImpl(array.begin(), array.end(), 0, begin);
  }
}

/*!
  \details No detailed description

  \tparam Integer description.
  \param [in] stage No description.
  \return No description
  */
template <typename Integer> inline
constexpr Integer Algorithm::Zisc::makePopcountMask(const std::size_t stage) noexcept
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
  \details No detailed description

  \tparam Integer No description.
  \tparam kStage No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer, std::size_t kStage> inline
constexpr Integer Algorithm::Zisc::popcountImpl(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr Integer l_mask = makePopcountMask<Integer>(kStage);
  constexpr Integer h_mask = cast<Integer>(~l_mask);
  const Integer l = x & l_mask;
  const Integer h = x & h_mask;
  Integer y = l + cast<Integer>(h >> (1 << kStage));
  constexpr std::size_t bit_size = 8 * sizeof(Integer);
  if constexpr ((2 << kStage) < bit_size)
    y = popcountImpl<Integer, kStage + 1>(y);
  return y;
}

/*!
  \details No detailed description

  \tparam RandomAccessIterator No description.
  \tparam OutputIterator No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] index No description.
  \param [out] first No description.
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
  \details No detailed description

  \tparam SrcInteger No description.
  \tparam DstInteger No description.
  \return No description
  */
template <typename SrcInteger, typename DstInteger> inline
constexpr DstInteger Algorithm::makeClzMask() noexcept
{
  const std::size_t size = 8 * (sizeof(DstInteger) - sizeof(SrcInteger));
  DstInteger mask = 0;
  for (std::size_t i = 0; i < size; ++i) {
    const DstInteger one = 0b01;
    mask = (mask << 1) | one;
  }
  return mask;
}

/*!
  \details No detailed description

  \tparam kImpl No description.
  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <Config::ImplType kImpl, typename Integer> inline
Integer Algorithm::clzImpl(const Integer x) noexcept
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
      y = cast<Integer>(__builtin_clzl(v));
    }
    else if constexpr ((size == 8) && (sizeof(long long) == 8)) {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned long long, long long>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(__builtin_clzll(v));
    }
    else  {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, uint, int>;
      constexpr IType mask = makeClzMask<Integer, IType>();
      constexpr std::size_t size_diff = 8 * (sizeof(IType) - size);
      const IType v = cast<IType>(x) << size_diff | mask;
      y = cast<Integer>(__builtin_clz(v));
    }
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    if constexpr (size == 2) {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned short, short>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(clzMsvcImpl(v));
    }
    else if constexpr (size == 8) {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned long long, long long>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(clzMsvcImpl(v));
    }
    else {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned int, int>;
      constexpr IType mask = makeClzMask<Integer, IType>();
      constexpr std::size_t size_diff = 8 * (sizeof(IType) - size);
      const IType v = cast<IType>(x) << size_diff | mask;
      y = cast<Integer>(clzMsvcImpl(v));
    }
  }
  else {
    y = Zisc::clz(x);
  }
  return y;
}

/*!
  \details No detailed description

  \tparam kImpl No description.
  \tparam Integer No description.
  \param [in] x No description.
  \return No description
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
      y = cast<Integer>(popcountMsvcImpl(v));
    }
    else if constexpr (size == 8) {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned long long, long long>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(popcountMsvcImpl(v));
    }
    else {
      constexpr bool is_uint = std::is_unsigned_v<Integer>;
      using IType = std::conditional_t<is_uint, unsigned int, int>;
      const IType v = cast<IType>(x);
      y = cast<Integer>(popcountMsvcImpl(v));
    }
  }
  else {
    y = Zisc::popcount(x);
  }
  return y;
}

/*!
  \details No detailed description

  \tparam Arithmetic description.
  \param [in] x No description.
  \return No description
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
template <typename Float> inline
constexpr Float invert(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point.");
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

  \tparam Arithmetic No description.
  \param [in] x No description.
  \return No description
  */
template <typename Arithmetic> inline
constexpr bool isNegative(const Arithmetic x) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  const auto result = Algorithm::isNegative(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
constexpr bool isOdd(const Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  const auto result = Algorithm::isOdd(x);
  return result;
}

} // namespace zisc

#endif // ZISC_ALGORITHM_INL_HPP
