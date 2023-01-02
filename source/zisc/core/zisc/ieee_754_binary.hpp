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

// Forward declaration
template <Ieee754BinaryFormat> class Ieee754BinarySoftwareImpl;
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
  using SoftwareImplT = Ieee754BinarySoftwareImpl<kFormat>;
  using HardwareImplT = Ieee754BinaryHardwareImpl<kFormat>;
#if ZISC_HAS_HARDWARE_HALF_IMPL == 1
  using ImplT =
      std::conditional_t<(Config::architecture() == Config::Architecture::kAmd64V3) ||
                         (Config::architecture() == Config::Architecture::kAmd64V4),
          HardwareImplT,
          SoftwareImplT>;
#else // ZISC_HAS_HARDWARE_HALF_IMPL
  using ImplT = SoftwareImplT;
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
  constexpr BitT bits() const noexcept;

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

 //! Return the hidden bit of normal values
  static constexpr BitT implicitBit() noexcept;

  //! Set the data to the given bits
  constexpr void setBits(const BitT data) noexcept;

  //! Return the sign bit mask
  static constexpr BitT signBitMask() noexcept;

  //! Return the significand bit mask
  static constexpr BitT significandBitMask() noexcept;

  //! Return the significand bit length
  static constexpr std::size_t significandBitSize() noexcept;

 private:
  //! Initialize a value with the given implementation
  constexpr Ieee754Binary(const ImplT& impl) noexcept;


  //! Return the underlying binary implementation
  constexpr const ImplT& impl() const noexcept;

  //! Return the underlying binary implementation
  constexpr ImplT& impl() noexcept;


  ImplT impl_;
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

// Classification and comparison

//! Specify a type is an instance type of the Ieee754Binary class
template <typename Type>
concept Ieee754BinaryClass = std::is_same_v<Binary16, std::remove_cv_t<Type>> ||
                             std::is_same_v<Binary32, std::remove_cv_t<Type>> ||
                             std::is_same_v<Binary64, std::remove_cv_t<Type>>;

//! Check if the given value is finite value
template <Ieee754BinaryClass Float>
constexpr bool isFinite(const Float& x) noexcept;

//! Check if the given value is finite value
template <std::floating_point Float>
constexpr bool isFinite(const Float& x) noexcept;

//! Check if the given value is infinite
template <Ieee754BinaryClass Float>
constexpr bool isInf(const Float& x) noexcept;

//! Check if the given value is infinite
template <std::floating_point Float>
constexpr bool isInf(const Float& x) noexcept;

//! Check if the given value is NaN
template <Ieee754BinaryClass Float>
constexpr bool isNan(const Float& x) noexcept;

//! Check if the given value is NaN
template <std::floating_point Float>
constexpr bool isNan(const Float& x) noexcept;

//! Check if the given value is normal
template <Ieee754BinaryClass Float>
constexpr bool isNormal(const Float& x) noexcept;

//! Check if the given value is normal
template <std::floating_point Float>
constexpr bool isNormal(const Float& x) noexcept;

//! Check if the given value is subnormal
template <Ieee754BinaryClass Float>
constexpr bool isSubnormal(const Float& x) noexcept;

//! Check if the given value is subnormal
template <std::floating_point Float>
constexpr bool isSubnormal(const Float& x) noexcept;

} // namespace zisc

#include "ieee_754_binary-inl.hpp"

#endif // ZISC_IEEE_754_BINARY_HPP
