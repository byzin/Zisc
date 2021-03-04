/*!
  \file ieee_754_binary.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_HPP
#define ZISC_IEEE_754_BINARY_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "concepts.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

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


// Type aliases
template <Ieee754BinaryFormat kFormat> class Ieee754Binary;
using Binary16 = Ieee754Binary<Ieee754BinaryFormat::kHalf>;
using Binary32 = Ieee754Binary<Ieee754BinaryFormat::kSingle>;
using Binary64 = Ieee754Binary<Ieee754BinaryFormat::kDouble>;

/*!
  \brief The software implementation of IEEE 754 binary interchange format

  No detailed description.

  \tparam kFormat Encoding format used in the interchange
  */
template <Ieee754BinaryFormat kFormat>
class Ieee754Binary
{
 public:
  static constexpr std::size_t kBitSize = cast<std::size_t>(kFormat);


  // Type aliases
  //! Internal data type
  using BitType = std::conditional_t<kBitSize == 16, uint16b,
                  std::conditional_t<kBitSize == 32, uint32b, uint64b>>;
  //! Bit representation type
  using FloatType = std::conditional_t<kBitSize == 32, float,
                    std::conditional_t<kBitSize == 64, double, BitType>>;


  //! Initialize a value with 0
  constexpr Ieee754Binary() noexcept;

  //! Initialize a value with a floating point value
  template <FloatingPoint Float>
  constexpr Ieee754Binary(const Float value) noexcept;

  //! Initialize a value with the given bits
  constexpr Ieee754Binary(const BitType data) noexcept;


  //! Return the value it's own
  constexpr Ieee754Binary operator+() const noexcept;

  //! Return the negated value
  constexpr Ieee754Binary operator-() const noexcept;

  //! Convert to binary16
  explicit constexpr operator Binary16() const noexcept;

  //! Convert to binary32
  explicit constexpr operator Binary32() const noexcept;

  //! Convert to binary64
  explicit constexpr operator Binary64() const noexcept;

  //! Conver to a float
  explicit constexpr operator float() const noexcept;

  //! Conver to a double 
  explicit constexpr operator double() const noexcept;

  //! Conver to a long double
  explicit constexpr operator long double() const noexcept;


  //! Convert to other format
  template <Ieee754BinaryFormat kDstFormat,
            Ieee754RoundingMode kRMode = kDefaultIeee754RoundingMode>
  constexpr Ieee754Binary<kDstFormat> convertTo() const noexcept;

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

  //! Round the given value with the trailing bits
  template <Ieee754RoundingMode kMode, UnsignedInteger Integer>
  static constexpr BitType round(const BitType bits,
                                 const Integer trailing_bits) noexcept;

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


  //! Convert a special value to the dst format
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754Binary<kDstFormat> convertSpecialValue() const noexcept;

  //! Scale down floating point precision
  template <Ieee754BinaryFormat kDstFormat, Ieee754RoundingMode kRMode>
  constexpr Ieee754Binary<kDstFormat> scaledDown() const noexcept;

  //! Scale up floating point precision
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754Binary<kDstFormat> scaledUp() const noexcept;

  // Bit manipulation

  //! Return the significand bits with implicit bit from the given bits
  static constexpr BitType getRealSignificandBits(const BitType data) noexcept;


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
template <std::size_t kBytes>
using BinaryFromBytes = Ieee754Binary<cast<Ieee754BinaryFormat>(8 * kBytes)>;

// Classification and comparison

//! Check if the given type is an instance type of the Ieee754Binary class
template <typename Type>
constexpr bool kIsIeee754Binary = std::is_same_v<Binary16, Type> ||
                                  std::is_same_v<Binary32, Type> ||
                                  std::is_same_v<Binary64, Type>;

//! Specify a type is floating point type or an instance of Ieee754Binary type
template <typename Type>
concept ZFloatingPoint = std::is_floating_point_v<Type> || kIsIeee754Binary<Type>;

//! Check if the given value is finite value
template <ZFloatingPoint Float>
constexpr bool isFinite(const Float& x) noexcept;

//! Check if the given value is infinite
template <ZFloatingPoint Float>
constexpr bool isInf(const Float& x) noexcept;

//! Check if the given value is NaN
template <ZFloatingPoint Float>
constexpr bool isNan(const Float& x) noexcept;

//! Check if the given value is normal
template <ZFloatingPoint Float>
constexpr bool isNormal(const Float& x) noexcept;

//! Check if the given value is subnormal
template <ZFloatingPoint Float>
constexpr bool isSubnormal(const Float& x) noexcept;

} // namespace zisc

#include "ieee_754_binary-inl.hpp"

#endif // ZISC_IEEE_754_BINARY_HPP
