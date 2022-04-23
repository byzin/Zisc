/*!
  \file concurrent_queue_test-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_CONCURRENT_QUEUE_TEST_INL_HPP
#define TEST_CONCURRENT_QUEUE_TEST_INL_HPP

#include "concurrent_queue_test.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/hash/fnv_1a_hash_engine.hpp"
#include "zisc/structure/queue.hpp"

namespace test {

/*!
  \details No detailed description

  \tparam QueueClass No description.
  \param [in] num_of_threads No description.
  \param [in] num_of_samples No description.
  \param [in] num_of_rounds No description.
  \param [in] sampler_seed No description.
  \param [out] queue No description.
  */
template <typename QueueClass> inline
void QueueTest::testConcurrentThroughputOp(
    const std::size_t num_of_threads,
    const std::size_t num_of_samples,
    const std::size_t num_of_rounds,
    const zisc::uint64b sampler_seed,
    zisc::Queue<QueueClass, zisc::uint64b>* queue)
{
  using zisc::uint64b;
  using Queue = std::remove_cvref_t<decltype(*queue)>;
  static_assert(Queue::isConcurrent(), "The queue doesn't support concurrency.");

  std::mt19937_64 sampler{sampler_seed};
  for (std::size_t round = 0; round < num_of_rounds; ++round) {
    queue->clear();
    ASSERT_EQ(0, queue->size()) << "The queue isn't cleared.";

    constexpr std::size_t batch_size = 64;

    //
    std::vector<zisc::uint8b> value_list;
    value_list.resize(num_of_samples, 0);
    std::atomic_int worker_lock{-1};
    std::atomic_size_t count{0};
    std::vector<std::thread> worker_list;
    worker_list.reserve(num_of_threads);
    // Enqueue test
    std::vector<uint64b> input_list;
    input_list.resize(num_of_samples);
    std::iota(input_list.begin(), input_list.end(), 0);
    std::shuffle(input_list.begin(), input_list.end(), sampler);
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      const auto& inputs = input_list;
      auto& values = value_list;
      worker_list.emplace_back([num_of_samples, &inputs, queue, &values, &count, &worker_lock]()
      {
        // Wait this thread until all threads become ready
        worker_lock.wait(-1, std::memory_order::acquire);
        // Do the actual test
        while (true) {
          const std::size_t index = count.fetch_add(1, std::memory_order::acq_rel);
          const std::size_t begin = index * batch_size;
          const std::size_t end = (std::min)((index + 1) * batch_size, num_of_samples);
          if (end <= begin)
            break;
          const std::size_t n = end - begin;
          std::for_each_n(inputs.begin() + begin, n, [queue, &values](const uint64b& in)
          {
            const std::optional<std::size_t> result = queue->enqueue(in);
            ASSERT_TRUE(result.has_value()) << "Enqueueing failed. input = " << in;
            values[in] = 1;
          });
        }
      });
    }

    // Start the test, notify all threads
    worker_lock.store(zisc::cast<int>(worker_list.size()));
    worker_lock.notify_all();
    // Wait the test done
    std::for_each_n(worker_list.begin(), num_of_threads, [](std::thread& w){w.join();});
    worker_list.clear();
    // Check result
    for (std::size_t i = 0; i < value_list.size(); ++i)
      ASSERT_EQ(1, value_list[i]) << "Enqueueing failed. input = " << i;
    ASSERT_EQ(num_of_samples, queue->size()) << "Enqueueing failed.";

    // Dequeue test
    std::fill(value_list.begin(), value_list.end(), 0);
    worker_lock.store(-1, std::memory_order::release);
    count.store(0, std::memory_order::release);
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      auto& values = value_list;
      worker_list.emplace_back([num_of_samples, queue, &values, &count, &worker_lock]()
      {
        // Wait this thread until all threads become ready
        worker_lock.wait(-1, std::memory_order::acquire);
        // Do the actual test
        while (true) {
          const std::size_t index = count.fetch_add(1, std::memory_order::acq_rel);
          const std::size_t begin = index * batch_size;
          const std::size_t end = (std::min)((index + 1) * batch_size, num_of_samples);
          if (end <= begin)
            break;
          for (std::size_t i = begin; i < end; ++i) {
            const std::optional<uint64b> result = queue->dequeue();
            ASSERT_TRUE(result.has_value()) << "Dequeuing failed.";
            values[*result] = 1;
          }
        }
      });
    }

    // Start the test, notify all threads
    worker_lock.store(zisc::cast<int>(worker_list.size()));
    worker_lock.notify_all();
    // Wait the test done
    std::for_each_n(worker_list.begin(), num_of_threads, [](std::thread& w){w.join();});
    worker_list.clear();
    // Check result
    for (std::size_t i = 0; i < value_list.size(); ++i)
      ASSERT_EQ(1, value_list[i]) << "Dequeuing failed. input = " << i;
    ASSERT_EQ(0, queue->size()) << "The queue isn't empty.";
  }
}

/*!
  \details No detailed description

  \tparam QueueClass No description.
  \param [in] num_of_threads No description.
  \param [in] num_of_samples No description.
  \param [in] num_of_rounds No description.
  \param [in] trial_time No description.
  \param [in] sampler_seed No description.
  \param [out] queue No description.
  */
