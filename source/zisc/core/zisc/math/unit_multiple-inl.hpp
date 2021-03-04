/*!
  \file unit_multiple-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIT_MULTIPLE_INL_HPP
#define ZISC_UNIT_MULTIPLE_INL_HPP

#include "unit_multiple.hpp"
// Standard C++ library
#include <algorithm>
#include <cstdint>
// Zisc
#include "fraction.hpp"
#include "math.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple() noexcept :
    value_{}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple(const Type value) noexcept :
    value_{value}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent>::UnitMultiple(const FractionType& value) noexcept :
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
        value_{(other.template cast<exponent()>()).value()}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::operator=(const Type value) noexcept
    -> UnitMultiple&
{
  set(value);
  return *this;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::operator=(const FractionType& value) noexcept
    -> UnitMultiple&
{
  set(value);
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
  set(other);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::base() noexcept -> Type
{
  return kBase;
}

/*!
  \details No detailed description

  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::exponent() noexcept -> Type
{
  return kExponent;
}

/*!
  \details No detailed description

  \tparam kToExponent No description.
  \return No description
  */
template <int64b kBase, int64b kExponent> template <int64b kToExponent> inline
constexpr UnitMultiple<kBase, kToExponent> UnitMultiple<kBase, kExponent>::cast() const noexcept
{
  auto v = value();
  constexpr auto exponent_diff = kToExponent - exponent();
  if constexpr (exponent_diff != 0) {
    constexpr auto k = (0 < exponent_diff)
        ? FractionType{1, zisc::pow(base(), exponent_diff)}
        : FractionType{zisc::pow(base(), -exponent_diff), 1};
    v = k * v;
  }
  return UnitMultiple<kBase, kToExponent>{v};
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <int64b kBase, int64b kExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::set(const Type value) noexcept
{
  value_ = value;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <int64b kBase, int64b kExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::set(const FractionType& value) noexcept
{
  value_ = value;
}

/*!
  \details No detailed description

  \tparam kOtherExponent No description.
  \param [in] other No description.
  */
template <int64b kBase, int64b kExponent> template <int64b kOtherExponent> inline
constexpr void UnitMultiple<kBase, kExponent>::set(
    const UnitMultiple<kBase, kOtherExponent>& other) noexcept
{
  value_ = (other.template cast<kExponent>()).value();
}

/*!
  \details No detailed description

  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::value() noexcept -> FractionType&
{
  return value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <int64b kBase, int64b kExponent> inline
constexpr auto UnitMultiple<kBase, kExponent>::value() const noexcept -> const FractionType&
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
constexpr UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)> operator+(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)>;
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
constexpr UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)> operator-(
    const UnitMultiple<kBase, kExponent1>& lhs,
    const UnitMultiple<kBase, kExponent2>& rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)>;
  const auto result = Multiple{lhs}.value() - Multiple{rhs}.value();
  return Multiple{result};
}

/*!
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent No description.
  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <int64b kBase, int64b kExponent, Integer Int> inline
constexpr UnitMultiple<kBase, kExponent> operator*(
    const UnitMultiple<kBase, kExponent>& lhs,
    const Int rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, kExponent>;
  using FracType = typename Multiple::FractionType;
  const auto result = lhs.value() * FracType{cast<typename FracType::Type>(rhs)};
  return Multiple{result};
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam kBase No description.
  \tparam kExponent No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Integer Int, int64b kBase, int64b kExponent> inline
constexpr UnitMultiple<kBase, kExponent> operator*(
    const Int lhs,
    const UnitMultiple<kBase, kExponent>& rhs) noexcept
{
  return rhs * lhs;
}

/*!
  \details No detailed description

  \tparam kBase No description.
  \tparam kExponent No description.
  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <int64b kBase, int64b kExponent, Integer Int> inline
constexpr UnitMultiple<kBase, kExponent> operator/(
    const UnitMultiple<kBase, kExponent>& lhs,
    const Int rhs) noexcept
{
  using Multiple = UnitMultiple<kBase, kExponent>;
  using FracType = typename Multiple::FractionType;
  const auto result = lhs.value() / FracType{cast<typename FracType::Type>(rhs)};
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
  using Multiple = UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)>;
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
  using Multiple = UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)>;
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
  using Multiple = UnitMultiple<kBase, (std::min)(kExponent1, kExponent2)>;
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
