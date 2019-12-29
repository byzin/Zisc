/*!
  \file spin_lock_mutex_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <mutex>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/simple_memory_resource.hpp"
#include "zisc/spin_lock_mutex.hpp"
#include "zisc/thread_manager.hpp"
#include "zisc/zisc_config.hpp"

TEST(SpinLockMutexTest, LockTest)
{
  zisc::SpinLockMutex locker{};
  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{&mem_resource};

  zisc::int64b value = 0;
  const zisc::int64b a = 5;
  auto test = [&locker, &value, &a](const zisc::uint /* thread_id */, const zisc::int64b /* i */)
  {
    std::unique_lock<zisc::SpinLockMutex> l{locker};
    value += a;
  };

  constexpr zisc::int64b start = 0;
  constexpr zisc::int64b end = 131072;
  auto result = thread_manager.enqueueLoop(test, start, end, &mem_resource);
  result->wait();

  const zisc::int64b expected = end * a;
  ASSERT_EQ(expected, value) << "The concurrency of spin lock isn't guaranteed.";
}
