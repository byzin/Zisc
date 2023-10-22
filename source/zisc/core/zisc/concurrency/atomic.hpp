/*!
  \file atomic.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ATOMIC_HPP
#define ZISC_ATOMIC_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
template <bool kOsSpecified> class AtomicWord;

/*!
  \brief Atomic functions

  No detailed description.
  */
class Atomic
{
 public:
  //! Represent atomic word value
  using WordValueType = int;


  //! Return the default memory order
  static constexpr auto defaultMemOrder() noexcept -> std::memory_order
  {
    return std::memory_order::seq_cst;
  }

  //! Atomically replace the value of the referenced object
  template <TriviallyCopyable Type>
  static void store(Type* ptr,
                    Type value,
                    const std::memory_order order = defaultMemOrder()) noexcept;

  //! Atomically obtain the value of the referenced object
  template <TriviallyCopyable Type>
  static auto load(const Type* ptr,
                   const std::memory_order order = defaultMemOrder()) noexcept -> Type;

  //! Atomically replace with the given value
  template <TriviallyCopyable Type>
  static auto exchange(Type* ptr,
                       Type value,
                       const std::memory_order order = defaultMemOrder()) noexcept -> Type;

  //! Atomically compare and perform exchange
  template <TriviallyCopyable Type>
  static auto compareAndExchange(
      Type* ptr,
      Type cmp,
      Type value,
      const std::memory_order success_order = defaultMemOrder(),
      const std::memory_order failure_order = defaultMemOrder()) noexcept -> Type;

  //! Perform atomic addition
  template <TriviallyCopyable Type>
  static auto add(Type* ptr,
                  const Type value,
                  const std::memory_order order = defaultMemOrder()) noexcept -> Type;

  //! Perform atomic subtraction
  template <TriviallyCopyable Type>
  static auto sub(Type* ptr,
                  const Type value,
                  const std::memory_order order = defaultMemOrder()) noexcept -> Type;

  //! Perform atomic increment
  template <TriviallyCopyable Type>
  static auto increment(Type* ptr,
                        const std::memory_order order = defaultMemOrder()) noexcept -> Type;

  //! Perform atomic decrement 
  template <TriviallyCopyable Type>
  static auto decrement(Type* ptr,
                        const std::memory_order order = defaultMemOrder()) noexcept -> Type;

  //!
  template <TriviallyCopyable Type>
  static auto min(Type* ptr,
                  const Type value,
                  const std::memory_order order = defaultMemOrder()) noexcept -> Type;

  //!
  template <TriviallyCopyable Type>
  static auto max(Type* ptr,
                  const Type value,
                  const std::memory_order order = defaultMemOrder()) noexcept -> Type;

  //! Perform atomic bitwise and
  template <Integer Int>
  static auto bitAnd(Int* ptr,
                    const Int value,
                    const std::memory_order order = defaultMemOrder()) noexcept -> Int;

  //! Perform atomic bitwise or
  template <Integer Int>
  static auto bitOr(Int* ptr,
                   const Int value,
                   const std::memory_order order = defaultMemOrder()) noexcept -> Int;

  //! Perform atomic bitwise or
  template <Integer Int>
  static auto bitXor(Int* ptr,
                    const Int value,
                    const std::memory_order order = defaultMemOrder()) noexcept -> Int;

  //! Indicate that the type is always lock-free
  template <TriviallyCopyable Type>
  static constexpr auto isAlwaysLockFree() noexcept -> bool;

  //! Check if the atomic operations are lock-free
  template <TriviallyCopyable Type>
  static auto isLockFree() noexcept -> bool;

  //! Perform an expression atomically
  template <TriviallyCopyable Type, typename Function, typename ...Types>
  requires InvocableR<Function, Type, Type, Types...>
  static auto perform(Type* ptr,
                      const std::memory_order order,
                      Function&& expression,
                      Types&&... arguments) noexcept -> Type;

  //! Perform an expression atomically
  template <TriviallyCopyable Type, typename Function, typename ...Types>
  requires InvocableR<Function, Type, Type, Types...>
  static auto perform(Type* ptr,
                      Function&& expression,
                      Types&&... arguments) noexcept -> Type;

  // Atomic wait-notification

  //! Block the thread until notified and the word value changed
  template <bool kOsSpecified>
  static void wait(AtomicWord<kOsSpecified>* word,
                   const WordValueType old,
                   const std::memory_order order = defaultMemOrder()) noexcept;

  //! Notify a thread blocked in wait
  template <bool kOsSpecified>
  static void notifyOne(AtomicWord<kOsSpecified>* word) noexcept;

  //! Notify all threads blocked in wait
  template <bool kOsSpecified>
  static void notifyAll(AtomicWord<kOsSpecified>* word) noexcept;

 private:
  //! Convert the memory order type
  static constexpr auto castMemOrder(const std::memory_order order) noexcept;

  //! Return memory order for loading
  static constexpr auto getLoadOrder(const std::memory_order order) noexcept -> std::memory_order;
};

// STL style function aliases

//! Atomically replace the value of the referenced object
template <TriviallyCopyable Type>
void atomic_store(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Atomically obtain the value of the referenced object
template <TriviallyCopyable Type>
auto atomic_load(
    const Type* ptr,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Type;

//! Atomically replace with the given value
template <TriviallyCopyable Type>
auto atomic_exchange(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Type;

//! Atomically compare and perform exchange
template <TriviallyCopyable Type>
auto atomic_compare_exchange(
    Type* ptr,
    const Type cmp,
    const Type value,
    const std::memory_order success_order = Atomic::defaultMemOrder(),
    const std::memory_order failure_order = Atomic::defaultMemOrder()) noexcept -> Type;

//! Perform atomic addition
template <TriviallyCopyable Type>
auto atomic_fetch_add(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Type;

//! Perform atomic subtraction
template <TriviallyCopyable Type>
auto atomic_fetch_sub(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Type;

//! Perform atomic increment
template <TriviallyCopyable Type>
auto atomic_fetch_inc(
    Type* ptr,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Type;

//! Perform atomic decrement 
template <TriviallyCopyable Type>
auto atomic_fetch_dec(
    Type* ptr,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Type;

//!
template <TriviallyCopyable Type>
auto atomic_fetch_min(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Type;

//!
template <TriviallyCopyable Type>
auto atomic_fetch_max(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Type;

//! Perform atomic bitwise and
template <Integer Int>
auto atomic_fetch_and(
    Int* ptr,
    const Int value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Int;

//! Perform atomic bitwise or
template <Integer Int>
auto atomic_fetch_or(
    Int* ptr,
    const Int value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Int;

//! Perform atomic bitwise or
template <Integer Int>
auto atomic_fetch_xor(
    Int* ptr,
    const Int value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept -> Int;

//! Block the thread until notified and the word value changed
template <bool kOsSpecified>
void atomic_wait(AtomicWord<kOsSpecified>* word,
                 const Atomic::WordValueType old,
                 const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Notify a thread blocked in wait
template <bool kOsSpecified>
void atomic_notify_one(AtomicWord<kOsSpecified>* word) noexcept;

//! Notify all threads blocked in wait
template <bool kOsSpecified>
void atomic_notify_all(AtomicWord<kOsSpecified>* word) noexcept;

} // namespace zisc

#include "atomic-inl.hpp"

#endif // ZISC_ATOMIC_HPP
