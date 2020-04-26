/*!
  \file ieee_754_binary-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_INL_HPP
#define ZISC_IEEE_754_BINARY_INL_HPP

#include "ieee_754_binary.hpp"
// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "algorithm.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr bool isFinite(const Float& x) noexcept
{
  if constexpr (kIsIeee754Binary<Float>) {
    return x.isFinite();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    const bool result = (-std::numeric_limits<Float>::max() <= x) &&
                        (x <= std::numeric_limits<Float>::max());
    return result;
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr bool isInf(const Float& x) noexcept
{
  if constexpr (kIsIeee754Binary<Float>) {
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
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr bool isNan(const Float& x) noexcept
{
  if constexpr (kIsIeee754Binary<Float>) {
    return x.isNan();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    const bool result = !(isFinite(x) || isInf(x));
    return result;
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr bool isNormal(const Float& x) noexcept
{
  if constexpr (kIsIeee754Binary<Float>) {
    return x.isNormal();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    const Float d = abs(x);
    const bool result = ((std::numeric_limits<Float>::min)() <= d) &&
                        (d <= (std::numeric_limits<Float>::max)());
    return result;
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr bool isSubnormal(const Float& x) noexcept
{
  if constexpr (kIsIeee754Binary<Float>) {
    return x.isSubnormal();
  }
  else {
    static_assert(kIsFloat<Float>, "Float isn't floating point type.");
    constexpr Float zero = cast<Float>(0);
    const Float d = abs(x);
    const bool result = (zero < d) && (d < (std::numeric_limits<Float>::min)());
    return result;
  }
}

/*!
  \details No detailed description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::Ieee754Binary() noexcept :
    data_{0} {
}

/*!
  \details No detailed description

  \param [in] data No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::Ieee754Binary(const BitType data) noexcept :
    data_{data}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::operator+() const noexcept
    -> Ieee754Binary
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::operator-() const noexcept
    -> Ieee754Binary
{
  const BitType b = cast<BitType>(bits() ^ signBitMask());
  const Ieee754Binary result{b};
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator float() const noexcept
{
  auto dst = convertTo<Ieee754BinaryFormat::kSingle>();
  const float result = dst.makeFloat(dst.bits());
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator double() const noexcept
{
  auto dst = convertTo<Ieee754BinaryFormat::kDouble>();
  const double result = dst.makeFloat(dst.bits());
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator long double() const noexcept
{
  const double d = cast<double>(*this);
  const long double result = cast<long double>(d);
  return result;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \tparam kRMode No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <Ieee754BinaryFormat kDstFormat, Ieee754RoundingMode kRMode> inline
constexpr Ieee754Binary<kDstFormat> Ieee754Binary<kFormat>::convertTo() const noexcept
{
  Ieee754Binary<kDstFormat> dst{};
  if constexpr (cast<std::size_t>(kDstFormat) < bitSize())
    dst = scaledDown<kDstFormat, kRMode>();
  else if constexpr (bitSize() < cast<std::size_t>(kDstFormat))
    dst = scaledUp<kDstFormat>();
  else
    dst = *this;
  return dst;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::epsilon() noexcept -> Ieee754Binary
{
  const Ieee754Binary o = one();
  // Compute the exponent of the epsilon
  BitType exp_bits = getBiasedExponent(o.bits()) - significandBitSize();
  exp_bits = cast<BitType>(exp_bits << significandBitSize());
  return Ieee754Binary{exp_bits};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::infinity() noexcept -> Ieee754Binary
{
  return Ieee754Binary{exponentBitMask()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isFinite() const noexcept
{
  const bool result = !(isInf() || isNan());
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isInf() const noexcept
{
  const BitType exp_bits = getExponentBits(bits());
  const BitType sig_bits = getSignificandBits(bits());
  const bool result = (exp_bits == exponentBitMask()) && (sig_bits == 0);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isNan() const noexcept
{
  const BitType exp_bits = getExponentBits(bits());
  const BitType sig_bits = getSignificandBits(bits());
  const bool result = (exp_bits == exponentBitMask()) && (0 < sig_bits);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isNormal() const noexcept
{
  const BitType exp_bits = getExponentBits(bits());
  const bool result = (0 < exp_bits) && (exp_bits < exponentBitMask());
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isSubnormal() const noexcept
{
  const BitType exp_bits = getExponentBits(bits());
  const BitType sig_bits = getSignificandBits(bits());
  const bool result = (exp_bits == 0) && (sig_bits != 0);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isZero() const noexcept
{
  const BitType value_bits = cast<BitType>(bits() & ~signBitMask());
  const bool result = value_bits == 0;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::max() noexcept -> Ieee754Binary
{
  const BitType min_bits = (min)().bits();
  const BitType max_bits = (exponentBitMask() - min_bits) | significandBitMask();
  return Ieee754Binary{max_bits};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::min() noexcept -> Ieee754Binary
{
  const BitType min_bits = BitType{0b1u} << significandBitSize();
  return Ieee754Binary{min_bits};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::one() noexcept -> Ieee754Binary
{
  const BitType exp_bits = cast<BitType>(exponentBias() << significandBitSize());
  return Ieee754Binary{exp_bits};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::quietNan() noexcept -> Ieee754Binary
{
  const BitType exp_bits = exponentBitMask();
  const BitType sig_bits = BitType{0b1u} << (significandBitSize() - 1);
  return Ieee754Binary{exp_bits | sig_bits};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::roundError() noexcept -> Ieee754Binary
{
  const auto o = one();
  BitType exp_bits = getBiasedExponent(o.bits()) - 1;
  exp_bits = cast<BitType>(exp_bits << significandBitSize());
  return Ieee754Binary{exp_bits};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::signalingNan() noexcept -> Ieee754Binary
{
  const BitType exp_bits = exponentBitMask();
  const BitType sig_bits = BitType{0b1u} << (significandBitSize() - 2);
  return Ieee754Binary{exp_bits | sig_bits};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::zero() noexcept -> Ieee754Binary
{
  return Ieee754Binary{};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::bits() const noexcept -> BitType
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754Binary<kFormat>::bitSize() noexcept
{
  static_assert(kBitSize == 8 * sizeof(Ieee754Binary),
                "The size of underlying data doesn't match with the format.");
  return kBitSize;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754Binary<kFormat>::exponentBias() noexcept
{
  const std::size_t bias = (0b1u << (exponentBitSize() - 1u)) - 1u;
  return bias;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::exponentBitMask() noexcept -> BitType
{
  BitType mask = 0b0;
  for (std::size_t bit = 0; bit < exponentBitSize(); ++bit) {
    constexpr BitType o = 0b1u;
    mask = mask | cast<BitType>(o << (significandBitSize() + bit));
  }
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754Binary<kFormat>::exponentBitSize() noexcept
{
  const std::size_t s = (kFormat == Ieee754BinaryFormat::kHalf) ? 5 :
                        (kFormat == Ieee754BinaryFormat::kSingle) ? 8 : 11;
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryFormat Ieee754Binary<kFormat>::format() noexcept
{
  return kFormat;
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getBiasedExponent(const BitType data) noexcept
    -> BitType
{
  const BitType b = getExponentBits(data);
  const BitType e = cast<BitType>(b >> significandBitSize());
  return e;
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr int Ieee754Binary<kFormat>::getExponent(const BitType data) noexcept
{
  const BitType biased_expt = getBiasedExponent(data);
  const int expt = cast<int>(biased_expt) - cast<int>(exponentBias());
  return expt;
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getExponentBits(const BitType data) noexcept
    -> BitType
{
  const BitType b = cast<BitType>(data & exponentBitMask());
  return b;
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getSignBit(const BitType data) noexcept
    -> BitType
{
  const BitType b = cast<BitType>(data & signBitMask());
  return b;
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getSignificandBits(const BitType data) noexcept
    -> BitType
{
  const BitType b = cast<BitType>(data & significandBitMask());
  return b;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::implicitBit() noexcept -> BitType
{
  const BitType b = BitType{0b1u} << significandBitSize();
  return b;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::makeBits(const FloatType value) noexcept
    -> BitType
{
  const auto exp_bits = makeExponentBits(value);
  const auto sig_bits = makeSignificandBits(value, exp_bits);
  const BitType b = cast<BitType>(makeSignBit(value) | exp_bits | sig_bits);
  return b;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::makeExponentBits(const FloatType value) noexcept
    -> BitType
{
  BitType b = zisc::isFinite(value) ? BitType{0b0u} : exponentBitMask();
  if (zisc::isNormal(value)) {
    constexpr std::size_t size = 8 * sizeof(uint64b);
    constexpr FloatType o = cast<FloatType>(1);
    constexpr FloatType p = calcExp2(cast<int>(size));
    constexpr FloatType n = o / p;
    FloatType v = abs(value);
    std::size_t expt = exponentBias();
    for (; p <= v; v = n * v)
      expt = expt + size;
    for (; v < o; v = p * v)
      expt = expt - size;
    const uint64b rest_value = cast<uint64b>(v);
    expt = expt + Algorithm::getExponent(rest_value);
    b = cast<BitType>(expt << significandBitSize());
  }
  return b;
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::makeFloat(const BitType data) noexcept
    -> FloatType
{
  const BitType exp_bits = getExponentBits(data);
  const BitType sig_bits = getSignificandBits(data);
  FloatType value = cast<FloatType>(0);
  if (exp_bits == exponentBitMask()) {
    // Special value case
    value = (sig_bits == 0)
        ? std::numeric_limits<FloatType>::infinity()
        : std::numeric_limits<FloatType>::quiet_NaN();
  }
  else if (exp_bits == 0) {
    // Subnormal value case
    constexpr auto unit = std::numeric_limits<FloatType>::denorm_min();
    const FloatType sig_v = cast<FloatType>(sig_bits);
    value = sig_v * unit;
  }
  else {
    // Normal value case
    constexpr auto unit = calcExp2(-cast<int>(significandBitSize()));
    const FloatType sig_v = cast<FloatType>(implicitBit() | sig_bits);
    const FloatType exp_v = calcExp2(getExponent(exp_bits));
    value = exp_v * (sig_v * unit);
  }

  // Align sign
  const BitType sign_bit = getSignBit(data);
  value = (sign_bit == 0) ? value : -value;

  return value;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::makeSignBit(const FloatType value) noexcept
    -> BitType
{
  const BitType b = isNegative(value) ? signBitMask() : BitType{0b0u};
  return b;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::makeSignificandBits(const FloatType value) noexcept
    -> BitType
{
  const auto exp_bits = makeExponentBits(value);
  const auto b = makeSignificandBits(value, exp_bits);
  return b;
}

/*!
  \details No detailed description

  \tparam UInt No description.
  \tparam kMode No description.
  \param [in] bits No description.
  \param [in] trailing_bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <Ieee754RoundingMode kMode, typename UInt> inline
constexpr auto Ieee754Binary<kFormat>::round(const BitType bits,
                                             const UInt trailing_bits) noexcept
    -> BitType
{
  static_assert(kIsUnsignedInteger<UInt>, "UInt isn't unsigned integer type.");
  const UInt middle = cast<UInt>(UInt{0b1} << (8 * sizeof(UInt) - 1));
  // Rounding to nearest
  // tie-break rule is 'nearest even'
  const bool is_rounded_up = ((trailing_bits == middle) && isOdd(bits)) ||
                             (middle < trailing_bits);
  const BitType result = is_rounded_up ? bits + 1 : bits;
  return result;
}

/*!
  \details No detailed description

  \param [in] data No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr void Ieee754Binary<kFormat>::setBits(const BitType data) noexcept
{
  data_ = data;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::signBitMask() noexcept -> BitType
{
  const BitType mask = BitType{0b1u} << (exponentBitSize() + significandBitSize());
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::significandBitMask() noexcept -> BitType
{
  BitType mask = 0b0;
  for (std::size_t bit = 0; bit < significandBitSize(); ++bit) {
    constexpr BitType o = 0b1u;
    mask = mask | cast<BitType>(o << bit);
  }
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754Binary<kFormat>::significandBitSize() noexcept
{
  const std::size_t s = (kFormat == Ieee754BinaryFormat::kHalf) ? 10 :
                        (kFormat == Ieee754BinaryFormat::kSingle) ? 23 : 52;
  return s;
}

/*!
  \details No detailed description

  \param [in] expt No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::calcExp2(const int expt) noexcept
    -> FloatType
{
  FloatType base = isNegative(expt) ? cast<FloatType>(0.5) : cast<FloatType>(2.0);
  FloatType x = cast<FloatType>(1);
  for (int e = abs(expt); 0 < e; e = e >> 1) {
    if (isOdd(e))
      x = x * base;
    if (1 < e)
      base = base * base;
  }
  return x;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> template <Ieee754BinaryFormat kDstFormat>
inline
constexpr Ieee754Binary<kDstFormat> Ieee754Binary<kFormat>::convertSpecialValue() const noexcept
{
  using DstBinary = Ieee754Binary<kDstFormat>;
  const DstBinary dst = isNan() ? DstBinary::quietNan() :
                        isInf() ? DstBinary::infinity()
                                : DstBinary::zero();
  return dst;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <Ieee754BinaryFormat kDstFormat, Ieee754RoundingMode kRMode> inline
constexpr Ieee754Binary<kDstFormat> Ieee754Binary<kFormat>::scaledDown() const noexcept
{
  using DstBinary = Ieee754Binary<kDstFormat>;
  using DstBitType = typename DstBinary::BitType;
  DstBinary dst = convertSpecialValue<kDstFormat>();
  if (isNormal() || isSubnormal()) {
    constexpr std::size_t exp_bias = exponentBias();
    constexpr std::size_t sig_size = significandBitSize();
    constexpr std::size_t dst_exp_bias = DstBinary::exponentBias();
    constexpr std::size_t dst_sig_size = DstBinary::significandBitSize();

    constexpr BitType lower_bound = ((exp_bias + 1) - dst_exp_bias) << sig_size;
    constexpr BitType upper_bound = ((exp_bias + 1) + dst_exp_bias) << sig_size;

    const BitType exp_bits = getExponentBits(bits());
    if (exp_bits < upper_bound) {
      constexpr std::size_t sig_size_diff = sig_size - dst_sig_size;
      constexpr BitType exp_bias_diff = (exp_bias - dst_exp_bias) << dst_sig_size;
      // Finite values
      const std::size_t shift_size = (exp_bits < lower_bound)
          ? getBiasedExponent(lower_bound - exp_bits) // Subnormal case
          : 0;                                        // Normal case
      if (shift_size <= dst_sig_size) {
        // Exponent bits
        const DstBitType dst_exp_bits = cast<DstBitType>((0 < shift_size)
            ? 0                                             // Subnormal case
            : (exp_bits >> sig_size_diff) - exp_bias_diff); // Normal case
        // Significand bits
        const BitType sig_bits = getRealSignificandBits(bits());
        const DstBitType dst_sig_bits = cast<DstBitType>(
            getSignificandBits(sig_bits >> shift_size) >> sig_size_diff);
        DstBitType dst_bits = cast<DstBitType>(dst_exp_bits | dst_sig_bits);
        // Rounding
        const std::size_t shift_diff = dst_sig_size - shift_size;
        BitType trailing_bits = getSignificandBits(sig_bits << shift_diff);
        trailing_bits = cast<BitType>(trailing_bits << (1 + exponentBitSize()));
        dst_bits = DstBinary::template round<kRMode>(dst_bits, trailing_bits);
        //
        dst.setBits(dst_bits);
      }
      else { // Underflow
        DstBitType dst_bits = 0;
        // Rounding
        const std::size_t shift_diff = shift_size - dst_sig_size;
        if (shift_diff <= significandBitSize()) {
          const BitType sig_bits = getRealSignificandBits(bits());
          BitType trailing_bits = getSignificandBits(sig_bits >> shift_diff);
          trailing_bits = cast<BitType>(trailing_bits << (1 + exponentBitSize()));
          dst_bits = DstBinary::template round<kRMode>(dst_bits, trailing_bits);
        }
        //
        dst.setBits(dst_bits);
      }
    }
    else { // Overflow
      dst = DstBinary::infinity();
    }
  }

  // Sign bit
  dst = (getSignBit(bits()) == 0) ? dst : -dst;

  return dst;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <Ieee754BinaryFormat kDstFormat> inline
constexpr Ieee754Binary<kDstFormat> Ieee754Binary<kFormat>::scaledUp() const noexcept
{
  using DstBinary = Ieee754Binary<kDstFormat>;
  using DstBitType = typename DstBinary::BitType;
  DstBinary dst = convertSpecialValue<kDstFormat>();
  if (isNormal() || isSubnormal()) {
    constexpr std::size_t exp_bias = exponentBias();
    constexpr std::size_t sig_size = significandBitSize();
    constexpr std::size_t dst_exp_bias = DstBinary::exponentBias();
    constexpr std::size_t dst_sig_size = DstBinary::significandBitSize();

    // Exponent bits
    const std::size_t expt = getBiasedExponent(bits());
    DstBitType dst_expt = cast<DstBitType>(expt + (dst_exp_bias - exp_bias));
    // Significand bits
    DstBitType dst_sig_bits = cast<DstBitType>(getSignificandBits(bits())) <<
                              (dst_sig_size - sig_size);
    const BitType exp_bits = getExponentBits(bits());
    if (exp_bits == 0) { // Subnormal case
      constexpr auto imp_expt = Algorithm::getExponent(DstBinary::implicitBit());
      const auto bit_expt = Algorithm::getExponent(dst_sig_bits);
      const DstBitType expt_diff = imp_expt - bit_expt;
      dst_expt = (1 + dst_expt) - expt_diff;
      dst_sig_bits = cast<DstBitType>(dst_sig_bits << expt_diff);
      dst_sig_bits = DstBinary::getSignificandBits(dst_sig_bits);
    }
    const DstBitType dst_exp_bits = cast<DstBitType>(dst_expt << dst_sig_size);
    const DstBitType dst_bits = cast<DstBitType>(dst_exp_bits | dst_sig_bits);
    dst.setBits(dst_bits);
  }

  // Sign bit
  dst = (getSignBit(bits()) == 0) ? dst : -dst;

  return dst;
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getRealSignificandBits(const BitType data) noexcept
    -> BitType
{
  const BitType b = cast<BitType>(implicitBit() | (data & significandBitMask()));
  return b;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \param [in] exp_bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::makeSignificandBits(
    const FloatType value,
    const BitType exp_bits) noexcept -> BitType
{
  BitType b = zisc::isNan(value) ? quietNan().bits() : BitType{0b0u};
  if (zisc::isSubnormal(value)) {
    constexpr auto denorm = std::numeric_limits<FloatType>::denorm_min();
    b = cast<BitType>(abs(value) / denorm);
  }
  else if (zisc::isNormal(value)) {
    constexpr FloatType denorm = calcExp2(cast<int>(significandBitSize()));
    const int expt = getExponent(exp_bits);
    const FloatType inv_exp_v = calcExp2(-expt);
    b = cast<BitType>(denorm * (inv_exp_v * abs(value)));
  }
  b = cast<BitType>(b & significandBitMask());
  return b;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator==(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = !(isNan(lhs) || isNan(rhs)) &&
                      ((lhs.isZero() && rhs.isZero()) || (lhs.bits() == rhs.bits()));
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator!=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = !(lhs == rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator<(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
{
  bool result = !(isNan(lhs) || isNan(rhs) || (lhs.isZero() && rhs.isZero()));
  if (result) {
    using Binary = Ieee754Binary<kFormat>;
    using SType = std::make_signed_t<typename Binary::BitType>;
    const auto get_signed = [](const auto u)
    {
      constexpr auto m = Binary::signBitMask();
      SType s = cast<SType>(u & ~m);
      s = ((u & m) == 0) ? s : -s;
      return s;
    };
    const SType sl = get_signed(lhs.bits());
    const SType sr = get_signed(rhs.bits());
    result = sl < sr;
  }
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator<=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = (lhs == rhs) || (lhs < rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator>(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = (rhs < lhs);
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator>=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = (rhs <= lhs);
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename Type, Ieee754RoundingMode kRMode> inline
constexpr Type castBinary(const float value) noexcept
{
  if constexpr (kIsFloat<Type>) {
    const Type result = cast<Type>(value);
    return result;
  }
  else if constexpr (kIsIeee754Binary<Type>) {
    const auto b = Binary32::makeBits(value);
    Binary32 b32{b};
    const Type result = castBinary<Type, Binary32::format(), kRMode>(b32);
    return result;
  }
  else {
    static_assert(std::is_void_v<Type>, "Invalid cast type is specified.");
    return Type{};
  }
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename Type, Ieee754RoundingMode kRMode> inline
constexpr Type castBinary(const double value) noexcept
{
  if constexpr (kIsFloat<Type>) {
    const Type result = cast<Type>(value);
    return result;
  }
  else if constexpr (kIsIeee754Binary<Type>) {
    const auto b = Binary64::makeBits(value);
    Binary64 b64{b};
    const Type result = castBinary<Type, Binary64::format(), kRMode>(b64);
    return result;
  }
  else {
    static_assert(std::is_void_v<Type>, "Invalid cast type is specified.");
    return Type{};
  }
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename Type, Ieee754RoundingMode kRMode> inline
constexpr Type castBinary(const long double value) noexcept
{
  const double d = cast<double>(value);
  const Type result = castBinary<Type, kRMode>(d);
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename Type, Ieee754BinaryFormat kFormat, Ieee754RoundingMode kRMode>
inline
constexpr Type castBinary(const Ieee754Binary<kFormat>& value) noexcept
{
  if constexpr (kIsFloat<Type>) {
    const Type result = cast<Type>(value);
    return result;
  }
  else if constexpr (kIsIeee754Binary<Type>) {
    constexpr auto dst_format = Type::format();
    const Type result = value.template convertTo<dst_format, kRMode>();
    return result;
  }
  else {
    static_assert(std::is_void_v<Type>, "Invalid cast type is specified.");
    return Type{};
  }
}

} // namespace zisc

// Specialize std::numeric_limits

namespace std {

template <zisc::Ieee754BinaryFormat kFormat>
class numeric_limits<zisc::Ieee754Binary<kFormat>>
{
 private:
  using BinaryType = zisc::Ieee754Binary<kFormat>;

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
  static constexpr bool is_iec559 =
      (kFormat == zisc::Ieee754BinaryFormat::kSingle) ||
      (kFormat == zisc::Ieee754BinaryFormat::kDouble);

  //! Identify types that represent a finite set of values
  static constexpr bool is_bounded = false;

  //! Identify types that handle overflows with modulo arithmetic
  static constexpr bool is_modulo = false;

  //! Number of radix digits that can be represented without change
  static constexpr int digits = zisc::cast<int>(BinaryType::significandBitSize() + 1);

  //! Number of base-10 digits that can be represented by the type
  static constexpr int digits10 = 
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? 3 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::digits10
          : std::numeric_limits<double>::digits10;

  //! Number of base-10 digits necessary to differentiate all values of this type
  static constexpr int max_digits10 = 
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? 5 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::max_digits10
          : std::numeric_limits<double>::max_digits10;

  //! The radix used by the representation
  static constexpr int radix = 2;

  //! One more than the smallest negative power of the radix
  static constexpr int min_exponent =
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? -13 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::min_exponent
          : std::numeric_limits<double>::min_exponent;

  //! The smallest negative power of ten
  static constexpr int min_exponent10 =
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? -4 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::min_exponent10
          : std::numeric_limits<double>::min_exponent10;

  //! One more than the largest integer power of the radix
  static constexpr int max_exponent =
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? 16 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::max_exponent
          : std::numeric_limits<double>::max_exponent;

  //! The largest integer power of ten
  static constexpr int max_exponent10 =
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? 4 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::max_exponent10
          : std::numeric_limits<double>::max_exponent10;

  //! Identify types which can cause arithmetic operations to trap
  static constexpr bool traps = true;

  //! Identify floating-point types that detect tinyness before rounding
  static constexpr bool tinyness_before = false;

  //! Return the smallest finite value
  static constexpr BinaryType min()
  {
    return (BinaryType::min)();
  }

  //! Return the lowest finite value
  static constexpr BinaryType lowest()
  {
    return -(BinaryType::max)();
  }

  //! Return the largest finite value
  static constexpr BinaryType max()
  {
    return +(BinaryType::max)();
  }

  //! Return the difference between 1.0 and the next representable value
  static constexpr BinaryType epsilon()
  {
    return BinaryType::epsilon();
  }

  //! Return the maximum rounding error
  static constexpr BinaryType round_error()
  {
    return BinaryType::roundError();
  }

  //! Return the positive infinity value
  static constexpr BinaryType infinity()
  {
    return BinaryType::infinity();
  }

  //! Return a quiet NaN value
  static constexpr BinaryType quiet_NaN()
  {
    return BinaryType::quietNan();
  }

  //! Return a signaling NaN value
  static constexpr BinaryType signaling_NaN()
  {
    return BinaryType::signalingNan();
  }

  //! Return the smallest positive subnormal value
  static constexpr BinaryType denorm_min()
  {
    return BinaryType{0b1u};
  }
};

} // namespace std

#endif // ZISC_IEEE_754_BINARY_INL_HPP
