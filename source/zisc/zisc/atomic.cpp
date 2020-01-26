/*!
  \file atomic.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "atomic.hpp"
// Standard C++ library
#include <condition_variable>
#include <limits>
#include <memory>
#include <mutex>
// Zisc
#include "utility.hpp"
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
void waitFallback(zisc::AtomicWord<kOsSpecified>* word, const int old) noexcept
{
  int padding = 0;
  const auto pred = [word, old, padding]() noexcept
  {
    static_cast<void>(padding);
    const bool result = word->get() != old;
    return result;
  };
  auto& condition = word->condition();
  std::unique_lock<std::mutex> locker{word->lock()};
  condition.wait(locker, pred);
}

//! Notify all threads blocked in wait
template <bool kOsSpecified> inline
void notifyOneFallback(zisc::AtomicWord<kOsSpecified>* word) noexcept
{
  auto& condition = word->condition();
  condition.notify_one();
}

//! Notify a thread blocked in wait
template <bool kOsSpecified> inline
void notifyAllFallback(zisc::AtomicWord<kOsSpecified>* word) noexcept
{
  auto& condition = word->condition();
  condition.notify_all();
}

#if defined(Z_LINUX)
inline
auto futex(int* addr, const int futex_op, const int val)
{
  static_assert(sizeof(int) == 4, "'int' must be 4 bytes length and aligned.");
  auto result = syscall(SYS_futex, addr, futex_op, val, nullptr, nullptr, 0);
  return result;
}
#endif // Z_LINUX

} // namespace

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] word No description.
  \param [in] old No description.
  */
template <>
void Atomic::wait<true>(AtomicWord<true>* word, const int old) noexcept
{
#if defined(Z_WINDOWS)
  do {
    PVOID addr = std::addressof(word->get());
    PVOID comp = treatAs<PVOID>(const_cast<int*>(std::addressof(old)));
    const auto result = WaitOnAddress(addr, comp, sizeof(old), INFINITE);
    static_cast<void>(result);
  } while (word->get() == old);
#elif defined(Z_LINUX)
  do {
    int* addr = std::addressof(word->get());
    const auto result = ::futex(addr, FUTEX_WAIT_PRIVATE, old);
    static_cast<void>(result);
  } while (word->get() == old);
#else
  ::waitFallback(word, old);
#endif
}

/*!
  \details No detailed description

  \param [in,out] word No description.
  \param [in] old No description.
  */
template <>
void Atomic::wait<false>(AtomicWord<false>* word, const int old) noexcept
{
  ::waitFallback(word, old);
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
  int* addr = std::addressof(word->get());
  constexpr int n = 1;
  const auto result = ::futex(addr, FUTEX_WAKE_PRIVATE, n);
  static_cast<void>(result);
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
  int* addr = std::addressof(word->get());
  constexpr int n = std::numeric_limits<int>::max();
  const auto result = ::futex(addr, FUTEX_WAKE_PRIVATE, n);
  static_cast<void>(result);
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

} // namespace zisc
