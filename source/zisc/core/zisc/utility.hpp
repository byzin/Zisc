/*!
  \file utility.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UTILITY_HPP
#define ZISC_UTILITY_HPP

// Standard C++ library
#include <concepts>
#include <type_traits>
// Zisc
#include "concepts.hpp"

namespace zisc {

//! Convert type from T to Type
template <typename Type, typename T>
requires std::is_nothrow_convertible_v<T, Type> ||
         std::is_nothrow_constructible_v<Type, T>
constexpr auto cast(T&& value) noexcept -> Type;

//! Map an integer value into a [0, 1) floating point value
template <std::floating_point Float, std::unsigned_integral Integer>
constexpr auto mapTo01(const Integer x) noexcept -> Float;

//! Convert to Type which refers to the same memory but is assumed to have distinct lifetime
template <Pointer Type, Pointer T>
[[nodiscard]]
auto reinterp(T p) noexcept -> Type;

// Conditional functions

//! Invoke the func with the args if the flag is true
template <typename ReturnT, typename Func, typename ...ArgTypes>
    requires std::is_nothrow_invocable_r_v<ReturnT, Func, ArgTypes...>
auto invokeIfTrue(const bool flag, ReturnT& result, Func&& func, ArgTypes&&... args) noexcept
    -> bool;

//! Invoke the func with the args if the flag is true
template <typename Func, typename ...ArgTypes>
    requires std::is_nothrow_invocable_v<Func, ArgTypes...>
auto invokeIfTrue(const bool flag, Func&& func, ArgTypes&&... args) noexcept
    -> bool;

//! Update the value in lhs with the rhs if the flag is true
template <typename Lhs, typename Rhs> requires std::is_nothrow_assignable_v<Lhs, Rhs>
auto updateIfTrue(const bool flag, Lhs&& lhs, Rhs&& rhs) noexcept
    -> bool;

} // namespace zisc

#include "utility-inl.hpp"

#endif // ZISC_UTILITY_HPP
