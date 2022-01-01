/*!
  \file query_value-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_QUERY_VALUE_INL_HPP
#define ZISC_QUERY_VALUE_INL_HPP

#include "query_value.hpp"
// Standard C++ library
#include <limits>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <UnsignedInteger T> inline
QueryValue<T>::QueryValue() noexcept : value_{invalidValue()}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <UnsignedInteger T> inline
QueryValue<T>::QueryValue(ConstType value) noexcept : value_{value}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <UnsignedInteger T> inline
auto QueryValue<T>::operator=(ConstType value) noexcept -> QueryValue&
{
  value_ = value;
  return *this;
}

/*!
  \details No detailed description
  */
template <UnsignedInteger T> inline
void QueryValue<T>::clear() noexcept
{
  value_ = invalidValue();
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger T> inline
auto QueryValue<T>::get() noexcept -> Reference
{
  return value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger T> inline
auto QueryValue<T>::get() const noexcept -> ConstReference
{
  return value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger T> inline
constexpr auto QueryValue<T>::invalidValue() noexcept -> Type
{
  constexpr auto invalid = (std::numeric_limits<Type>::max)();
  return invalid;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger T> inline
bool QueryValue<T>::isValid() const noexcept
{
  const bool result = get() != invalidValue();
  return result;
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <UnsignedInteger T> inline
bool operator==(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept
{
  const bool result = lhs.get() == rhs.get();
  return result;
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <UnsignedInteger T> inline
bool operator!=(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept
{
  const bool result = lhs.get() != rhs.get();
  return result;
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <UnsignedInteger T> inline
bool operator<(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept
{
  const bool result = lhs.get() < rhs.get();
  return result;
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <UnsignedInteger T> inline
bool operator<=(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept
{
  const bool result = lhs.get() <= rhs.get();
  return result;
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <UnsignedInteger T> inline
bool operator>(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept
{
  const bool result = lhs.get() > rhs.get();
  return result;
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <UnsignedInteger T> inline
bool operator>=(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept
{
  const bool result = lhs.get() >= rhs.get();
  return result;
}

} /* namespace zisc */

#endif /* ZISC_QUERY_VALUE_INL_HPP */
