/*!
  \file spin_lock_mutex-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SPIN_LOCK_MUTEX_INL_HPP
#define ZISC_SPIN_LOCK_MUTEX_INL_HPP

#include "spin_lock_mutex.hpp"
// Standard C++ library
#include <atomic>
#include <thread>

namespace zisc {

/*!
  \details No detailed description
  */
inline
SpinLockMutex::SpinLockMutex() noexcept
{
}

/*!
  \details No detailed description
  */
inline
SpinLockMutex::~SpinLockMutex() noexcept
{
}

/*!
  \details No detailed description
  */
inline
void SpinLockMutex::lock() noexcept
{
  while (!tryLock()) {
    while (lock_state_.test(std::memory_order::relaxed)) {
      // spin lock
      std::this_thread::yield();
    }
  }
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool SpinLockMutex::try_lock() noexcept
{
  return tryLock();
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool SpinLockMutex::tryLock() noexcept
{
  return !lock_state_.test_and_set(std::memory_order::acquire);
}

/*!
  \details No detailed description
  */
inline
void SpinLockMutex::unlock() noexcept
{
  lock_state_.clear(std::memory_order::release);
}

} // namespace zisc

#endif // ZISC_SPIN_LOCK_MUTEX_INL_HPP
