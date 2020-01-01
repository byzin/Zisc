/*!
  \file unit_multiple.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIT_MULTIPLE_HPP
#define ZISC_UNIT_MULTIPLE_HPP

// Standard C++ library
#include <cstdint>
// Zisc
#include "fraction.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Represent a multiple of a unit

  No detailed description.

  \tparam kBase No description.
  \tparam kExponent No description.
  */
template <int64b kBase, int64b kExponent>
class UnitMultiple
{
 public:
  //! The type that is used for value representation
  using FractionType = Fraction<int64b>;

  //! The type that is used in UnitMultiple for arithmetic
  using ArithmeticType = FractionType::ArithmeticType;


  //! Initialize with 0
  constexpr UnitMultiple() noexcept;

  //! Initialize with (value * kBase^kExponent)
  constexpr UnitMultiple(const ArithmeticType value) noexcept;

  //! Initialize with (value * kBase^kExponent)
  constexpr UnitMultiple(const FractionType& value) noexcept;

  //! Change the exponent of the base
  template <ArithmeticType kOtherExponent>
  constexpr UnitMultiple(const UnitMultiple<kBase, kOtherExponent>& other) noexcept;


  //! Set the value
  constexpr UnitMultiple& operator=(const ArithmeticType value) noexcept;

  //! Set the value
  constexpr UnitMultiple& operator=(const FractionType& value) noexcept;

  //! Change the exponent of the base
  template <ArithmeticType kOtherExponent>
  constexpr UnitMultiple& operator=(const UnitMultiple<kBase, kOtherExponent>& other) noexcept;


  //! Return the base value
  static constexpr ArithmeticType base() noexcept;

  //! Return the exponent value
  static constexpr ArithmeticType exponent() noexcept;

  //! Represent this value as (x * kBase^kToExponent)
  template <ArithmeticType kToExponent>
  constexpr UnitMultiple<kBase, kToExponent> representedAs() const noexcept;

  //! Set the value
  constexpr void setValue(const ArithmeticType value) noexcept;

  //! Set the value
  constexpr void setValue(const FractionType& value) noexcept;

  //! Change the exponent of the base
  template <ArithmeticType kOtherExponent>
  constexpr void setValue(const UnitMultiple<kBase, kOtherExponent>& other) noexcept;

  //! Return the value
  constexpr FractionType& value() noexcept;

  //! Return the value
  constexpr const FractionType& value() const noexcept;

 private:
  FractionType value_;
};

// Binary prefix aliases
using ByteUnit = UnitMultiple<1024, 0>;
using KibiUnit = UnitMultiple<1024, 1>;
using MebiUnit = UnitMultiple<1024, 2>;
using GibiUnit = UnitMultiple<1024, 3>;

//! Perform addition operation on two values in the smaller exponent of the unit
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr UnitMultiple<kBase, std::min(kExponent1, kExponent2)> operator+(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept;

//! Perform subtraction operation on two values in the smaller exponent of the unit
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr UnitMultiple<kBase, std::min(kExponent1, kExponent2)> operator-(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept;

//! Perform multiplication operation on two values in the smaller exponent of the unit
template <int64b kBase, int64b kExponent, typename Integer>
constexpr UnitMultiple<kBase, kExponent> operator*(
    const UnitMultiple<kBase, kExponent>& lhs,
    const Integer rhs) noexcept;

//! Perform multiplication operation on two values in the smaller exponent of the unit
template <typename Integer, int64b kBase, int64b kExponent>
constexpr UnitMultiple<kBase, kExponent> operator*(
    const Integer lhs,
    const UnitMultiple<kBase, kExponent>& rhs) noexcept;

//! Perform division operation on two values in the smaller exponent of the unit
template <int64b kBase, int64b kExponent, typename Integer>
constexpr UnitMultiple<kBase, kExponent> operator/(
    const UnitMultiple<kBase, kExponent>& lhs,
    const Integer rhs) noexcept;

//! Check if the two values are equal
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr bool operator==(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept;

//! Check if the two values are equal
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr bool operator!=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept;

//!
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr bool operator<(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept;

//!
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr bool operator<=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept;

//!
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr bool operator>(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept;

//!
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr bool operator>=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept;

} // namespace zisc

#include "unit_multiple-inl.hpp"

#endif // ZISC_UNIT_MULTIPLE_HPP
