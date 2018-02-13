/*!
  \file floating_point_bit.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOATING_POINT_BIT_HPP
#define ZISC_FLOATING_POINT_BIT_HPP

// Standard C++ library
#include <cstddef>
// Zisc
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

template <std::size_t FloatBitSize> struct FloatingPointUtility;

/*!
  */
template <typename Float>
class FloatingPointBit
{
  static_assert(kIsFloat<Float>, "Float isn't floating point.");

 public:
  using BitType = typename FloatingPointUtility<sizeof(Float)>::BitType;
  using FloatType = Float;


  //! Return a exponent sign bit mask
  static constexpr BitType exponentSignBitMask() noexcept;

  //! Return a exponent value bit mask
  static constexpr BitType exponentValueBitMask() noexcept;

  //! Return a exponent bit mask
  static constexpr BitType exponentBitMask() noexcept;

  //! Return a exponent bit size
  static constexpr BitType exponentBitSize() noexcept;

  //! Get exponent bits of value
  static BitType getExponentBits(const FloatType value) noexcept;

  //! Get mantissa bits of value
  static BitType getMantissaBits(const FloatType value) noexcept;

  //! Get a sign bit of value
  static BitType getSignBit(const FloatType value) noexcept;

  //! Return a half value bits of exponent bits
  static constexpr BitType halfExponentBits(const BitType exponent_bits) noexcept;

  //! Check if a exponent is odd
  static bool isOddExponent(const FloatType value) noexcept;

  //! Check if a exponent is odd
  static constexpr bool isOddExponent(const BitType exponent_bits) noexcept;

  //! Check if a exponent is positive
  static bool isPositiveExponent(const FloatType value) noexcept;

  //! Check if a exponent is positive
  static constexpr bool isPositiveExponent(const BitType exponent_bits) noexcept;

  //! Make a floating point
  static FloatType makeFloat(const BitType mantissa_bits, 
                             const BitType exponent_bits) noexcept;

  //! Make a floating point
  static FloatType makeFloat(const BitType sign_bit,
                             const BitType mantissa_bits, 
                             const BitType exponent_bits) noexcept;

  //! Return a mantissa bit mask
  static constexpr BitType mantissaBitMask() noexcept;

  //! Return a mantissa bit size
  static constexpr BitType mantissaBitSize() noexcept;

  //! Map a integer value into [0, 1) float
  static constexpr FloatType mapTo01Float(BitType x) noexcept;

  //! Return a sign bit mask
  static constexpr BitType signBitMask() noexcept;

  //! Return a sign bit size
  static constexpr BitType signBitSize() noexcept;

 private:
  union BitF
  {
    BitType bit_;
    FloatType float_;

    constexpr BitF(const BitType bit) : bit_{bit} {}
    constexpr BitF(const FloatType f) : float_{f} {}
  };
  static_assert(sizeof(BitF) == sizeof(FloatType), "BitF is invalidate.");
};

//! A float bit class
using FloatBit = FloatingPointBit<float>;
//! A double bit class
using DoubleBit = FloatingPointBit<double>;

} // namespace zisc

#include "floating_point_bit-inl.hpp"

#endif // ZISC_FLOATING_POINT_BIT_HPP
