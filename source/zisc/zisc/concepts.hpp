/*!
  \file concepts.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONCEPTS_HPP
#define ZISC_CONCEPTS_HPP

#include "concepts-inl.hpp"

namespace zisc {

// Type concepts

//! Specify a type is the same as another type
template <typename T, typename U>
concept SameAs = inner::SameAs<T, U>;

//! Specify a type is derived from another type
template <typename Derived, typename Base>
concept DerivedFrom = inner::DerivedFrom<Derived, Base>;

//! Specify a type is an integer type
template <typename Type>
concept Integer = inner::Integer<Type>;

//! Specify a type is an integer type which is signed
template <typename Type>
concept SignedInteger = inner::SignedInteger<Type>;

//! Specify a type is an integer type which is unsigned
template <typename Type>
concept UnsignedInteger = inner::UnsignedInteger<Type>;

//! Specify a type is a floating-point type
template <typename Type>
concept FloatingPoint = inner::FloatingPoint<Type>;

// Callable concepts

//! Specify a callable type can be invoked with a given set of argument
template <typename Func, typename ...Args>
concept Invocable = inner::Invocable<Func, Args...>;

} // namespace zisc

#endif // ZISC_CONCEPTS_HPP
