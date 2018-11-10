/*!
  \file floating_point-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOATING_POINT_INL_HPP
#define ZISC_FLOATING_POINT_INL_HPP

#include "floating_point.hpp"
// Standard C++ library
#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr std::size_t FloatingPoint<kFormat>::exponentBias() noexcept
{
  const std::size_t bias = (0b1u << (exponentBitSize() - 1)) - 1u;
  return bias;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::exponentBitMask() noexcept -> BitType
{
  BitType mask = 0b0u;
  for (std::size_t bit = 0; bit < exponentBitSize(); ++bit) {
    constexpr BitType one = 0b1u;
    mask = cast<BitType>(mask | (one << (bit + significandBitSize())));
  }
  return mask;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr std::size_t FloatingPoint<kFormat>::exponentBitSize() noexcept
{
  const std::size_t bit_size = (kFormat == FloatingPointFormat::kHalf) ? 5 :
                               (kFormat == FloatingPointFormat::kSingle) ? 8 : 11;
  return bit_size;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::getBits(const FloatType value) noexcept
    -> BitType
{
  return getSignBit(value) | getExponentBits(value) | getSignificandBits(value);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::getExponentBits(const FloatType value)
    noexcept  -> BitType
{
  BitType bit = (isInf(value) || isNan(value))
      ? exponentBitMask()
      : BitType{0b0u};
  if (isNormal(value)) {
    FloatType v = abs(value);
    std::size_t exponent = exponentBias();

    constexpr std::size_t size = 8 * sizeof(BitType);
    constexpr FloatType p = getPowered(size);
    constexpr FloatType n = cast<FloatType>(1.0) / p;
    for (; p <= v; v = n * v)
      exponent = exponent + size;
    for (; v < cast<FloatType>(1.0); v = p * v)
      exponent = exponent - size;

    bit = cast<BitType>(v);
    for (; cast<BitType>(0x1u) < bit; bit = bit >> 1)
      ++exponent;

    bit = cast<BitType>(exponent) << significandBitSize();
  }
  return bit;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::getSignificandBits(const FloatType value)
    noexcept  -> BitType
{
  BitType bit = isNan(value)
      ? BitType{0b01u} << (significandBitSize() - 1)
      : BitType{0b0u};

  int exponent = 0;

  constexpr FloatType s = getPowered(significandBitSize());
  const FloatType v = isSubnormal(value) ? s * abs(value) : abs(value);
  if (isNormal(v)) {
    exponent = cast<int>(getExponentBits(v) >> significandBitSize()) -
               cast<int>(exponentBias());
    const FloatType t = (0 < exponent)
      ? cast<FloatType>(1.0) / getPowered(exponent)
      : getPowered(-exponent);
    bit = cast<BitType>(s * (t * v));
  }
  if (isSubnormal(value)) {
    exponent = exponent + cast<int>(exponentBias()) - 1;
    bit = (BitType{0b1u} << exponent) |
          (bit >> (cast<int>(significandBitSize()) - exponent));
  }
  bit = bit & significandBitMask();
  return bit;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::getSignBit(const FloatType value)
    noexcept  -> BitType
{
  const BitType bit = isNegative(value)
      ? BitType{0b1u} << (exponentBitSize() + significandBitSize())
      : BitType{0b0u};
  return bit;
}

/*!
  \details
  Please see "Generating uniform doubles in the unit interval"
  on 'http://xoroshiro.di.unimi.it/' for the details
  */
template <FloatingPointFormat kFormat> template <typename UInt> inline
constexpr auto FloatingPoint<kFormat>::mapTo01(const UInt x) noexcept
    -> FloatType
{
  static_assert(kIsUnsignedInteger<UInt>, "UInt isn't unsigned integer.");
  constexpr FloatType k =
      cast<FloatType>(1.0) /
      cast<FloatType>(cast<BitType>(1) << (significandBitSize() + 1));
  const BitType r = expandToBitSize(x) >> exponentBitSize();
  return k * cast<FloatType>(r);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::signBitMask() noexcept -> BitType
{
  const auto mask = BitType{0b1u} << (exponentBitSize() + significandBitSize());
  return mask;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::significandBitMask() noexcept -> BitType
{
  BitType mask = 0b0u;
  for (std::size_t bit = 0; bit < significandBitSize(); ++bit) {
    constexpr BitType one = 0b1u;
    mask = cast<BitType>(mask | (one << bit));
  }
  return mask;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr std::size_t FloatingPoint<kFormat>::significandBitSize() noexcept
{
  const std::size_t bit_size = (kFormat == FloatingPointFormat::kHalf) ? 10 :
                               (kFormat == FloatingPointFormat::kSingle) ? 23 : 52;
  return bit_size;
}

/*!
  */
template <FloatingPointFormat kFormat> template <typename UInt> inline
constexpr auto FloatingPoint<kFormat>::expandToBitSize(const UInt x) noexcept
    -> BitType
{
  static_assert(kIsUnsignedInteger<UInt>, "UInt isn't unsigned integer.");
  constexpr std::size_t bit_size = sizeof(BitType);
  constexpr std::size_t int_size = sizeof(UInt);
  if constexpr (int_size == bit_size) {
    return x;
  }
  else if constexpr (int_size < bit_size) {
    constexpr std::size_t diff = bit_size - int_size;
    return cast<BitType>(x) << (diff * 8);
  }
  else {
    constexpr std::size_t diff = int_size - bit_size;
    return cast<BitType>(x >> (diff * 8));
  }
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::getPowered(std::size_t exponent)
    noexcept -> FloatType 
{
  FloatType base = cast<FloatType>(2.0);
  FloatType x = cast<FloatType>(1.0);
  for (; 0 < exponent; exponent = exponent >> 1) {
    if (isOdd(exponent))
      x = x * base;
    if (1 < exponent)
      base = base * base;
  }
  return x;
}

/*!
  */
template <typename Float> inline
constexpr bool isFinite(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const bool result = !(isInf(x) || isNan(x));
  return result;
}

/*!
  */
template <typename Float> inline
constexpr bool isInf(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const bool result = (x == std::numeric_limits<Float>::infinity()) ||
                      (x == -std::numeric_limits<Float>::infinity());
  return result;
}

/*!
  */
template <typename Float> inline
constexpr bool isNan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const bool result = x != x;
  return result;
}

/*!
  */
template <typename Float> inline
constexpr bool isNormal(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float d = abs(x);
  const bool result = (std::numeric_limits<Float>::min() <= d) &&
                      (d <= std::numeric_limits<Float>::max());
  return result;
}

/*!
  */
template <typename Float> inline
constexpr bool isSubnormal(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float zero = cast<Float>(0.0);
  const Float d = abs(x);
  const bool result = (zero < d) && (d < std::numeric_limits<Float>::min());
  return result;
}

} // namespace zisc

#endif // ZISC_FLOATING_POINT_INL_HPP
