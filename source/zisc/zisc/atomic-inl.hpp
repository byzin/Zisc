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
#include <utility>
// Zisc
#include "zisc/utility.hpp"

namespace zisc {

/*!
  */
template <typename Integer> inline
Integer Atomic::add(Integer* ptr, const Integer value) noexcept
{
  const auto old = addImpl<Integer, implType()>(ptr, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::sub(Integer* ptr, const Integer value) noexcept
{
  const auto old = subImpl<Integer, implType()>(ptr, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::exchange(Integer* ptr, const Integer value) noexcept
{
  const auto old = exchangeImpl<Integer, implType()>(ptr, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::increment(Integer* ptr) noexcept
{
  const auto old = incrementImpl<Integer, implType()>(ptr);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::decrement(Integer* ptr) noexcept
{
  const auto old = decrementImpl<Integer, implType()>(ptr);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::compareAndExchange(Integer* ptr,
                                   const Integer cmp,
                                   const Integer value) noexcept
{
  const auto old = compareAndExchangeImpl<Integer, implType()>(ptr, cmp, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::min(Integer* ptr, const Integer value) noexcept
{
  const auto old = minImpl<Integer, implType()>(ptr, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::max(Integer* ptr, const Integer value) noexcept
{
  const auto old = maxImpl<Integer, implType()>(ptr, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::andBit(Integer* ptr, const Integer value) noexcept
{
  const auto old = andBitImpl<Integer, implType()>(ptr, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::orBit(Integer* ptr, const Integer value) noexcept
{
  const auto old = orBitImpl<Integer, implType()>(ptr, value);
  return old;
}

/*!
  */
template <typename Integer> inline
Integer Atomic::xorBit(Integer* ptr, const Integer value) noexcept
{
  const auto old = xorBitImpl<Integer, implType()>(ptr, value);
  return old;
}

/*!
  */
inline
constexpr auto Atomic::implType() noexcept -> ImplType
{
  ImplType type = ImplType::kGcc;
#if defined(Z_GCC)
  type = ImplType::kGcc;
#elif defined(Z_CLANG)
  type = ImplType::kClang;
#elif defined(Z_MSVC)
  type = ImplType::kMsvc;
#else
  static_assert(false, "Unsupported implementation.");
#endif
  return type;
}

/*!
  */
template <typename Type> inline
constexpr bool Atomic::isAlwaysLockFree() noexcept
{
  const bool flag = isAlwaysLockFreeImpl<Type, implType()>();
  return flag;
}

/*!
  */
template <typename Type> inline
bool Atomic::isLockFree() noexcept
{
  const bool flag = isLockFreeImpl<Type, implType()>();
  return flag;
}

/*!
  */
template <typename Integer, typename Function, typename ...Types> inline
Integer Atomic::perform(Integer* ptr,
                        Function&& expression,
                        Types&&... arguments) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  static_assert(std::is_invocable_r_v<Integer, Function, Integer, Types...>,
                "The Function isn't invocable.");
  auto old = *ptr;
  auto cmp = cast<Integer>(0);
  do {
    cmp = old;
    const auto value = expression(cmp, std::forward<Types>(arguments)...);
    old = compareAndExchange<Integer>(ptr, cmp, value);
  } while (old != cmp);
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::addImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    old = __atomic_fetch_add(ptr, value, __ATOMIC_SEQ_CST);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::subImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    old = __atomic_fetch_sub(ptr, value, __ATOMIC_SEQ_CST);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::exchangeImpl(Integer* ptr, Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    __atomic_exchange(ptr, &value, &old, __ATOMIC_SEQ_CST);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::incrementImpl(Integer* ptr) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    constexpr Integer v = cast<Integer>(1);
    old = addImpl<Integer, kImpl>(ptr, v);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::decrementImpl(Integer* ptr) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    constexpr Integer v = cast<Integer>(1);
    old = subImpl<Integer, kImpl>(ptr, v);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::compareAndExchangeImpl(Integer* ptr,
                                       Integer cmp,
                                       Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    constexpr auto mem_order = __ATOMIC_SEQ_CST;
    __atomic_compare_exchange(ptr, &cmp, &value, false, mem_order, mem_order);
  }
  return cmp;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::minImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  static_assert(sizeof(Integer) == 4, "The size of integer isn't 4 byte.");
  Integer old = cast<Integer>(0);
  if constexpr (kImpl == ImplType::kClang)
  {
    old = __atomic_fetch_min(ptr, value, __ATOMIC_SEQ_CST);
  }
  else if constexpr (kImpl == ImplType::kGcc)
  {
    const auto min_impl = [](const Integer lhs, const Integer rhs)
    {
      return (lhs < rhs) ? lhs : rhs;
    };
    old = perform(ptr, min_impl, value);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::maxImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  static_assert(sizeof(Integer) == 4, "The size of integer isn't 4 byte.");
  Integer old = cast<Integer>(0);
  if constexpr (kImpl == ImplType::kClang)
  {
    old = __atomic_fetch_max(ptr, value, __ATOMIC_SEQ_CST);
  }
  else if constexpr (kImpl == ImplType::kGcc)
  {
    const auto max_impl = [](const Integer lhs, const Integer rhs)
    {
      return (lhs < rhs) ? rhs : lhs;
    };
    old = perform(ptr, max_impl, value);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::andBitImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    old = __atomic_fetch_and(ptr, value, __ATOMIC_SEQ_CST);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::orBitImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    old = __atomic_fetch_or(ptr, value, __ATOMIC_SEQ_CST);
  }
  return old;
}

/*!
  */
template <typename Integer, Atomic::ImplType kImpl> inline
Integer Atomic::xorBitImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    old = __atomic_fetch_xor(ptr, value, __ATOMIC_SEQ_CST);
  }
  return old;
}

/*!
  */
template <typename Type, Atomic::ImplType kImpl> inline
constexpr bool Atomic::isAlwaysLockFreeImpl() noexcept
{
  bool flag = false;
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    constexpr std::size_t size = sizeof(Type);
    flag = __atomic_always_lock_free(size, nullptr);
  }
  return flag;
}

/*!
  */
template <typename Type, Atomic::ImplType kImpl> inline
bool Atomic::isLockFreeImpl() noexcept
{
  bool flag = false;
  if constexpr ((kImpl == ImplType::kGcc) || (kImpl == ImplType::kClang))
  {
    constexpr std::size_t size = sizeof(Type);
    flag = __atomic_is_lock_free(size, nullptr);
  }
  return flag;
}

} // namespace zisc

#endif // ZISC_ATOMIC_INL_HPP
