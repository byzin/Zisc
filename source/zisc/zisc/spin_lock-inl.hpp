/*!
  \file spin_lock-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SPIN_LOCK_INL_HPP
#define ZISC_SPIN_LOCK_INL_HPP

#include "spin_lock.hpp"
// Standard C++ library
#include <atomic>
#include <thread>
// Zisc
#include "error.hpp"

namespace zisc {

/*!
  */
inline
SpinLock::SpinLock(std::atomic_flag* locker) noexcept :
    locker_{locker}
{
  ZISC_ASSERT(locker_ != nullptr, "The locker is null.");
  lock();
}

/*!
  */
inline
SpinLock::~SpinLock()
{
  unlock();
}

/*!
  */
inline
void SpinLock::lock() noexcept
{
  while (locker_->test_and_set(std::memory_order_acquire)) {
    std::this_thread::yield();
  }
}

/*!
  */
inline
void SpinLock::unlock() noexcept
{
  locker_->clear(std::memory_order_release);
}

} // namespace zisc

#endif // ZISC_SPIN_LOCK_INL_HPP
