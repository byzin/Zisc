/*!
  \file unit_multiple.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIT_MULTIPLE_HPP
#define ZISC_UNIT_MULTIPLE_HPP

// Standard C++ library
#include <algorithm>
#include <cstdint>
// Zisc
#include "fraction.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
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
  using Type = FractionType::Type;


  //! Initialize with 0
  constexpr UnitMultiple() noexcept = default;

  //! Initialize with (value * kBase^kExponent)
  explicit constexpr UnitMultiple(const Type value) noexcept;

  //! Initialize with (value * kBase^kExponent)
  explicit constexpr UnitMultiple(const FractionType& value) noexcept;

  //! Change the exponent of the base
  template <Type kOtherExponent>
  explicit constexpr UnitMultiple(const UnitMultiple<kBase, kOtherExponent>& other) noexcept;


  //! Set the value
  constexpr auto operator=(const Type value) noexcept -> UnitMultiple&;

  //! Set the value
  constexpr auto operator=(const FractionType& value) noexcept -> UnitMultiple&;

  //! Change the exponent of the base
  template <Type kOtherExponent>
  constexpr auto operator=(const UnitMultiple<kBase, kOtherExponent>& other) noexcept -> UnitMultiple&;


  //! Return the base value
  static constexpr auto base() noexcept -> Type;

  //! Represent this value as (x * kBase^kToExponent)
  template <Type kToExponent>
  constexpr auto cast() const noexcept -> UnitMultiple<kBase, kToExponent>;

  //! Return the exponent value
  static constexpr auto exponent() noexcept -> Type;

  //! Set the value
  constexpr void set(const Type value) noexcept;

  //! Set the value
  constexpr void set(const FractionType& value) noexcept;

  //! Change the exponent of the base
  template <Type kOtherExponent>
  constexpr void set(const UnitMultiple<kBase, kOtherExponent>& other) noexcept;

  //! Return the value
  [[nodiscard]]
  constexpr auto value() noexcept -> FractionType&;

  //! Return the value
  [[nodiscard]]
  constexpr auto value() const noexcept -> const FractionType&;

 private:
  FractionType value_;
};

// Binary prefix aliases
using ByteUnit = UnitMultiple<1024, 0>;
using KibiUnit = UnitMultiple<1024, 1>;
using MebiUnit = UnitMultiple<1024, 2>;
using GibiUnit = UnitMultiple<1024, 3>;

#if !defined(Z_MSVC)

//! Perform addition operation on two values in the smaller exponent of the unit
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr auto operator+(const UnitMultiple<kBase, kExponent1>& lhs,
                         const UnitMultiple<kBase, kExponent2>& rhs) noexcept
    -> UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)>;

//! Perform subtraction operation on two values in the smaller exponent of the unit
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr auto operator-(const UnitMultiple<kBase, kExponent1>& lhs,
                         const UnitMultiple<kBase, kExponent2>& rhs) noexcept
    -> UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)>;

//! Perform multiplication operation on two values in the smaller exponent of the unit
template <int64b kBase, int64b kExponent, Integer Int>
constexpr auto operator*(const UnitMultiple<kBase, kExponent>& lhs,
                         const Int rhs) noexcept
    -> UnitMultiple<kBase, kExponent>;

//! Perform multiplication operation on two values in the smaller exponent of the unit
template <Integer Int, int64b kBase, int64b kExponent>
constexpr auto operator*(const Int lhs,
                         const UnitMultiple<kBase, kExponent>& rhs) noexcept
    -> UnitMultiple<kBase, kExponent>;

//! Perform division operation on two values in the smaller exponent of the unit
template <int64b kBase, int64b kExponent, Integer Int>
constexpr auto operator/(const UnitMultiple<kBase, kExponent>& lhs,
                         const Int rhs) noexcept
    -> UnitMultiple<kBase, kExponent>;

//! Check if the two values are equal
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr auto operator==(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept -> bool;

//! Check if the two values are equal
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr auto operator!=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept -> bool;

//!
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr auto operator<(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept -> bool;

//!
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr auto operator<=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept -> bool;

//!
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr auto operator>(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept -> bool;

//!
template <int64b kBase, int64b kExponent1, int64b kExponent2>
constexpr auto operator>=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept -> bool;

#endif // Z_MSVC

} // namespace zisc

#include "unit_multiple-inl.hpp"

#endif // ZISC_UNIT_MULTIPLE_HPP
