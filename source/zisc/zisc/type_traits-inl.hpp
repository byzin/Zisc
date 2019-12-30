/*!
  \file type_traits-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_TYPE_TRAITS_INL_HPP
#define ZISC_TYPE_TRAITS_INL_HPP

#include "type_traits.hpp"
// Standard C++ library
#include <type_traits>
#include <iterator>
#include <string>
// Zisc
#include "utility.hpp"

namespace zisc {

namespace inner {

//! Check if Type is boolean type
template <typename Type>
using IsBoolean = std::is_same<bool, std::remove_cv_t<Type>>;

//! Check if Type is bloating point type
template <typename Type>
using IsFloat = std::is_floating_point<std::remove_cv_t<Type>>;

//! Check if Type is integer type
template <typename Type>
struct IsInteger
{
  static constexpr bool value = std::is_integral_v<std::remove_cv_t<Type>> && 
                                !IsBoolean<Type>::value;
};

//! Check if Type is unsigned integer type
template <typename Type>
struct IsUnsignedInteger
{
  static constexpr bool value = IsInteger<Type>::value && 
                                std::is_unsigned_v<std::remove_cv_t<Type>>;
};

//! Check if Type is signed integer type
template <typename Type>
struct IsSignedInteger
{
  static constexpr bool value = IsInteger<Type>::value && 
                                !IsUnsignedInteger<Type>::value;
};

template <typename Type>
struct IteratorTrue : std::true_type {};

//!
struct IsIteratorTester
{
  template <typename Type>
  static IteratorTrue<typename std::iterator_traits<Type>::iterator_category> test(int);

  template <typename Type>
  static std::false_type test(...);

  static constexpr int kValue = 0;
};

//! Check if Type is iterator type
template <typename Type>
struct IsIterator : decltype(IsIteratorTester::test<Type>(IsIteratorTester::kValue)) {};

//! Check if Type is iterator type
template <>
struct IsIterator<void> : std::false_type {};

//! Check if Type is iterator type
template <>
struct IsIterator<void*> : std::false_type {};

template <typename Type>
struct RandomAccessIteratorTrue : std::false_type {};

template <>
struct RandomAccessIteratorTrue<std::random_access_iterator_tag> : std::true_type {};

//!
struct IsRandomAccessIteratorTester
{
  template <typename Type>
  static RandomAccessIteratorTrue<typename std::iterator_traits<Type>::
      iterator_category> test(int);

  template <typename Type>
  static std::false_type test(...);

  static constexpr int kValue = 0;
};

//! Check if Type is iterator type
template <typename Type>
struct IsRandomAccessIterator : 
    decltype(IsRandomAccessIteratorTester::test<Type>(IsRandomAccessIteratorTester::
        kValue)) {};

//! Check if Type is iterator type
template <>
struct IsRandomAccessIterator<void> : std::false_type {};

//! Check if Type is iterator type
template <>
struct IsRandomAccessIterator<void*> : std::false_type {};

//! Check if Type is boolean type
template <typename Type>
using IsStdString = std::is_same<std::string, std::remove_cv_t<Type>>;

} // namespace inner

} // namespace zisc

#endif // ZISC_TYPE_TRAITS_INL_HPP
