/*!
  \file type_traits.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_TYPE_TRAITS_HPP
#define ZISC_TYPE_TRAITS_HPP

// Standard C++ library
#include <functional>
#include <string>
#include <type_traits>

#include "type_traits-inl.hpp"

namespace zisc {

// Type properties

//! Check if Type is boolean type
template <typename Type>
constexpr bool kIsBoolean = zisc_type_traits::IsBoolean<Type>::value;

//! Check if Type is bloating point type
template <typename Type>
constexpr bool kIsFloat = zisc_type_traits::IsFloat<Type>::value;

//! Check if Type is integer type
template <typename Type>
constexpr bool kIsInteger = zisc_type_traits::IsInteger<Type>::value;

//! Check if Type is unsigned integer type
template <typename Type>
constexpr bool kIsUnsignedInteger = zisc_type_traits::IsUnsignedInteger<Type>::value;

//! Check if Type is signed integer type
template <typename Type>
constexpr bool kIsSignedInteger = zisc_type_traits::IsSignedInteger<Type>::value;

template <typename Type>
constexpr bool kIsIterator = zisc_type_traits::IsIterator<Type>::value;

template <typename Type>
constexpr bool kIsRandomAccessIterator = 
    zisc_type_traits::IsRandomAccessIterator<Type>::value;

// SFINAE

constexpr void* kEnabler = nullptr;

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

template <typename T, typename Type = void* const>
using EnableIfIterator = EnableIf<kIsIterator<T>, Type>;

template <typename T, typename Type = void* const>
using EnableIfRandomAccessIterator = EnableIf<kIsRandomAccessIterator<T>, Type>;

template <typename T, typename... Args>
using EnableIfConstructible = EnableIf<std::is_constructible<T, Args...>::value>;

} // namespace zisc

#endif // ZISC_TYPE_TRAITS_HPP
