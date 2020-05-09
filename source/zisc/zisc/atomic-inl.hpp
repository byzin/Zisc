/*!
  \file atomic-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ATOMIC_INL_HPP
#define ZISC_ATOMIC_INL_HPP

#include "atomic.hpp"
// Standard C++ library
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
// Zisc
#include "non_copyable.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

#if defined(Z_WINDOWS) || defined(Z_LINUX)

/*!
  \brief No brief description

  No detailed description.
  */
template <>
struct AtomicWord<true> : NonCopyable<AtomicWord<true>>
{
  //! Construct an atomic word
  AtomicWord() : word_{0} {static_cast<void>(padding_);}

  //! Construct an atomic word
  AtomicWord(const WordType value) : word_{value} {}


  //! Return the underlying word
  WordType& get() noexcept
  {
    return word_;
  }

  //! Return the underlying word
  const WordType& get() const noexcept
  {
    return word_;
  }

  //! Check if the atomic word is specialized
  static constexpr bool isSpecialized() noexcept
  {
    return true;
  }

  //! Set a value to the underlying word atomically
  void set(const WordType value) noexcept
  {
    Atomic::exchange(&word_, value);
  }

 private:
  WordType word_;
  int padding_ = 0;
};

#endif // Z_WINDOWS || Z_LINUX

/*!
  \brief No brief description

  No detailed description.
  */
template <bool kOsSpecified>
struct AtomicWord : NonCopyable<AtomicWord<kOsSpecified>>
{
  //! Construct an atomic word
  AtomicWord() : word_{0} {static_cast<void>(padding_);}

  //! Construct an atomic word
  AtomicWord(const WordType value) : word_{value} {}


  //! Return the underlying word
  WordType& get() noexcept
  {
    return word_;
  }

  //! Return the underlying word
  const WordType& get() const noexcept
  {
    return word_;
  }

  //! Check if the atomic word is specialized
  static constexpr bool isSpecialized() noexcept
  {
    return false;
  }

  //! Set a value to the underlying word atomically
  void set(const WordType value) noexcept
  {
    std::unique_lock<std::mutex> locker_{lock()};
    word_ = value;
  }

  //! Return the underlying mutex
  std::mutex& lock() noexcept
  {
    return lock_;
  }

  //! Return the underlying condition variable
  std::condition_variable& condition() noexcept
  {
    return condition_;
  }

