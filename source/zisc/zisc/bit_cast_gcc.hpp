/*!
  \file bit_cast_gcc.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BIT_CAST_GCC_HPP
#define ZISC_BIT_CAST_GCC_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc_config.hpp"

namespace zisc {

namespace gcc {

/*!
  \brief No brief description

  No detailed description.
  */
enum class Ieee754BinaryFormat : std::size_t
{
  kHalf = 16,
  kSingle = 32,
  kDouble = 64
};

/*!
  \brief No brief description

  No detailed description.
  */
enum class Ieee754RoundingMode : int
{
//  kDownward, //!< Rounding towards negative infinity
  kToNearest, //!< Rounding towards nearest representable value
//  kTowardZero, //!< Rounding towards zero
//  kUpward //!< Rounding towards positive infinity
};


constexpr Ieee754RoundingMode kDefaultIeee754RoundingMode = Ieee754RoundingMode::kToNearest;


/*!
  \brief The software implementation of IEEE 754 binary interchange format

  No detailed description.

  \tparam kFormat Encoding format used in the interchange
  */
template <Ieee754BinaryFormat kFormat>
class Ieee754Binary
{
 public:
  static constexpr std::size_t kBitSize = static_cast<std::size_t>(kFormat);


  // Type aliases
  //! Internal data type
  using BitType = std::conditional_t<kBitSize == 16, uint16b,
                  std::conditional_t<kBitSize == 32, uint32b, uint64b>>;
  //! Bit representation type
  using FloatType = std::conditional_t<kBitSize == 32, float,
                    std::conditional_t<kBitSize == 64, double, BitType>>;


  //! Initialize a value with 0
  constexpr Ieee754Binary() noexcept;

  //! Initialize a value with the given bits
  constexpr Ieee754Binary(const BitType data) noexcept;


  //! Return the value it's own
  constexpr Ieee754Binary operator+() const noexcept;

  //! Return the negated value
  constexpr Ieee754Binary operator-() const noexcept;


  //! Return the machine epsilon
  static constexpr Ieee754Binary epsilon() noexcept;

  //! Return the infinity
  static constexpr Ieee754Binary infinity() noexcept;

  //! Check if the underlying value is finite
  constexpr bool isFinite() const noexcept;

  //! Check if the underlying value is infinity
  constexpr bool isInf() const noexcept;

  //! Check if the underlying value is NaN 
  constexpr bool isNan() const noexcept;

  //! Check if the underlying value is normal
  constexpr bool isNormal() const noexcept;

  //! Check if the underlying value is subnormal
  constexpr bool isSubnormal() const noexcept;

  //! Check if the underlying value is zero
  constexpr bool isZero() const noexcept;

  //! Return the largest normal value
  static constexpr Ieee754Binary max() noexcept;

  //! Return the smallest normal value
  static constexpr Ieee754Binary min() noexcept;

  //! Return the positive one value
  static constexpr Ieee754Binary one() noexcept;

  //! Return a quiet NaN
  static constexpr Ieee754Binary quietNan() noexcept;

  //! Return the largest possible rounding error in ULPs
  static constexpr Ieee754Binary roundError() noexcept;

  //! Return a signaling NaN
  static constexpr Ieee754Binary signalingNan() noexcept;

  //! Return the positive zero
  static constexpr Ieee754Binary zero() noexcept;

  // Utility

  //! Map an integer value into a [0, 1) floating point value
  template <typename UInt>
  static constexpr FloatType mapTo01(const UInt x) noexcept;

  // Bit manipulation

  //! Return the bits in which a floating point is encoded
  constexpr BitType bits() const noexcept;

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
  static constexpr BitType getBiasedExponent(const BitType data) noexcept;

  //! Return the exponent from the given bits
  static constexpr int getExponent(const BitType data) noexcept;

  //! Return the exponent bits from the given bits
  static constexpr BitType getExponentBits(const BitType data) noexcept;

  //! Return the sign bit from the given bits
  static constexpr BitType getSignBit(const BitType data) noexcept;

