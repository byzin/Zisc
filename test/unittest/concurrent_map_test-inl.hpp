/*!
  \file concurrent_map_test-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_CONCURRENT_MAP_TEST_INL_HPP
#define TEST_CONCURRENT_MAP_TEST_INL_HPP

#include "concurrent_map_test.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <limits>
#include <numeric>
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
#include "zisc/structure/map.hpp"

namespace test {

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t Zipfian::numOfItemsMax() noexcept
{
  return (std::numeric_limits<std::size_t>::max)();
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t Zipfian::numOfItemsMin() noexcept
{
  return 2;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr double Zipfian::zipfianConst() noexcept
{
  return 0.75;
}

/*!
  \details No detailed description

  \tparam MapClass No description.
  \param [in] num_of_threads No description.
  \param [in] num_of_samples No description.
  \param [in] num_of_keys No description.
  \param [in] num_of_rounds No description.
  \param [in] use_sparse No description.
  \param [in] use_zipfian No description.
  \param [in] zipfian_param No description.
  \param [out] map No description.
  \return No description
  */
template <typename MapClass> inline
void MapTest::testConcurrentThroughputOp(
    const std::size_t num_of_threads,
    const std::size_t num_of_samples,
    const std::size_t num_of_keys,
    const std::size_t num_of_rounds,
    const zisc::uint64b sampler_seed,
    const bool use_sparse,
    const bool use_zipfian,
    const double zipfian_param,
    zisc::Map<MapClass, zisc::uint64b>* map)
{
  using zisc::uint64b;
  using Map = std::remove_cvref_t<decltype(*map)>;
  static_assert(Map::isConcurrent(), "The tree doesn't support concurrency.");
  std::mt19937_64 sampler{sampler_seed};
  const auto [source_list, input_list] = generateMapInputList(num_of_samples,
                                                              num_of_keys,
                                                              use_sparse,
                                                              use_zipfian,
                                                              zipfian_param,
                                                              sampler);
  for (std::size_t round = 0; round < num_of_rounds; ++round) {
    // map->clear(); The test must work without the clear
    ASSERT_EQ(0, map->size()) << "The search tree isn't cleared.";

    constexpr std::size_t batch_size = 64;
    // 
    std::atomic_int worker_lock{-1};
    std::atomic_size_t count{0};
    std::vector<std::thread> worker_list;
    worker_list.reserve(num_of_threads);
    // Insertion test
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      const auto& inputs = input_list;
      worker_list.emplace_back([num_of_samples, &inputs, map, &count, &worker_lock]()
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
          std::for_each_n(inputs.begin() + begin, n, [map](const uint64b& in)
          {
            [[maybe_unused]] const auto result = map->add(in);
          });
        }
      });
    }
    // Start the test, notify all threads
    worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
    worker_lock.notify_all();
    // Wait the test done
    std::for_each_n(worker_list.begin(), num_of_threads, [](std::thread& w){w.join();});
    worker_list.clear();

    // Search test
    auto sequence = input_list;
    std::shuffle(sequence.begin(), sequence.end(), sampler);
    worker_lock.store(-1, std::memory_order::release);
    count.store(0, std::memory_order::release);
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      const auto& inputs = sequence;
      worker_list.emplace_back([num_of_samples, &inputs, map, &count, &worker_lock]()
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
          std::for_each_n(inputs.begin() + begin, n, [map](const uint64b& in)
          {
            const std::optional<std::size_t> result = map->contain(in);
            ASSERT_TRUE(result.has_value()) << "Key not found, input=" << in;
            const std::size_t index = *result;
            ASSERT_EQ(in, map->get(index)) << "Map construction failed, input=" << in;
          });
        }
      });
    }
    // Find min value
    {
      const auto expected = std::min_element(sequence.begin(), sequence.end());
      const auto result = map->findMinKey();
      ASSERT_TRUE(result.has_value()) << "Finding min element failed.";
      ASSERT_EQ(*expected, **result) << "Finding min element failed.";
    }
    // Start the test, notify all threads
    worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
    worker_lock.notify_all();
    // Wait the test done
    std::for_each_n(worker_list.begin(), num_of_threads, [](std::thread& w){w.join();});
    worker_list.clear();

    // Delete test
    std::shuffle(sequence.begin(), sequence.end(), sampler);
    worker_lock.store(-1, std::memory_order::release);
    count.store(0, std::memory_order::release);
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      const auto& inputs = sequence;
      worker_list.emplace_back([num_of_samples, &inputs, map, &count, &worker_lock]()
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
          std::for_each_n(inputs.begin() + begin, n, [map](const uint64b& in)
          {
            [[maybe_unused]] const auto result = map->remove(in);
          });
        }
      });
    }
    // Start the test, notify all threads
    worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
    worker_lock.notify_all();
    // Wait the test done
    std::for_each_n(worker_list.begin(), num_of_threads, [](std::thread& w){w.join();});
    ASSERT_EQ(0, map->size()) << "The search tree isn't empty.";
  }
}

/*!
  \details No detailed description

  \tparam MapClass No description.
  \param [in] num_of_threads No description.
  \param [in] num_of_samples No description.
  \param [in] num_of_keys No description.
  \param [in] num_of_rounds No description.
  \param [in] trial_time No description.
  \param [in] use_sparse No description.
  \param [in] use_zipfian No description.
  \param [in] zipfian_param No description.
  \param [out] map No description.
  \return No description
  */
