/*!
  \file bit_cast_gcc-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BIT_CAST_GCC_INL_HPP
#define ZISC_BIT_CAST_GCC_INL_HPP

#include "bit_cast_gcc.hpp"
// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc_config.hpp"

namespace zisc {

namespace gcc {

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
  if constexpr (std::is_signed_v<Arithmetic>) {
    constexpr auto zero = static_cast<Arithmetic>(0);
    const bool result = x < zero;
    return result;
  }
  else {
    return false;
  }
}

/*!
  \details No detailed description

  \tparam Arithmetic No description.
  \param [in] x No description.
  \return No description
  */
template <typename Arithmetic> inline
constexpr Arithmetic abs(const Arithmetic x) noexcept
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

  \tparam Integer No description.
  \param [in] x \a x should be greater than 0
  \return The exponent corresponds the \a x in the base-2 if \a x is positive, 
          otherwize \a x 
  */
template <typename Integer> inline
constexpr Integer getExponentValue(Integer x) noexcept
{
  static_assert(std::is_integral_v<Integer>, "Integer isn't integer type.");
  constexpr auto zero = static_cast<Integer>(0);
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
    static_assert(std::is_floating_point_v<Float>, "Float isn't floating point type.");
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
    static_assert(std::is_floating_point_v<Float>, "Float isn't floating point type.");
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
    static_assert(std::is_floating_point_v<Float>, "Float isn't floating point type.");
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
    static_assert(std::is_floating_point_v<Float>, "Float isn't floating point type.");
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
    static_assert(std::is_floating_point_v<Float>, "Float isn't floating point type.");
    constexpr Float zero = static_cast<Float>(0);
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
  const BitType b = static_cast<BitType>(bits() ^ signBitMask());
  const Ieee754Binary result{b};
  return result;
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
  exp_bits = static_cast<BitType>(exp_bits << significandBitSize());
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
  const BitType value_bits = static_cast<BitType>(bits() & ~signBitMask());
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
  const BitType exp_bits = static_cast<BitType>(exponentBias() << significandBitSize());
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
  exp_bits = static_cast<BitType>(exp_bits << significandBitSize());
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

  \tparam UInt No description.
  \param [in] x No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> template <typename UInt> inline
constexpr auto Ieee754Binary<kFormat>::mapTo01(const UInt x) noexcept
    -> FloatType
{
  static_assert(std::is_integral_v<UInt>, "UInt isn't unsigned integer.");
  constexpr FloatType k =
      static_cast<FloatType>(1) /
      static_cast<FloatType>(static_cast<BitType>(0b1u) << (significandBitSize() + 1));
  const BitType r = static_cast<BitType>(expandBits(x) >> exponentBitSize());
  const FloatType y = k * static_cast<FloatType>(r);
  return y;
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
    mask = mask | static_cast<BitType>(o << (significandBitSize() + bit));
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
  const BitType e = static_cast<BitType>(b >> significandBitSize());
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
  const int expt = static_cast<int>(biased_expt) - static_cast<int>(exponentBias());
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
  const BitType b = static_cast<BitType>(data & exponentBitMask());
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
  const BitType b = static_cast<BitType>(data & signBitMask());
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
  const BitType b = static_cast<BitType>(data & significandBitMask());
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
  const BitType b = static_cast<BitType>(makeSignBit(value) | exp_bits | sig_bits);
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
  BitType b = zisc::gcc::isFinite(value) ? BitType{0b0u} : exponentBitMask();
  if (zisc::gcc::isNormal(value)) {
    constexpr std::size_t size = 8 * sizeof(uint64b);
    constexpr FloatType o = static_cast<FloatType>(1);
    constexpr FloatType p = calcExp2(static_cast<int>(size));
    constexpr FloatType n = o / p;
    FloatType v = abs(value);
    std::size_t expt = exponentBias();
    for (; p <= v; v = n * v)
      expt = expt + size;
    for (; v < o; v = p * v)
      expt = expt - size;
    const uint64b rest_value = static_cast<uint64b>(v);
    expt = expt + getExponentValue(rest_value);
    b = static_cast<BitType>(expt << significandBitSize());
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
  FloatType value = static_cast<FloatType>(0);
  if (exp_bits == exponentBitMask()) {
    // Special value case
    value = (sig_bits == 0)
        ? std::numeric_limits<FloatType>::infinity()
        : std::numeric_limits<FloatType>::quiet_NaN();
  }
  else if (exp_bits == 0) {
    // Subnormal value case
    constexpr auto unit = std::numeric_limits<FloatType>::denorm_min();
    const FloatType sig_v = static_cast<FloatType>(sig_bits);
    value = sig_v * unit;
  }
  else {
    // Normal value case
    constexpr auto unit = calcExp2(-static_cast<int>(significandBitSize()));
    const FloatType sig_v = static_cast<FloatType>(implicitBit() | sig_bits);
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
  static_assert(std::is_integral_v<UInt>, "UInt isn't unsigned integer type.");
  const UInt middle = static_cast<UInt>(UInt{0b1} << (8 * sizeof(UInt) - 1));
  // Rounding to nearest
  // tie-break rule is 'nearest even'
  const bool is_rounded_up = ((trailing_bits == middle) && (bits & 0b1)) ||
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
    mask = mask | static_cast<BitType>(o << bit);
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
  FloatType base = isNegative(expt) ? static_cast<FloatType>(0.5) : static_cast<FloatType>(2.0);
  FloatType x = static_cast<FloatType>(1);
  for (int e = abs(expt); 0 < e; e = e >> 1) {
    if ((e & 0b1))
      x = x * base;
    if (1 < e)
      base = base * base;
  }
  return x;
}

/*!
  \details No detailed description

  \tparam UInt No description.
  \param [in] x No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> template <typename UInt> inline
constexpr auto Ieee754Binary<kFormat>::expandBits(const UInt x) noexcept -> BitType
{
  static_assert(std::is_integral_v<UInt>, "UInt isn't unsigned integer.");
  constexpr std::size_t x_size = 8 * sizeof(UInt);
  constexpr std::size_t bit_size = 8 * sizeof(BitType);
  BitType result = 0;
  if constexpr (x_size == bit_size) {
    result = x;
  }
  else if constexpr (x_size < bit_size) {
    constexpr std::size_t diff = bit_size - x_size;
    result = static_cast<BitType>(static_cast<BitType>(x) << diff);
  }
  else {
    constexpr std::size_t diff = x_size - bit_size;
    result = static_cast<BitType>(x >> diff);
  }
  return result;
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
  const BitType b = static_cast<BitType>(implicitBit() | (data & significandBitMask()));
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
  BitType b = zisc::gcc::isNan(value) ? quietNan().bits() : BitType{0b0u};
  if (zisc::gcc::isSubnormal(value)) {
    constexpr auto denorm = std::numeric_limits<FloatType>::denorm_min();
    b = static_cast<BitType>(abs(value) / denorm);
  }
  else if (zisc::gcc::isNormal(value)) {
    constexpr FloatType denorm = calcExp2(static_cast<int>(significandBitSize()));
    const int expt = getExponent(exp_bits);
    const FloatType inv_exp_v = calcExp2(-expt);
    b = static_cast<BitType>(denorm * (inv_exp_v * abs(value)));
  }
  b = static_cast<BitType>(b & significandBitMask());
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
      SType s = static_cast<SType>(u & ~m);
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

} // namespace gcc

} // namespace zisc

#endif // ZISC_BIT_CAST_GCC_INL_HPP
