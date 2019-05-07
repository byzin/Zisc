/*!
  \file spin_lock_mutex-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SPIN_LOCK_MUTEX_INL_HPP
#define ZISC_SPIN_LOCK_MUTEX_INL_HPP

#include "spin_lock_mutex.hpp"
// Standard C++ library
#include <atomic>
#include <thread>
// Zisc
#include "error.hpp"

namespace zisc {

/*!
  */
inline
SpinLockMutex::SpinLockMutex() noexcept
{
}

/*!
  */
inline
SpinLockMutex::~SpinLockMutex() noexcept
{
}

/*!
  */
inline
void SpinLockMutex::lock() noexcept
{
  while (!tryLock()) {
    ; // spin lock
  }
}

/*!
  */
inline
bool SpinLockMutex::try_lock() noexcept
{
  return tryLock();
}

/*!
  */
inline
bool SpinLockMutex::tryLock() noexcept
{
  return !locker_.test_and_set(std::memory_order_acquire);
}

/*!
  */
inline
void SpinLockMutex::unlock() noexcept
{
  locker_.clear(std::memory_order_release);
}

} // namespace zisc

#endif // ZISC_SPIN_LOCK_MUTEX_INL_HPP
