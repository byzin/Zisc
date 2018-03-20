/*!
  \file floating_point_bit-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOATING_POINT_BIT_INL_HPP
#define ZISC_FLOATING_POINT_BIT_INL_HPP

#include "floating_point_bit.hpp"
// Standard C++ library
#include <cstddef>
#include <cstdint>
#include <limits>
// Zisc
#include "floating_point.hpp"
#include "math.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Float> inline
constexpr std::size_t FloatingPointBit<Float>::exponentBias() noexcept
{
  return FloatingPoint<sizeof(FloatType)>::exponentBias();
}

/*!
  */
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::exponentBitMask() noexcept -> BitType
{
  return FloatingPoint<sizeof(FloatType)>::exponentBitMask();
}

/*!
  */
template <typename Float> inline
constexpr std::size_t FloatingPointBit<Float>::exponentBitSize() noexcept
{
  return FloatingPoint<sizeof(FloatType)>::exponentBitSize();
}

/*!
  */
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::getBits(const FloatType value)
    noexcept -> BitType
{
  return getSignBit(value) | getExponentBits(value) | getSignificandBits(value);
}

/*!
  */
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::getExponentBits(const FloatType value)
    noexcept -> BitType
{
  BitType bit = (isInf(value) || isNan(value))
      ? exponentBitMask()
      : BitType{0b0u};
  if (isNormal(value)) {
    FloatType v = abs(value);
    std::size_t exponent = exponentBias();

    constexpr std::size_t size = 8 * sizeof(BitType);
    constexpr FloatType p = constant::power<size>(cast<FloatType>(2.0));
    constexpr FloatType n = constant::power<size>(cast<FloatType>(0.5));
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
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::getSignificandBits(const FloatType value)
    noexcept -> BitType
{
  BitType bit = isNan(value)
      ? BitType{0b01} << (significandBitSize() - 1)
      : BitType{0b0u};

  int exponent = 0;

  constexpr FloatType s = constant::power<significandBitSize()>(cast<FloatType>(2.0));
  const FloatType v = isSubnormal(value)
      ? s * abs(value)
      : abs(value);

  if (isNormal(v)) {
    exponent = cast<int>(getExponentBits(v) >> significandBitSize()) -
               cast<int>(exponentBias());
    const FloatType t = constant::pow(cast<FloatType>(2.0), -exponent);
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
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::getSignBit(const FloatType value)
    noexcept -> BitType
{
  const BitType bit = isNegative(value)
      ? BitType{0b1u} << (exponentBitSize() + significandBitSize())
      : BitType{0b0u};
  return bit;
}

/*!
  */
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::makeFloat(
    const BitType exponent_bits,
    const BitType significand_bits) noexcept -> FloatType
{
  return makeFloat(0, exponent_bits, significand_bits);
}

/*!
  */
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::makeFloat(
    const BitType /* sign_bit */,
    const BitType /* exponent_bits */,
    const BitType /* significand_bits */) noexcept -> FloatType
{
  return static_cast<FloatType>(0.0);
}

/*!
  \details
  Please see "Generating uniform doubles in the unit interval"
  on 'http://xoroshiro.di.unimi.it/' for the details
  */
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::mapTo01Float(BitType x) noexcept
    -> FloatType
{
  constexpr FloatType k =
      constant::invert(cast<FloatType>(cast<BitType>(1) << (significandBitSize() + 1)));
  x = x >> exponentBitSize();
  return k * cast<FloatType>(x);
}

/*!
  */
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::signBitMask() noexcept -> BitType
{
  return FloatingPoint<sizeof(FloatType)>::signBitMask();
}

/*!
  */
template <typename Float> inline
constexpr auto FloatingPointBit<Float>::significandBitMask() noexcept -> BitType
{
  return FloatingPoint<sizeof(FloatType)>::significandBitMask();
}

/*!
  */
template <typename Float> inline
constexpr std::size_t FloatingPointBit<Float>::significandBitSize() noexcept
{
  return FloatingPoint<sizeof(FloatType)>::significandBitSize();
}

} // namespace zisc

#endif // ZISC_FLOATING_POINT_BIT_INL_HPP
