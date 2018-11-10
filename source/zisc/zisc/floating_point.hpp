/*!
  \file floating_point.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOATING_POINT_HPP
#define ZISC_FLOATING_POINT_HPP

// Standard C++ library
#include <cstdint>
#include <cstddef>
#include <type_traits>
// Zisc
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
enum class FloatingPointFormat
{
  kHalf,
  kSingle,
  kDouble
};

/*!
  \brief Replesent a floating point based on IEEE 754
  */
template <FloatingPointFormat kFormat>
class FloatingPoint
{
 public:
  using FloatType =
    std::conditional_t<kFormat == FloatingPointFormat::kHalf, uint16b,
    std::conditional_t<kFormat == FloatingPointFormat::kSingle, float, double>>;
  using BitType =
    std::conditional_t<kFormat == FloatingPointFormat::kHalf, uint16b,
    std::conditional_t<kFormat == FloatingPointFormat::kSingle, uint32b, uint64b>>;


  //! Return the exponent bias
  static constexpr std::size_t exponentBias() noexcept;

  //! Return the exponent bit mask
  static constexpr BitType exponentBitMask() noexcept;

  //! Return the exponent bit size
  static constexpr std::size_t exponentBitSize() noexcept;

  //! Get bits of a float value
  static constexpr BitType getBits(const FloatType value) noexcept;

  //! Get exponent bits of a float value
  static constexpr BitType getExponentBits(const FloatType value) noexcept;

  //! Get significand bits of a float value
  static constexpr BitType getSignificandBits(const FloatType value) noexcept;

  //! Get a sign bit of a float value
  static constexpr BitType getSignBit(const FloatType value) noexcept;

  //! Map an integer value into a [0, 1) float
  template <typename UInt>
  static constexpr FloatType mapTo01(const UInt x) noexcept;

  //! Return the sign bit mask
  static constexpr BitType signBitMask() noexcept;

  //! Return the significand bit mask
  static constexpr BitType significandBitMask() noexcept;

  //! Return the significand bit size
  static constexpr std::size_t significandBitSize() noexcept;

 private:
  //! Expand an input value to the bittype size
  template <typename UInt>
  static constexpr BitType expandToBitSize(const UInt x) noexcept;

  //!
  static constexpr FloatType getPowered(std::size_t exponent) noexcept;

  BitType data_;
};

// Classification and comparison

//! Check if the given number has finite value
template <typename Float>
constexpr bool isFinite(const Float x) noexcept;

//! Check if the given number is infinite
template <typename Float>
constexpr bool isInf(const Float x) noexcept;

//! Check if the given number is NaN
template <typename Float>
constexpr bool isNan(const Float x) noexcept;

//! Check if the given number is normal
template <typename Float>
constexpr bool isNormal(const Float x) noexcept;

//! Check if the given number is subnormal
template <typename Float>
constexpr bool isSubnormal(const Float x) noexcept;


// Type alias
using HalfFloat = FloatingPoint<FloatingPointFormat::kHalf>;
using SingleFloat = FloatingPoint<FloatingPointFormat::kSingle>;
using DoubleFloat = FloatingPoint<FloatingPointFormat::kDouble>;
template <std::size_t kBytes>
using FloatingPointFromBytes =
  std::conditional_t<kBytes == sizeof(HalfFloat), HalfFloat,
  std::conditional_t<kBytes == sizeof(SingleFloat), SingleFloat,
  std::conditional_t<kBytes == sizeof(DoubleFloat), DoubleFloat, void>>>;

} // namespace zisc

#include "floating_point-inl.hpp"

#endif // ZISC_FLOATING_POINT_HPP
