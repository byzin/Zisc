/*!
  \file spin_lock_mutex_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <mutex>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/concurrency/spin_lock_mutex.hpp"
#include "zisc/concurrency/thread_manager.hpp"
#include "zisc/memory/alloc_free_resource.hpp"

TEST(SpinLockMutexTest, LockTest)
{
  zisc::SpinLockMutex locker{};
  zisc::AllocFreeResource mem_resource;
  zisc::ThreadManager thread_manager{&mem_resource};

  zisc::int64b value = 0;
  static constexpr zisc::int64b a = 5;
  auto test = [&locker, &value](const zisc::int64b /* i */, const zisc::int64b /* thread_id */)
  {
    const std::unique_lock<zisc::SpinLockMutex> l{locker};
    value += a;
  };

  constexpr zisc::int64b start = 0;
  constexpr zisc::int64b end = 4 * 1310720;
  thread_manager.setCapacity(end);
  auto result = thread_manager.enqueueLoop(test, start, end);
  result.wait();

  const zisc::int64b expected = end * a;
  ASSERT_EQ(expected, value) << "The concurrency of spin lock isn't guaranteed.";
}
