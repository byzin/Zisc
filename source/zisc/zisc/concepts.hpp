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

// Check compiler
#if !defined(__cpp_concepts)
static_assert(false, "'Concepts' isn't supported in the compiler.")
#endif

// Standard C++ library
#include <functional>
#include <type_traits>
#include <utility>

namespace zisc {

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

//! Specify a type is a pointer
template <typename Type>
concept Pointer = std::is_pointer_v<Type>;

//! Specify a type is arithmetic
template <typename Type>
concept Arithmetic = std::is_arithmetic_v<Type>;

//! Specify a type is scalar
template <typename Type>
concept Scalar = std::is_scalar_v<Type>;

//! Specify a type is trivial
template <typename Type>
concept Trivial = std::is_trivial_v<Type>;

//! Specify a type is trivially copyable
template <typename Type>
concept TriviallyCopyable = std::is_trivially_copyable_v<Type>;

//! Specify a type is default constructible
template <typename Type>
concept DefaultConstructible = std::is_default_constructible_v<Type>;

//! Specify a type is copy constructible
template <typename Type>
concept CopyConstructible = std::is_copy_constructible_v<Type>;

//! Specify a type is move constructible
template <typename Type>
concept MoveConstructible = std::is_move_constructible_v<Type>;

//! Specify a type is copy assignable 
template <typename Type>
concept CopyAssignable = std::is_copy_assignable_v<Type>;

//! Specify a type is move assignable 
template <typename Type>
concept MoveAssignable = std::is_move_assignable_v<Type>;

//! Specify a type is reference type
template <typename Type>
concept Reference = std::is_reference_v<Type>;

//! Specify a type is non-reference type
template <typename Type>
concept NonReference = !std::is_reference_v<Type>;

// Callable concepts

//! Specify a callable type can be invoked with a given set of argument
template <typename Func, typename ...Args>
concept Invocable = std::is_invocable_v<Func, Args...>;

//! Specify a callable type can be invoked with a given set of argument
template <typename Func, typename Return, typename ...Args>
concept InvocableR = std::is_invocable_r_v<Return, Func, Args...>;

} // namespace zisc

#endif // ZISC_CONCEPTS_HPP
