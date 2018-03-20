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
// Zisc
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

template <std::size_t kN> struct FloatRepresentation;

/*!
  \brief Replesent a n-byte floating point
  */
template <std::size_t kN>
class FloatingPoint
{
 public:
  using FloatType = typename FloatRepresentation<kN>::FloatType;
  using BitType = typename FloatRepresentation<kN>::RepresentationType;


  //! Return the exponent bias
  static constexpr std::size_t exponentBias() noexcept;

  //! Return the exponent bit mask
  static constexpr BitType exponentBitMask() noexcept;

  //! Return the exponent bit size
  static constexpr std::size_t exponentBitSize() noexcept;

  //! Return the sign bit mask
  static constexpr BitType signBitMask() noexcept;

  //! Return the significand bit mask
  static constexpr BitType significandBitMask() noexcept;

  //! Return the significand bit size
  static constexpr std::size_t significandBitSize() noexcept;

 private:
  static_assert(kIsUnsignedInteger<BitType>, "BitType isn't unsigned integer.");
  static_assert(sizeof(BitType) == kN, "BitType isn't n-byte.");

  BitType data_;
};

// Type alias
using HalfFloat = FloatingPoint<2>;
using SingleFloat = FloatingPoint<4>;
using DoubleFloat = FloatingPoint<8>;

} // namespace zisc

#include "floating_point-inl.hpp"

#endif // ZISC_FLOATING_POINT_HPP
