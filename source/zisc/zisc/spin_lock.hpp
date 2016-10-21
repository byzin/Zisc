/*!
  \file spin_lock.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SPIN_LOCK_HPP
#define ZISC_SPIN_LOCK_HPP

namespace zisc {

// Standard C++ library
#include <atomic>
// Zisc
#include "non_copyable.hpp"

/*!
  \brief SpinLock class provides spin lock functions
  \details
  See the following URL
  http://en.cppreference.com/w/cpp/atomic/atomic_flag
  */
class SpinLock : public NonCopyable<SpinLock>
{
 public:
  //! Lock the locker
  SpinLock(std::atomic_flag* locker) noexcept;

  //! Unlock the locker
  ~SpinLock();

 private:
  //! Lock the locker
  void lock() noexcept;

  //! Unlock the locker
  void unlock() noexcept;


  std::atomic_flag* locker_;
};

} // namespace zisc

#include "spin_lock-inl.hpp"

#endif // ZISC_SPIN_LOCK_HPP
