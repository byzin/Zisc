/*!
  \file atomic-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ATOMIC_INL_HPP
#define ZISC_ATOMIC_INL_HPP

#include "atomic.hpp"
// Standard C++ library
#include <condition_variable>
#include <atomic>
#include <cstddef>
#include <mutex>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] order No description.
  \return No description
  */
inline
constexpr auto Atomic::castMemOrder(const std::memory_order order) noexcept
{
#if defined(Z_CLANG)
  using OrderT = decltype(__ATOMIC_SEQ_CST);

  // memory order value check
  static_assert(__ATOMIC_RELAXED == static_cast<OrderT>(std::memory_order::relaxed));
  static_assert(__ATOMIC_CONSUME == static_cast<OrderT>(std::memory_order::consume));
  static_assert(__ATOMIC_ACQUIRE == static_cast<OrderT>(std::memory_order::acquire));
  static_assert(__ATOMIC_RELEASE == static_cast<OrderT>(std::memory_order::release));
  static_assert(__ATOMIC_ACQ_REL == static_cast<OrderT>(std::memory_order::acq_rel));
  static_assert(__ATOMIC_SEQ_CST == static_cast<OrderT>(std::memory_order::seq_cst));

  return static_cast<OrderT>(order);
#else // Z_CLANG
  return order;
#endif // Z_CLANG
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  */
template <TriviallyCopyable Type> inline
void Atomic::store(Type* ptr,
                   Type value,
                   const std::memory_order order) noexcept
{
#if defined(Z_CLANG)
  __atomic_store(ptr, &value, castMemOrder(order));
#else // Z_CLANG
  std::atomic_ref<Type>{*ptr}.store(value, order);
#endif // Z_CLANG
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] ptr No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::load(const Type* ptr, const std::memory_order order) noexcept
{
  using T = std::remove_cvref_t<Type>;
  T* p = const_cast<T*>(ptr);
  Type result = cast<Type>(0);
#if defined(Z_CLANG)
  __atomic_load(p, &result, castMemOrder(order));
#else // Z_CLANG
  result = std::atomic_ref<T>{*p}.load(order);
#endif // Z_CLANG
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::exchange(Type* ptr,
                      Type value,
                      const std::memory_order order) noexcept
{
  Type old = cast<Type>(0);
#if defined(Z_CLANG)
  __atomic_exchange(ptr, &value, &old, castMemOrder(order));
#else // Z_CLANG
  old = std::atomic_ref<Type>{*ptr}.exchange(value, order);
#endif // Z_CLANG
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] cmp No description.
  \param [in] value No description.
  \param [in] success_order No description.
  \param [in] failure_order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::compareAndExchange(Type* ptr,
                                Type cmp,
                                Type value,
                                const std::memory_order success_order,
                                const std::memory_order failure_order) noexcept
{
  const auto s = castMemOrder(success_order);
  const auto f = castMemOrder(failure_order);
#if defined(Z_CLANG)
  __atomic_compare_exchange(ptr, &cmp, &value, false, s, f);
#else // Z_CLANG
  std::atomic_ref<Type>{*ptr}.compare_exchange_strong(cmp, value, s, f);
#endif // Z_CLANG
  return cmp;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::add(Type* ptr,
                 const Type value,
                 const std::memory_order order) noexcept
{
  const Type old =
#if defined(Z_CLANG)
      __atomic_fetch_add(ptr, value, castMemOrder(order));
#else // Z_CLANG
      std::atomic_ref<Type>{*ptr}.fetch_add(value, order);
#endif // Z_CLANG
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::sub(Type* ptr,
                 const Type value,
                 const std::memory_order order) noexcept
{
  const Type old =
#if defined(Z_CLANG)
      __atomic_fetch_sub(ptr, value, castMemOrder(order));
#else // Z_CLANG
      std::atomic_ref<Type>{*ptr}.fetch_sub(value, order);
#endif // Z_CLANG
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::increment(Type* ptr, const std::memory_order order) noexcept
{
  constexpr Type one = cast<Type>(1);
  const Type old = add(ptr, one, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::decrement(Type* ptr, const std::memory_order order) noexcept
{
  constexpr Type one = cast<Type>(1);
  const auto old = sub(ptr, one, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::min(Type* ptr, const Type value, const std::memory_order order) noexcept
{
  const auto func = [](const Type lhs, const Type rhs)
  {
    return (lhs < rhs) ? lhs : rhs;
  };
  Type old = cast<Type>(0);
#if defined(Z_CLANG)
  if constexpr (Integer<Type>)
    old = __atomic_fetch_min(ptr, value, castMemOrder(order));
  else
#endif // Z_CLANG
    old = perform(ptr, order, func, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type Atomic::max(Type* ptr, const Type value, const std::memory_order order) noexcept
{
  const auto func = [](const Type lhs, const Type rhs)
  {
    return (lhs < rhs) ? rhs : lhs;
  };
  Type old = cast<Type>(0);
#if defined(Z_CLANG)
  if constexpr (Integer<Type>)
    old = __atomic_fetch_max(ptr, value, castMemOrder(order));
  else
#endif // Z_CLANG
    old = perform(ptr, order, func, value);
  return old;

}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <Integer Int> inline
Int Atomic::bitAnd(Int* ptr, const Int value, const std::memory_order order) noexcept
{
  const Int old =
#if defined(Z_CLANG)
      __atomic_fetch_and(ptr, value, castMemOrder(order));
#else // Z_CLANG
      std::atomic_ref<Int>{*ptr}.fetch_and(value, order);
#endif // Z_CLANG
  return old;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <Integer Int> inline
Int Atomic::bitOr(Int* ptr, const Int value, const std::memory_order order) noexcept
{
  const Int old =
#if defined(Z_CLANG)
      __atomic_fetch_or(ptr, value, castMemOrder(order));
#else // Z_CLANG
      std::atomic_ref<Int>{*ptr}.fetch_or(value, order);
#endif // Z_CLANG
  return old;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <Integer Int> inline
Int Atomic::bitXor(Int* ptr, const Int value, const std::memory_order order) noexcept
{
  const Int old =
#if defined(Z_CLANG)
      __atomic_fetch_xor(ptr, value, castMemOrder(order));
#else // Z_CLANG
      std::atomic_ref<Int>{*ptr}.fetch_xor(value, order);
#endif // Z_CLANG
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
constexpr bool Atomic::isAlwaysLockFree() noexcept
{
  [[maybe_unused]] constexpr std::size_t size = sizeof(Type);
  const bool result =
#if defined(Z_CLANG)
      __atomic_always_lock_free(size, nullptr);
#else // Z_CLANG
      std::atomic_ref<Type>::is_always_lock_free;
#endif // Z_CLANG
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
bool Atomic::isLockFree() noexcept
{
  [[maybe_unused]] constexpr std::size_t size = sizeof(Type);
  [[maybe_unused]] Type value = cast<Type>(0);
  const bool result =
#if defined(Z_CLANG)
      __atomic_is_lock_free(size, nullptr);
#else // Z_CLANG
      std::atomic_ref<Type>{value}.is_lock_free();
#endif // Z_CLANG
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam Function No description.
  \tparam Types No description.
  \param [in,out] ptr No description.
  \param [in] order No description.
  \param [in] expression No description.
  \param [in] arguments No description.
  \return No description
  */
template <TriviallyCopyable Type, typename Function, typename ...Types>
requires InvocableR<Function, Type, Type, Types...>
inline
Type Atomic::perform(Type* ptr,
                     const std::memory_order order,
                     Function&& expression,
                     Types&&... arguments) noexcept
{
  Type old = *ptr;
  Type cmp = cast<Type>(0);
  do {
    cmp = old;
    const Type value = expression(cmp, std::forward<Types>(arguments)...);
    old = compareAndExchange(ptr, cmp, value, order, std::memory_order::acquire);
  } while (old != cmp);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam Function No description.
  \tparam Types No description.
  \param [in,out] ptr No description.
  \param [in] expression No description.
  \param [in] arguments No description.
  \return No description
  */
template <TriviallyCopyable Type, typename Function, typename ...Types>
requires InvocableR<Function, Type, Type, Types...>
inline
Type Atomic::perform(Type* ptr,
                     Function&& expression,
                     Types&&... arguments) noexcept
{
  const Type old = perform(ptr,
                           defaultMemOrder(),
                           std::forward<Function>(expression),
                           std::forward<Types>(arguments)...);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  */
template <TriviallyCopyable Type> inline
void atomic_store(Type* ptr,
                  const Type value,
                  const std::memory_order order) noexcept
{
  Atomic::store(ptr, value, order);
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] ptr No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_load(const Type* ptr, const std::memory_order order) noexcept
{
  const Type result = Atomic::load(ptr, order);
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_exchange(Type* ptr,
                     const Type value,
                     const std::memory_order order) noexcept
{
  const Type old = Atomic::exchange(ptr, value, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] cmp No description.
  \param [in] value No description.
  \param [in] success_order No description.
  \param [in] failure_order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_compare_exchange(Type* ptr,
                             const Type cmp,
                             const Type value,
                             const std::memory_order success_order,
                             const std::memory_order failure_order) noexcept
{
  const Type old = Atomic::compareAndExchange(ptr, cmp, value, success_order, failure_order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_fetch_add(Type* ptr,
                      const Type value,
                      const std::memory_order order) noexcept
{
  const Type old = Atomic::add(ptr, value, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_fetch_sub(Type* ptr,
                      const Type value,
                      const std::memory_order order) noexcept
{
  const Type old = Atomic::sub(ptr, value, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_fetch_inc(Type* ptr,
                      const std::memory_order order) noexcept
{
  const Type old = Atomic::increment(ptr, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_fetch_dec(Type* ptr,
                      const std::memory_order order) noexcept
{
  const Type old = Atomic::decrement(ptr, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_fetch_min(Type* ptr,
                      const Type value,
                      const std::memory_order order) noexcept
{
  const Type old = (Atomic::min)(ptr, value, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <TriviallyCopyable Type> inline
Type atomic_fetch_max(Type* ptr,
                      const Type value,
                      const std::memory_order order) noexcept
{
  const Type old = (Atomic::max)(ptr, value, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <Integer Int> inline
Int atomic_fetch_and(Int* ptr,
                     const Int value,
                     const std::memory_order order) noexcept
{
  const Int old = Atomic::bitAnd(ptr, value, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <Integer Int> inline
Int atomic_fetch_or(Int* ptr,
                    const Int value,
                    const std::memory_order order) noexcept
{
  const Int old = Atomic::bitOr(ptr, value, order);
  return old;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \param [in] order No description.
  \return No description
  */
template <Integer Int> inline
Int atomic_fetch_xor(Int* ptr,
                     const Int value,
                     const std::memory_order order) noexcept
{
  const Int old = Atomic::bitXor(ptr, value, order);
  return old;
}

/*!
  \details No detailed description

  \tparam kOsSpecified No description.
  \param [in] word No description.
  \param [in] old No description.
  \param [in] order No description.
  */
template <bool kOsSpecified> inline
void atomic_wait(AtomicWord<kOsSpecified>* word,
                 const Atomic::WordValueType old,
                 const std::memory_order order) noexcept
{
  Atomic::wait(word, old, order);
}

/*!
  \details No detailed description

  \tparam kOsSpecified No description.
  \param [in] word No description.
  */
template <bool kOsSpecified> inline
void atomic_notify_one(AtomicWord<kOsSpecified>* word) noexcept
{
  Atomic::notifyOne(word);
}

/*!
  \details No detailed description

  \tparam kOsSpecified No description.
  \param [in] word No description.
  */
template <bool kOsSpecified> inline
void atomic_notify_all(AtomicWord<kOsSpecified>* word) noexcept
{
  Atomic::notifyAll(word);
}

} // namespace zisc

#endif // ZISC_ATOMIC_INL_HPP