template <typename MapClass> inline
void MapTest::testConcurrentThroughputTime(
    const std::size_t num_of_threads,
    const std::size_t num_of_samples,
    const std::size_t num_of_keys,
    const std::size_t num_of_rounds,
    const std::size_t update_percent,
    const zisc::int64b trial_time,
    const zisc::uint64b sampler_seed,
    const bool use_sparse,
    const bool use_zipfian,
    const double zipfian_param,
    zisc::Map<MapClass, zisc::uint64b>* map)
{
  using zisc::uint64b;
  using Map = std::remove_cvref_t<decltype(*map)>;
  static_assert(Map::isConcurrent(), "The tree doesn't support concurrency.");
  std::mt19937_64 sampler{sampler_seed};
  const auto [source_list, input_list] = generateMapInputList(num_of_samples,
                                                              2 * num_of_keys,
                                                              use_sparse,
                                                              use_zipfian,
                                                              zipfian_param,
                                                              sampler);
  const auto op_list = generateMapOpList(num_of_samples, update_percent);
  double average_throughput = 0.0;
  for (std::size_t round = 0; round < num_of_rounds; ++round) {
    map->clear();
    ASSERT_EQ(0, map->size()) << "The search tree isn't cleared.";

    // Initialize the search tree with the keys
    std::for_each_n(source_list.begin(), num_of_keys, [map](const uint64b& in)
    {
      [[maybe_unused]] const auto result = map->add(in);
    });
    ASSERT_EQ(num_of_keys, map->size()) << "The search tree addition failed.";

    // 
    std::vector<std::size_t> total_list;
    total_list.resize(num_of_threads, 0);
    std::vector<zisc::int64b> added_list;
    added_list.resize(num_of_threads, 0);
    std::atomic_flag finish{};
    std::atomic_int worker_lock{-1};
    std::vector<std::thread> worker_list;
    worker_list.reserve(num_of_threads);
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      const auto& inputs = input_list;
      worker_list.emplace_back([num_of_threads, num_of_samples, trial_time, &inputs, &op_list, i, map, &total_list, &added_list, &finish, &worker_lock]()
      {
        // Wait this thread until all threads become ready
        worker_lock.wait(-1, std::memory_order::acquire);
        // Do the actual test
        testConcurrentThroughputTimeImpl(num_of_threads, num_of_samples, trial_time,
                                         inputs, op_list, i,
                                         map, &total_list, &added_list, &finish);
      });
    }

    // Start the test, notify all threads
    const auto start_time = Clock::now();
    worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
    worker_lock.notify_all();

    // Wait the test done
    std::for_each_n(worker_list.begin(), num_of_threads, [](std::thread& w){w.join();});
    const auto end_time = Clock::now();

    if (finish.test(std::memory_order::acquire)) {
      const zisc::int64b time = calcElapsedTime(start_time, end_time).count() / 1000;
      std::cout << "  [Warning] out of samples, finished in " << time << " ms."
                << std::endl;
    }
    {
      const zisc::int64b updates = std::reduce(added_list.begin(), added_list.end());
      ASSERT_EQ(num_of_keys + updates, map->size()) << "Search tree operation failed.";
    }
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
  std::cout << "  avg throughput=" << std::scientific << average_throughput
            << " (Mop/s)." << std::endl;
}

/*!
  \details No detailed description

  \tparam MapClass No description.
  \param [in] num_of_threads No description.
  \param [in] num_of_samples No description.
  \param [in] trial_time No description.
  \param [in] input_list No description.
  \param [in] op_list No description.
  \param [in] i No description.
  \param [out] map No description.
  \param [out] total_list No description.
  \param [out] added_list No description.
  \param [out] finish No description.
  */
template <typename MapClass> inline
void MapTest::testConcurrentThroughputTimeImpl(
    const std::size_t num_of_threads,
    const std::size_t num_of_samples,
    const zisc::int64b trial_time,
    const std::vector<zisc::uint64b>& input_list,
    const std::vector<Operation>& op_list,
    const std::size_t i,
    zisc::Map<MapClass, zisc::uint64b>* map,
    std::vector<std::size_t>* total_list,
    std::vector<zisc::int64b>* added_list,
    std::atomic_flag* finish)
{
  using Clock = std::chrono::system_clock;
  using zisc::int64b;
  using zisc::uint64b;

  const std::chrono::milliseconds trial_time_ms{trial_time};
  const auto time_max = std::chrono::duration_cast<Clock::duration>(trial_time_ms);

  const auto start_time = Clock::now();
  const std::size_t mp = num_of_samples / num_of_threads;
  int64b added = 0;
  for (std::size_t j = i * mp, count = 0, total = 0; ; ++j, ++count, ++total) {
    // Every once in a while check if time is over
    if (count == 100) {
      count = 0;
      const auto current_time = Clock::now();
      const int64b elapsed_time = calcElapsedTime(start_time, current_time).count();
      if (time_max.count() < elapsed_time || finish->test(std::memory_order::acquire)) {
        (*total_list)[i] = total;
        (*added_list)[i] = added;
        break;
      }
    }
    // Quit early if out of samples
    if (j == (i + 1) * mp) {
      finish->test_and_set(std::memory_order::acq_rel);
      (*total_list)[i] = total;
      (*added_list)[i] = added;
      break;
    }
    const zisc::uint64b value = input_list[j];
    switch (op_list[j]) {
     case Operation::kAdd: {
      const std::optional<std::size_t> result = map->add(value);
      if (result.has_value())
        ++added;
      break;
     }
     case Operation::kRemove: {
      const std::optional<std::size_t> result = map->remove(value);
      if (result.has_value())
        --added;
      break;
     }
     case Operation::kContain: {
      [[maybe_unused]] const auto result = map->contain(value);
      break;
     }
     default:
      break;
    }
  }
}

} /* namespace test */

#endif /* TEST_CONCURRENT_MAP_TEST_INL_HPP */
