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
#include <limits>
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
    std::conditional_t<kFormat == FloatingPointFormat::kHalf, int16b,
    std::conditional_t<kFormat == FloatingPointFormat::kSingle, float, double>>;
  using BitType =
    std::conditional_t<kFormat == FloatingPointFormat::kHalf, uint16b,
    std::conditional_t<kFormat == FloatingPointFormat::kSingle, uint32b, uint64b>>;


  //! Initialize a value with 0
  constexpr FloatingPoint() noexcept;

  //! Initialize a value with a data
  constexpr FloatingPoint(const BitType data) noexcept;

  //! Copy the given data
  template <FloatingPointFormat kSrcFormat>
  constexpr FloatingPoint(const FloatingPoint<kSrcFormat>& other) noexcept;


  //! Copy the given value
  template <FloatingPointFormat kSrcFormat>
  constexpr FloatingPoint& operator=(const FloatingPoint<kSrcFormat>& other) noexcept;

  //! Return the (+/-) negated value
  constexpr FloatingPoint operator-() const noexcept;


  //! Copy the given data
  template <FloatingPointFormat kSrcFormat>
  constexpr void assign(const FloatingPoint<kSrcFormat>& other) noexcept;

  //! Return a bit data of a value
  constexpr BitType bits() const noexcept;

  //! Return the downscaled data
  template <FloatingPointFormat kDstFormat>
  constexpr FloatingPoint<kDstFormat> downscaled() const noexcept;

  //! Return the machine epsilon
  static constexpr FloatingPoint epsilon() noexcept;

  //! Return the exponent bias
  static constexpr std::size_t exponentBias() noexcept;

  //! Return the exponent bit mask
  static constexpr BitType exponentBitMask() noexcept;

  //! Return the exponent bit size
  static constexpr std::size_t exponentBitSize() noexcept;

  //! Get a value from a float
  static FloatingPoint fromFloat(const FloatType f) noexcept;

  //! Get bits of a float value
  static constexpr BitType getBits(const FloatType value) noexcept;

  //! Get exponent bits of a float value
  static constexpr BitType getExponentBits(const FloatType value) noexcept;

  //! Get significand bits of a float value
  static constexpr BitType getSignificandBits(const FloatType value) noexcept;

  //! Get a sign bit of a float value
  static constexpr BitType getSignBit(const FloatType value) noexcept;

  //! Return the positive infinity value
  static constexpr FloatingPoint infinity() noexcept;

  //! Check if the value is finite
  constexpr bool isFinite() const noexcept;

  //! Check if the value is infinity
  constexpr bool isInf() const noexcept;

  //! Check if the value is NaN
  constexpr bool isNan() const noexcept;

  //! Check if the value is normal
  constexpr bool isNormal() const noexcept;

  //! Check if the value is subnormal
  constexpr bool isSubnormal() const noexcept;

  //! Check if the value is zero
  constexpr bool isZero() const noexcept;

  //! Make a float
  static constexpr FloatType makeFloat(const BitType exponent_bits,
                                       const BitType significand_bits) noexcept;

  //! Make a float
  static constexpr FloatType makeFloat(BitType sign_bit,
                                       BitType exponent_bits,
                                       BitType significand_bits) noexcept;

  //! Map an integer value into a [0, 1) float
  template <typename UInt>
  static constexpr FloatType mapTo01(const UInt x) noexcept;

  //! Return the largest finite value
  static constexpr FloatingPoint max() noexcept;

  //! Return the smallest finite value
  static constexpr FloatingPoint min() noexcept;

  //! Return the positive one value
  static constexpr FloatingPoint one() noexcept;

  //! Return a quiet NaN
  static constexpr FloatingPoint quietNan() noexcept;

  //! Return the round error
  static constexpr FloatingPoint roundError() noexcept;

  //! Return a signaling NaN
  static constexpr FloatingPoint signalingNan() noexcept;

  //! Return the sign bit mask
  static constexpr BitType signBitMask() noexcept;

  //! Return the significand bit mask
  static constexpr BitType significandBitMask() noexcept;

  //! Return the significand bit size
  static constexpr std::size_t significandBitSize() noexcept;

  //! Set a float value
  void setFloat(const FloatType data) noexcept;

  //! Set bits
  constexpr void set(const BitType data) noexcept;

  //! Convert to a floating-point value
  FloatType toFloat() const noexcept;

  //! Return the upscaled data
  template <FloatingPointFormat kDstFormat>
  constexpr FloatingPoint<kDstFormat> upscaled() const noexcept;

  //! Return the positive zero value
  static constexpr FloatingPoint zero() noexcept;

 private:
  union FloatData
  {
    FloatType f_;
    BitType b_;
    FloatData(const FloatType f) : f_{f} {}
    FloatData(const BitType b) : b_{b} {}
  };


  //! Expand an input value to the bittype size
  template <typename UInt>
  static constexpr BitType expandToBitSize(const UInt x) noexcept;

  //!
  static constexpr FloatType getPowered(const int exponent) noexcept;


  BitType data_;
};

