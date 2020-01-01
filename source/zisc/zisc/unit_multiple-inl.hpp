/*!
  \file unit_multiple-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
  \details No detailed description
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple() noexcept : value_{}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple(
    const ArithmeticType value) noexcept :
        value_{value}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple(
    const FractionType& value) noexcept :
        value_{value}
{
}

/*!
  \details No detailed description

  \tparam kOtherExponent No description.
  \param [in] other No description.
  */
template <int64b kBase, int64b kExponent> template <int64b kOtherExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple(
    const UnitMultiple<kBase, kOtherExponent>& other) noexcept :
        value_{(other.template representedAs<exponent()>()).value()}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::operator=(
    const ArithmeticType value) noexcept -> UnitMultiple&
{
  setValue(value);
  return *this;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::operator=(
    const FractionType& value) noexcept -> UnitMultiple&
{
  setValue(value);
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <int64b kBase, int64b kExponent> template <int64b kOtherExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::operator=(
    const UnitMultiple<kBase, kOtherExponent>& other) noexcept -> UnitMultiple&
{
  setValue(other);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::base() noexcept -> ArithmeticType
{
  return kBase;
}

/*!
  \details No detailed description

  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::exponent() noexcept -> ArithmeticType
{
  return kExponent;
}

/*!
  \details No detailed description

  \tparam kToExponent No description.
  \return No description
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
  \details No detailed description

  \param [in] value No description.
  */
template <int64b kBase, int64b kExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::setValue(
    const ArithmeticType value) noexcept
{
  value_ = value;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <int64b kBase, int64b kExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::setValue(
    const FractionType& value) noexcept
{
  value_ = value;
}

/*!
  \details No detailed description

  \tparam kOtherExponent No description.
  \param [in] other No description.
  */
template <int64b kBase, int64b kExponent> template <int64b kOtherExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::setValue(
    const UnitMultiple<kBase, kOtherExponent>& other) noexcept
{
  value_ = (other.template representedAs<kExponent>()).value();
}

/*!
  \details No detailed description

  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::value() noexcept
    -> FractionType&
{
  return value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::value() const noexcept
    -> const FractionType&
{
  return value_;
}

/*!
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent1 No description.
  \tparam kExponent2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent1 No description.
  \tparam kExponent2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent No description.
  \tparam Integer No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <int64b kBase, int64b kExponent, typename Integer> inline
constexpr UnitMultiple<kBase, kExponent> operator*(
    const UnitMultiple<kBase, kExponent>& lhs,
    const Integer rhs) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  using Multiple = UnitMultiple<kBase, kExponent>;
  using FracType = typename Multiple::FractionType;
  const auto result = lhs.value() *
                      FracType{cast<typename FracType::ArithmeticType>(rhs)};
  return Multiple{result};
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kBase No description.
  \tparam kExponent No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent No description.
  \tparam Integer No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <int64b kBase, int64b kExponent, typename Integer> inline
constexpr UnitMultiple<kBase, kExponent> operator/(
    const UnitMultiple<kBase, kExponent>& lhs,
    const Integer rhs) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  using Multiple = UnitMultiple<kBase, kExponent>;
  using FracType = typename Multiple::FractionType;
  const auto result = lhs.value() /
                      FracType{cast<typename FracType::ArithmeticType>(rhs)};
  return Multiple{result};
}

/*!
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent1 No description.
  \tparam kExponent2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent1 No description.
  \tparam kExponent2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator!=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  const bool result = !(lhs == rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent1 No description.
  \tparam kExponent2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent1 No description.
  \tparam kExponent2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent1 No description.
  \tparam kExponent2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator>(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  const bool result = rhs < lhs;
  return result;
}

/*!
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent1 No description.
  \tparam kExponent2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <int64b kBase, int64b kExponent1, int64b kExponent2> inline
constexpr bool operator>=(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  const bool result = rhs <= lhs;
  return result;
}

} // namespace zisc

#endif // ZISC_UNIT_MULTIPLE_INL_HPP
