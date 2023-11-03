/*!
  \file ieee_754_binary.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_HPP
#define ZISC_IEEE_754_BINARY_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
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

// Format utilities
//! Define a binary format of the given bytes
template <std::size_t kBytes>
constexpr Ieee754BinaryFormat kBinFormatFromBytes = static_cast<Ieee754BinaryFormat>(8 * kBytes);

//! Define a binary format of same size as the given float
template <std::floating_point Float>
constexpr Ieee754BinaryFormat kBinFormatFromFloat = kBinFormatFromBytes<sizeof(Float)>;

//! Get the bit size of the given binary format
template <Ieee754BinaryFormat kFormat>
constexpr std::size_t kBinFormatBitSize = static_cast<std::size_t>(kFormat);

// Forward declaration
template <Ieee754BinaryFormat, std::float_round_style kRoundStyle> class Ieee754BinarySoftwareImpl;
template <Ieee754BinaryFormat> class Ieee754BinaryHardwareImpl;

} // namespace zisc

#include "internal/ieee_754_binary_software_impl.hpp"
#include "internal/ieee_754_binary_hardware_impl.hpp"

namespace zisc {

// Type aliases
template <Ieee754BinaryFormat kFormat> class Ieee754Binary;
using Binary16 = Ieee754Binary<Ieee754BinaryFormat::kHalf>;
using Binary32 = Ieee754Binary<Ieee754BinaryFormat::kSingle>;
using Binary64 = Ieee754Binary<Ieee754BinaryFormat::kDouble>;

//! Define a binary type of the given bytes
template <std::size_t kBytes>
using BinaryFromBytes = Ieee754Binary<kBinFormatFromBytes<kBytes>>;

//! Define a binary type of same size as the given float
template <std::floating_point Float>
using BinaryFromFloat = Ieee754Binary<kBinFormatFromFloat<Float>>;

/*!
  \brief Represent IEEE 754 binary interchange format

  No detailed description.

  \tparam kFormat Encoding format used in the interchange
  */
template <Ieee754BinaryFormat kFormat>
class Ieee754Binary
{
  // Implementation types
  using SoftImplT = Ieee754BinarySoftwareImpl<kFormat, std::float_round_style::round_to_nearest>;
  using HardImplT = Ieee754BinaryHardwareImpl<kFormat>;
#if ZISC_HAS_HARDWARE_HALF_IMPL == 1
  using ImplT = std::conditional_t<(Config::architecture() == Config::Architecture::kAmd64V3) ||
                                   (Config::architecture() == Config::Architecture::kAmd64V4),
                                   HardImplT,
                                   SoftImplT>;
#else // ZISC_HAS_HARDWARE_HALF_IMPL
  using ImplT = SoftImplT;
#endif

 public:
  //! Bit representation type
  using BitT = typename ImplT::BitT;


  //! Initialize a value with 0
  constexpr Ieee754Binary() noexcept = default;

  //! Initialize a value with the given floating point value
  template <std::floating_point Float>
  constexpr Ieee754Binary(const Float value) noexcept;

  //! Initialize a value with the given bits
  constexpr Ieee754Binary(const BitT bits) noexcept;

  //! Initialize a value with the given implementation
  constexpr Ieee754Binary(const ImplT& impl) noexcept;


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


  //! Return the machine epsilon
  static constexpr auto epsilon() noexcept -> Ieee754Binary;

  //! Return the infinity
  static constexpr auto infinity() noexcept -> Ieee754Binary;

  //! Check if the underlying value is finite
  [[nodiscard]]
  constexpr auto isFinite() const noexcept -> bool;

  //! Check if the underlying value is infinity
  [[nodiscard]]
  constexpr auto isInf() const noexcept -> bool;

  //! Check if the underlying value is NaN 
  [[nodiscard]]
  constexpr auto isNan() const noexcept -> bool;

  //! Check if the underlying value is normal
  [[nodiscard]]
  constexpr auto isNormal() const noexcept -> bool;

  //! Check if the underlying value is subnormal
  [[nodiscard]]
  constexpr auto isSubnormal() const noexcept -> bool;

  //! Check if the underlying value is zero
  [[nodiscard]]
  constexpr auto isZero() const noexcept -> bool;

  //! Return the largest normal value
  static constexpr auto max() noexcept -> Ieee754Binary;

  //! Return the smallest normal value
  static constexpr auto min() noexcept -> Ieee754Binary;

  //! Return the positive one value
  static constexpr auto one() noexcept -> Ieee754Binary;

  //! Return a quiet NaN
  static constexpr auto quietNan() noexcept -> Ieee754Binary;

  //! Return the largest possible rounding error in ULPs
  static constexpr auto roundError() noexcept -> Ieee754Binary;

  //! Return the float round style
  static constexpr auto roundStyle() noexcept -> std::float_round_style;

