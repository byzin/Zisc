/*!
  \file spin_lock_mutex.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SPIN_LOCK_MUTEX_HPP
#define ZISC_SPIN_LOCK_MUTEX_HPP

// Standard C++ library
#include <atomic>
// Zisc
#include "zisc/non_copyable.hpp"

namespace zisc {

/*!
  \brief SpinLockMutex class provides spin lock functions

  For more detail, please see the following link:
  <a href="http://en.cppreference.com/w/cpp/atomic/atomic_flag">std::atomic_flag</a>.
  */
class SpinLockMutex : private NonCopyable<SpinLockMutex>
{
 public:
  //! Lock the mutex
  void lock() noexcept;

  //! Try to lock the mutex for STL compatible
  auto try_lock() noexcept -> bool;

  //! Try to lock the mutex
  auto tryLock() noexcept -> bool;

  //! Unlock the mutex
  void unlock() noexcept;

 private:
  std::atomic_flag lock_state_;
};

} // namespace zisc

#include "spin_lock_mutex-inl.hpp"

#endif // ZISC_SPIN_LOCK_MUTEX_HPP
