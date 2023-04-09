/*!
  \file ieee_754_binary_hardware_impl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_HARDWARE_IMPL_HPP
#define ZISC_IEEE_754_BINARY_HARDWARE_IMPL_HPP

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
  \brief The hardware implementation of IEEE 754 binary interchange format

  No detailed description.

  \tparam kFormat Encoding format used in the interchange
  */
template <Ieee754BinaryFormat kFormat>
class Ieee754BinaryHardwareImpl
{
#if defined(Z_CLANG) || defined(Z_GCC)
#define ZISC_HAS_HARDWARE_HALF_IMPL 1
  using HalfT = _Float16;
#else
#define ZISC_HAS_HARDWARE_HALF_IMPL 0
  using HalfT = int16b;
#endif

 public:
  //! Bit representation type
  using BitT = std::conditional_t<kFormat == Ieee754BinaryFormat::kHalf,   uint16b,
               std::conditional_t<kFormat == Ieee754BinaryFormat::kSingle, uint32b,
                                                                           uint64b>>;

  //! Internal data type
  using DataT = std::conditional_t<kFormat == Ieee754BinaryFormat::kHalf,   HalfT,
                std::conditional_t<kFormat == Ieee754BinaryFormat::kSingle, float,
                                                                            double>>;


  //! Initialize a value with 0
  constexpr Ieee754BinaryHardwareImpl() noexcept = default;

  //! Initialize a value with the given floating point value
  template <std::floating_point Float>
  constexpr Ieee754BinaryHardwareImpl(const Float value) noexcept;

  //! Initialize a value with the given floating point value
  constexpr Ieee754BinaryHardwareImpl(const HalfT value) noexcept;

  //! Initialize a value with the given bits data
  constexpr Ieee754BinaryHardwareImpl(const BitT bits) noexcept;


  //! Convert to other format
  template <Ieee754BinaryFormat kDstFormat>
  constexpr Ieee754BinaryHardwareImpl<kDstFormat> convertTo() const noexcept;

  //! Convert to a floating point
  template <std::floating_point Float>
  constexpr Float convertTo() const noexcept;

  //! Return the underlying floating point value
  constexpr DataT& data() noexcept;

  //! Return the underlying floating point value
  constexpr const DataT& data() const noexcept;

  //! Return the bit length used in the interchange encoding
  static constexpr std::size_t bitSize() noexcept;

  //! Return the format
  static constexpr Ieee754BinaryFormat format() noexcept;

  //! Return the float round style
  static constexpr std::float_round_style roundStyle() noexcept;

  //! Set the data to the given bits
  constexpr void setBits(const BitT bits) noexcept;

 private:
  DataT data_;
};

// Assignment

//! Apply addition assignment
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat>& operator+=(
    Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//! Apply subtraction assignment
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat>& operator-=(
    Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//! Apply multiplication assignment
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat>& operator*=(
    Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//! Apply division assignment
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat>& operator/=(
    Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

// Arithmetic

//! Return the value it's own
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat> operator+(
    const Ieee754BinaryHardwareImpl<kFormat>& value) noexcept;

//! Return the negated value
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat> operator-(
    const Ieee754BinaryHardwareImpl<kFormat>& value) noexcept;

//! Compute an addition
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat> operator+(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//! Compute an subtraction
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat> operator-(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//! Compute an multiplication
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat> operator*(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//! Compute an division
template <Ieee754BinaryFormat kFormat>
constexpr Ieee754BinaryHardwareImpl<kFormat> operator/(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

// Comparison

//! Check if two values are equal
template <Ieee754BinaryFormat kFormat>
constexpr bool operator==(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//! Check if two values aren't equal
template <Ieee754BinaryFormat kFormat>
constexpr bool operator!=(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat>
constexpr bool operator<(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat>
constexpr bool operator<=(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat>
constexpr bool operator>(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

//!
template <Ieee754BinaryFormat kFormat>
constexpr bool operator>=(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept;

} // namespace zisc

#include "ieee_754_binary_hardware_impl-inl.hpp"

#endif // ZISC_IEEE_754_BINARY_HARDWARE_IMPL_HPP