//! Check if two floating-value are equal in value
template <FloatingPointFormat kFormat>
constexpr bool operator==(const FloatingPoint<kFormat>& lhs,
                          const FloatingPoint<kFormat>& rhs) noexcept;

//! Check if two floating-value aren't equal in value
template <FloatingPointFormat kFormat>
constexpr bool operator!=(const FloatingPoint<kFormat>& lhs,
                          const FloatingPoint<kFormat>& rhs) noexcept;

//!
template <FloatingPointFormat kFormat>
constexpr bool operator<(const FloatingPoint<kFormat>& lhs,
                         const FloatingPoint<kFormat>& rhs) noexcept;

//!
template <FloatingPointFormat kFormat>
constexpr bool operator<=(const FloatingPoint<kFormat>& lhs,
                          const FloatingPoint<kFormat>& rhs) noexcept;

//!
template <FloatingPointFormat kFormat>
constexpr bool operator>(const FloatingPoint<kFormat>& lhs,
                         const FloatingPoint<kFormat>& rhs) noexcept;

//!
template <FloatingPointFormat kFormat>
constexpr bool operator>=(const FloatingPoint<kFormat>& lhs,
                          const FloatingPoint<kFormat>& rhs) noexcept;

// Classification and comparison

//! Check if the given number has finite value
template <typename Float>
constexpr bool isFinite(const Float& x) noexcept;

//! Check if the given number is infinite
template <typename Float>
constexpr bool isInf(const Float& x) noexcept;

//! Check if the given number is NaN
template <typename Float>
constexpr bool isNan(const Float& x) noexcept;

//! Check if the given number is normal
template <typename Float>
constexpr bool isNormal(const Float& x) noexcept;

//! Check if the given number is subnormal
template <typename Float>
constexpr bool isSubnormal(const Float& x) noexcept;


// Type alias
using HalfFloat = FloatingPoint<FloatingPointFormat::kHalf>;
using SingleFloat = FloatingPoint<FloatingPointFormat::kSingle>;
using DoubleFloat = FloatingPoint<FloatingPointFormat::kDouble>;
template <std::size_t kBytes>
using FloatingPointFromBytes =
  std::conditional_t<kBytes == sizeof(HalfFloat), HalfFloat,
  std::conditional_t<kBytes == sizeof(SingleFloat), SingleFloat,
  std::conditional_t<kBytes == sizeof(DoubleFloat), DoubleFloat, void>>>;

template <typename Type>
constexpr bool kIsZiscFloatType = std::is_same_v<HalfFloat, Type> ||
                                  std::is_same_v<SingleFloat, Type> ||
                                  std::is_same_v<DoubleFloat, Type>;

} // namespace zisc

// Specialize std::numeric_limits

namespace std {

template <zisc::FloatingPointFormat kFormat>
class numeric_limits<zisc::FloatingPoint<kFormat>>;

} // namespace std

#include "floating_point-inl.hpp"

#endif // ZISC_FLOATING_POINT_HPP
