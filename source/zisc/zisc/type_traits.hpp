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
#include <string>
#include <string_view>

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class TypeTraits
{
  //! A type without const volatile qualifiers
  template <typename Type>
  using RawType = std::remove_cv_t<Type>;

  //! Check if the given type \a Type is string type
  template <typename CharType, typename Type>
  static constexpr bool kIsStringImpl =
      std::is_same_v<std::basic_string<CharType>, RawType<Type>> ||
      std::is_same_v<std::basic_string_view<CharType>, RawType<Type>>;

 public:
  static constexpr void* kEnabler = nullptr;

  //! Check if the given type \a Type is boolean type
  template <typename Type>
  static constexpr bool kIsBoolean = std::is_same_v<bool, RawType<Type>>;

  //! Check if the given type \a Type is floating-point type
  template <typename Type>
  static constexpr bool kIsFloat = std::is_floating_point_v<Type>;

  //! Check if the given type \a Type is integer type
  template <typename Type>
  static constexpr bool kIsInteger = !kIsBoolean<Type> &&
                                     std::is_integral_v<Type>;

  //! Check if the given type \a Type is unsigned integer type
  template <typename Type>
  static constexpr bool kIsUnsignedInteger = std::is_unsigned_v<Type> &&
                                             kIsInteger<Type>;

  //! Check if the given type \a Type is signed integer type
  template <typename Type>
  static constexpr bool kIsSignedInteger = std::is_signed_v<Type> &&
                                           kIsInteger<Type>;

  //! Check if the given type \a Type is string type
  template <typename Type>
  static constexpr bool kIsString = kIsStringImpl<char, Type> ||
                                    kIsStringImpl<wchar_t, Type> ||
                                    kIsStringImpl<char16_t, Type> ||
                                    kIsStringImpl<char32_t, Type>;


  template <bool Flag, typename Type = void* const>
  using EnableIf = typename std::enable_if<Flag, Type>::type;

  template <typename T, typename Type = void* const>
  using EnableIfBoolean = EnableIf<kIsBoolean<T>, Type>;

  template <typename T, typename Type = void* const>
  using EnableIfFloat = EnableIf<kIsFloat<T>, Type>;

  template <typename T, typename Type = void* const>
  using EnableIfInteger = EnableIf<kIsInteger<T>, Type>;

  template <typename T, typename Type = void* const>
  using EnableIfUnsignedInteger = EnableIf<kIsUnsignedInteger<T>, Type>;

  template <typename T, typename Type = void* const>
  using EnableIfSignedInteger = EnableIf<kIsSignedInteger<T>, Type>;

  template <typename T, typename U, typename Type = void* const>
  using EnableIfSame = EnableIf<std::is_same<T, U>::value, Type>;

  template <typename B, typename D, typename Type = void* const>
  using EnableIfBaseOf = EnableIf<std::is_base_of<B, D>::value, Type>;

  template <typename T, typename... Args>
  using EnableIfConstructible = EnableIf<std::is_constructible_v<T, Args...>>;
};

// Type properties

//! Check if Type is boolean type
template <typename Type>
constexpr bool kIsBoolean = TypeTraits::kIsBoolean<Type>;

//! Check if Type is bloating point type
template <typename Type>
constexpr bool kIsFloat = TypeTraits::kIsFloat<Type>;

//! Check if Type is integer type
template <typename Type>
constexpr bool kIsInteger = TypeTraits::kIsInteger<Type>;

//! Check if Type is unsigned integer type
template <typename Type>
constexpr bool kIsUnsignedInteger = TypeTraits::kIsUnsignedInteger<Type>;

//! Check if Type is signed integer type
template <typename Type>
constexpr bool kIsSignedInteger = TypeTraits::kIsSignedInteger<Type>;

//! Check if the Type is std::string
template <typename Type>
constexpr bool kIsString = TypeTraits::kIsString<Type>;

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
