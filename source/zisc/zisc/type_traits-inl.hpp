/*!
  \file type_traits-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_TYPE_TRAITS_INL_HPP
#define ZISC_TYPE_TRAITS_INL_HPP

#include "type_traits.hpp"
// Standard C++ library
#include <type_traits>
#include <string>
#include <string_view>

namespace zisc {

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
constexpr bool TypeTraits::isBoolean() noexcept
{
  using BType = std::remove_cv_t<Type>;
  const bool result = std::is_same_v<bool, BType>;
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
constexpr bool TypeTraits::isFloat() noexcept
{
  const bool result = std::is_floating_point_v<Type>;
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
constexpr bool TypeTraits::isInteger() noexcept
{
  const bool result = !isBoolean<Type>() && std::is_integral_v<Type>;
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
constexpr bool TypeTraits::isUnsignedInteger() noexcept
{
  const bool result = std::is_unsigned_v<Type> && isInteger<Type>();
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
constexpr bool TypeTraits::isSignedInteger() noexcept
{
  const bool result = std::is_signed_v<Type> && isInteger<Type>();
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
constexpr bool TypeTraits::isString() noexcept
{
  const bool result = isStringImpl<char, Type>() ||
                      isStringImpl<wchar_t, Type>() ||
                      isStringImpl<char16_t, Type>() ||
                      isStringImpl<char32_t, Type>();
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename CharT, typename Type> inline
constexpr bool TypeTraits::isStringImpl() noexcept
{
  using SType = std::remove_cv_t<Type>;
  const bool result = std::is_same_v<std::basic_string<CharT>, SType> ||
                      std::is_same_v<std::basic_string_view<CharT>, SType>;
  return result;
}

} // namespace zisc

#endif // ZISC_TYPE_TRAITS_INL_HPP
