/*!
  \file concepts-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONCEPTS_INL_HPP
#define ZISC_CONCEPTS_INL_HPP

// Check compiler
#if !defined(__cpp_concepts)
static_assert(false, "'Concepts' isn't supported in the compiler.")
#endif

// Standard C++ library
#include <functional>
#include <type_traits>
#include <utility>

namespace zisc {

namespace inner {

// Type concepts

//! Specify a type is the same as another type
template <typename T, typename U>
concept SameAs = std::is_same_v<U, T>;

//! Specify a type is derived from another type
template <typename Derived, typename Base>
concept DerivedFrom = std::is_base_of_v<Base, Derived>;

//! Specify a type is an integer type
template <typename Type>
concept Integer = std::is_integral_v<Type> && !SameAs<bool, Type>;

//! Specify a type is an integer type which is signed
template <typename Type>
concept SignedInteger = Integer<Type> && std::is_signed_v<Type>;

//! Specify a type is an integer type which is unsigned
template <typename Type>
concept UnsignedInteger = Integer<Type> && std::is_unsigned_v<Type>;

//! Specify a type is a floating-point type
template <typename Type>
concept FloatingPoint = std::is_floating_point_v<Type>;

// Callable concepts

//! Specify a callable type can be invoked with a given set of argument
template <typename Func, typename ...Args>
concept Invocable = requires(Func&& func, Args&&... args) {
  std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
};

} // namespace inner

} // namespace zisc

#include "concepts.hpp"

#endif // ZISC_CONCEPTS_INL_HPP
