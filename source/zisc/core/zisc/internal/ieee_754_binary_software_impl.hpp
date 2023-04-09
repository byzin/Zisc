/*!
  \file ieee_754_binary_software_impl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_HPP
#define ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "zisc/ieee_754_binary.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief The software implementation of IEEE 754 binary interchange format

  No detailed description.

  \tparam kFormat Encoding format used in the interchange
  \tparam kRoundStyle Float round style
  */
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
class Ieee754BinarySoftwareImpl
{
 public:
  //! Bit representation type
  using BitT = std::conditional_t<kFormat == Ieee754BinaryFormat::kHalf,   uint16b,
               std::conditional_t<kFormat == Ieee754BinaryFormat::kSingle, uint32b,
                                                                           uint64b>>;
  //! Internal data type
  using DataT = BitT;


  //! Initialize a value with 0
  constexpr Ieee754BinarySoftwareImpl() noexcept = default;

  //! Initialize a value with the given floating point value
  template <std::floating_point Float>
  constexpr Ieee754BinarySoftwareImpl(const Float value) noexcept;

  //! Initialize a value with the given bit data
  constexpr Ieee754BinarySoftwareImpl(const BitT bits) noexcept;


  //! Convert to other format
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle> convertTo() const noexcept;

  //! Convert to a floating point
  template <std::floating_point Float>
  constexpr Float convertTo() const noexcept;

  //! Return the bits in which a floating point is encoded
  constexpr const DataT& data() const noexcept;

  //! Return the machine epsilon
  static constexpr BitT epsilon() noexcept;

  //! Return the infinity
  static constexpr BitT infinity() noexcept;

  //! Check if the underlying value is finite
  static constexpr bool isFinite(const BitT bits) noexcept;

  //! Check if the underlying value is infinity
  static constexpr bool isInf(const BitT bits) noexcept;

  //! Check if the underlying value is NaN 
  static constexpr bool isNan(const BitT bits) noexcept;

  //! Check if the underlying value is normal
  static constexpr bool isNormal(const BitT bits) noexcept;

  //! Check if the underlying value is subnormal
  static constexpr bool isSubnormal(const BitT bits) noexcept;

  //! Check if the underlying value is zero
  static constexpr bool isZero(const BitT bits) noexcept;

  //! Return the largest normal value
  static constexpr BitT max() noexcept;

  //! Return the smallest normal value
  static constexpr BitT min() noexcept;

  //! Return the positive one value
  static constexpr BitT one() noexcept;

  //! Return a quiet NaN
  static constexpr BitT quietNan() noexcept;

  //! Return the largest possible rounding error in ULPs
  static constexpr BitT roundError() noexcept;

  //! Return the float round style
  static constexpr std::float_round_style roundStyle() noexcept;

  //! Return a signaling NaN
  static constexpr BitT signalingNan() noexcept;

  //! Return the positive zero
  static constexpr BitT zero() noexcept;

  // Bit manipulation

  //! Return the bit length used in the interchange encoding
  static constexpr std::size_t bitSize() noexcept;

  //! Return the exponent bias
  static constexpr std::size_t exponentBias() noexcept;

  //! Return the exponent bit length
  static constexpr BitT exponentBitMask() noexcept;

  //! Return the exponent bit length
  static constexpr std::size_t exponentBitSize() noexcept;

  //! Return the format
  static constexpr Ieee754BinaryFormat format() noexcept;

  //! Return the biased exponent from the given bits
  static constexpr BitT getBiasedExponent(const BitT bits) noexcept;

  //! Return the exponent from the given bits
  static constexpr int getExponent(const BitT bits) noexcept;

  //! Return the exponent bits from the given bits
  static constexpr BitT getExponentBits(const BitT bits) noexcept;

  //! Return the sign bit from the given bits
  static constexpr BitT getSignBit(const BitT bits) noexcept;

  //! Return the signed value of the given bits
  static constexpr std::make_signed_t<BitT> getSigned(const BitT bits) noexcept;

  //! Return the significand bits from the given bits
  static constexpr BitT getSignificandBits(const BitT bits) noexcept;

  //! Return the significand and exponent bits from the given bits
  static constexpr BitT getSignificandExponentBits(const BitT bits) noexcept;

  //! Return the hidden bit of normal values
  static constexpr BitT implicitBit() noexcept;

  //! Return the negative of the given value
  static constexpr BitT negateBits(const BitT bits) noexcept;

  //! Round the given value with the trailing bits
  template <UnsignedInteger Integer>
  static constexpr BitT round(const BitT bits,
                              const Integer trailing_bits) noexcept;

  //! Set the data to the given bits
  constexpr void setBits(const BitT bits) noexcept;

  //! Return the sign bit mask
  static constexpr BitT signBitMask() noexcept;

  //! Return the significand bit mask
  static constexpr BitT significandBitMask() noexcept;

  //! Return the significand bit length
  static constexpr std::size_t significandBitSize() noexcept;

  //! Return the significand and exponent bit mask
  static constexpr BitT significandExponentBitMask() noexcept;

  //! Return the significand and exponent bit length
  static constexpr std::size_t significandExponentBitSize() noexcept;

 private:
  static_assert(kBinFormatBitSize<kFormat> == 8 * sizeof(DataT),
                "The size of underlying data doesn't match with the format.");


  //! Convert the given floating point value to a binary
  template <std::floating_point Float>
  static constexpr Ieee754BinarySoftwareImpl<kBinFormatFromFloat<Float>, kRoundStyle> toBinary(const Float value) noexcept;

  //! Convert a special value to the dst format
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle> convertSpecialValue() const noexcept;

  //! Scale down floating point precision
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle> scaledDown() const noexcept;

  //! Scale up floating point precision
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle> scaledUp() const noexcept;

  // Bit manipulation

  //! Return the significand bits with implicit bit from the given bits
  static constexpr BitT getRealSignificandBits(const BitT bits) noexcept;


  DataT data_;
};

// Assignment

//! Apply addition assignment
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& operator+=(
    Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//! Apply subtraction assignment
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& operator-=(
    Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//! Apply multiplication assignment
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& operator*=(
    Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//! Apply division assignment
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& operator/=(
    Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

// Arithmetic

//! Return the value it's own
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator+(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& value) noexcept;

//! Return the negated value
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator-(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& value) noexcept;

//!
template <template <std::floating_point T> typename Func,
          Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> doOperation(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//! Compute an addition
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator+(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//! Compute a subtraction
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator-(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//! Compute a multiplication
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator*(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//! Compute a division
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr Ieee754BinarySoftwareImpl<kFormat, kRoundStyle> operator/(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

// Comparison

//! Check if two values are equal
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr bool operator==(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//! Check if two values aren't equal
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr bool operator!=(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr bool operator<(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr bool operator<=(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr bool operator>(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr bool operator>=(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept;

} // namespace zisc

#include "ieee_754_binary_software_impl-inl.hpp"

#endif // ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_HPP
