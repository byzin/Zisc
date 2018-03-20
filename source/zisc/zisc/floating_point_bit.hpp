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
#include <cstdint>
// Zisc
#include "floating_point.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Float>
class FloatingPointBit
{
 public:
  using FloatType = Float;
  using BitType = typename FloatingPoint<sizeof(Float)>::BitType;


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

  //! Make a floating point
  static constexpr FloatType makeFloat(const BitType exponent_bits, 
                                       const BitType significand_bits) noexcept;

  //! Make a floating point
  static constexpr FloatType makeFloat(const BitType sign_bit,
                                       const BitType exponent_bits, 
                                       const BitType significand_bits) noexcept;

  //! Map an integer value into a [0, 1) float
  static constexpr FloatType mapTo01Float(BitType x) noexcept;

  //! Return the sign bit mask
  static constexpr BitType signBitMask() noexcept;

  //! Return the significand bit mask
  static constexpr BitType significandBitMask() noexcept;

  //! Return the significand bit size
  static constexpr std::size_t significandBitSize() noexcept;

 private:
  static_assert(kIsFloat<FloatType>, "Float isn't floating point.");
};

// Type alias
using FloatBit = FloatingPointBit<float>; //!< A float bit class
using DoubleBit = FloatingPointBit<double>; //!< A double bit class

} // namespace zisc

#include "floating_point_bit-inl.hpp"

#endif // ZISC_FLOATING_POINT_BIT_HPP
