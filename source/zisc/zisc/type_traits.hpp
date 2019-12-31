/*!
  \file type_traits.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_TYPE_TRAITS_HPP
#define ZISC_TYPE_TRAITS_HPP

// Standard C++ library
#include <type_traits>

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class TypeTraits
{
 public:
  static constexpr void* kEnabler = nullptr;


  //! Check if the given type \a Type is boolean type
  template <typename Type>
  static constexpr bool isBoolean() noexcept;

  //! Check if the given type \a Type is floating-point type
  template <typename Type>
  static constexpr bool isFloat() noexcept;

  //! Check if the given type \a Type is integer type
  template <typename Type>
  static constexpr bool isInteger() noexcept;

  //! Check if the given type \a Type is unsigned integer type
  template <typename Type>
  static constexpr bool isUnsignedInteger() noexcept;

  //! Check if the given type \a Type is signed integer type
  template <typename Type>
  static constexpr bool isSignedInteger() noexcept;

  //! Check if the given type \a Type is string type
  template <typename Type>
  static constexpr bool isString() noexcept;


  template <bool Flag, typename Type = void* const>
  using EnableIf = typename std::enable_if<Flag, Type>::type;

  template <typename T, typename Type = void* const>
  using EnableIfBoolean = EnableIf<isBoolean<T>(), Type>;

  template <typename T, typename Type = void* const>
  using EnableIfFloat = EnableIf<isFloat<T>(), Type>;

  template <typename T, typename Type = void* const>
  using EnableIfInteger = EnableIf<isInteger<T>(), Type>;

  template <typename T, typename Type = void* const>
  using EnableIfUnsignedInteger = EnableIf<isUnsignedInteger<T>(), Type>;

  template <typename T, typename Type = void* const>
  using EnableIfSignedInteger = EnableIf<isSignedInteger<T>(), Type>;

  template <typename T, typename U, typename Type = void* const>
  using EnableIfSame = EnableIf<std::is_same<T, U>::value, Type>;

  template <typename B, typename D, typename Type = void* const>
  using EnableIfBaseOf = EnableIf<std::is_base_of<B, D>::value, Type>;

  template <typename T, typename... Args>
  using EnableIfConstructible = EnableIf<std::is_constructible_v<T, Args...>>;

 private:
  //! Check if the given type \a Type is string type
  template <typename CharT, typename Type>
  static constexpr bool isStringImpl() noexcept;
};

// Type properties

//! Check if Type is boolean type
template <typename Type>
constexpr bool kIsBoolean = TypeTraits::isBoolean<Type>();

//! Check if Type is bloating point type
template <typename Type>
constexpr bool kIsFloat = TypeTraits::isFloat<Type>();

//! Check if Type is integer type
template <typename Type>
constexpr bool kIsInteger = TypeTraits::isInteger<Type>();

//! Check if Type is unsigned integer type
template <typename Type>
constexpr bool kIsUnsignedInteger = TypeTraits::isUnsignedInteger<Type>();

//! Check if Type is signed integer type
template <typename Type>
constexpr bool kIsSignedInteger = TypeTraits::isSignedInteger<Type>();

//! Check if the Type is std::string
template <typename Type>
constexpr bool kIsString = TypeTraits::isString<Type>();

// SFINAE

constexpr void* kEnabler = TypeTraits::kEnabler;

template <bool Flag, typename Type = void* const>
using EnableIf = TypeTraits::EnableIf<Flag, Type>;

template <typename T, typename Type = void* const>
using EnableIfBoolean = TypeTraits::EnableIfBoolean<T, Type>;

template <typename T, typename Type = void* const>
using EnableIfFloat = TypeTraits::EnableIfFloat<T, Type>;

template <typename T, typename Type = void* const>
using EnableIfInteger = TypeTraits::EnableIfInteger<T, Type>;

template <typename T, typename Type = void* const>
using EnableIfUnsignedInteger = TypeTraits::EnableIfUnsignedInteger<T, Type>;

template <typename T, typename Type = void* const>
using EnableIfSignedInteger = TypeTraits::EnableIfSignedInteger<T, Type>;

template <typename T, typename U, typename Type = void* const>
using EnableIfSame = TypeTraits::EnableIfSame<T, U, Type>;

template <typename B, typename D, typename Type = void* const>
using EnableIfBaseOf = TypeTraits::EnableIfBaseOf<B, D, Type>;

template <typename T, typename... Args>
using EnableIfConstructible = TypeTraits::EnableIfConstructible<T, Args...>;

} // namespace zisc

#include "type_traits-inl.hpp"


#endif // ZISC_TYPE_TRAITS_HPP
