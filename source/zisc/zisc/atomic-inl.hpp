/*!
  \file atomic-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ATOMIC_INL_HPP
#define ZISC_ATOMIC_INL_HPP

#include "atomic.hpp"
// Standard C++ library
#include <cstddef>
#include <type_traits>

namespace zisc {

/*!
  */
template <typename Integer> inline
Integer Atomic::add(Integer* ptr, const Integer value) noexcept
{
  const Integer old = addImpl(ptr, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::sub(Integer* ptr, const Integer value) noexcept
{
  const Integer old = subImpl(ptr, value);
  return old;
}

/*!
  */
template <typename Type> inline
constexpr bool Atomic::isAlwaysLockFree() noexcept
{
  const bool flag = isAlwaysLockFreeImpl<Type>();
  return flag;
}

/*!
  */
template <typename Type> inline
bool Atomic::isLockFree() noexcept
{
  const bool flag = isLockFreeImpl<Type>();
  return flag;
}

#if defined(Z_GCC) || defined(Z_CLANG)

/*!
  */
template <typename Integer> inline
Integer Atomic::addImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  const auto old = __atomic_fetch_add(ptr, value, __ATOMIC_SEQ_CST);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::subImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  const auto old = __atomic_fetch_sub(ptr, value, __ATOMIC_SEQ_CST);
  return old;
}

/*!
  */
template <typename Type> inline
constexpr bool Atomic::isAlwaysLockFreeImpl() noexcept
{
  constexpr std::size_t size = sizeof(Type);
  const bool flag = __atomic_always_lock_free(size, nullptr);
  return flag;
}

/*!
  */
template <typename Type> inline
bool Atomic::isLockFreeImpl() noexcept
{
  constexpr std::size_t size = sizeof(Type);
  const bool flag = __atomic_is_lock_free(size, nullptr);
  return flag;
}

#elif defined(Z_MSVC)

#endif

} // namespace zisc

#endif // ZISC_ATOMIC_INL_HPP
