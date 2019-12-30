/*!
  \file floating_point-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
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
#include "algorithm.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr FloatingPoint<kFormat>::FloatingPoint() noexcept :
    data_{0b0u}
{
  static_assert(sizeof(FloatingPoint) == sizeof(BitType),
                "The size of FloatingPoint is wrong.");
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr FloatingPoint<kFormat>::FloatingPoint(const BitType data) noexcept :
    FloatingPoint()
{
  set(data);
}

/*!
  */
template <FloatingPointFormat kFormat> template <FloatingPointFormat kSrcFormat>
inline
constexpr FloatingPoint<kFormat>::FloatingPoint(
    const FloatingPoint<kSrcFormat>& other) noexcept :
        FloatingPoint()
{
  assign(other);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator-() const noexcept -> FloatingPoint
{
  const BitType v = bits() ^ signBitMask();
  return FloatingPoint{v};
}

/*!
  */
template <FloatingPointFormat kFormat> template <FloatingPointFormat kSrcFormat>
inline
constexpr auto FloatingPoint<kFormat>::operator=(
    const FloatingPoint<kSrcFormat>& other) noexcept -> FloatingPoint&
{
  assign(other);
  return *this;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator+=(
    const FloatingPoint& other) noexcept -> FloatingPoint&
{
  *this = (*this + other);
  return *this;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator-=(
    const FloatingPoint& other) noexcept -> FloatingPoint&
{
  *this = (*this - other);
  return *this;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator*=(
    const FloatingPoint& other) noexcept -> FloatingPoint&
{
  *this = (*this * other);
  return *this;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator/=(
    const FloatingPoint& other) noexcept -> FloatingPoint&
{
  *this = (*this / other);
  return *this;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator++() noexcept -> FloatingPoint&
{
  *this += one();
  return *this;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator--() noexcept -> FloatingPoint&
{
  *this -= one();
  return *this;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator++(int) noexcept -> FloatingPoint
{
  const auto result = *this;
  *this += one();
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::operator--(int) noexcept -> FloatingPoint
{
  const auto result = *this;
  *this -= one();
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr FloatingPoint<kFormat>::operator float() const noexcept
{
  return SingleFloat{*this}.toFloat();
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr FloatingPoint<kFormat>::operator double() const noexcept
{
  return DoubleFloat{*this}.toFloat();
}

/*!
  */
template <FloatingPointFormat kFormat> template <FloatingPointFormat kSrcFormat>
inline
constexpr void FloatingPoint<kFormat>::assign(
    const FloatingPoint<kSrcFormat>& other) noexcept
{
  using SrcFloat = FloatingPoint<kSrcFormat>;
  if constexpr (sizeof(SrcFloat) < sizeof(FloatingPoint))
    data_ = other.template upscaled<kFormat>().bits();
  else if constexpr (sizeof(FloatingPoint) < sizeof(SrcFloat))
    data_ = other.template downscaled<kFormat>().bits();
  else
    data_ = other.bits();
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::bits() const noexcept -> BitType
{
  return data_;
}

/*!
  */
template <FloatingPointFormat kFormat> template <FloatingPointFormat kDstFormat>
inline
constexpr FloatingPoint<kDstFormat> FloatingPoint<kFormat>::downscaled()
    const noexcept
{
  using DstFloat = FloatingPoint<kDstFormat>;
  using DstBitType = typename DstFloat::BitType;
  static_assert(sizeof(DstFloat) < sizeof(FloatingPoint),
                "The size of dst float should be less than the size of this.");
  DstFloat dst;
  // Check special values
  if (isNan()) {
    dst = (bits() == quietNan().bits())
        ? DstFloat::quietNan()
        : DstFloat::signalingNan();
  }
  else if (isInf()) {
    dst = DstFloat::infinity();
  }
  else if (isZero()) {
    dst = DstFloat::zero();
  }
  // Finite values
  else {
    constexpr std::size_t exp_bias = exponentBias();
    constexpr std::size_t dst_exp_bias = DstFloat::exponentBias();
    constexpr std::size_t sig_size = significandBitSize();
    constexpr auto sig_mask = significandBitMask();
    constexpr std::size_t dst_sig_size = DstFloat::significandBitSize();

    constexpr BitType lower_bound = ((exp_bias + 1) - dst_exp_bias) << sig_size;
    constexpr BitType upper_bound = ((exp_bias + 1) + dst_exp_bias) << sig_size;

    const BitType src_exp_bit = bits() & exponentBitMask();
    if (src_exp_bit < upper_bound) {
      constexpr std::size_t sig_size_diff = sig_size - dst_sig_size;
      constexpr BitType exp_bias_diff = (exp_bias - dst_exp_bias) << dst_sig_size;
      // Finite values
      const std::size_t shift_size = (src_exp_bit < lower_bound)
          ? (lower_bound - src_exp_bit) >> sig_size // Subnormal case
          : 0;                        // Normal case
      if (shift_size <= dst_sig_size) {
        // Exponent bits
        const DstBitType dst_exp_bit = cast<DstBitType>((src_exp_bit < lower_bound)
           ? 0                                                // Subnormal
           : (src_exp_bit >> sig_size_diff) - exp_bias_diff); // Normal
        // Significand bits
        const DstBitType dst_sig_bit = cast<DstBitType>(
            (((implicitBit() | (bits() & sig_mask)) >> shift_size) & sig_mask) >>
            sig_size_diff);
        DstBitType dst_bit = dst_exp_bit | dst_sig_bit;
        // Rounding
        const DstBitType truncated_bit = cast<DstBitType>(
            (((bits() << dst_sig_size) & sig_mask) >> shift_size) >> sig_size_diff);
        dst_bit = DstFloat::round(dst_bit, truncated_bit);

        dst.set(dst_bit);
      }
      // Underflow
      else {
        dst = DstFloat::zero();
      }
    }
    // Overflow
    else {
      dst = DstFloat::infinity();
    }
  }

  const auto sign_bit = bits() & signBitMask();
  return (0 < sign_bit) ? -dst : dst;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::epsilon() noexcept -> FloatingPoint
{
  // Compute the exponent of the epsilon
  BitType exp_bit = exponentBitMask() >> significandBitSize();
  exp_bit = exp_bit - (exponentBias() + 1 + significandBitSize());
  exp_bit = cast<BitType>(exp_bit << significandBitSize());
  return FloatingPoint{exp_bit};
}

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
    mask = mask | cast<BitType>(one << (significandBitSize() + bit));
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
constexpr auto FloatingPoint<kFormat>::fromFloat(const FloatType value) noexcept
    -> FloatingPoint
{
  return FloatingPoint{makeBits(value)};
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::implicitBit() noexcept -> BitType
{
  return BitType{0b1u} << significandBitSize();
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::infinity() noexcept -> FloatingPoint
{
  return FloatingPoint{exponentBitMask()};
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool FloatingPoint<kFormat>::isFinite() const noexcept
{
  const bool result = !(isInf() || isNan());
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool FloatingPoint<kFormat>::isInf() const noexcept
{
  const BitType exp_bit = bits() & exponentBitMask();
  const BitType significand_bit = bits() & significandBitMask();
  const bool result = (exp_bit == exponentBitMask()) && (significand_bit == 0);
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool FloatingPoint<kFormat>::isNan() const noexcept
{
  const BitType exp_bit = bits() & exponentBitMask();
  const BitType significand_bit = bits() & significandBitMask();
  const bool result = (exp_bit == exponentBitMask()) && (0 < significand_bit);
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool FloatingPoint<kFormat>::isNormal() const noexcept
{
  const BitType exp_bit = bits() & exponentBitMask();
  const bool result = (0 < exp_bit) && (exp_bit < exponentBitMask());
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool FloatingPoint<kFormat>::isSubnormal() const noexcept
{
  const BitType exp_bit = bits() & exponentBitMask();
  const BitType significand_bit = bits() & significandBitMask();
  const bool result = (exp_bit == 0) && (0 < significand_bit);
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool FloatingPoint<kFormat>::isZero() const noexcept
{
  const BitType value_bit = bits() & ~signBitMask();
  const bool result = value_bit == 0;
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::makeBits(const FloatType value) noexcept
    -> BitType
{
  const auto exp_bit = makeExponentBits(value);
  return makeSignBit(value) | exp_bit | makeSignificandBits(value, exp_bit);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::makeExponentBits(const FloatType value)
    noexcept  -> BitType
{
  BitType data = (zisc::isInf(value) || zisc::isNan(value))
      ? exponentBitMask()
      : BitType{0b0u};
  if (zisc::isNormal(value)) {
    constexpr std::size_t size = 8 * sizeof(BitType);
    constexpr FloatType p = getPowered(cast<int>(size));
    constexpr FloatType n = cast<FloatType>(1) / p;
    FloatType v = abs(value);
    std::size_t exponent = exponentBias();
    for (; p <= v; v = n * v)
      exponent = exponent + size;
    for (; v < cast<FloatType>(1); v = p * v)
      exponent = exponent - size;
    for (auto bit = cast<BitType>(v); 0b1u < bit; bit = bit >> 1)
      ++exponent;
    data = cast<BitType>(exponent) << significandBitSize();
  }
  return data;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::makeFloat(const BitType data) noexcept
    -> FloatType
{
  const BitType exponent_bits = data & exponentBitMask();
  const BitType significand_bits = data & significandBitMask();
  FloatType value = cast<FloatType>(0);
  if (exponent_bits == exponentBitMask()) {
    // Special value case
    value = (significand_bits == 0)
        ? std::numeric_limits<FloatType>::infinity()
        : std::numeric_limits<FloatType>::quiet_NaN();
  }
  else if (exponent_bits == 0) {
    // Subnormal case
    constexpr auto unit = std::numeric_limits<FloatType>::denorm_min();
    const FloatType sig_v = cast<FloatType>(significand_bits);
    value = sig_v * unit;
  }
  else {
    // normalized value
    constexpr FloatType unit = getPowered(-cast<int>(significandBitSize()));
    const FloatType sig_v = cast<FloatType>(implicitBit() | significand_bits);
    const int exponent = cast<int>(exponent_bits >> significandBitSize()) -
                         cast<int>(exponentBias());
    const FloatType exp_v = getPowered(exponent);
    value = exp_v * (sig_v * unit);
  }
  const BitType sign_bit = data & signBitMask();
  return (sign_bit == signBitMask()) ? -value : value;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::makeSignificandBits(const FloatType value)
    noexcept -> BitType
{
  const auto exp_bit = makeExponentBits(value);
  return makeSignificandBits(value, exp_bit);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::makeSignBit(const FloatType value)
    noexcept  -> BitType
{
  const BitType bit = isNegative(value) ? signBitMask() : BitType{0b0u};
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
      cast<FloatType>(1) /
      cast<FloatType>(cast<BitType>(0b1u) << (significandBitSize() + 1));
  const BitType r = expandToBitSize(x) >> exponentBitSize();
  return k * cast<FloatType>(r);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::max() noexcept -> FloatingPoint
{
  constexpr BitType min_bit = BitType{0b1u} << significandBitSize();
  constexpr BitType max_bit = (exponentBitMask() - min_bit) | significandBitMask();
  return FloatingPoint{max_bit};
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::min() noexcept -> FloatingPoint
{
  constexpr BitType min_bit = BitType{0b1u} << significandBitSize();
  return FloatingPoint{min_bit};
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::one() noexcept -> FloatingPoint
{
  const auto exp_bit = cast<BitType>(exponentBias() << significandBitSize());
  return FloatingPoint{exp_bit};
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::quietNan() noexcept -> FloatingPoint
{
  const BitType exp_bit = exponentBitMask();
  const BitType significand_bit = BitType{0b1u} << (significandBitSize() - 1);
  return FloatingPoint{exp_bit | significand_bit};
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::round(
    const BitType bit,
    const BitType truncated_bit) noexcept -> BitType
{
  constexpr auto middle = BitType{0b1} << (significandBitSize() - 1);
  return (((truncated_bit == middle) && isOdd(bit)) || (middle < truncated_bit))
      ? bit + 1
      : bit;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::roundError() noexcept -> FloatingPoint
{
  auto exp_bit = one().bits() >> significandBitSize();
  exp_bit = exp_bit - 1;
  exp_bit = exp_bit << significandBitSize();
  return FloatingPoint{exp_bit};
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::signalingNan() noexcept -> FloatingPoint
{
  const BitType exp_bit = exponentBitMask();
  const BitType significand_bit = BitType{0b1u} << (significandBitSize() - 2);
  return FloatingPoint{exp_bit | significand_bit};
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
    mask = mask | cast<BitType>(one << bit);
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
template <FloatingPointFormat kFormat> inline
constexpr void FloatingPoint<kFormat>::setFloat(const FloatType value) noexcept
{
  data_ = makeBits(value);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr void FloatingPoint<kFormat>::set(const BitType data) noexcept
{
  data_ = data;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::toFloat() const noexcept -> FloatType
{
  return makeFloat(bits());
}

/*!
  */
template <FloatingPointFormat kFormat> template <FloatingPointFormat kDstFormat>
inline
constexpr FloatingPoint<kDstFormat> FloatingPoint<kFormat>::upscaled()
    const noexcept
{
  using DstFloat = FloatingPoint<kDstFormat>;
  using DstBitType = typename DstFloat::BitType;
  static_assert(sizeof(FloatingPoint) < sizeof(DstFloat),
                "The size of dst float should be greater than the size of this.");
  DstFloat dst;
  // Check special values
  if (isNan()) {
    dst = (bits() == quietNan().bits())
        ? DstFloat::quietNan()
        : DstFloat::signalingNan();
  }
  else if (isInf()) {
    dst = DstFloat::infinity();
  }
  else if (isZero()) {
    dst = DstFloat::zero();
  }
  // Finite values
  else {
    constexpr std::size_t exp_bias = exponentBias();
    constexpr std::size_t dst_exp_bias = DstFloat::exponentBias();
    constexpr std::size_t sig_size = significandBitSize();
    constexpr auto sig_mask = significandBitMask();
    constexpr std::size_t dst_sig_size = DstFloat::significandBitSize();

    const BitType src_exp_bit = (bits() & exponentBitMask()) >> sig_size;
    // Exponent bits
    DstBitType dst_ext_bit = src_exp_bit + (dst_exp_bias - exp_bias);
    // Significand bits
    DstBitType dst_sig_bit = cast<DstBitType>(bits() & sig_mask) << 
                             (dst_sig_size - sig_size);
    if (src_exp_bit == 0) {
      // Subnormal values
      ++dst_ext_bit; // for the hidden bit
      while ((dst_sig_bit & DstFloat::implicitBit()) != DstFloat::implicitBit()) {
        --dst_ext_bit;
        dst_sig_bit = dst_sig_bit << 1;
      }
      dst_sig_bit = dst_sig_bit & DstFloat::significandBitMask();
    }
    dst_ext_bit = dst_ext_bit << dst_sig_size;

    dst.set(dst_ext_bit | dst_sig_bit);
  }

  const auto sign_bit = bits() & signBitMask();
  return (0 < sign_bit) ? -dst : dst;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::zero() noexcept -> FloatingPoint
{
  return FloatingPoint{};
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
constexpr auto FloatingPoint<kFormat>::getPowered(const int exponent)
    noexcept -> FloatType 
{
  FloatType base = isNegative(exponent)
      ? cast<FloatType>(0.5)
      : cast<FloatType>(2.0);
  FloatType x = cast<FloatType>(1);
  for (int e = abs(exponent); 0 < e; e = e >> 1) {
    if (isOdd(e))
      x = x * base;
    if (1 < e)
      base = base * base;
  }
  return x;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr auto FloatingPoint<kFormat>::makeSignificandBits(const FloatType value,
                                                           const BitType exp_bit)
    noexcept  -> BitType
{
  BitType data = zisc::isNan(value) ? quietNan().bits() : BitType{0b0u};
  if (zisc::isSubnormal(value)) {
    constexpr auto denorm = std::numeric_limits<FloatType>::denorm_min();
    data = cast<BitType>(abs(value) / denorm); 
  }
  else if (zisc::isNormal(value)) {
    constexpr FloatType denorm = getPowered(significandBitSize());
    const int exponent = cast<int>(exp_bit >> significandBitSize()) -
                         cast<int>(exponentBias());
    const FloatType inv_exp_v = getPowered(-exponent);
    data = cast<BitType>(denorm * (inv_exp_v * abs(value)));
  }
  data = data & significandBitMask();
  return data;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr FloatingPoint<kFormat> operator+(
    const FloatingPoint<kFormat>& lhs,
    const FloatingPoint<kFormat>& rhs) noexcept
{
  using Float = FloatingPoint<kFormat>;
  if constexpr (kFormat == FloatingPointFormat::kHalf) {
    const auto result = SingleFloat{lhs} + SingleFloat{rhs};
    return HalfFloat{result};
  }
  else {
    const auto result = lhs.toFloat() + rhs.toFloat();
    return Float::fromFloat(result);
  }
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr FloatingPoint<kFormat> operator-(
    const FloatingPoint<kFormat>& lhs,
    const FloatingPoint<kFormat>& rhs) noexcept
{
  using Float = FloatingPoint<kFormat>;
  if constexpr (kFormat == FloatingPointFormat::kHalf) {
    const auto result = SingleFloat{lhs} - SingleFloat{rhs};
    return HalfFloat{result};
  }
  else {
    const auto result = lhs.toFloat() - rhs.toFloat();
    return Float::fromFloat(result);
  }
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr FloatingPoint<kFormat> operator*(
    const FloatingPoint<kFormat>& lhs,
    const FloatingPoint<kFormat>& rhs) noexcept
{
  using Float = FloatingPoint<kFormat>;
  if constexpr (kFormat == FloatingPointFormat::kHalf) {
    const auto result = SingleFloat{lhs} * SingleFloat{rhs};
    return HalfFloat{result};
  }
  else {
    const auto result = lhs.toFloat() * rhs.toFloat();
    return Float::fromFloat(result);
  }
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr FloatingPoint<kFormat> operator/(
    const FloatingPoint<kFormat>& lhs,
    const FloatingPoint<kFormat>& rhs) noexcept
{
  using Float = FloatingPoint<kFormat>;
  if constexpr (kFormat == FloatingPointFormat::kHalf) {
    const auto result = SingleFloat{lhs} / SingleFloat{rhs};
    return HalfFloat{result};
  }
  else {
    const auto result = lhs.toFloat() / rhs.toFloat();
    return Float::fromFloat(result);
  }
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool operator==(const FloatingPoint<kFormat>& lhs,
                          const FloatingPoint<kFormat>& rhs) noexcept
{
  return !isNan(lhs) && ((isZero(lhs) && isZero(rhs)) || (lhs.bits() == rhs.bits()));
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool operator!=(const FloatingPoint<kFormat>& lhs,
                          const FloatingPoint<kFormat>& rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool operator<(const FloatingPoint<kFormat>& lhs,
                         const FloatingPoint<kFormat>& rhs) noexcept
{
  bool result = !(isNan(lhs) || isNan(rhs) || (isZero(lhs) && isZero(rhs)));
  if (result) {
    using SType = std::make_signed_t<typename FloatingPoint<kFormat>::BitType>;
    const auto get_signed_value = [](const auto v)
    {
      constexpr auto m = FloatingPoint<kFormat>::signBitMask();
      return ((v & m) == 0) ? cast<SType>(v & ~m) : -cast<SType>(v & ~m);
    };
    result = get_signed_value(lhs.bits()) < get_signed_value(rhs.bits());
  }
  return result;
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool operator<=(const FloatingPoint<kFormat>& lhs,
                          const FloatingPoint<kFormat>& rhs) noexcept
{
  return (lhs == rhs) || (lhs < rhs);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool operator>(const FloatingPoint<kFormat>& lhs,
                         const FloatingPoint<kFormat>& rhs) noexcept
{
  return (rhs < lhs);
}

/*!
  */
template <FloatingPointFormat kFormat> inline
constexpr bool operator>=(const FloatingPoint<kFormat>& lhs,
                          const FloatingPoint<kFormat>& rhs) noexcept
{
  return (rhs <= lhs);
}

/*!
  */
template <typename Float> inline
constexpr bool isFinite(const Float& x) noexcept
{
  if constexpr (kIsZiscFloatType<Float>) {
    return x.isFinite();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    const bool result = !(isInf(x) || isNan(x));
    return result;
  }
}

/*!
  */
template <typename Float> inline
constexpr bool isInf(const Float& x) noexcept
{
  if constexpr (kIsZiscFloatType<Float>) {
    return x.isInf();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    const bool result = (x == std::numeric_limits<Float>::infinity()) ||
                        (x == -std::numeric_limits<Float>::infinity());
    return result;
  }
}

/*!
  */
template <typename Float> inline
constexpr bool isNan(const Float& x) noexcept
{
  if constexpr (kIsZiscFloatType<Float>) {
    return x.isNan();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    const bool result = x != x;
    return result;
  }
}

/*!
  */
template <typename Float> inline
constexpr bool isNormal(const Float& x) noexcept
{
  if constexpr (kIsZiscFloatType<Float>) {
    return x.isNormal();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    const Float d = abs(x);
    const bool result = (std::numeric_limits<Float>::min() <= d) &&
                        (d <= std::numeric_limits<Float>::max());
    return result;
  }
}

/*!
  */
template <typename Float> inline
constexpr bool isSubnormal(const Float& x) noexcept
{
  if constexpr (kIsZiscFloatType<Float>) {
    return x.isSubnormal();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    constexpr Float zero = cast<Float>(0);
    const Float d = abs(x);
    const bool result = (zero < d) && (d < std::numeric_limits<Float>::min());
    return result;
  }
}

/*!
  */
template <typename Float> inline
constexpr bool isZero(const Float& x) noexcept
{
  if constexpr (kIsZiscFloatType<Float>) {
    return x.isZero();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    constexpr Float zero = cast<Float>(0);
    const bool result = (zero == x);
    return result;
  }
}

} // namespace zisc

// Specialize std::numeric_limits

namespace std {

template <zisc::FloatingPointFormat kFormat>
class numeric_limits<zisc::FloatingPoint<kFormat>>
{
 private:
  using Float = zisc::FloatingPoint<kFormat>;

 public:
  //! Identify types for which std::numeric_limits is specialized
  static constexpr bool is_specialized = true;

  //! Identify signed types
  static constexpr bool is_signed = true;

  //! Identify integer types
  static constexpr bool is_integer = false;

  //! Identify exact types
  static constexpr bool is_exact = false;

  //! Identify floating-point types that can represent the special value "positive infinity"
  static constexpr bool has_infinity = true;

  //! Identify floating-point types that can represent the special value "quiet not-a-number"
  static constexpr bool has_quiet_NaN = true;

  //! Identify floating-point types that can represent the special value "signalig not-a-number"
  static constexpr bool has_signaling_NaN = true;

  //! Identify the denormalization style used by the floating-point type
  static constexpr bool has_denorm = true;

  //!
  static constexpr bool has_denorm_loss = false; 

  //! Identify the rounding style used by the type
  static constexpr std::float_round_style round_style = std::round_to_nearest;

  //! Identify the IEC 559/IEEE 754 floating-point type
  static constexpr bool is_iec559 = true;

  //! Identify types that represent a finite set of values
  static constexpr bool is_bounded = true;

  //! Identify types that handle overflows with modulo arithmetic
  static constexpr bool is_modulo = false;

  //! Number of radix digits that can be represented without change
  static constexpr int digits = zisc::cast<int>(Float::significandBitSize() + 1);

  //! Number of base-10 digits that can be represented by the type
  static constexpr int digits10 = 
      (kFormat == zisc::FloatingPointFormat::kHalf)
          ? 2 :
      (kFormat == zisc::FloatingPointFormat::kSingle)
          ? std::numeric_limits<float>::digits10
          : std::numeric_limits<double>::digits10;

  //! Number of base-10 digits necessary to differentiate all values of this type
  static constexpr int max_digits10 = 
      (kFormat == zisc::FloatingPointFormat::kHalf)
          ? 5 :
      (kFormat == zisc::FloatingPointFormat::kSingle)
          ? std::numeric_limits<float>::max_digits10
          : std::numeric_limits<double>::max_digits10;

  //! The radix used by the representation
  static constexpr int radix = 2;

  //! One more than the smallest negative power of the radix
  static constexpr int min_exponent =
      (kFormat == zisc::FloatingPointFormat::kHalf)
          ? -13 :
      (kFormat == zisc::FloatingPointFormat::kSingle)
          ? std::numeric_limits<float>::min_exponent
          : std::numeric_limits<double>::min_exponent;

  //! The smallest negative power of ten
  static constexpr int min_exponent10 =
      (kFormat == zisc::FloatingPointFormat::kHalf)
          ? -4 :
      (kFormat == zisc::FloatingPointFormat::kSingle)
          ? std::numeric_limits<float>::min_exponent10
          : std::numeric_limits<double>::min_exponent10;

  //! One more than the largest integer power of the radix
  static constexpr int max_exponent =
      (kFormat == zisc::FloatingPointFormat::kHalf)
          ? 16 :
      (kFormat == zisc::FloatingPointFormat::kSingle)
          ? std::numeric_limits<float>::max_exponent
          : std::numeric_limits<double>::max_exponent;

  //! The largest integer power of ten
  static constexpr int max_exponent10 =
      (kFormat == zisc::FloatingPointFormat::kHalf)
          ? 4 :
      (kFormat == zisc::FloatingPointFormat::kSingle)
          ? std::numeric_limits<float>::max_exponent10
          : std::numeric_limits<double>::max_exponent10;

  //! Identify types which can cause arithmetic operations to trap
  static constexpr bool traps = false;

  //! Identify floating-point types that detect tinyness before rounding
  static constexpr bool tinyness_before = false;

  //! Return the smallest finite value
  static constexpr Float min()
  {
    return Float::min();
  }

  //! Return the lowest finite value
  static constexpr Float lowest()
  {
    return -Float::max();
  }

  //! Return the largest finite value
  static constexpr Float max()
  {
    return Float::max();
  }

  //! Return the difference between 1.0 and the next representable value
  static constexpr Float epsilon()
  {
    return Float::epsilon();
  }

  //! Return the maximum rounding error
  static constexpr Float round_error()
  {
    return Float::roundError();
  }

  //! Return the positive infinity value
  static constexpr Float infinity()
  {
    return Float::infinity();
  }

  //! Return a quiet NaN value
  static constexpr Float quiet_NaN()
  {
    return Float::quietNan();
  }

  //! Return a signaling NaN value
  static constexpr Float signaling_NaN()
  {
    return Float::signalingNan();
  }

  //! Return the smallest positive subnormal value
  static constexpr Float denorm_min()
  {
    return Float{0b1u};
  }
};

} // namespace std

#endif // ZISC_FLOATING_POINT_INL_HPP
