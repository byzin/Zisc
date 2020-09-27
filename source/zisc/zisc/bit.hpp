/*!
  \file bit.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BIT_HPP
#define ZISC_BIT_HPP

// Zisc
#include "concepts.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class Bit
{
 public:
  //! Find the smallest integral power of two not less than the given value
  template <UnsignedInteger Integer>
  static constexpr Integer ceil(const Integer x) noexcept;

  //! Find the largest integral power of two not greater than the given value
  template <UnsignedInteger Integer>
  static constexpr Integer floor(const Integer x) noexcept;

  //! Find the smallest number of bits needed to represent the given value
  template <UnsignedInteger Integer>
  static constexpr Integer getWidth(const Integer x) noexcept;

  //! Check if a number is an integral power of two
  template <UnsignedInteger Integer>
  static constexpr bool isPowerOf2(const Integer x) noexcept;
};

// STL style function aliases

//! Check if a number is an integral power of two
template <UnsignedInteger Integer>
static constexpr bool has_single_bit(const Integer x) noexcept;

//! Find the smallest integral power of two not less than the given value
template <UnsignedInteger Integer>
static constexpr Integer bit_ceil(const Integer x) noexcept;

//! Find the largest integral power of two not greater than the given value
template <UnsignedInteger Integer>
static constexpr Integer bit_floor(const Integer x) noexcept;

//! Find the smallest number of bits needed to represent the given value
template <UnsignedInteger Integer>
static constexpr Integer bit_width(const Integer x) noexcept;

} // namespace zisc

#include "bit-inl.hpp"

#endif // ZISC_BIT_HPP
