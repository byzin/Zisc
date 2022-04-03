/*!
  \file ieee_754_binary_software_impl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_HPP
#define ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
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
  */
template <Ieee754BinaryFormat kFormat>
class Ieee754BinarySoftwareImpl
{
 public:
  //! Bit representation type
  using BitType = std::conditional_t<kFormat == Ieee754BinaryFormat::kHalf,   uint16b,
                  std::conditional_t<kFormat == Ieee754BinaryFormat::kSingle, uint32b,
                                                                              uint64b>>;
  //! Internal data type
  using DataType = BitType;


  //! Initialize a value with 0
  constexpr Ieee754BinarySoftwareImpl() noexcept;

  //! Initialize a value with the given floating point value
  template <std::floating_point Float>
  constexpr Ieee754BinarySoftwareImpl(const Float value) noexcept;

  //! Initialize a value with the given bit data
  constexpr Ieee754BinarySoftwareImpl(const BitType bits) noexcept;


  //! Return the negative of the value
  constexpr Ieee754BinarySoftwareImpl operator-() const noexcept;


  //! Convert to other format
  template <Ieee754BinaryFormat kDstFormat,
            Ieee754RoundingMode kRMode = kDefaultIeee754RoundingMode>
  constexpr Ieee754BinarySoftwareImpl<kDstFormat> convertTo() const noexcept;

  //! Convert to a floating point
  template <std::floating_point Float,
            Ieee754RoundingMode kRMode = kDefaultIeee754RoundingMode>
  constexpr Float convertTo() const noexcept;

  //! Return the bits in which a floating point is encoded
  constexpr DataType data() const noexcept;

  //! Return the machine epsilon
  static constexpr BitType epsilon() noexcept;

  //! Return the infinity
  static constexpr BitType infinity() noexcept;

  //! Check if the underlying value is finite
  static constexpr bool isFinite(const BitType bits) noexcept;

  //! Check if the underlying value is infinity
  static constexpr bool isInf(const BitType bits) noexcept;

  //! Check if the underlying value is NaN 
  static constexpr bool isNan(const BitType bits) noexcept;

  //! Check if the underlying value is normal
  static constexpr bool isNormal(const BitType bits) noexcept;

  //! Check if the underlying value is subnormal
  static constexpr bool isSubnormal(const BitType bits) noexcept;

  //! Check if the underlying value is zero
  static constexpr bool isZero(const BitType bits) noexcept;

  //! Return the largest normal value
  static constexpr BitType max() noexcept;

  //! Return the smallest normal value
  static constexpr BitType min() noexcept;

  //! Return the positive one value
  static constexpr BitType one() noexcept;

  //! Return a quiet NaN
  static constexpr BitType quietNan() noexcept;

  //! Return the largest possible rounding error in ULPs
  static constexpr BitType roundError() noexcept;

  //! Return a signaling NaN
  static constexpr BitType signalingNan() noexcept;

  //! Return the positive zero
  static constexpr BitType zero() noexcept;

  // Bit manipulation

  //! Return the bit length used in the interchange encoding
  static constexpr std::size_t bitSize() noexcept;

  //! Return the exponent bias
  static constexpr std::size_t exponentBias() noexcept;

  //! Return the exponent bit length
  static constexpr BitType exponentBitMask() noexcept;

  //! Return the exponent bit length
  static constexpr std::size_t exponentBitSize() noexcept;

  //! Return the format
  static constexpr Ieee754BinaryFormat format() noexcept;

  //! Return the biased exponent from the given bits
  static constexpr BitType getBiasedExponent(const BitType bits) noexcept;

  //! Return the exponent from the given bits
  static constexpr int getExponent(const BitType bits) noexcept;

  //! Return the exponent bits from the given bits
  static constexpr BitType getExponentBits(const BitType bits) noexcept;

  //! Return the sign bit from the given bits
  static constexpr BitType getSignBit(const BitType bits) noexcept;

  //! Return the significand bits from the given bits
  static constexpr BitType getSignificandBits(const BitType bits) noexcept;

  //! Return the hidden bit of normal values
  static constexpr BitType implicitBit() noexcept;

  //! Return the nevative of the given value
  static constexpr BitType negateBits(const BitType bits) noexcept;

  //! Round the given value with the trailing bits
  template <Ieee754RoundingMode kMode, UnsignedInteger Integer>
  static constexpr BitType round(const BitType bits,
                                 const Integer trailing_bits) noexcept;

  //! Set the data to the given bits
  constexpr void setBits(const BitType bits) noexcept;

  //! Return the sign bit mask
  static constexpr BitType signBitMask() noexcept;

  //! Return the significand bit mask
  static constexpr BitType significandBitMask() noexcept;

  //! Return the significand bit length
  static constexpr std::size_t significandBitSize() noexcept;

 private:
  static_assert(kBinFormatBitSize<kFormat> == 8 * sizeof(DataType),
                "The size of underlying data doesn't match with the format.");


  //! Convert the given floating point value to a binary
  template <std::floating_point Float>
  static constexpr Ieee754BinarySoftwareImpl<kBinFormatFromFloat<Float>> toBinary(const Float value) noexcept;

  //! Convert a special value to the dst format
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754BinarySoftwareImpl<kDstFormat> convertSpecialValue() const noexcept;

  //! Scale down floating point precision
  template <Ieee754BinaryFormat kDstFormat, Ieee754RoundingMode kRMode>
  constexpr Ieee754BinarySoftwareImpl<kDstFormat> scaledDown() const noexcept;

  //! Scale up floating point precision
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754BinarySoftwareImpl<kDstFormat> scaledUp() const noexcept;

  // Bit manipulation

  //! Return the significand bits with implicit bit from the given bits
  static constexpr BitType getRealSignificandBits(const BitType bits) noexcept;


  DataType data_;
};

} // namespace zisc

#include "ieee_754_binary_software_impl-inl.hpp"

#endif // ZISC_IEEE_754_BINARY_SOFTWARE_IMPL_HPP
