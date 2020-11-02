/*!
  \file atomic.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
template <bool kOsSpecified> class AtomicWord;
using AtomicWordType = int;
template <bool kOsSpecified> class AtomicWordExtra;

/*!
  \brief Atomic functions

  No detailed description.
  */
class Atomic
{
 public:
  //! Return the default memory order
  static constexpr std::memory_order defaultMemOrder() noexcept
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
  static Type load(const Type* ptr,
                   const std::memory_order order = defaultMemOrder()) noexcept;

  //! Atomically replace with the given value
  template <TriviallyCopyable Type>
  static Type exchange(Type* ptr,
                       Type value,
                       const std::memory_order order = defaultMemOrder()) noexcept;

  //! Atomically compare and perform exchange
  template <TriviallyCopyable Type>
  static Type compareAndExchange(
      Type* ptr,
      Type cmp,
      Type value,
      const std::memory_order success_order = defaultMemOrder(),
      const std::memory_order failure_order = defaultMemOrder()) noexcept;

  //! Perform atomic addition
  template <TriviallyCopyable Type>
  static Type add(Type* ptr,
                  const Type value,
                  const std::memory_order order = defaultMemOrder()) noexcept;

  //! Perform atomic subtraction
  template <TriviallyCopyable Type>
  static Type sub(Type* ptr,
                  const Type value,
                  const std::memory_order order = defaultMemOrder()) noexcept;

  //! Perform atomic increment
  template <TriviallyCopyable Type>
  static Type increment(Type* ptr,
                        const std::memory_order order = defaultMemOrder()) noexcept;

  //! Perform atomic decrement 
  template <TriviallyCopyable Type>
  static Type decrement(Type* ptr,
                        const std::memory_order order = defaultMemOrder()) noexcept;

  //!
  template <TriviallyCopyable Type>
  static Type min(Type* ptr,
                  const Type value,
                  const std::memory_order order = defaultMemOrder()) noexcept;

  //!
  template <TriviallyCopyable Type>
  static Type max(Type* ptr,
                  const Type value,
                  const std::memory_order order = defaultMemOrder()) noexcept;

  //! Perform atomic bitwise and
  template <Integer Int>
  static Int bitAnd(Int* ptr,
                    const Int value,
                    const std::memory_order order = defaultMemOrder()) noexcept;

  //! Perform atomic bitwise or
  template <Integer Int>
  static Int bitOr(Int* ptr,
                   const Int value,
                   const std::memory_order order = defaultMemOrder()) noexcept;

  //! Perform atomic bitwise or
  template <Integer Int>
  static Int bitXor(Int* ptr,
                    const Int value,
                    const std::memory_order order = defaultMemOrder()) noexcept;

  //! Indicate that the type is always lock-free
  template <TriviallyCopyable Type>
  static constexpr bool isAlwaysLockFree() noexcept;

  //! Check if the atomic operations are lock-free
  template <TriviallyCopyable Type>
  static bool isLockFree() noexcept;

  //! Perform an expression atomically
  template <TriviallyCopyable Type, typename Function, typename ...Types>
  requires InvocableR<Function, Type, Type, Types...>
  static Type perform(Type* ptr,
                      const std::memory_order order,
                      Function&& expression,
                      Types&&... arguments) noexcept;

  //! Perform an expression atomically
  template <TriviallyCopyable Type, typename Function, typename ...Types>
  requires InvocableR<Function, Type, Type, Types...>
  static Type perform(Type* ptr,
                      Function&& expression,
                      Types&&... arguments) noexcept;

  // Atomic wait-notification

  //! Block the thread until notified and the word value changed
  template <bool kOsSpecialization>
  static void wait(AtomicWord<kOsSpecialization>* word,
                   const AtomicWordType old) noexcept;

  //! Notify a thread blocked in wait
  template <bool kOsSpecialization>
  static void notifyOne(AtomicWord<kOsSpecialization>* word) noexcept;

  //! Notify all threads blocked in wait
  template <bool kOsSpecialization>
  static void notifyAll(AtomicWord<kOsSpecialization>* word) noexcept;

 private:
  //! Convert the memory order type
  static auto castMemOrder(const std::memory_order order) noexcept;

  //! Indicate that the type is always lock-free
  template <typename Type, Config::ImplType kImpl>
  static constexpr bool isAlwaysLockFreeImpl() noexcept;

  //! Check if the atomic operations are lock-free
  template <typename Type, Config::ImplType kImpl>
  static bool isLockFreeImpl() noexcept;
};

/*!
  \brief No brief description

  No detailed description.

  \tparam kOsSpecified No description.
  */
template <bool kOsSpecified>
class AtomicWord : private NonCopyable<AtomicWord<kOsSpecified>>
{
 public:
  //! Construct an atomic word
  AtomicWord() noexcept;

  //! Construct an atomic word
  AtomicWord(const AtomicWordType value) noexcept;


  //! Return the extra data
  AtomicWordExtra<kOsSpecified>& extra() noexcept;

  //! Return the underlying word
  AtomicWordType& get() noexcept;

  //! Return the underlying word
  const AtomicWordType& get() const noexcept;

  //! Check if the atomic word is specialized
  static constexpr bool isSpecialized() noexcept;

  //! Set a value to the underlying word atomically
  void set(const AtomicWordType value,
           const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

 private:
  AtomicWordType word_ = 0;
  [[no_unique_address]] AtomicWordExtra<kOsSpecified> extra_;
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
Type atomic_load(
    const Type* ptr,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Atomically replace with the given value
template <TriviallyCopyable Type>
Type atomic_exchange(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Atomically compare and perform exchange
template <TriviallyCopyable Type>
Type atomic_compare_exchange(
    Type* ptr,
    const Type cmp,
    const Type value,
    const std::memory_order success_order = Atomic::defaultMemOrder(),
    const std::memory_order failure_order = Atomic::defaultMemOrder()) noexcept;

//! Perform atomic addition
template <TriviallyCopyable Type>
Type atomic_fetch_add(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Perform atomic subtraction
template <TriviallyCopyable Type>
Type atomic_fetch_sub(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Perform atomic increment
template <TriviallyCopyable Type>
Type atomic_fetch_inc(
    Type* ptr,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Perform atomic decrement 
template <TriviallyCopyable Type>
Type atomic_fetch_dec(
    Type* ptr,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//!
template <TriviallyCopyable Type>
Type atomic_fetch_min(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//!
template <TriviallyCopyable Type>
Type atomic_fetch_max(
    Type* ptr,
    const Type value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Perform atomic bitwise and
template <Integer Int>
Int atomic_fetch_and(
    Int* ptr,
    const Int value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Perform atomic bitwise or
template <Integer Int>
Int atomic_fetch_or(
    Int* ptr,
    const Int value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Perform atomic bitwise or
template <Integer Int>
Int atomic_fetch_xor(
    Int* ptr,
    const Int value,
    const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

//! Block the thread until notified and the word value changed
template <bool kOsSpecialization>
void atomic_wait(AtomicWord<kOsSpecialization>* word,
                 const AtomicWordType old) noexcept;

//! Notify a thread blocked in wait
template <bool kOsSpecialization>
void atomic_notify_one(AtomicWord<kOsSpecialization>* word) noexcept;

//! Notify all threads blocked in wait
template <bool kOsSpecialization>
void atomic_notify_all(AtomicWord<kOsSpecialization>* word) noexcept;

} // namespace zisc

#include "atomic-inl.hpp"

#endif // ZISC_ATOMIC_HPP