 private:
  WordType word_;
  int padding_ = 0;
  std::mutex lock_;
  std::condition_variable condition_;
};

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::add(Integer* ptr, const Integer value) noexcept
{
  const auto old = addImpl<Integer, Config::implType()>(ptr, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::sub(Integer* ptr, const Integer value) noexcept
{
  const auto old = subImpl<Integer, Config::implType()>(ptr, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::exchange(Integer* ptr, const Integer value) noexcept
{
  const auto old = exchangeImpl<Integer, Config::implType()>(ptr, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::increment(Integer* ptr) noexcept
{
  const auto old = incrementImpl<Integer, Config::implType()>(ptr);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::decrement(Integer* ptr) noexcept
{
  const auto old = decrementImpl<Integer, Config::implType()>(ptr);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] cmp No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::compareAndExchange(Integer* ptr,
                                   const Integer cmp,
                                   const Integer value) noexcept
{
  const auto old = compareAndExchangeImpl<Integer, Config::implType()>(ptr, cmp, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::min(Integer* ptr, const Integer value) noexcept
{
  const auto old = minImpl<Integer, Config::implType()>(ptr, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::max(Integer* ptr, const Integer value) noexcept
{
  const auto old = maxImpl<Integer, Config::implType()>(ptr, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::andBit(Integer* ptr, const Integer value) noexcept
{
  const auto old = andBitImpl<Integer, Config::implType()>(ptr, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::orBit(Integer* ptr, const Integer value) noexcept
{
  const auto old = orBitImpl<Integer, Config::implType()>(ptr, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer> inline
Integer Atomic::xorBit(Integer* ptr, const Integer value) noexcept
{
  const auto old = xorBitImpl<Integer, Config::implType()>(ptr, value);
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
constexpr bool Atomic::isAlwaysLockFree() noexcept
{
  const bool flag = isAlwaysLockFreeImpl<Type, Config::implType()>();
  return flag;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
bool Atomic::isLockFree() noexcept
{
  const bool flag = isLockFreeImpl<Type, Config::implType()>();
  return flag;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam Function No description.
  \tparam Types No description.
  \param [in,out] ptr No description.
  \param [in] expression No description.
  \param [in] arguments No description.
  \return No description
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

// template explicit instantiation
template <>
void Atomic::wait<false>(AtomicWord<false>* word, const WordType old) noexcept;
template <>
void Atomic::wait<true>(AtomicWord<true>* word, const WordType old) noexcept;
template <>
void Atomic::notifyOne<false>(AtomicWord<false>* word) noexcept;
template <>
void Atomic::notifyOne<true>(AtomicWord<true>* word) noexcept;
template <>
void Atomic::notifyAll<false>(AtomicWord<false>* word) noexcept;
template <>
void Atomic::notifyAll<true>(AtomicWord<true>* word) noexcept;

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::addImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    old = __atomic_fetch_add(ptr, value, __ATOMIC_SEQ_CST);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto o = treatAs<InterlockedType<size>*>(&old);
    auto v = treatAs<const InterlockedType<size>*>(&value);
    if constexpr (size == 1) {
      *o = _InterlockedExchangeAdd8(p, *v);
    }
    else if constexpr (size == 2) {
      *o = _InterlockedExchangeAdd16(p, *v);
    }
    else if constexpr (size == 4) {
      *o = _InterlockedExchangeAdd(p, *v);
    }
    else if constexpr (size == 8) {
      *o = _InterlockedExchangeAdd64(p, *v);
    }
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::subImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    old = __atomic_fetch_sub(ptr, value, __ATOMIC_SEQ_CST);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto o = treatAs<InterlockedType<size>*>(&old);
    auto v = treatAs<const InterlockedType<size>*>(&value);
    if constexpr (size == 1) {
      *o = _InterlockedExchangeAdd8(p, -*v);
    }
    else if constexpr (size == 2) {
      *o = _InterlockedExchangeAdd16(p, -*v);
    }
    else if constexpr (size == 4) {
      *o = _InterlockedExchangeAdd(p, -*v);
    }
    else if constexpr (size == 8) {
      *o = _InterlockedExchangeAdd64(p, -*v);
    }
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::exchangeImpl(Integer* ptr, Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    __atomic_exchange(ptr, &value, &old, __ATOMIC_SEQ_CST);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto o = treatAs<InterlockedType<size>*>(&old);
    auto v = treatAs<const InterlockedType<size>*>(&value);
    if constexpr (size == 1) {
      *o = _InterlockedExchange8(p, *v);
    }
    else if constexpr (size == 2) {
      *o = _InterlockedExchange16(p, *v);
    }
    else if constexpr (size == 4) {
      *o = _InterlockedExchange(p, *v);
    }
    else if constexpr (size == 8) {
      *o = _InterlockedExchange64(p, *v);
    }
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::incrementImpl(Integer* ptr) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    constexpr Integer v = cast<Integer>(1);
    old = addImpl<Integer, kImpl>(ptr, v);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto o = treatAs<InterlockedType<size>*>(&old);
    constexpr auto v = cast<InterlockedType<size>>(1);
    if constexpr (size == 1) {
      *o = _InterlockedExchangeAdd8(p, v);
    }
    else if constexpr (size == 2) {
      *o = _InterlockedExchangeAdd16(p, v);
    }
    else if constexpr (size == 4) {
      *o = _InterlockedExchangeAdd(p, v);
    }
    else if constexpr (size == 8) {
      *o = _InterlockedExchangeAdd64(p, v);
    }
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::decrementImpl(Integer* ptr) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    constexpr Integer v = cast<Integer>(1);
    old = subImpl<Integer, kImpl>(ptr, v);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto o = treatAs<InterlockedType<size>*>(&old);
    constexpr auto v = cast<InterlockedType<size>>(-1);
    if constexpr (size == 1) {
      *o = _InterlockedExchangeAdd8(p, v);
    }
    else if constexpr (size == 2) {
      *o = _InterlockedExchangeAdd16(p, v);
    }
    else if constexpr (size == 4) {
      *o = _InterlockedExchangeAdd(p, v);
    }
    else if constexpr (size == 8) {
      *o = _InterlockedExchangeAdd64(p, v);
    }
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] cmp No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::compareAndExchangeImpl(Integer* ptr,
                                       Integer cmp,
                                       Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    constexpr auto mem_order = __ATOMIC_SEQ_CST;
    __atomic_compare_exchange(ptr, &cmp, &value, false, mem_order, mem_order);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto c = treatAs<InterlockedType<size>*>(&cmp);
    auto v = treatAs<const InterlockedType<size>*>(&value);
    if constexpr (size == 1) {
      *c = _InterlockedCompareExchange8(p, *v, *c);
    }
    else if constexpr (size == 2) {
      *c = _InterlockedCompareExchange16(p, *v, *c);
    }
    else if constexpr (size == 4) {
      *c = _InterlockedCompareExchange(p, *v, *c);
    }
    else if constexpr (size == 8) {
      *c = _InterlockedCompareExchange64(p, *v, *c);
    }
  }
  return cmp;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::minImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  constexpr std::size_t size = sizeof(Integer);
  if constexpr ((kImpl == Config::ImplType::kClang) && (size == 4)) {
    old = __atomic_fetch_min(ptr, value, __ATOMIC_SEQ_CST);
  }
  else {
    const auto min_impl = [](const Integer lhs, const Integer rhs)
    {
      return (lhs < rhs) ? lhs : rhs;
    };
    old = perform(ptr, min_impl, value);
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::maxImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  constexpr std::size_t size = sizeof(Integer);
  if constexpr ((kImpl == Config::ImplType::kClang) && (size == 4)) {
    old = __atomic_fetch_max(ptr, value, __ATOMIC_SEQ_CST);
  }
  else {
    const auto max_impl = [](const Integer lhs, const Integer rhs)
    {
      return (lhs < rhs) ? rhs : lhs;
    };
    old = perform(ptr, max_impl, value);
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::andBitImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    old = __atomic_fetch_and(ptr, value, __ATOMIC_SEQ_CST);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto o = treatAs<InterlockedType<size>*>(&old);
    auto v = treatAs<const InterlockedType<size>*>(&value);
    if constexpr (size == 1) {
      *o = _InterlockedAnd8(p, *v);
    }
    else if constexpr (size == 2) {
      *o = _InterlockedAnd16(p, *v);
    }
    else if constexpr (size == 4) {
      *o = _InterlockedAnd(p, *v);
    }
    else if constexpr (size == 8) {
      *o = _InterlockedAnd64(p, *v);
    }
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::orBitImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    old = __atomic_fetch_or(ptr, value, __ATOMIC_SEQ_CST);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto o = treatAs<InterlockedType<size>*>(&old);
    auto v = treatAs<const InterlockedType<size>*>(&value);
    if constexpr (size == 1) {
      *o = _InterlockedOr8(p, *v);
    }
    else if constexpr (size == 2) {
      *o = _InterlockedOr16(p, *v);
    }
    else if constexpr (size == 4) {
      *o = _InterlockedOr(p, *v);
    }
    else if constexpr (size == 8) {
      *o = _InterlockedOr64(p, *v);
    }
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \tparam kImpl No description.
  \param [in,out] ptr No description.
  \param [in] value No description.
  \return No description
  */
template <typename Integer, Config::ImplType kImpl> inline
Integer Atomic::xorBitImpl(Integer* ptr, const Integer value) noexcept
{
  static_assert(std::is_integral_v<Integer>, "The Integer isn't integer type.");
  Integer old = cast<Integer>(0);
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    old = __atomic_fetch_xor(ptr, value, __ATOMIC_SEQ_CST);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    constexpr std::size_t size = sizeof(Integer);
    auto p = treatAs<InterlockedType<size>*>(ptr);
    auto o = treatAs<InterlockedType<size>*>(&old);
    auto v = treatAs<const InterlockedType<size>*>(&value);
    if constexpr (size == 1) {
      *o = _InterlockedXor8(p, *v);
    }
    else if constexpr (size == 2) {
      *o = _InterlockedXor16(p, *v);
    }
    else if constexpr (size == 4) {
      *o = _InterlockedXor(p, *v);
    }
    else if constexpr (size == 8) {
      *o = _InterlockedXor64(p, *v);
    }
  }
  return old;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam kImpl No description.
  \return No description
  */
template <typename Type, Config::ImplType kImpl> inline
constexpr bool Atomic::isAlwaysLockFreeImpl() noexcept
{
  bool flag = false;
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    constexpr std::size_t size = sizeof(Type);
    flag = __atomic_always_lock_free(size, nullptr);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    //! \todo Confirm if is it lock free
    flag = true;
  }
  return flag;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam kImpl No description.
  \return No description
  */
template <typename Type, Config::ImplType kImpl> inline
bool Atomic::isLockFreeImpl() noexcept
{
  bool flag = false;
  if constexpr ((kImpl == Config::ImplType::kGcc) ||
                (kImpl == Config::ImplType::kClang)) {
    constexpr std::size_t size = sizeof(Type);
    flag = __atomic_is_lock_free(size, nullptr);
  }
  else if constexpr (kImpl == Config::ImplType::kMsvc) {
    //! \todo Confirm if is it lock free
    flag = true;
  }
  return flag;
}

} // namespace zisc

#endif // ZISC_ATOMIC_INL_HPP
