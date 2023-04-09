/*!
  \file ieee_754_binary_software_impl-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_INL_HPP
#define ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_INL_HPP

#include "ieee_754_binary_software_impl.hpp"
// Standard C++ library
#include <bit>
#include <concepts>
#include <cstddef>
#include <functional>
#include <limits>
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

  \param [in] value No description.
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
template <std::floating_point Float> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::Ieee754BinarySoftwareImpl(const Float value) noexcept :
    Ieee754BinarySoftwareImpl(toBinary(value).template convertTo<kFormat>())
{
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::Ieee754BinarySoftwareImpl(const BitT bits) noexcept :
    data_{bits}
{
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
template <Ieee754BinaryFormat kDstFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::convertTo() const noexcept
    -> Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>
{
  Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle> dst{};
  if constexpr (kBinFormatBitSize<kDstFormat> < bitSize())
    dst = scaledDown<kDstFormat>();
  else if constexpr (bitSize() < kBinFormatBitSize<kDstFormat>)
    dst = scaledUp<kDstFormat>();
  else // same format, nothing to do here
    dst = *this;
  return dst;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
template <std::floating_point Float> inline
constexpr Float Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::convertTo() const noexcept
{
  const auto dst = convertTo<kBinFormatFromFloat<Float>>();
  return zisc::bit_cast<Float>(dst);
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::data() const noexcept -> const DataT&
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::epsilon() noexcept -> BitT 
{
  // Compute the exponent of the epsilon
  const BitT eps_exp = getBiasedExponent(one()) - static_cast<BitT>(significandBitSize());
  const auto bits = static_cast<BitT>(eps_exp << significandBitSize());
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::infinity() noexcept -> BitT
{
  const BitT bits = exponentBitMask();
  return bits;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::isFinite(const BitT bits) noexcept
{
  const BitT exp_bits = getExponentBits(bits);
  const bool result = exp_bits != exponentBitMask();
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::isInf(const BitT bits) noexcept
{
  const BitT sig_exp_bits = getSignificandExponentBits(bits);
  const bool result = sig_exp_bits == exponentBitMask();
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::isNan(const BitT bits) noexcept
{
  const BitT sig_exp_bits = getSignificandExponentBits(bits);
  const bool result = sig_exp_bits > exponentBitMask();
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::isNormal(const BitT bits) noexcept
{
  const BitT exp_bits = getExponentBits(bits);
  const bool result = (0 < exp_bits) && (exp_bits < exponentBitMask());
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::isSubnormal(const BitT bits) noexcept
{
  const BitT sig_exp_bits = getSignificandExponentBits(bits);
  const bool result = (0 < sig_exp_bits) && (sig_exp_bits <= significandBitMask());
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::isZero(const BitT bits) noexcept
{
  const BitT sig_exp_bits = getSignificandExponentBits(bits);
  const bool result = sig_exp_bits == 0;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::max() noexcept -> BitT
{
  const BitT bits = (exponentBitMask() - (min)()) | significandBitMask();
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::min() noexcept -> BitT
{
  const BitT bits = BitT{0b1u} << significandBitSize();
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::one() noexcept -> BitT 
{
  const BitT bits = static_cast<BitT>(exponentBias() << significandBitSize());
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::quietNan() noexcept -> BitT
{
  const BitT exp_bits = exponentBitMask();
  const BitT sig_bits = BitT{0b1u} << (significandBitSize() - 1);
  const BitT bits = exp_bits | sig_bits;
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::roundError() noexcept -> BitT
{
  const BitT round_exp = getBiasedExponent(one()) - 1;
  const BitT bits = static_cast<BitT>(round_exp << significandBitSize());
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr std::float_round_style Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::roundStyle() noexcept
{
  return kRoundStyle;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::signalingNan() noexcept -> BitT
{
  const BitT exp_bits = exponentBitMask();
  const BitT sig_bits = BitT{0b1u} << (significandBitSize() - 2);
  const BitT bits = exp_bits | sig_bits;
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::zero() noexcept -> BitT
{
  const BitT bits = Ieee754BinarySoftwareImpl{}.data();
  return bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::bitSize() noexcept
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
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::exponentBias() noexcept
{
  const std::size_t bias = (0b1u << (exponentBitSize() - 1u)) - 1u;
  return bias;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::exponentBitMask() noexcept -> BitT
{
  BitT mask = 0b0u;
  for (std::size_t bit = 0; bit < exponentBitSize(); ++bit) {
    constexpr BitT o = 0b1u;
    mask = mask | static_cast<BitT>(o << (significandBitSize() + bit));
  }
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::exponentBitSize() noexcept
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
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinaryFormat Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::format() noexcept
{
  return kFormat;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::getBiasedExponent(const BitT bits) noexcept
    -> BitT
{
  const BitT b = getExponentBits(bits);
  const BitT e = static_cast<BitT>(b >> significandBitSize());
  return e;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr int Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::getExponent(const BitT bits) noexcept
{
  const BitT biased_expt = getBiasedExponent(bits);
  const int expt = static_cast<int>(biased_expt) - static_cast<int>(exponentBias());
  return expt;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::getExponentBits(const BitT bits) noexcept
    -> BitT
{
  const BitT b = static_cast<BitT>(bits & exponentBitMask());
  return b;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::getSignBit(const BitT bits) noexcept
    -> BitT
{
  const BitT b = static_cast<BitT>(bits & signBitMask());
  return b;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::getSigned(const BitT bits) noexcept
    -> std::make_signed_t<BitT>
{
  using SignedT = std::make_signed_t<BitT>;
  constexpr BitT m = signBitMask();
  auto s = static_cast<SignedT>(bits & ~m);
  s = ((bits & m) == 0) ? s : -s;
  return s;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::getSignificandBits(const BitT bits) noexcept
    -> BitT
{
  const BitT b = static_cast<BitT>(bits & significandBitMask());
  return b;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::getSignificandExponentBits(const BitT bits) noexcept
    -> BitT
{
  const BitT b = static_cast<BitT>(bits & significandExponentBitMask());
  return b;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::implicitBit() noexcept -> BitT
{
  const BitT b = BitT{0b1u} << significandBitSize();
  return b;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::negateBits(const BitT bits) noexcept
    -> BitT
{
  const BitT b = static_cast<BitT>(bits ^ signBitMask());
  return b;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] bits No description.
  \param [in] trailing_bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
template <UnsignedInteger Integer> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::round(
    const BitT bits,
    const Integer trailing_bits) noexcept -> BitT
{
  static_assert(roundStyle() == std::float_round_style::round_to_nearest);
  const auto middle = static_cast<Integer>(Integer{0b1} << (8 * sizeof(Integer) - 1));
  // Rounding to nearest
  // tie-break rule is 'nearest even'
  const bool is_rounded_up = ((trailing_bits == middle) && isOdd(bits)) ||
                             (middle < trailing_bits);
  const BitT result = is_rounded_up ? bits + 1 : bits;
  return result;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr void Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::setBits(const BitT bits) noexcept
{
  data_ = bits;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::signBitMask() noexcept -> BitT
{
  const BitT mask = BitT{0b1u} << (exponentBitSize() + significandBitSize());
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::significandBitMask() noexcept -> BitT
{
  BitT mask = 0b0;
  for (std::size_t bit = 0; bit < significandBitSize(); ++bit) {
    constexpr BitT o = 0b1u;
    mask = mask | static_cast<BitT>(o << bit);
  }
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::significandBitSize() noexcept
{
  const std::size_t s = (kFormat == Ieee754BinaryFormat::kHalf) ?   10 :
                        (kFormat == Ieee754BinaryFormat::kSingle) ? 23 :
                                                                    52;
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::significandExponentBitMask() noexcept -> BitT
{
  return static_cast<BitT>(~signBitMask());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr std::size_t Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::significandExponentBitSize() noexcept
{
  return significandBitSize() + exponentBitSize();
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
template <std::floating_point Float> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::toBinary(const Float value) noexcept
    -> Ieee754BinarySoftwareImpl<kBinFormatFromFloat<Float>, kRoundStyle>
{
  using BinaryT = Ieee754BinarySoftwareImpl<kBinFormatFromFloat<Float>, kRoundStyle>;
  const auto u = zisc::bit_cast<typename BinaryT::BitT>(value);
  const BinaryT dst{u};
  return dst;
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
template <Ieee754BinaryFormat kDstFormat> inline
constexpr Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::convertSpecialValue() const noexcept
{
  using DstBinaryT = Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>;
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
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
template <Ieee754BinaryFormat kDstFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::scaledDown() const noexcept
    -> Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>
{
  using DstBinaryT = Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>;
  using DstBitT = typename DstBinaryT::BitT;
  DstBinaryT dst = convertSpecialValue<kDstFormat>();
  if (isNormal(data()) || isSubnormal(data())) {
    constexpr std::size_t exp_bias = exponentBias();
    constexpr std::size_t sig_size = significandBitSize();
    constexpr std::size_t dst_exp_bias = DstBinaryT::exponentBias();
    constexpr std::size_t dst_sig_size = DstBinaryT::significandBitSize();

    constexpr BitT lower_bound = ((exp_bias + 1) - dst_exp_bias) << sig_size;
    constexpr BitT upper_bound = ((exp_bias + 1) + dst_exp_bias) << sig_size;

    const BitT exp_bits = getExponentBits(data());
    if (exp_bits < upper_bound) {
      constexpr std::size_t sig_size_diff = sig_size - dst_sig_size;
      constexpr BitT exp_bias_diff = (exp_bias - dst_exp_bias) << dst_sig_size;
      // Finite values
      const std::size_t shift_size = (exp_bits < lower_bound)
          ? getBiasedExponent(lower_bound - exp_bits) // Subnormal case
          : 0;                                        // Normal case
      if (shift_size <= dst_sig_size) {
        // Exponent bits
        const DstBitT dst_exp_bits = static_cast<DstBitT>((0 < shift_size)
            ? 0                                             // Subnormal case
            : (exp_bits >> sig_size_diff) - exp_bias_diff); // Normal case
        // Significand bits
        const BitT sig_bits = getRealSignificandBits(data());
        const auto dst_sig_bits = static_cast<DstBitT>(
            getSignificandBits(sig_bits >> shift_size) >> sig_size_diff);
        auto dst_bits = static_cast<DstBitT>(dst_exp_bits | dst_sig_bits);
        // Rounding
        const std::size_t shift_diff = dst_sig_size - shift_size;
        BitT trailing_bits = getSignificandBits(sig_bits << shift_diff);
        trailing_bits = static_cast<BitT>(trailing_bits << (1 + exponentBitSize()));
        dst_bits = DstBinaryT::round(dst_bits, trailing_bits);
        //
        dst.setBits(dst_bits);
      }
      else { // Underflow
        DstBitT dst_bits = 0;
        // Rounding
        const std::size_t shift_diff = shift_size - dst_sig_size;
        if (shift_diff <= significandBitSize()) {
          const BitT sig_bits = getRealSignificandBits(data());
          BitT trailing_bits = getSignificandBits(sig_bits >> shift_diff);
          trailing_bits = static_cast<BitT>(trailing_bits << (1 + exponentBitSize()));
          dst_bits = DstBinaryT::round(dst_bits, trailing_bits);
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
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
template <Ieee754BinaryFormat kDstFormat> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::scaledUp() const noexcept
    -> Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>
{
  using DstBinaryT = Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>;
  using DstBitT = typename DstBinaryT::DataT;
  DstBinaryT dst = convertSpecialValue<kDstFormat>();
  if (isNormal(data()) || isSubnormal(data())) {
    constexpr std::size_t exp_bias = exponentBias();
    constexpr std::size_t sig_size = significandBitSize();
    constexpr std::size_t dst_exp_bias = DstBinaryT::exponentBias();
    constexpr std::size_t dst_sig_size = DstBinaryT::significandBitSize();

    // Exponent bits
    const std::size_t expt = getBiasedExponent(data());
    auto dst_expt = static_cast<DstBitT>(expt + (dst_exp_bias - exp_bias));
    // Significand bits
    DstBitT dst_sig_bits = static_cast<DstBitT>(getSignificandBits(data())) <<
                           (dst_sig_size - sig_size);
    const BitT exp_bits = getExponentBits(data());
    if (exp_bits == 0) { // Subnormal case
      using DstBitT = typename DstBinaryT::BitT;
      constexpr DstBitT imp_expt = std::bit_width(DstBinaryT::implicitBit());
      const DstBitT bit_expt = std::bit_width(dst_sig_bits);
      const DstBitT expt_diff = static_cast<DstBitT>(imp_expt) - bit_expt;
      dst_expt = (1 + dst_expt) - expt_diff;
      dst_sig_bits = static_cast<DstBitT>(dst_sig_bits << expt_diff);
      dst_sig_bits = DstBinaryT::getSignificandBits(dst_sig_bits);
    }
    const DstBitT dst_exp_bits = static_cast<DstBitT>(dst_expt << dst_sig_size);
    const DstBitT dst_bits = static_cast<DstBitT>(dst_exp_bits | dst_sig_bits);
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
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr auto Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::getRealSignificandBits(const BitT bits) noexcept
    -> BitT
{
  const auto b = static_cast<BitT>(implicitBit() | (bits & significandBitMask()));
  return b;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& operator+=(
    Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  lhs = lhs + rhs;
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& operator-=(
    Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  lhs = lhs - rhs;
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& operator*=(
    Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  lhs = lhs * rhs;
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& operator/=(
    Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  lhs = lhs / rhs;
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator+(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& value) noexcept
{
  return value;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator-(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& value) noexcept
{
  return {Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>::negateBits(value.data())};
}

/*!
  \details No detailed description

  \tparam Func No description.
  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <template <std::floating_point T> typename Func,
          Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> doOperation(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  using BinT = Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;
  using FormatT = Ieee754BinaryFormat;
  using FloatT = std::conditional_t<kFormat == FormatT::kHalf,   float,
                 std::conditional_t<kFormat == FormatT::kSingle, float,
                 std::conditional_t<kFormat == FormatT::kDouble, double,
                                                                             void>>>;
  const auto to_float = [](const BinT& b) noexcept
  {
    if constexpr (BinT::format() == FormatT::kHalf)
      return bit_cast<FloatT>(b.template convertTo<FormatT::kSingle>());
    else
      return bit_cast<FloatT>(b);
  };

  const auto to_bin = [](const FloatT& f) noexcept
  {
    if constexpr (BinT::format() == FormatT::kHalf) {
      using DstBinT = Ieee754BinarySoftwareImpl<FormatT::kSingle, kRoundStyle>;
      return bit_cast<DstBinT>(f).template convertTo<FormatT::kHalf>();
    }
    else {
      return bit_cast<BinT>(f);
    }
  };

  const auto x = to_float(lhs);
  const auto y = to_float(rhs);
  const auto z = Func<FloatT>{}(x, y);
  return to_bin(z);
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator+(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  return doOperation<std::plus>(lhs, rhs);
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator-(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  return doOperation<std::minus>(lhs, rhs);
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator*(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  return doOperation<std::multiplies>(lhs, rhs);
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator/(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  return doOperation<std::divides>(lhs, rhs);
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool operator==(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                          const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  using BinT = Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;
  using BitT = typename BinT::BitT;
  const BitT ld = lhs.data();
  const BitT rd = rhs.data();
  const auto sld = BinT::getSigned(ld);
  const auto srd = BinT::getSigned(rd);
  const bool result = !(BinT::isNan(ld) || BinT::isNan(rd)) && (sld == srd);
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool operator!=(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                          const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool operator<(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                         const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  using BinT = Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;
  using BitT = typename BinT::BitT;
  const BitT ld = lhs.data();
  const BitT rd = rhs.data();
  const auto sld = BinT::getSigned(ld);
  const auto srd = BinT::getSigned(rd);
  const bool result = !(BinT::isNan(ld) || BinT::isNan(rd)) && (sld < srd);
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool operator<=(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                          const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  using BinT = Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;
  using BitT = typename BinT::BitT;
  const BitT ld = lhs.data();
  const BitT rd = rhs.data();
  const auto sld = BinT::getSigned(ld);
  const auto srd = BinT::getSigned(rd);
  const bool result = !(BinT::isNan(ld) || BinT::isNan(rd)) && (sld <= srd);
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool operator>(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                         const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  return rhs < lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \tparam kRoundStyle No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle> inline
constexpr bool operator>=(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                          const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
{
  return rhs <= lhs;
}

} // namespace zisc

#endif // ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_INL_HPP
