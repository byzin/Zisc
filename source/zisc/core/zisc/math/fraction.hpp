/*!
  \file fraction.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FRACTION_HPP
#define ZISC_FRACTION_HPP

// Standard C++ library
#include <concepts>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam Int No description.

  \todo Implement explicit conversion
  */
template <SignedInteger Int>
class Fraction
{
 public:
  //! The type that is used in Fraction for arithmetic
  using Type = Int;


  //! Creata a fraction (0 / 1)
  constexpr Fraction() noexcept = default;

  //! Create a fraction (numerator / 1)
  explicit constexpr Fraction(const Type n) noexcept;

  //! Create a fraction (numerator / denominator)
  constexpr Fraction(const Type n, const Type d) noexcept;


  //!
  constexpr auto operator-() const noexcept -> Fraction;

  //!
  constexpr auto operator+=(const Fraction& other) noexcept -> Fraction&;

  //!
  constexpr auto operator-=(const Fraction& other) noexcept -> Fraction&;

  //!
  constexpr auto operator*=(const Fraction& other) noexcept -> Fraction&;

  //!
  constexpr auto operator/=(const Fraction& other) noexcept -> Fraction&;

  //! Convert to a float
  explicit constexpr operator float() const noexcept;

  //! Convert to a float
  explicit constexpr operator double() const noexcept;

  //! Convert to a float
  explicit constexpr operator long double() const noexcept;


  //! Return the denominator
  [[nodiscard]]
  constexpr auto denom() noexcept -> Int&;

  //! Return the denominator
  [[nodiscard]]
  constexpr auto denom() const noexcept -> const Int&;

  //! Return the numerator 
  [[nodiscard]]
  constexpr auto numer() noexcept -> Int&;

  //! Return the numerator 
  [[nodiscard]]
  constexpr auto numer() const noexcept -> const Int&;

  //! Return a inverse fraction
  [[nodiscard]]
  constexpr auto invert() const noexcept -> Fraction;

  //! Convert a fraction to a float
  template <std::floating_point Float>
  constexpr auto toFloat() const noexcept -> Float;

 private:
  Type numerator_ = static_cast<Type>(0),
       denominator_ = static_cast<Type>(1);
};

// Aliases
using Fraction16 = Fraction<int16b>;
using Fraction32 = Fraction<int32b>;
using Fraction64 = Fraction<int64b>;

//! Perform addition operation on two values
template <SignedInteger Int>
constexpr auto operator+(const Fraction<Int>& lhs,
                         const Fraction<Int>& rhs) noexcept -> Fraction<Int>;

//! Perform subtraction operation on two values
template <SignedInteger Int>
constexpr auto operator-(const Fraction<Int>& lhs,
                         const Fraction<Int>& rhs) noexcept -> Fraction<Int>;

//! Perform multiplication operation on two values
template <SignedInteger Int>
constexpr auto operator*(const Fraction<Int>& lhs,
                         const Fraction<Int>& rhs) noexcept -> Fraction<Int>;

//! Perform division operation on two values
template <SignedInteger Int>
constexpr auto operator/(const Fraction<Int>& lhs,
                         const Fraction<Int>& rhs) noexcept -> Fraction<Int>;

//! Check if two fractions are equal in value
template <SignedInteger Int>
constexpr auto operator==(const Fraction<Int>& lhs,
                          const Fraction<Int>& rhs) noexcept -> bool;

//! Check if two fractions aren't equal in value
template <SignedInteger Int>
constexpr auto operator!=(const Fraction<Int>& lhs,
                          const Fraction<Int>& rhs) noexcept -> bool;

//!
template <SignedInteger Int>
constexpr auto operator<(const Fraction<Int>& lhs,
                         const Fraction<Int>& rhs) noexcept -> bool;

//!
template <SignedInteger Int>
constexpr auto operator<=(const Fraction<Int>& lhs,
                          const Fraction<Int>& rhs) noexcept -> bool;

//!
template <SignedInteger Int>
constexpr auto operator>(const Fraction<Int>& lhs,
                         const Fraction<Int>& rhs) noexcept -> bool;

//!
template <SignedInteger Int>
constexpr auto operator>=(const Fraction<Int>& lhs,
                          const Fraction<Int>& rhs) noexcept -> bool;

} // namespace zisc

#include "fraction-inl.hpp"

namespace zisc {

constexpr Fraction32 kPiFraction{355, 113}; //!< Pi constant

} // namespace zisc

#endif // ZISC_FRACTION_HPP