  //! Return a signaling NaN
  static constexpr auto signalingNan() noexcept -> Ieee754Binary;

  //! Return the positive zero
  static constexpr auto zero() noexcept -> Ieee754Binary;

  // Bit manipulation

  //! Return the bits in which a floating point is encoded
  [[nodiscard]]
  constexpr auto bits() const noexcept -> BitT;

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

  //! Return the underlying binary implementation
  [[nodiscard]]
  constexpr auto impl() noexcept -> ImplT&;

  //! Return the underlying binary implementation
  [[nodiscard]]
  constexpr auto impl() const noexcept -> const ImplT&;

  //! Return the hidden bit of normal values
  static constexpr auto implicitBit() noexcept -> BitT;

  //! Return the negative of the given value
  static constexpr auto negateBits(const BitT bits) noexcept -> BitT;

  //! Set the data to the given bits
  constexpr void setBits(const BitT data) noexcept;

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
  ImplT impl_;
};

// Assignment

//! Apply addition assignment
template <Ieee754BinaryFormat kFormat>
constexpr auto operator+=(Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>&;

//! Apply subtraction assignment
template <Ieee754BinaryFormat kFormat>
constexpr auto operator-=(Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>&;

//! Apply multiplication assignment
template <Ieee754BinaryFormat kFormat>
constexpr auto operator*=(Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>&;

//! Apply division assignment
template <Ieee754BinaryFormat kFormat>
constexpr auto operator/=(Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>&;

// Artihmeteic

//! Return the value it's own
template <Ieee754BinaryFormat kFormat>
constexpr auto operator+(const Ieee754Binary<kFormat>& value) noexcept
    -> Ieee754Binary<kFormat>;

//! Return the negated value
template <Ieee754BinaryFormat kFormat>
constexpr auto operator-(const Ieee754Binary<kFormat>& value) noexcept
    -> Ieee754Binary<kFormat>;

//! Comppute an addition
template <Ieee754BinaryFormat kFormat>
constexpr auto operator+(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>;

//! Comppute a subtraction
template <Ieee754BinaryFormat kFormat>
constexpr auto operator-(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>;

//! Comppute a multiplication
template <Ieee754BinaryFormat kFormat>
constexpr auto operator*(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>;

//! Comppute a division 
template <Ieee754BinaryFormat kFormat>
constexpr auto operator/(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>;

// Comparison

//! Check if two values are equal
template <Ieee754BinaryFormat kFormat>
constexpr auto operator==(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept -> bool;

//! Check if two values aren't equal
template <Ieee754BinaryFormat kFormat>
constexpr auto operator!=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept -> bool;

//!
template <Ieee754BinaryFormat kFormat>
constexpr auto operator<(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept -> bool;

//!
template <Ieee754BinaryFormat kFormat>
constexpr auto operator<=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept -> bool;

//!
template <Ieee754BinaryFormat kFormat>
constexpr auto operator>(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept -> bool;

//!
template <Ieee754BinaryFormat kFormat>
constexpr auto operator>=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept -> bool;

// Classification and comparison

//! Specify a type is an instance type of the Ieee754Binary class
template <typename Type>
concept Ieee754BinaryClass = std::is_same_v<Binary16, std::remove_cv_t<Type>> ||
                             std::is_same_v<Binary32, std::remove_cv_t<Type>> ||
                             std::is_same_v<Binary64, std::remove_cv_t<Type>>;

//! Check if the given value is finite value
template <Ieee754BinaryClass Float>
constexpr auto isFinite(const Float& x) noexcept -> bool;

//! Check if the given value is finite value
template <std::floating_point Float>
constexpr auto isFinite(const Float& x) noexcept -> bool;

//! Check if the given value is infinite
template <Ieee754BinaryClass Float>
constexpr auto isInf(const Float& x) noexcept -> bool;

//! Check if the given value is infinite
template <std::floating_point Float>
constexpr auto isInf(const Float& x) noexcept -> bool;

//! Check if the given value is NaN
template <Ieee754BinaryClass Float>
constexpr auto isNan(const Float& x) noexcept -> bool;

//! Check if the given value is NaN
template <std::floating_point Float>
constexpr auto isNan(const Float& x) noexcept -> bool;

//! Check if the given value is normal
template <Ieee754BinaryClass Float>
constexpr auto isNormal(const Float& x) noexcept -> bool;

//! Check if the given value is normal
template <std::floating_point Float>
constexpr auto isNormal(const Float& x) noexcept -> bool;

//! Check if the given value is subnormal
template <Ieee754BinaryClass Float>
constexpr auto isSubnormal(const Float& x) noexcept -> bool;

//! Check if the given value is subnormal
template <std::floating_point Float>
constexpr auto isSubnormal(const Float& x) noexcept -> bool;

} // namespace zisc

#include "ieee_754_binary-inl.hpp"

#endif // ZISC_IEEE_754_BINARY_HPP
