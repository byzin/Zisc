/*!
  \file concepts.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
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
#include <string>
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

//! Specify that a type is implicitly convertible to another type
template <typename From, typename To>
concept ConvertibleTo = std::is_convertible_v<From, To> &&
                        requires (std::add_rvalue_reference_t<From> (&f)()) {
                            static_cast<To>(f());
                        };

//! Specify a type is a character type
template <typename Type>
concept Character = std::is_same_v<char, std::remove_cv_t<Type>> ||
                    std::is_same_v<wchar_t, std::remove_cv_t<Type>> ||
                    std::is_same_v<char8_t, std::remove_cv_t<Type>> ||
                    std::is_same_v<char16_t, std::remove_cv_t<Type>> ||
                    std::is_same_v<char32_t, std::remove_cv_t<Type>>;

//! Specify a type is a character pointer type
template <typename Type>
concept CharPointer = std::is_pointer_v<Type> &&
                      Character<std::remove_pointer_t<Type>>;

//! Specify a type is a std::basic_string type
template <typename Type>
concept StdString = requires (Type) {
  std::is_same_v<std::basic_string<typename Type::value_type,
                                   typename Type::traits_type,
                                   typename Type::allocator_type>,
                 std::remove_cv_t<Type>>;
};

//! Specify a type is a std::basic_string_view type
template <typename Type>
concept StdStringView = requires (Type) {
  std::is_same_v<std::basic_string_view<typename Type::value_type,
                                        typename Type::traits_type>,
                 std::remove_cv_t<Type>>;
};

//! Specify a type is a string type
template <typename Type>
concept String = CharPointer<Type> || StdString<Type> || StdStringView<Type>;

//! Specify a type is an integer type
template <typename Type>
concept Integer = std::is_integral_v<Type> && !SameAs<bool, Type>;

//! Specify a type is a 8bit integer type
template <typename Type>
concept Integer8 = Integer<Type> && (sizeof(Type) == 1);

//! Specify a type is a 16bit integer type
template <typename Type>
concept Integer16 = Integer<Type> && (sizeof(Type) == 2);

//! Specify a type is a 32bit integer type
template <typename Type>
concept Integer32 = Integer<Type> && (sizeof(Type) == 4);

//! Specify a type is a 64bit integer type
template <typename Type>
concept Integer64 = Integer<Type> && (sizeof(Type) == 8);

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

// Comparison concepts

// Exposition only
template <typename B>
concept BooleanTestableImpl = ConvertibleTo<B, bool>;

//! Specify that a type can be used in Boolean contexts
template <typename B>
concept BooleanTestable = BooleanTestableImpl<B> &&
                          requires (B&& b) {
                              {!std::forward<B>(b)} -> BooleanTestableImpl;
                          };

// Exposition only
template <typename T, typename U>
concept WeaklyEqualityComparableWith = requires (std::remove_reference_t<T>& t,
                                                 std::remove_reference_t<U>& u) {
  {t == u} -> BooleanTestable;
  {t != u} -> BooleanTestable;
  {u == t} -> BooleanTestable;
  {u != t} -> BooleanTestable;
};

//! Specify that operator '==' is an equivalence relation
template <typename T>
concept EqualityComparable = WeaklyEqualityComparableWith<T, T>;

// Callable concepts

//! Specify a callable type can be invoked with a given set of argument
template <typename Func, typename ...Args>
concept Invocable = std::is_invocable_v<Func, Args...>;

//! Specify a callable type can be invoked with a given set of argument
template <typename Func, typename Return, typename ...Args>
concept InvocableR = std::is_invocable_r_v<Return, Func, Args...>;

} // namespace zisc

#endif // ZISC_CONCEPTS_HPP
