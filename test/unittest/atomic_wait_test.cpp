/*!
  \file atomic_wait_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \note No notation.
  \attention No attention.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>
#include <type_traits>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/concurrency/atomic.hpp"
#include "zisc/concurrency/atomic_word.hpp"

namespace {

template <bool specialization>
void testWaitNotification(zisc::AtomicWord<specialization>* word)
{
  using Clock = std::chrono::high_resolution_clock;

  word->store(0, std::memory_order::release);

  auto job = [word]() noexcept
  {
    auto to_millisec = [](const Clock::duration t)
    {
      return std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
    };
    auto start_time = Clock::now();
    zisc::atomic_wait(word, 0, std::memory_order::acquire);
    auto current_time = Clock::now();
    auto elapsed_time = to_millisec(current_time - start_time);
    std::cout << "Elapsed time: " << elapsed_time << " ms" << std::endl;
    ASSERT_GT(elapsed_time, 1200);
    zisc::atomic_wait(word, 1, std::memory_order::acquire);
    current_time = Clock::now();
    elapsed_time = to_millisec(current_time - start_time);
    std::cout << "Elapsed time: " << elapsed_time << " ms" << std::endl;
    ASSERT_GT(elapsed_time, 2400);
  };
  std::thread job_thread{job};

  const std::chrono::milliseconds one_sec{1000};
  const std::chrono::milliseconds h{250};
  std::this_thread::sleep_for(one_sec);
  zisc::atomic_notify_one(word);
  std::this_thread::sleep_for(h);
  word->store(1, std::memory_order::release);
  zisc::atomic_notify_one(word);
  std::this_thread::sleep_for(one_sec);
  zisc::atomic_notify_all(word);
  std::this_thread::sleep_for(h);
  word->store(2, std::memory_order::release);
  zisc::atomic_notify_all(word);

  job_thread.join();
}

} // namespace

TEST(AtomicTest, WaitNotificationTest)
{
  zisc::AtomicWord<false> word{};
  std::cout << "OS specialized: " << decltype(word)::isSpecialized() << std::endl;
  std::cout << "AtomicWord alignment: "
            << std::alignment_of_v<zisc::AtomicWord<false>> << std::endl;
  std::cout << "AtomicWord size     : "
            << sizeof(zisc::AtomicWord<false>) << std::endl;
  ::testWaitNotification(std::addressof(word));
}

TEST(AtomicTest, WaitNotificationOsTest)
{
  zisc::AtomicWord<true> word{};
  std::cout << "OS specialized: " << decltype(word)::isSpecialized() << std::endl;
  std::cout << "AtomicWord alignment: "
            << std::alignment_of_v<zisc::AtomicWord<true>> << std::endl;
  std::cout << "AtomicWord size     : "
            << sizeof(zisc::AtomicWord<true>) << std::endl;
  ::testWaitNotification(std::addressof(word));
}