template <typename QueueClass> inline
void QueueTest::testConcurrentThroughputTime(
    const std::size_t num_of_threads,
    const std::size_t num_of_samples,
    const std::size_t num_of_rounds,
    const zisc::int64b trial_time,
    const zisc::uint64b sampler_seed,
    zisc::Queue<QueueClass, zisc::uint64b>* queue)
{
  using zisc::uint64b;
  using Queue = std::remove_cvref_t<decltype(*queue)>;
  static_assert(Queue::isConcurrent(), "The queue doesn't support concurrency.");

  std::mt19937_64 sampler{sampler_seed};
  double average_throughput = 0.0;
  for (std::size_t round = 0; round < num_of_rounds; ++round) {
    queue->clear();
    ASSERT_EQ(0, queue->size()) << "The queue isn't cleared.";

    // Initialize the queue with random numbers
    initQueue(num_of_samples / 2, sampler, queue);
    ASSERT_EQ(num_of_samples / 2, queue->size()) << "The queue isn't initialized.";

    std::vector<std::mt19937_64> sampler_list;
    sampler_list.reserve(num_of_threads);
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      const std::size_t t = num_of_threads * round + i;
      const uint64b sampler_seed = zisc::Fnv1aHash64::hash(zisc::cast<uint64b>(t));
      sampler_list.emplace_back(sampler_seed);
    }

    //
    std::vector<std::size_t> total_list;
    total_list.resize(num_of_threads, 0);
    std::atomic_int worker_lock{-1};
    std::vector<std::thread> worker_list;
    worker_list.reserve(num_of_threads);
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      worker_list.emplace_back([trial_time, i, queue, &sampler_list, &total_list, &worker_lock]()
      {
        std::mt19937_64& sampler = sampler_list[i];
        // Wait this thread until all threads become ready
        worker_lock.wait(-1, std::memory_order::acquire);
        // Do the actual test
        testConcurrentThroughputTimeImpl(trial_time, i, queue, &sampler, &total_list);
      });
    }

    // Start the test, notify all threads
    const auto start_time = Clock::now();
    worker_lock.store(zisc::cast<int>(worker_list.size()));
    worker_lock.notify_all();

    // Wait the test done
    std::for_each_n(worker_list.begin(), num_of_threads, [](std::thread& w){w.join();});
    const auto end_time = Clock::now();

    {
      const std::chrono::microseconds time = calcElapsedTime(start_time, end_time);
      const std::size_t total_op = std::reduce(total_list.begin(), total_list.end());
      const double throughput = calcMops(total_op, time);
      std::cout << "  [" << round << "] throughput=" << std::scientific << throughput
                << " (Mop/s)." << std::endl;
      average_throughput += throughput;
    }
  }
  average_throughput /= zisc::cast<double>(num_of_rounds);
  std::cout << "average throughput=" << std::scientific << average_throughput
            << " (Mop/s)." << std::endl;
}

/*!
  \details No detailed description

  \tparam QueueClass No description.
  \param [in] num_of_samples No description.
  \param [in] sampler_seed No description.
  \param [out] queue No description.
  */
template <typename QueueClass> inline
void QueueTest::initQueue(const std::size_t num_of_samples,
                          std::mt19937_64& sampler,
                          zisc::Queue<QueueClass, zisc::uint64b>* queue)
{
  for (std::size_t i = 0; i < num_of_samples; ++i) {
    const zisc::uint64b value = sampler();
    const std::optional<std::size_t> result = queue->enqueue(value);
    ASSERT_TRUE(result.has_value()) << "Initializing a queue failed.";
  }
}

/*!
  \details No detailed description

  \tparam QueueClass No description.
  \param [in] trial_time No description.
  \param [in] i No description.
  \param [in] sampler No description.
  \param [out] queue No description.
  \param [out] total_list No description.
  */
template <typename QueueClass>
void QueueTest::testConcurrentThroughputTimeImpl(
    const zisc::int64b trial_time,
    const std::size_t i,
    zisc::Queue<QueueClass, zisc::uint64b>* queue,
    std::mt19937_64* sampler,
    std::vector<std::size_t>* total_list)
{
  using Clock = std::chrono::system_clock;
  using zisc::int64b;
  using zisc::uint64b;

  const std::chrono::milliseconds trial_time_ms{trial_time};
  const auto time_max = std::chrono::duration_cast<Clock::duration>(trial_time_ms);

  const auto start_time = Clock::now();
  std::size_t count = 0;
  std::size_t total = 0;
  uint64b value = 0;
  while (true) {
    // Every once in a while check if time is over
    if (count == 100) {
      count = 0;
      const auto current_time = Clock::now();
      const int64b elapsed_time = calcElapsedTime(start_time, current_time).count();
      if (time_max.count() < elapsed_time) {
        (*total_list)[i] = total;
        break;
      }
    }
    const uint64b op = (*sampler)() % 2;
    if (op == 1) {
      const uint64b v = value++;
      [[maybe_unused]] const std::optional<std::size_t> result = queue->enqueue(v);
    }
    else {
      [[maybe_unused]] const std::optional<uint64b> result = queue->dequeue();
    }
    ++count;
    ++total;
  }
}

} /* namespace test */

#endif /* TEST_CONCURRENT_QUEUE_TEST_INL_HPP */