  //! Return the significand bits from the given bits
  static constexpr BitType getSignificandBits(const BitType data) noexcept;

  //! Return the hidden bit of normal values
  static constexpr BitType implicitBit() noexcept;

  //! Make bits from the given floating point
  static constexpr BitType makeBits(const FloatType value) noexcept;

  //! Make exponent bits from the given floating point
  static constexpr BitType makeExponentBits(const FloatType value) noexcept;

  //! Make a floating point from the given bits
  static constexpr FloatType makeFloat(const BitType data) noexcept;

  //! Make a sign bit from the given floating point
  static constexpr BitType makeSignBit(const FloatType value) noexcept;

  //! Make significand bits from the given floating point
  static constexpr BitType makeSignificandBits(const FloatType value) noexcept;

  //! Round the given value with the trailing bits
  template <Ieee754RoundingMode kMode, typename UInt>
  static constexpr BitType round(const BitType bits,
                                 const UInt trailing_bits) noexcept;

  //! Set the data to the given bits
  constexpr void setBits(const BitType data) noexcept;

  //! Return the sign bit mask
  static constexpr BitType signBitMask() noexcept;

  //! Return the significand bit mask
  static constexpr BitType significandBitMask() noexcept;

  //! Return the significand bit length
  static constexpr std::size_t significandBitSize() noexcept;

 private:
  static_assert(kBitSize == 8 * sizeof(BitType));
  static_assert(kBitSize == 8 * sizeof(FloatType));


  //! Calculate the power of two with the given integer as the exponent
  static constexpr FloatType calcExp2(const int expt) noexcept;

  // Bit manipulation

  //! Expand the given bits to the bit length of the internal data type
  template <typename UInt>
  static constexpr BitType expandBits(const UInt x) noexcept;

  //! Return the significand bits with implicit bit from the given bits
  static constexpr BitType getRealSignificandBits(const BitType data) noexcept;

  //! Make significand bits from the given floating point
  static constexpr BitType makeSignificandBits(const FloatType value,
                                               const BitType exp_bits) noexcept;


  BitType data_;
};

//! Check if two values are equal
template <Ieee754BinaryFormat kFormat>
constexpr bool operator==(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept;

//! Check if two values aren't equal
template <Ieee754BinaryFormat kFormat>
constexpr bool operator!=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat>
constexpr bool operator<(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat>
constexpr bool operator<=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat>
constexpr bool operator>(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat>
constexpr bool operator>=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept;

// Type aliases
using Binary16 = Ieee754Binary<Ieee754BinaryFormat::kHalf>;
using Binary32 = Ieee754Binary<Ieee754BinaryFormat::kSingle>;
using Binary64 = Ieee754Binary<Ieee754BinaryFormat::kDouble>;
template <std::size_t kBytes>
using BinaryFromBytes = Ieee754Binary<static_cast<Ieee754BinaryFormat>(8 * kBytes)>;

// Classification and comparison

//! Check if the given type is an instance type of the Ieee754Binary class
template <typename Type>
constexpr bool kIsIeee754Binary = std::is_same_v<Binary16, Type> ||
                                  std::is_same_v<Binary32, Type> ||
                                  std::is_same_v<Binary64, Type>;

//! Check if the given value is finite value
template <typename Float>
constexpr bool isFinite(const Float& x) noexcept;

//! Check if the given value is infinite
template <typename Float>
constexpr bool isInf(const Float& x) noexcept;

//! Check if the given value is NaN
template <typename Float>
constexpr bool isNan(const Float& x) noexcept;

//! Check if the given value is normal
template <typename Float>
constexpr bool isNormal(const Float& x) noexcept;

//! Check if the given value is subnormal
template <typename Float>
constexpr bool isSubnormal(const Float& x) noexcept;

} // namespace gcc

} // namespace zisc

#include "bit_cast_gcc-inl.hpp"

#endif // ZISC_BIT_CAST_GCC_HPP
