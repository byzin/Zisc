/*!
  \file spin_lock_mutex.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
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
  //! Construct the mutex. The mutex is in unlocked state.
  SpinLockMutex() noexcept;

  //!
  ~SpinLockMutex() noexcept;


  //! Lock the mutex
  void lock() noexcept;

  //! Try to lock the mutex for STL compatible
  bool try_lock() noexcept;

  //! Try to lock the mutex
  bool tryLock() noexcept;

  //! Unlock the mutex
  void unlock() noexcept;

 private:
  std::atomic_flag locker_ = ATOMIC_FLAG_INIT;
};

} // namespace zisc

#include "spin_lock_mutex-inl.hpp"

#endif // ZISC_SPIN_LOCK_MUTEX_HPP
