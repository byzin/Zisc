/*!
  \file atomic.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "atomic.hpp"
// Standard C++ library
#include <atomic>
#include <condition_variable>
#include <limits>
#include <memory>
#include <mutex>
// Zisc
#include "atomic_word.hpp"
#include "zisc/bit.hpp"
#include "zisc/utility.hpp"
// Platform
#if defined(Z_WINDOWS)
#include <Windows.h>
#elif defined(Z_LINUX)
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>
#endif

namespace {

//! Block the thread until notified and the word value changed
template <bool kOsSpecified> inline
void waitFallback(zisc::AtomicWord<kOsSpecified>* word,
                  const zisc::Atomic::WordValueType old,
                  const std::memory_order order) noexcept
{
  const auto pred = [word, old, order]() noexcept
  {
    const bool result = word->load(order) != old;
    return result;
  };
  std::condition_variable& condition = word->condition();
  std::unique_lock<std::mutex> locker{word->lock()};
  condition.wait(locker, pred);
}

//! Notify all threads blocked in wait
template <bool kOsSpecified> inline
void notifyOneFallback(zisc::AtomicWord<kOsSpecified>* word) noexcept
{
  std::condition_variable& condition = word->condition();
  condition.notify_one();
}

//! Notify a thread blocked in wait
template <bool kOsSpecified> inline
void notifyAllFallback(zisc::AtomicWord<kOsSpecified>* word) noexcept
{
  std::condition_variable& condition = word->condition();
  condition.notify_all();
}

#if defined(Z_LINUX)
inline
long futex(zisc::Atomic::WordValueType* addr,
           const zisc::Atomic::WordValueType futex_op,
           const zisc::Atomic::WordValueType val)
{
  static_assert(sizeof(zisc::Atomic::WordValueType) == 4,
                "'zisc::Atomic::WordValueType' must be 4 bytes length and aligned.");
  const long result = syscall(SYS_futex, addr, futex_op, val, nullptr, nullptr, 0);
  return result;
}
#endif // Z_LINUX

} // namespace

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] word No description.
  \param [in] old No description.
  \param [in] order No description.
  */
template <>
void Atomic::wait<true>(AtomicWord<true>* word,
                        const Atomic::WordValueType old,
                        const std::memory_order order) noexcept
{
  if constexpr (AtomicWord<true>::isSpecialized()) {
    do {
#if defined(Z_WINDOWS)
      static_assert(sizeof(Atomic::WordValueType*) == sizeof(PVOID));
      static_assert(alignof(Atomic::WordValueType*) == alignof(PVOID));
      PVOID addr = std::addressof(word->get());
      PVOID comp = bit_cast<PVOID>(std::addressof(old));
      [[maybe_unused]] auto result = WaitOnAddress(addr, comp, sizeof(old), INFINITE);
#elif defined(Z_LINUX)
      Atomic::WordValueType* addr = std::addressof(word->get());
      [[maybe_unused]] const long result = ::futex(addr, FUTEX_WAIT_PRIVATE, old);
#endif
    } while (word->load(order) == old);
  }
  else {
    ::waitFallback(word, old, order);
  }
}

/*!
  \details No detailed description

  \param [in,out] word No description.
  \param [in] old No description.
  \param [in] order No description.
  */
template <>
void Atomic::wait<false>(AtomicWord<false>* word,
                         const Atomic::WordValueType old,
                         const std::memory_order order) noexcept
{
  ::waitFallback(word, old, order);
}

/*!
  \details No detailed description

  \param [in,out] word No description.
  */
template <>
void Atomic::notifyOne<true>(AtomicWord<true>* word) noexcept
{
#if defined(Z_WINDOWS)
  PVOID addr = std::addressof(word->get());
  WakeByAddressSingle(addr);
#elif defined(Z_LINUX)
  Atomic::WordValueType* addr = std::addressof(word->get());
  constexpr Atomic::WordValueType n = 1;
  [[maybe_unused]] const long result = ::futex(addr, FUTEX_WAKE_PRIVATE, n);
#else
  ::notifyOneFallback(word);
#endif
}

/*!
  \details No detailed description

  \param [in,out] word No description.
  */
template <>
void Atomic::notifyOne<false>(AtomicWord<false>* word) noexcept
{
  ::notifyOneFallback(word);
}

/*!
  \details No detailed description

  \param [in,out] word No description.
  */
template <>
void Atomic::notifyAll<true>(AtomicWord<true>* word) noexcept
{
#if defined(Z_WINDOWS)
  PVOID addr = std::addressof(word->get());
  WakeByAddressAll(addr);
#elif defined(Z_LINUX)
  Atomic::WordValueType* addr = std::addressof(word->get());
  constexpr Atomic::WordValueType n = (std::numeric_limits<Atomic::WordValueType>::max)();
  [[maybe_unused]] const long result = ::futex(addr, FUTEX_WAKE_PRIVATE, n);
#else
  ::notifyAllFallback(word);
#endif
}

/*!
  \details No detailed description

  \param [in,out] word No description.
  */
template <>
void Atomic::notifyAll<false>(AtomicWord<false>* word) noexcept
{
  ::notifyAllFallback(word);
}

// Size check
#if defined(Z_WINDOWS) || defined(Z_LINUX)
static_assert(sizeof(AtomicWord<true>) == sizeof(Atomic::WordValueType));
#endif // Z_WINDOWS || Z_LINUX

} // namespace zisc
