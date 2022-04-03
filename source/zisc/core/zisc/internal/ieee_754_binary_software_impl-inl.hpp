/*!
  \file ieee_754_binary_software_impl-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_INL_HPP
#define ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_INL_HPP

#include "ieee_754_binary_software_impl.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/bit.hpp"
#include "zisc/ieee_754_binary.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinarySoftwareImpl<kFormat>::Ieee754BinarySoftwareImpl() noexcept :
    data_{0}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <Ieee754BinaryFormat kFormat> template <std::floating_point Float> inline
constexpr Ieee754BinarySoftwareImpl<kFormat>::Ieee754BinarySoftwareImpl(const Float value) noexcept :
    Ieee754BinarySoftwareImpl(toBinary(value).template convertTo<kFormat>())
{
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinarySoftwareImpl<kFormat>::Ieee754BinarySoftwareImpl(const BitType bits) noexcept :
    data_{bits}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::operator-() const noexcept
    -> Ieee754BinarySoftwareImpl
{
  const Ieee754BinarySoftwareImpl result{negateBits(data())};
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
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::convertTo() const noexcept
    -> Ieee754BinarySoftwareImpl<kDstFormat>
{
  Ieee754BinarySoftwareImpl<kDstFormat> dst{};
  if constexpr (kBinFormatBitSize<kDstFormat> < bitSize())
    dst = scaledDown<kDstFormat, kRMode>();
  else if constexpr (bitSize() < kBinFormatBitSize<kDstFormat>)
    dst = scaledUp<kDstFormat>();
  else // same format, nothing to do here
    dst = *this;
  return dst;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \tparam kRMode No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <std::floating_point Float, Ieee754RoundingMode kRMode> inline
constexpr Float Ieee754BinarySoftwareImpl<kFormat>::convertTo() const noexcept
{
  const auto dst = convertTo<kBinFormatFromFloat<Float>, kRMode>();
  return bit_cast<Float>(dst);
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::data() const noexcept -> DataType
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::epsilon() noexcept -> BitType 
{
  // Compute the exponent of the epsilon
  const BitType eps_exp = getBiasedExponent(one()) - cast<BitType>(significandBitSize());
  const auto bits = cast<BitType>(eps_exp << significandBitSize());
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::infinity() noexcept -> BitType
{
  const BitType bits = exponentBitMask();
  return bits;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat>::isFinite(const BitType bits) noexcept
{
  const bool result = !(isInf(bits) || isNan(bits));
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat>::isInf(const BitType bits) noexcept
{
  const BitType exp_bits = getExponentBits(bits);
  const BitType sig_bits = getSignificandBits(bits);
  const bool result = (exp_bits == exponentBitMask()) && (sig_bits == 0);
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat>::isNan(const BitType bits) noexcept
{
  const BitType exp_bits = getExponentBits(bits);
  const BitType sig_bits = getSignificandBits(bits);
  const bool result = (exp_bits == exponentBitMask()) && (0 < sig_bits);
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat>::isNormal(const BitType bits) noexcept
{
  const BitType exp_bits = getExponentBits(bits);
  const bool result = (0 < exp_bits) && (exp_bits < exponentBitMask());
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat>::isSubnormal(const BitType bits) noexcept
{
  const BitType exp_bits = getExponentBits(bits);
  const BitType sig_bits = getSignificandBits(bits);
  const bool result = (exp_bits == 0) && (sig_bits != 0);
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat>::isZero(const BitType bits) noexcept
{
  const BitType value_bits = cast<BitType>(bits & ~signBitMask());
  const bool result = value_bits == 0;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::max() noexcept -> BitType
{
  const BitType bits = (exponentBitMask() - (min)()) | significandBitMask();
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::min() noexcept -> BitType
{
  const BitType bits = BitType{0b1u} << significandBitSize();
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::one() noexcept -> BitType 
{
  const BitType bits = cast<BitType>(exponentBias() << significandBitSize());
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::quietNan() noexcept -> BitType
{
  const BitType exp_bits = exponentBitMask();
  const BitType sig_bits = BitType{0b1u} << (significandBitSize() - 1);
  const BitType bits = exp_bits | sig_bits;
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::roundError() noexcept -> BitType
{
  const BitType round_exp = getBiasedExponent(one()) - 1;
  const BitType bits = cast<BitType>(round_exp << significandBitSize());
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::signalingNan() noexcept -> BitType
{
  const BitType exp_bits = exponentBitMask();
  const BitType sig_bits = BitType{0b1u} << (significandBitSize() - 2);
  const BitType bits = exp_bits | sig_bits;
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::zero() noexcept -> BitType
{
  const BitType bits = Ieee754BinarySoftwareImpl{}.data();
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat>::bitSize() noexcept
{
  constexpr std::size_t bit_size = kBinFormatBitSize<kFormat>;
  static_assert(bit_size == 8 * sizeof(Ieee754BinarySoftwareImpl),
                "The size of underlying data doesn't match with the format.");
  return bit_size;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat>::exponentBias() noexcept
{
  const std::size_t bias = (0b1u << (exponentBitSize() - 1u)) - 1u;
  return bias;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::exponentBitMask() noexcept -> BitType
{
  BitType mask = 0b0u;
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
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat>::exponentBitSize() noexcept
{
  const std::size_t s = (kFormat == Ieee754BinaryFormat::kHalf) ?   5 :
                        (kFormat == Ieee754BinaryFormat::kSingle) ? 8 :
                                                                    11;
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryFormat Ieee754BinarySoftwareImpl<kFormat>::format() noexcept
{
  return kFormat;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::getBiasedExponent(const BitType bits) noexcept
    -> BitType
{
  const BitType b = getExponentBits(bits);
  const BitType e = cast<BitType>(b >> significandBitSize());
  return e;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr int Ieee754BinarySoftwareImpl<kFormat>::getExponent(const BitType bits) noexcept
{
  const BitType biased_expt = getBiasedExponent(bits);
  const int expt = cast<int>(biased_expt) - cast<int>(exponentBias());
  return expt;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::getExponentBits(const BitType bits) noexcept
    -> BitType
{
  const BitType b = cast<BitType>(bits & exponentBitMask());
  return b;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::getSignBit(const BitType bits) noexcept
    -> BitType
{
  const BitType b = cast<BitType>(bits & signBitMask());
  return b;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::getSignificandBits(const BitType bits) noexcept
    -> BitType
{
  const BitType b = cast<BitType>(bits & significandBitMask());
  return b;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::implicitBit() noexcept -> BitType
{
  const BitType b = BitType{0b1u} << significandBitSize();
  return b;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::negateBits(const BitType bits) noexcept
    -> BitType
{
  const BitType b = cast<BitType>(bits ^ signBitMask());
  return b;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kMode No description.
  \param [in] bits No description.
  \param [in] trailing_bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <Ieee754RoundingMode kMode, UnsignedInteger Integer> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::round(
    const BitType bits,
    const Integer trailing_bits) noexcept -> BitType
{
  const auto middle = cast<Integer>(Integer{0b1} << (8 * sizeof(Integer) - 1));
  // Rounding to nearest
  // tie-break rule is 'nearest even'
  const bool is_rounded_up = ((trailing_bits == middle) && isOdd(bits)) ||
                             (middle < trailing_bits);
  const BitType result = is_rounded_up ? bits + 1 : bits;
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr void Ieee754BinarySoftwareImpl<kFormat>::setBits(const BitType bits) noexcept
{
  data_ = bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::signBitMask() noexcept -> BitType
{
  const BitType mask = BitType{0b1u} << (exponentBitSize() + significandBitSize());
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::significandBitMask() noexcept -> BitType
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
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat>::significandBitSize() noexcept
{
  const std::size_t s = (kFormat == Ieee754BinaryFormat::kHalf) ?   10 :
                        (kFormat == Ieee754BinaryFormat::kSingle) ? 23 :
                                                                    52;
  return s;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> template <std::floating_point Float> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::toBinary(const Float value) noexcept
    -> Ieee754BinarySoftwareImpl<kBinFormatFromFloat<Float>>
{
  using BinaryT = Ieee754BinarySoftwareImpl<kBinFormatFromFloat<Float>>;
  const auto u = bit_cast<typename BinaryT::BitType>(value);
  const BinaryT dst{u};
  return dst;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> template <Ieee754BinaryFormat kDstFormat> inline
constexpr Ieee754BinarySoftwareImpl<kDstFormat> Ieee754BinarySoftwareImpl<kFormat>::convertSpecialValue() const noexcept
{
  using DstBinaryT = Ieee754BinarySoftwareImpl<kDstFormat>;
  const DstBinaryT dst = isNan(data()) ? DstBinaryT::quietNan() :
                         isInf(data()) ?  DstBinaryT::infinity()
                                       :  DstBinaryT::zero();
  return dst;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <Ieee754BinaryFormat kDstFormat, Ieee754RoundingMode kRMode> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::scaledDown() const noexcept
    -> Ieee754BinarySoftwareImpl<kDstFormat>
{
  using DstBinaryT = Ieee754BinarySoftwareImpl<kDstFormat>;
  using DstBitT = typename DstBinaryT::BitType;
  DstBinaryT dst = convertSpecialValue<kDstFormat>();
  if (isNormal(data()) || isSubnormal(data())) {
    constexpr std::size_t exp_bias = exponentBias();
    constexpr std::size_t sig_size = significandBitSize();
    constexpr std::size_t dst_exp_bias = DstBinaryT::exponentBias();
    constexpr std::size_t dst_sig_size = DstBinaryT::significandBitSize();

    constexpr BitType lower_bound = ((exp_bias + 1) - dst_exp_bias) << sig_size;
    constexpr BitType upper_bound = ((exp_bias + 1) + dst_exp_bias) << sig_size;

    const BitType exp_bits = getExponentBits(data());
    if (exp_bits < upper_bound) {
      constexpr std::size_t sig_size_diff = sig_size - dst_sig_size;
      constexpr BitType exp_bias_diff = (exp_bias - dst_exp_bias) << dst_sig_size;
      // Finite values
      const std::size_t shift_size = (exp_bits < lower_bound)
          ? getBiasedExponent(lower_bound - exp_bits) // Subnormal case
          : 0;                                        // Normal case
      if (shift_size <= dst_sig_size) {
        // Exponent bits
        const DstBitT dst_exp_bits = cast<DstBitT>((0 < shift_size)
            ? 0                                             // Subnormal case
            : (exp_bits >> sig_size_diff) - exp_bias_diff); // Normal case
        // Significand bits
        const BitType sig_bits = getRealSignificandBits(data());
        const auto dst_sig_bits = cast<DstBitT>(
            getSignificandBits(sig_bits >> shift_size) >> sig_size_diff);
        auto dst_bits = cast<DstBitT>(dst_exp_bits | dst_sig_bits);
        // Rounding
        const std::size_t shift_diff = dst_sig_size - shift_size;
        BitType trailing_bits = getSignificandBits(sig_bits << shift_diff);
        trailing_bits = cast<BitType>(trailing_bits << (1 + exponentBitSize()));
        dst_bits = DstBinaryT::template round<kRMode>(dst_bits, trailing_bits);
        //
        dst.setBits(dst_bits);
      }
      else { // Underflow
        DstBitT dst_bits = 0;
        // Rounding
        const std::size_t shift_diff = shift_size - dst_sig_size;
        if (shift_diff <= significandBitSize()) {
          const BitType sig_bits = getRealSignificandBits(data());
          BitType trailing_bits = getSignificandBits(sig_bits >> shift_diff);
          trailing_bits = cast<BitType>(trailing_bits << (1 + exponentBitSize()));
          dst_bits = DstBinaryT::template round<kRMode>(dst_bits, trailing_bits);
        }
        //
        dst.setBits(dst_bits);
      }
    }
    else { // Overflow
      dst = DstBinaryT::infinity();
    }
  }

  // Sign bit
  dst = (getSignBit(data()) == 0) ? dst : -dst;

  return dst;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <Ieee754BinaryFormat kDstFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::scaledUp() const noexcept
    -> Ieee754BinarySoftwareImpl<kDstFormat>
{
  using DstBinaryT = Ieee754BinarySoftwareImpl<kDstFormat>;
  using DstBitT = typename DstBinaryT::DataType;
  DstBinaryT dst = convertSpecialValue<kDstFormat>();
  if (isNormal(data()) || isSubnormal(data())) {
    constexpr std::size_t exp_bias = exponentBias();
    constexpr std::size_t sig_size = significandBitSize();
    constexpr std::size_t dst_exp_bias = DstBinaryT::exponentBias();
    constexpr std::size_t dst_sig_size = DstBinaryT::significandBitSize();

    // Exponent bits
    const std::size_t expt = getBiasedExponent(data());
    auto dst_expt = cast<DstBitT>(expt + (dst_exp_bias - exp_bias));
    // Significand bits
    DstBitT dst_sig_bits = cast<DstBitT>(getSignificandBits(data())) <<
                           (dst_sig_size - sig_size);
    const BitType exp_bits = getExponentBits(data());
    if (exp_bits == 0) { // Subnormal case
      constexpr auto imp_expt = bit_width(DstBinaryT::implicitBit());
      const auto bit_expt = bit_width(dst_sig_bits);
      const DstBitT expt_diff = imp_expt - bit_expt;
      dst_expt = (1 + dst_expt) - expt_diff;
      dst_sig_bits = cast<DstBitT>(dst_sig_bits << expt_diff);
      dst_sig_bits = DstBinaryT::getSignificandBits(dst_sig_bits);
    }
    const DstBitT dst_exp_bits = cast<DstBitT>(dst_expt << dst_sig_size);
    const DstBitT dst_bits = cast<DstBitT>(dst_exp_bits | dst_sig_bits);
    dst.setBits(dst_bits);
  }

  // Sign bit
  dst = (getSignBit(data()) == 0) ? dst : -dst;

  return dst;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat>::getRealSignificandBits(const BitType bits) noexcept
    -> BitType
{
  const auto b = cast<BitType>(implicitBit() | (bits & significandBitMask()));
  return b;
}

} // namespace zisc

#endif // ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_INL_HPP
