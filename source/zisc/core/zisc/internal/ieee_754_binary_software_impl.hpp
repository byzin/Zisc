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
  explicit constexpr Ieee754BinarySoftwareImpl(const Float value) noexcept;

  //! Initialize a value with the given bit data
  explicit constexpr Ieee754BinarySoftwareImpl(const BitT bits) noexcept;


  //! Convert to other format
  template <Ieee754BinaryFormat kDstFormat>
  constexpr auto convertTo() const noexcept -> Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>;

  //! Convert to a floating point
  template <std::floating_point Float>
  constexpr auto convertTo() const noexcept -> Float;

  //! Return the bits in which a floating point is encoded
  [[nodiscard]]
  constexpr auto data() const noexcept -> const DataT&;

  //! Return the machine epsilon
  static constexpr auto epsilon() noexcept -> BitT;

  //! Return the infinity
  static constexpr auto infinity() noexcept -> BitT;

  //! Check if the underlying value is finite
  static constexpr auto isFinite(const BitT bits) noexcept -> bool;

  //! Check if the underlying value is infinity
  static constexpr auto isInf(const BitT bits) noexcept -> bool;

  //! Check if the underlying value is NaN 
  static constexpr auto isNan(const BitT bits) noexcept -> bool;

  //! Check if the underlying value is normal
  static constexpr auto isNormal(const BitT bits) noexcept -> bool;

  //! Check if the underlying value is subnormal
  static constexpr auto isSubnormal(const BitT bits) noexcept -> bool;

  //! Check if the underlying value is zero
  static constexpr auto isZero(const BitT bits) noexcept -> bool;

  //! Return the largest normal value
  static constexpr auto max() noexcept -> BitT;

  //! Return the smallest normal value
  static constexpr auto min() noexcept -> BitT;

  //! Return the positive one value
  static constexpr auto one() noexcept -> BitT;

  //! Return a quiet NaN
  static constexpr auto quietNan() noexcept -> BitT;

  //! Return the largest possible rounding error in ULPs
  static constexpr auto roundError() noexcept -> BitT;

  //! Return the float round style
  static constexpr auto roundStyle() noexcept -> std::float_round_style;

  //! Return a signaling NaN
  static constexpr auto signalingNan() noexcept -> BitT;

  //! Return the positive zero
  static constexpr auto zero() noexcept -> BitT;

  // Bit manipulation

  //! Return the bit length used in the interchange encoding
  static constexpr auto bitSize() noexcept -> std::size_t;

  //! Return the exponent bias
  static constexpr auto exponentBias() noexcept -> std::size_t;

  //! Return the exponent bit length
  static constexpr auto exponentBitMask() noexcept -> BitT;

  //! Return the exponent bit length
  static constexpr auto exponentBitSize() noexcept -> std::size_t;

  //! Return the format
  static constexpr auto format() noexcept -> Ieee754BinaryFormat;

  //! Return the biased exponent from the given bits
  static constexpr auto getBiasedExponent(const BitT bits) noexcept -> BitT;

  //! Return the exponent from the given bits
  static constexpr auto getExponent(const BitT bits) noexcept -> int;

  //! Return the exponent bits from the given bits
  static constexpr auto getExponentBits(const BitT bits) noexcept -> BitT;

  //! Return the sign bit from the given bits
  static constexpr auto getSignBit(const BitT bits) noexcept -> BitT;

  //! Return the signed value of the given bits
  static constexpr auto getSigned(const BitT bits) noexcept -> std::make_signed_t<BitT>;

  //! Return the significand bits from the given bits
  static constexpr auto getSignificandBits(const BitT bits) noexcept -> BitT;

  //! Return the significand and exponent bits from the given bits
  static constexpr auto getSignificandExponentBits(const BitT bits) noexcept -> BitT;

  //! Return the hidden bit of normal values
  static constexpr auto implicitBit() noexcept -> BitT;

  //! Return the negative of the given value
  static constexpr auto negateBits(const BitT bits) noexcept -> BitT;

  //! Round the given value with the trailing bits
  template <UnsignedInteger Integer>
  static constexpr auto round(const BitT bits,
                              const Integer trailing_bits) noexcept -> BitT;

  //! Set the data to the given bits
  constexpr void setBits(const BitT bits) noexcept;

  //! Return the sign bit mask
  static constexpr auto signBitMask() noexcept -> BitT;

  //! Return the significand bit mask
  static constexpr auto significandBitMask() noexcept -> BitT;

  //! Return the significand bit length
  static constexpr auto significandBitSize() noexcept -> std::size_t;

  //! Return the significand and exponent bit mask
  static constexpr auto significandExponentBitMask() noexcept -> BitT;

  //! Return the significand and exponent bit length
  static constexpr auto significandExponentBitSize() noexcept -> std::size_t;

 private:
  static_assert(kBinFormatBitSize<kFormat> == 8 * sizeof(DataT),
                "The size of underlying data doesn't match with the format.");


  //! Convert the given floating point value to a binary
  template <std::floating_point Float>
  static constexpr auto toBinary(const Float value) noexcept
      -> Ieee754BinarySoftwareImpl<kBinFormatFromFloat<Float>, kRoundStyle>;

  //! Convert a special value to the dst format
  template <Ieee754BinaryFormat kDstFormat>
  constexpr auto convertSpecialValue() const noexcept
      -> Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>;

  //! Scale down floating point precision
  template <Ieee754BinaryFormat kDstFormat>
  constexpr auto scaledDown() const noexcept
      -> Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>;

  //! Scale up floating point precision
  template <Ieee754BinaryFormat kDstFormat>
  constexpr auto scaledUp() const noexcept
      -> Ieee754BinarySoftwareImpl<kDstFormat, kRoundStyle>;

  // Bit manipulation

  //! Return the significand bits with implicit bit from the given bits
  static constexpr auto getRealSignificandBits(const BitT bits) noexcept -> BitT;


  DataT data_;
};

// Assignment

//! Apply addition assignment
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator+=(Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                          const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>&;

//! Apply subtraction assignment
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator-=(Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                          const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>&;

//! Apply multiplication assignment
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator*=(Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                          const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>&;

//! Apply division assignment
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator/=(Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                          const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>&;

// Arithmetic

//! Return the value it's own
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator+(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& value) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;

//! Return the negated value
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator-(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& value) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;

//!
template <template <std::floating_point T> typename Func,
          Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto doOperation(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                           const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;

//! Compute an addition
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator+(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                         const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;

//! Compute a subtraction
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator-(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                         const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;

//! Compute a multiplication
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator*(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                         const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;

//! Compute a division
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator/(const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
                         const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept
    -> Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>;

// Comparison

//! Check if two values are equal
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator==(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept -> bool;

//! Check if two values aren't equal
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator!=(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept -> bool;

//!
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator<(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept -> bool;

//!
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator<=(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept -> bool;

//!
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator>(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept -> bool;

//!
template <Ieee754BinaryFormat kFormat, std::float_round_style kRoundStyle>
constexpr auto operator>=(
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& lhs,
    const Ieee754BinarySoftwareImpl<kFormat, kRoundStyle>& rhs) noexcept -> bool;

} // namespace zisc

#include "ieee_754_binary_software_impl-inl.hpp"

#endif // ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_HPP
