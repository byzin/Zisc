/*!
  \file fraction.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FRACTION_HPP
#define ZISC_FRACTION_HPP

// Standard C++ library
#include <type_traits>
// Zisc
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
 */
template <typename SignedInteger>
class Fraction
{
  static_assert(kIsSignedInteger<SignedInteger>,
                "SignedInteger isn't signed integer type.");

 public:
  //! The type that is used in Fraction for arithmetic
  using ArithmeticType = SignedInteger;


  //! Creata a fraction (0 / 1)
  constexpr Fraction() noexcept;

  //! Create a fraction (numerator / 1)
  constexpr Fraction(const SignedInteger n) noexcept;

  //! Create a fraction (numerator / denominator)
  constexpr Fraction(const SignedInteger n, const SignedInteger d) noexcept;


  //!
  constexpr Fraction operator-() const noexcept;

  //!
  constexpr Fraction& operator+=(const Fraction& other) noexcept;

  //!
  constexpr Fraction& operator-=(const Fraction& other) noexcept;

  //!
  constexpr Fraction& operator*=(const Fraction& other) noexcept;

  //!
  constexpr Fraction& operator/=(const Fraction& other) noexcept;


  //! Return the denominator
  constexpr SignedInteger& denominator() noexcept;

  //! Return the denominator
  constexpr const SignedInteger& denominator() const noexcept;

  //! Return the numerator 
  constexpr SignedInteger& numerator() noexcept;

  //! Return the numerator 
  constexpr const SignedInteger& numerator() const noexcept;

  //! Return a inverse fraction
  constexpr Fraction invert() const noexcept;

  //! Convert a fraction to a float
  template <typename Float>
  constexpr Float toFloat() const noexcept;

 private:
  //! Return the absolute value
  static constexpr SignedInteger abs(const SignedInteger n) noexcept;


  SignedInteger numerator_,
                denominator_;
};

// Aliases
using Fraction16 = Fraction<int16b>;
using Fraction32 = Fraction<int32b>;
using Fraction64 = Fraction<int64b>;

//! Perform addition operation on two values
template <typename SignedInteger>
constexpr Fraction<SignedInteger> operator+(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;
 
//! Perform subtraction operation on two values
template <typename SignedInteger>
constexpr Fraction<SignedInteger> operator-(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

//! Perform multiplication operation on two values
template <typename SignedInteger>
constexpr Fraction<SignedInteger> operator*(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

//! Perform division operation on two values
template <typename SignedInteger>
constexpr Fraction<SignedInteger> operator/(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

//! Check if two fractions are equal in value
template <typename SignedInteger>
constexpr bool operator==(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

//! Check if two fractions aren't equal in value
template <typename SignedInteger>
constexpr bool operator!=(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

//!
template <typename SignedInteger>
constexpr bool operator<(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

//!
template <typename SignedInteger>
constexpr bool operator<=(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

//!
template <typename SignedInteger>
constexpr bool operator>(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

//!
template <typename SignedInteger>
constexpr bool operator>=(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept;

} // namespace zisc

#include "fraction-inl.hpp"

namespace zisc {

constexpr Fraction32 kPiFraction{355, 113};

} // namespace zisc

#endif // ZISC_FRACTION_HPP
