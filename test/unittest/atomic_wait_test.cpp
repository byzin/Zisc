/*!
  \file atomic_wait_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \note No notation.
  \attention No attention.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>
#include <type_traits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/atomic.hpp"
#include "zisc/zisc_config.hpp"

namespace {

template <bool specialization>
void testWaitNotification(zisc::AtomicWord<specialization>* word)
{
  using Clock = std::chrono::high_resolution_clock;

  word->set(0);

  auto job = [word]() noexcept
  {
    auto to_millisec = [](const Clock::duration t)
    {
      return std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
    };
    auto start_time = Clock::now();
    zisc::Atomic::wait(word, 0);
    auto current_time = Clock::now();
    auto elapsed_time = to_millisec(current_time - start_time);
    std::cout << "Elapsed time: " << elapsed_time << " ms" << std::endl;
    ASSERT_GT(elapsed_time, 1200);
    zisc::Atomic::wait(word, 1);
    current_time = Clock::now();
    elapsed_time = to_millisec(current_time - start_time);
    std::cout << "Elapsed time: " << elapsed_time << " ms" << std::endl;
    ASSERT_GT(elapsed_time, 2400);
  };
  std::thread job_thread{job};

  const std::chrono::milliseconds one_sec{1000};
  const std::chrono::milliseconds h{250};
  std::this_thread::sleep_for(one_sec);
  zisc::Atomic::notifyOne(word);
  std::this_thread::sleep_for(h);
  word->set(1);
  zisc::Atomic::notifyOne(word);
  std::this_thread::sleep_for(one_sec);
  zisc::Atomic::notifyAll(word);
  std::this_thread::sleep_for(h);
  word->set(2);
  zisc::Atomic::notifyAll(word);

  job_thread.join();
}

} // namespace

TEST(AtomicTest, WaitNotificationTest)
{
  zisc::AtomicWord<false> word{};
  std::cout << "OS specialized: " << word.isSpecialized() << std::endl;
  ::testWaitNotification(std::addressof(word));
}

TEST(AtomicTest, WaitNotificationOsTest)
{
  zisc::AtomicWord<true> word{};
  std::cout << "OS specialized: " << word.isSpecialized() << std::endl;
  ::testWaitNotification(std::addressof(word));
}