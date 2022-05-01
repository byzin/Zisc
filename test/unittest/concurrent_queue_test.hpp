/*!
  \file concurrent_queue_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_CONCURRENT_QUEUE_TEST_HPP
#define TEST_CONCURRENT_QUEUE_TEST_HPP

// Standard C++ library
#include <chrono>
#include <cstddef>
#include <random>
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/structure/queue.hpp"

namespace test {

class QueueTest
{
 public:
  //!
  template <typename QueueClass>
  static void testConcurrentThroughputOp(
      const std::size_t num_of_threads,
      const std::size_t num_of_samples,
      const std::size_t num_of_rounds,
      const zisc::uint64b sampler_seed,
      zisc::Queue<QueueClass, zisc::uint64b>* queue);

  //!
  template <typename QueueClass>
  static void testConcurrentThroughputTime(
      const std::size_t num_of_threads,
      const std::size_t num_of_samples,
      const std::size_t num_of_rounds,
      const zisc::int64b trial_time,
      const zisc::uint64b sampler_seed,
      zisc::Queue<QueueClass, zisc::uint64b>* queue);


  static constexpr std::size_t kNumOfDefaultThreads = 128;
  static constexpr std::size_t kNumOfDefaultSamples = 20'000'000;
  static constexpr std::size_t kNumOfDefaultRounds = 4;
  static constexpr zisc::int64b kDefaultTrialTime = 10'000; // in milliseconds
  static constexpr zisc::uint64b kDefaultSamplerSeed = 123'456'789u;

 private:
  using Clock = std::chrono::system_clock;


  //
  static std::chrono::microseconds calcElapsedTime(
      const Clock::time_point start_time,
      const Clock::time_point end_time) noexcept;

  //
  static double calcMops(const std::size_t total_op,
                         const std::chrono::microseconds elapsed_time) noexcept;

  //!
  template <typename QueueClass>
  static void initQueue(const std::size_t num_of_samples,
                        std::mt19937_64& sampler,
                        zisc::Queue<QueueClass, zisc::uint64b>* queue);

  //!
  template <typename QueueClass>
  static void testConcurrentThroughputTimeImpl(
      const zisc::int64b trial_time,
      const std::size_t i,
      zisc::Queue<QueueClass, zisc::uint64b>* queue,
      std::mt19937_64* sampler,
      std::vector<std::size_t>* total_list);
};

} /* namespace test */

#include "concurrent_queue_test-inl.hpp"

#endif /* TEST_CONCURRENT_QUEUE_TEST_HPP */
