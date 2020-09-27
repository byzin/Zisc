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
#include "zisc_config.hpp"

namespace zisc {

// Feature test

//! Test if a compiler has the bit cast feature
template <Config::ImplType kImpl = Config::implType()> [[nodiscard]]
constexpr bool hasStdBitCast() noexcept;

//! Test if a compiler has the bit operations feature
template <Config::ImplType kImpl = Config::implType()> [[nodiscard]]
constexpr bool hasStdBitOperations() noexcept;

//! Test if a compiler has the power-of-2 operations feature
template <Config::ImplType kImpl = Config::implType()> [[nodiscard]]
constexpr bool hasStdPowerOf2Operations() noexcept;

/*!
  \brief No brief description

  No detailed description.
  */
class Bit
{
 public:

  // Bit operations

  //! Count the number of consecutive 0 bits, starting from the msb
  template <UnsignedInteger Integer>
  static constexpr int countLZero(const Integer x) noexcept;

  //! Count the number of consecutive 1 bits, starting from the msb
  template <UnsignedInteger Integer>
  static constexpr int countLOne(const Integer x) noexcept;

  //! Count the number of 1 bits
  template <UnsignedInteger Integer>
  static constexpr int countPop(const Integer x) noexcept;

  //! Count the number of consecutive 0 bits, starting from the lsb
  template <UnsignedInteger Integer>
  static constexpr int countRZero(const Integer x) noexcept;

  //! Count the number of consecutive 1 bits, starting from the lsb
  template <UnsignedInteger Integer>
  static constexpr int countROne(const Integer x) noexcept;

  // Power-of-2 operations

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

 private:
  //! std implementation
  class Std
  {
   public:
    // Bit operations

    //! Count the number of consecutive 0 bits, starting from the msb
    template <UnsignedInteger Integer>
    static constexpr int countLZero(const Integer x) noexcept;

    //! Count the number of consecutive 1 bits, starting from the msb
    template <UnsignedInteger Integer>
    static constexpr int countLOne(const Integer x) noexcept;

    //! Count the number of 1 bits
    template <UnsignedInteger Integer>
    static constexpr int countPop(const Integer x) noexcept;

    //! Count the number of consecutive 0 bits, starting from the lsb
    template <UnsignedInteger Integer>
    static constexpr int countRZero(const Integer x) noexcept;

    //! Count the number of consecutive 1 bits, starting from the lsb
    template <UnsignedInteger Integer>
    static constexpr int countROne(const Integer x) noexcept;

    // Power-of-2 operations

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

  //! Fallback implementation
  class Zisc
  {
   public:
    // Power-of-2 operations

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
};

// STL style function aliases

// Bit operations

//! Count the number of consecutive 0 bits, starting from the msb
template <UnsignedInteger Integer>
constexpr int countl_zero(const Integer x) noexcept;

//! Count the number of consecutive 1 bits, starting from the msb
template <UnsignedInteger Integer>
constexpr int countl_one(const Integer x) noexcept;

//! Count the number of consecutive 0 bits, starting from the lsb
template <UnsignedInteger Integer>
constexpr int countr_zero(const Integer x) noexcept;

//! Count the number of consecutive 1 bits, starting from the lsb
template <UnsignedInteger Integer>
constexpr int countr_one(const Integer x) noexcept;

//! Count the number of 1 bits
template <UnsignedInteger Integer>
constexpr int popcount(const Integer x) noexcept;

// Power-of-2 operations

//! Check if a number is an integral power of two
template <UnsignedInteger Integer>
constexpr bool has_single_bit(const Integer x) noexcept;

//! Find the smallest integral power of two not less than the given value
template <UnsignedInteger Integer>
constexpr Integer bit_ceil(const Integer x) noexcept;

//! Find the largest integral power of two not greater than the given value
template <UnsignedInteger Integer>
constexpr Integer bit_floor(const Integer x) noexcept;

//! Find the smallest number of bits needed to represent the given value
template <UnsignedInteger Integer>
constexpr Integer bit_width(const Integer x) noexcept;

} // namespace zisc

#include "bit-inl.hpp"

#endif // ZISC_BIT_HPP
