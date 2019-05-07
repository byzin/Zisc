/*!
  \file unit_multiple-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIT_MULTIPLE_INL_HPP
#define ZISC_UNIT_MULTIPLE_INL_HPP

#include "unit_multiple.hpp"
// Standard C++ library
#include <cstdint>
// Zisc
#include "fraction.hpp"
#include "math.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple() noexcept : value_{}
{
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple(
    const ArithmeticType value) noexcept :
        value_{value}
{
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple(
    const FractionType& value) noexcept :
        value_{value}
{
}

/*!
  */
template <int64b kBase, int64b kExponent> template <int64b kOtherExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple(
    const UnitMultiple<kBase, kOtherExponent>& other) noexcept :
        value_{(other.template representedAs<exponent()>()).value()}
{
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::operator=(
    const ArithmeticType value) noexcept -> UnitMultiple&
{
  setValue(value);
  return *this;
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::operator=(
    const FractionType& value) noexcept -> UnitMultiple&
{
  setValue(value);
  return *this;
}

/*!
  */
template <int64b kBase, int64b kExponent> template <int64b kOtherExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::operator=(
    const UnitMultiple<kBase, kOtherExponent>& other) noexcept -> UnitMultiple&
{
  setValue(other);
  return *this;
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::base() noexcept -> ArithmeticType
{
  return kBase;
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::exponent() noexcept -> ArithmeticType
{
  return kExponent;
}

/*!
  */
template <int64b kBase, int64b kExponent> template <int64b kToExponent> inline
constexpr UnitMultiple<kBase, kToExponent> UnitMultiple<kBase, kExponent>::
    representedAs() const noexcept
{
  auto v = value();
  constexpr auto exponent_diff = kToExponent - exponent();
  if constexpr (exponent_diff != 0) {
    constexpr auto k = (0 < exponent_diff)
        ? FractionType{1, zisc::power<exponent_diff>(base())}
        : FractionType{zisc::power<-exponent_diff>(base()), 1};
    v = k * v;
  }
  return UnitMultiple<kBase, kToExponent>{v};
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::setValue(
    const ArithmeticType value) noexcept
{
  value_ = value;
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::setValue(
    const FractionType& value) noexcept
{
  value_ = value;
}

/*!
  */
template <int64b kBase, int64b kExponent> template <int64b kOtherExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::setValue(
    const UnitMultiple<kBase, kOtherExponent>& other) noexcept
{
  value_ = (other.template representedAs<kExponent>()).value();
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::value() noexcept
    -> FractionType&
{
  return value_;
}

/*!
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::value() const noexcept
    -> const FractionType&
{
  return value_;
}

/*!
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr UnitMultiple<kBase, std::min(kExponent1, kExponent2)> operator+(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, std::min(kExponent1, kExponent2)>;
  const auto result = Multiple{lhs}.value() + Multiple{rhs}.value();
  return Multiple{result};
}

/*!
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr UnitMultiple<kBase, std::min(kExponent1, kExponent2)> operator-(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, std::min(kExponent1, kExponent2)>;
  const auto result = Multiple{lhs}.value() - Multiple{rhs}.value();
  return Multiple{result};
}

/*!
  */
template <int64b kBase, int64b kExponent, typename Integer> inline
constexpr UnitMultiple<kBase, kExponent> operator*(
    const UnitMultiple<kBase, kExponent>& lhs,
    const Integer rhs) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  using Multiple = UnitMultiple<kBase, kExponent>;
  const auto result = lhs.value() *
      typename Multiple::FractionType{cast<typename Multiple::ArithmeticType>(rhs)};
  return Multiple{result};
}

/*!
  */
template <typename Integer, int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent> operator*(
    const Integer lhs,
    const UnitMultiple<kBase, kExponent>& rhs) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  return rhs * lhs;
}

/*!
  */
template <int64b kBase, int64b kExponent, typename Integer> inline
constexpr UnitMultiple<kBase, kExponent> operator/(
    const UnitMultiple<kBase, kExponent>& lhs,
    const Integer rhs) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  using Multiple = UnitMultiple<kBase, kExponent>;
  const auto result = lhs.value() /
      typename Multiple::FractionType{cast<typename Multiple::ArithmeticType>(rhs)};
  return Multiple{result};
}

/*!
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator==(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, std::min(kExponent1, kExponent2)>;
  const auto result = Multiple{lhs}.value() == Multiple{rhs}.value();
  return result;
}

/*!
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator!=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator<(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, std::min(kExponent1, kExponent2)>;
  const auto result = Multiple{lhs}.value() < Multiple{rhs}.value();
  return result;
}

/*!
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator<=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, std::min(kExponent1, kExponent2)>;
  const auto result = Multiple{lhs}.value() <= Multiple{rhs}.value();
  return result;
}

/*!
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator>(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  return rhs < lhs;
}

/*!
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator>=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  return rhs <= lhs;
}

} // namespace zisc

#endif // ZISC_UNIT_MULTIPLE_INL_HPP
