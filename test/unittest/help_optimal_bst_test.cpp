/*!
  \file help_optimal_bst_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/stopwatch.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/data_structure/help_optimal_bst.hpp"
#include "zisc/memory/simple_memory_resource.hpp"
// Test
//#include "lock_free_bounded_queue_test.hpp"

TEST(HelpOptimalBstTest, ConstructorTest)
{
  using Bst = zisc::HelpOptimalBst;

  zisc::SimpleMemoryResource mem_resource;
  std::unique_ptr<Bst> bst;
  //
  {
    Bst bst1{&mem_resource};
    bst = std::make_unique<Bst>(std::move(bst1));
  }
  ASSERT_EQ(Bst::defaultCapacity(), bst->capacity())
      << "Constructing of HelpOptimalBst failed.";

  //
  std::size_t cap = 4096;
  {
    Bst bst1{cap, &mem_resource};
    bst = std::make_unique<Bst>(std::move(bst1));
  }
  ASSERT_EQ(cap, bst->capacity())
      << "Constructing of HelpOptimalBst failed.";

  //
  cap = 5000;
  {
    Bst bst1{cap, &mem_resource};
    bst = std::make_unique<Bst>(std::move(bst1));
  }
  cap = 8192;
  ASSERT_EQ(cap, bst->capacity())
      << "Constructing of HelpOptimalBst failed.";
}

TEST(HelpOptimalBstTest, OperationTest)
{
  using Bst = zisc::HelpOptimalBst;

  zisc::SimpleMemoryResource mem_resource;
  Bst bst{&mem_resource};

  std::vector<int> value_list{{7, 6, 4, 8, 2, 1, 5, 3, 15, 100, 0}};
  std::vector<int> value2_list{{10, 25000, -1}};

  for (const int value : value_list) {
    {
      const auto [result, index] = bst.add(value);
      ASSERT_TRUE(result) << "Adding value '" << value << "' into the bst failed.";
    }
    {
      const auto [result, index] = bst.add(value);
      ASSERT_FALSE(result) << "Adding value '" << value << "' into the bst failed.";
    }
  }

  for (const int value : value_list) {
    const bool result = bst.contain(value);
    ASSERT_TRUE(result) << "Quering value '" << value << "' from the bst failed.";
  }
  for (const int value : value2_list) {
    const bool result = bst.contain(value);
    ASSERT_FALSE(result) << "Quering value '" << value << "' from the bst failed.";
  }

  for (const int value : value_list) {
    const auto [result, index] = bst.add(value);
    ASSERT_FALSE(result) << "Adding value '" << value << "' into the bst failed.";
  }

  for (const int value : value_list) {
    {
      const bool result = bst.remove(value);
      ASSERT_TRUE(result) << "Removing value '" << value << "' from the bst failed.";
    }
    {
      const bool result = bst.remove(value);
      ASSERT_FALSE(result) << "Removing value '" << value << "' from the bst failed.";
    }
  }
  for (const int value : value2_list) {
    const bool result = bst.remove(value);
    ASSERT_FALSE(result) << "Removing value '" << value << "' from the bst failed.";
  }

  for (const int value : value_list) {
    const bool result = bst.contain(value);
    ASSERT_FALSE(result) << "Quering value '" << value << "' from the bst failed.";
  }

  for (const int value : value_list) {
    const bool result = bst.remove(value);
    ASSERT_FALSE(result) << "Removing value '" << value << "' from the bst failed.";
  }

  for (const int value : value_list) {
    const auto [result, index] = bst.add(value);
    ASSERT_TRUE(result) << "Adding value '" << value << "' into the bst failed.";
  }
}

TEST(HelpOptimalBstTest, AddRemoveTest)
{
  using Bst = zisc::HelpOptimalBst;

  const std::array<int, 16> candidate_list{{-171717, -1000, -1, 0, 1, 3, 4, 9, 15, 16, 100, 2000, 10300, 50000, 360000, (std::numeric_limits<int>::max)()}};

  std::mt19937_64 rand_engine{123'567'789};
  std::uniform_int_distribution<std::size_t> sampler1{0, 1};
  std::uniform_int_distribution<std::size_t> sampler2{0, candidate_list.size() - 1};

  std::map<int, bool> value_list;
  for (const auto candidate : candidate_list)
    value_list[candidate] = false;

  zisc::SimpleMemoryResource mem_resource;
  Bst bst{candidate_list.size() * 2, &mem_resource};
//  Bst bst{&mem_resource};

  constexpr std::size_t n = 1000'000;
  for (std::size_t i = 0; i < n; ++i) {
    if (i == 157)
      std::cout << "Ready." << std::endl;
    const std::size_t op = sampler1(rand_engine);

    const std::size_t index = sampler2(rand_engine);
    const int candidate = candidate_list[index];
    auto& flag = value_list[candidate];

    if (op == 0) { // Add
      auto [result, id] = bst.add(candidate);
      if (flag) {
        ASSERT_FALSE(result) << "BST add operation failed.";
      }
      else {
        ASSERT_TRUE(result) << "BST add operation failed.";
        flag = true;
      }
    }
    else { // Remove
      auto result = bst.remove(candidate);
      if (flag) {
        ASSERT_TRUE(result) << "BST remove operation failed.";
        flag = false;
      }
      else {
        ASSERT_FALSE(result) << "BST remove operation failed.";
      }
    }

    std::cout << "[" << i << "] op=" << op << ", candidate[" << index << "]=" << candidate << ", flag=" << flag << std::endl;
    bst.printTree(&std::cout);

    for (auto v : value_list) {
      if (v.second)
        ASSERT_TRUE(bst.contain(v.first)) << "BST contain(" << v.first << ") operation failed.";
      else
        ASSERT_FALSE(bst.contain(v.first)) << "BST contain(" << v.first << ") operation failed.";
    }
  }
}

//TEST(ScalableCircularQueueTest, QueueTest)
//{
//  using Queue = zisc::ScalableCircularQueue<int>;
//
//  zisc::SimpleMemoryResource mem_resource;
//  Queue q{8, &mem_resource};
//  const char* message = "Enqueuing of ScalableCircularQueue failed.";
//  ASSERT_TRUE(q.isEmpty()) << message;
//  ASSERT_TRUE(q.enqueue(7)) << message;
//  ASSERT_TRUE(q.enqueue(6)) << message;
//  ASSERT_TRUE(q.enqueue(4)) << message;
//  ASSERT_TRUE(q.enqueue(8)) << message;
//  ASSERT_TRUE(q.enqueue(2)) << message;
//  ASSERT_TRUE(q.enqueue(1)) << message;
//  ASSERT_TRUE(q.enqueue(5)) << message;
//  ASSERT_TRUE(q.enqueue(3)) << message;
//  ASSERT_EQ(8, q.size()) << message;
//
//  auto enqueue_overflow = [&q](const int value)
//  {
//    try {
//      q.enqueue(value);
//    }
//    catch (const Queue::OverflowError& error) {
//      std::cout << error.what() << " value: " << error.get() << std::endl;
//      throw;
//    }
//  };
//  ASSERT_THROW(enqueue_overflow(9), Queue::OverflowError) << message;
//
//  q.clear();
//
//  for (std::size_t i = 0; i < 65536; ++i)
//    q.dequeue();
//  ASSERT_TRUE(q.isEmpty()) << message;
//  {
//    const int v = 1;
//    ASSERT_TRUE(q.enqueue(v)) << message;
//  }
//  ASSERT_TRUE(q.enqueue(5)) << message;
//  ASSERT_TRUE(q.enqueue(3)) << message;
//  ASSERT_TRUE(q.enqueue(8)) << message;
//  ASSERT_TRUE(q.enqueue(2)) << message;
//  ASSERT_TRUE(q.enqueue(7)) << message;
//  ASSERT_TRUE(q.enqueue(6)) << message;
//  ASSERT_TRUE(q.enqueue(4)) << message;
//  ASSERT_EQ(8, q.size()) << message;
//  ASSERT_THROW(enqueue_overflow(9), Queue::OverflowError) << message;
//
//  message = "Dequeuing of ScalableCircularQueue failed.";
//  const auto check_dequeue = [message](const std::tuple<bool, int>& value,
//                                       const int expected)
//  {
//    ASSERT_TRUE(std::get<0>(value)) << message;
//    ASSERT_EQ(expected, std::get<1>(value)) << message << " Expected: " << expected;
//  };
//  check_dequeue(q.dequeue(), 1);
//  check_dequeue(q.dequeue(), 5);
//  check_dequeue(q.dequeue(), 3);
//  check_dequeue(q.dequeue(), 8);
//  check_dequeue(q.dequeue(), 2);
//  check_dequeue(q.dequeue(), 7);
//  check_dequeue(q.dequeue(), 6);
//  check_dequeue(q.dequeue(), 4);
//  {
//    auto result = q.dequeue();
//    ASSERT_FALSE(std::get<0>(result)) << message;
//  }
//  ASSERT_TRUE(q.isEmpty()) << message;
//}
//
//namespace {
//
//class Movable : public zisc::NonCopyable<Movable>
//{
// public:
//  Movable() {}
//
//  Movable(const int v) : value_{v} {}
//
//  Movable(Movable&& other) : value_{other.value_}
//  {
//    other.value_ = -1;
//  }
//
//  Movable& operator=(Movable&& other)
//  {
//    std::swap(value_, other.value_);
//    return *this;
//  }
//
//  explicit operator int() const
//  {
//    return value_;
//  }
//
// private:
//  int value_ = -1;
//};
//
//}
//
//TEST(ScalableCircularQueueTest, QueueMovableValueTest)
//{
//  using Queue = zisc::ScalableCircularQueue<::Movable>;
//
//  zisc::SimpleMemoryResource mem_resource;
//  Queue q{8, &mem_resource};
//  const char* message = "Enqueuing of ScalableCircularQueue failed.";
//  ASSERT_TRUE(q.isEmpty()) << message;
//  ASSERT_TRUE(q.enqueue(::Movable{1})) << message;
//  ASSERT_TRUE(q.enqueue(::Movable{5})) << message;
//  ASSERT_TRUE(q.enqueue(::Movable{3})) << message;
//  ASSERT_TRUE(q.enqueue(::Movable{8})) << message;
//  ASSERT_TRUE(q.enqueue(::Movable{2})) << message;
//  ASSERT_TRUE(q.enqueue(::Movable{7})) << message;
//  ASSERT_TRUE(q.enqueue(::Movable{6})) << message;
//  ASSERT_TRUE(q.enqueue(::Movable{4})) << message;
//  ASSERT_EQ(8, q.size()) << message;
//
//  auto enqueue_overflow = [&q](const int value)
//  {
//    try {
//      q.enqueue(::Movable{value});
//    }
//    catch (Queue::OverflowError& error) {
//      auto r = std::move(error.get());
//      std::cout << error.what() << " value: " << zisc::cast<int>(r) << std::endl;
//      throw;
//    }
//  };
//  ASSERT_THROW(enqueue_overflow(9), Queue::OverflowError) << message;
//
//  message = "Dequeuing of ScalableCircularQueue failed.";
//  const auto check_dequeue = [message](const std::tuple<bool, ::Movable>& value,
//                                       const int expected)
//  {
//    ASSERT_TRUE(std::get<0>(value)) << message;
//    const ::Movable& v = std::get<1>(value);
//    ASSERT_EQ(expected, zisc::cast<int>(v)) << message << " Expected: " << expected;
//  };
//  check_dequeue(q.dequeue(), 1);
//  check_dequeue(q.dequeue(), 5);
//  check_dequeue(q.dequeue(), 3);
//  check_dequeue(q.dequeue(), 8);
//  check_dequeue(q.dequeue(), 2);
//  check_dequeue(q.dequeue(), 7);
//  check_dequeue(q.dequeue(), 6);
//  check_dequeue(q.dequeue(), 4);
//  {
//    auto result = q.dequeue();
//    ASSERT_FALSE(std::get<0>(result)) << message;
//  }
//  ASSERT_TRUE(q.isEmpty()) << message;
//}
//
//TEST(ScalableCircularQueueTest, MultiThreadTest)
//{
//  using Queue = zisc::ScalableCircularQueue<std::size_t>;
//
//  static constexpr std::size_t num_of_threads = 1 << 5;
//  static constexpr std::size_t works_per_thread = 1 << 19; 
//  static constexpr std::size_t num_of_works = num_of_threads * works_per_thread;
//  std::cout << "Num of threads: " << num_of_threads << std::endl;
//  std::cout << "Num of elements: " << num_of_works << std::endl;
//
//  zisc::SimpleMemoryResource mem_resource;
//  Queue q{num_of_works, &mem_resource};
//
//  ASSERT_EQ(num_of_works, q.capacity())
//      << "Changing the size of the queue of ScalableCircularQueue is failed.";
//
//  std::vector<std::thread> workers;
//  workers.reserve(num_of_threads);
//
//  // Test multiple producer
//  zisc::Stopwatch stopwatch;
//  std::atomic_size_t counter = 0;
//  auto enqueue_job = [&q, &counter]()
//  {
//    const std::size_t id = counter++;
//    for (std::size_t i = 0; i < works_per_thread; ++i) {
//      const std::size_t value = id * works_per_thread + i;
//      q.enqueue(value);
//    }
//  };
//  stopwatch.start();
//  for (std::size_t i = 0; i < num_of_threads; ++i)
//    workers.emplace_back(enqueue_job);
//  for (std::size_t i = 0; i < num_of_threads; ++i)
//    workers[i].join();
//  auto elapsed_time = stopwatch.elapsedTime();
//  {
//    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time);
//    std::cout << "Multiple producer time: " << millis.count() << " ms" << std::endl;
//  }
//  stopwatch.stop();
//  workers.clear();
//
//  // Validate the enqueing results
//  ASSERT_EQ(num_of_works, q.size()) << "Multiple producer test failed.";
//  std::vector<std::size_t> results;
//  {
//    results.resize(num_of_works);
//    const auto& data = q.data();
//    std::copy(data.begin(), data.end(), results.begin());
//    std::sort(results.begin(), results.end());
//    for (std::size_t i = 0; i < num_of_works; ++i)
//      ASSERT_EQ(i, results[i]) << "Multiple producer test failed.";
//  }
//
//  // Test multiple consumer
//  std::fill(results.begin(), results.end(), 0);
//  auto dequeue_job = [&q, &results]() noexcept
//  {
//    for (std::size_t i = 0; i < works_per_thread; ++i) {
//      const auto result = q.dequeue();
//      const bool flag = std::get<0>(result);
//      const std::size_t value = std::get<1>(result);
//      if (flag)
//        results[value] = 1;
//    }
//  };
//  stopwatch.start();
//  for (std::size_t i = 0; i < num_of_threads; ++i)
//    workers.emplace_back(dequeue_job);
//  for (std::size_t i = 0; i < num_of_threads; ++i)
//    workers[i].join();
//  elapsed_time = stopwatch.elapsedTime();
//  {
//    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time);
//    std::cout << "Multiple consumer time: " << millis.count() << " ms" << std::endl;
//  }
//
//  // Valudate the dequeuing results
//  ASSERT_TRUE(q.isEmpty()) << "Multiple consumer test failed.";
//  for (std::size_t i = 0; i < num_of_works; ++i)
//    ASSERT_TRUE(results[i]) << "Multiple consumer test failed.";
//}
//
//TEST(ScalableCircularQueueTest, ConcurrencyTest)
//{
//  constexpr std::size_t num_of_threads = 64;
//  constexpr std::size_t num_of_thread_tasks = 0b1u << 21;
//  constexpr std::size_t num_of_tasks = num_of_threads * num_of_thread_tasks;
//  using zisc::uint64b;
//  using zisc::uint32b;
//  using Queue = zisc::ScalableCircularQueue<uint64b>;
//  using QTest = LockFreeBoundedQueueTest<Queue, num_of_threads, num_of_thread_tasks>;
//  static_assert(num_of_tasks <= Queue::capacityMax());
//
//  zisc::SimpleMemoryResource mem_resource;
//  Queue q{&mem_resource};
//  q.setCapacity(num_of_tasks);
//
//  uint64b elapsed_time = 0;
//  bool result = QTest::testEnqueue(q, elapsed_time);
//  ASSERT_TRUE(result) << "The enqueue operation in multi-thread failed.";
//  ASSERT_EQ(num_of_tasks, q.size());
//  std::cout << "Multiple producer time: " << elapsed_time << " ms" << std::endl;
//
//  {
//    const auto& elems = q.data();
//    std::vector<uint64b> indices;
//    indices.resize(elems.size(), 0);
//    std::copy(elems.begin(), elems.end(), indices.begin());
//    std::sort(indices.begin(), indices.end());
//    [[maybe_unused]] auto end = std::unique(indices.begin(), indices.end());
//    auto ite = indices.begin();
//    for (std::size_t index = 0; index < num_of_tasks; ++index) {
//      ASSERT_EQ(index, *ite) << "Enqueue concurrency test failed.";
//      ++ite;
//    }
//  }
//
//  std::vector<uint32b> data;
//  data.resize(num_of_tasks, 0);
//  result = QTest::testDequeue(q, data, elapsed_time);
//  ASSERT_TRUE(result) << "The dequeue operation in multi-thread failed.";
//  ASSERT_EQ(0, q.size());
//  std::cout << "Multiple consumer time: " << elapsed_time << " ms" << std::endl;
//  for (auto flag : data) {
//    ASSERT_TRUE(flag) << "Dequeue concurrency test failed.";
//  }
//}
//
//TEST(ScalableCircularQueueTest, ConcurrencyLoopTest)
//{
//  constexpr std::size_t num_of_threads = 64;
//  constexpr std::size_t num_of_thread_tasks = 0b1u << 21;
//  constexpr std::size_t num_of_tasks = num_of_threads * num_of_thread_tasks;
//  using zisc::uint64b;
//  using zisc::uint32b;
//  using Queue = zisc::ScalableCircularQueue<uint64b>;
//  using QTest = LockFreeBoundedQueueTest<Queue, num_of_threads, num_of_thread_tasks>;
//  static_assert(num_of_tasks <= Queue::capacityMax());
//
//  zisc::SimpleMemoryResource mem_resource;
//  Queue q{&mem_resource};
//  q.setCapacity(num_of_tasks);
//
//  for (std::size_t loop = 0; loop < 4; ++loop) {
//    std::cout << "Loop[" << (loop + 1) << "]" << std::endl;
//    uint64b elapsed_time = 0;
//    bool result = QTest::testEnqueue(q, elapsed_time);
//    ASSERT_TRUE(result) << "The enqueue operation in multi-thread failed.";
//    ASSERT_EQ(num_of_tasks, q.size());
//    std::cout << "    Multiple producer time: " << elapsed_time << " ms" << std::endl;
//
//    {
//      const auto& elems = q.data();
//      std::vector<uint64b> indices;
//      indices.resize(elems.size(), 0);
//      std::copy(elems.begin(), elems.end(), indices.begin());
//      std::sort(indices.begin(), indices.end());
//      [[maybe_unused]] auto end = std::unique(indices.begin(), indices.end());
//      auto ite = indices.begin();
//      for (std::size_t index = 0; index < num_of_tasks; ++index) {
//        ASSERT_EQ(index, *ite) << "Enqueue concurrency test failed.";
//        ++ite;
//      }
//    }
//
//    std::vector<uint32b> data;
//    data.resize(num_of_tasks, 0);
//    result = QTest::testDequeue(q, data, elapsed_time);
//    ASSERT_TRUE(result) << "The dequeue operation in multi-thread failed.";
//    ASSERT_EQ(0, q.size());
//    std::cout << "    Multiple consumer time: " << elapsed_time << " ms" << std::endl;
//    for (auto flag : data) {
//      ASSERT_TRUE(flag) << "Dequeue concurrency test failed.";
//    }
//  }
//}
//
//TEST(ScalableCircularQueueTest, ConcurrencyTest2)
//{
//  constexpr std::size_t num_of_threads = 64;
//  constexpr std::size_t num_of_thread_tasks = 0b1u << 20;
//  constexpr std::size_t num_of_tasks = num_of_threads * num_of_thread_tasks;
//  using zisc::uint64b;
//  using zisc::uint32b;
//  using Queue = zisc::ScalableCircularQueue<uint64b>;
//  using QTest = LockFreeBoundedQueueTest<Queue, num_of_threads, num_of_thread_tasks>;
//  static_assert(num_of_tasks <= Queue::capacityMax());
//
//  zisc::SimpleMemoryResource mem_resource;
//  Queue q{&mem_resource};
//  q.setCapacity(2 * num_of_tasks);
//
//  uint64b elapsed_time = 0;
//  bool result = QTest::testEnqueue(q, elapsed_time);
//  ASSERT_TRUE(result) << "The enqueue operation in multi-thread failed.";
//  ASSERT_EQ(num_of_tasks, q.size());
//  std::cout << "Multiple producer time: " << elapsed_time << " ms" << std::endl;
//
//  {
//    const auto& elems = q.data();
//    std::vector<uint64b> indices;
//    indices.resize(elems.size(), 0);
//    std::copy(elems.begin(), elems.end(), indices.begin());
//    std::sort(indices.begin(), indices.end());
//    [[maybe_unused]] auto end = std::unique(indices.begin(), indices.end());
//    auto ite = indices.begin();
//    for (std::size_t index = 0; index < num_of_tasks; ++index) {
//      ASSERT_EQ(index, *ite) << "Enqueue concurrency test failed.";
//      ++ite;
//    }
//  }
//
//  std::vector<uint32b> data;
//  data.resize(num_of_tasks, 0);
//  result = QTest::testEnqueueDequeue(q, data, elapsed_time);
//  ASSERT_TRUE(result) << "The enqueue dequeue operations in multi-thread failed.";
//  ASSERT_EQ(num_of_tasks, q.size());
//  std::cout << "Multiple prodcons time: " << elapsed_time << " ms" << std::endl;
//  for (auto flag : data) {
//    ASSERT_TRUE(flag) << "Dequeue concurrency test failed.";
//  }
//
//  {
//    const auto& elems = q.data();
//    std::vector<uint64b> indices;
//    indices.resize(elems.size(), 0);
//    std::copy(elems.begin(), elems.end(), indices.begin());
//    std::sort(indices.begin(), indices.end());
//    [[maybe_unused]] auto end = std::unique(indices.begin(), indices.end());
//    auto ite = indices.begin();
//    for (std::size_t index = 0; index < num_of_tasks; ++index) {
//      ASSERT_EQ(index, *ite) << "Enqueue concurrency test failed.";
//      ++ite;
//    }
//  }
//
//  std::fill(data.begin(), data.end(), 0);
//  result = QTest::testDequeue(q, data, elapsed_time);
//  ASSERT_TRUE(result) << "The dequeue operation in multi-thread failed.";
//  ASSERT_EQ(0, q.size());
//  std::cout << "Multiple consumer time: " << elapsed_time << " ms" << std::endl;
//  for (auto flag : data) {
//    ASSERT_TRUE(flag) << "Dequeue concurrency test failed.";
//  }
//}
//
//TEST(ScalableCircularQueueTest, ConcurrencyLoopTest2)
//{
//  constexpr std::size_t num_of_threads = 64;
//  constexpr std::size_t num_of_thread_tasks = 0b1u << 20;
//  constexpr std::size_t num_of_tasks = num_of_threads * num_of_thread_tasks;
//  using zisc::uint64b;
//  using zisc::uint32b;
//  using Queue = zisc::ScalableCircularQueue<uint64b>;
//  using QTest = LockFreeBoundedQueueTest<Queue, num_of_threads, num_of_thread_tasks>;
//  static_assert(num_of_tasks <= Queue::capacityMax());
//
//  zisc::SimpleMemoryResource mem_resource;
//  Queue q{&mem_resource};
//  q.setCapacity(2 * num_of_tasks);
//
//  for (std::size_t loop = 0; loop < 4; ++loop) {
//    std::cout << "Loop[" << (loop + 1) << "]" << std::endl;
//    uint64b elapsed_time = 0;
//    bool result = QTest::testEnqueue(q, elapsed_time);
//    ASSERT_TRUE(result) << "The enqueue operation in multi-thread failed.";
//    ASSERT_EQ(num_of_tasks, q.size());
//    std::cout << "    Multiple producer time: " << elapsed_time << " ms" << std::endl;
//
//    {
//      const auto& elems = q.data();
//      std::vector<uint64b> indices;
//      indices.resize(elems.size(), 0);
//      std::copy(elems.begin(), elems.end(), indices.begin());
//      std::sort(indices.begin(), indices.end());
//      [[maybe_unused]] auto end = std::unique(indices.begin(), indices.end());
//      auto ite = indices.begin();
//      for (std::size_t index = 0; index < num_of_tasks; ++index) {
//        ASSERT_EQ(index, *ite) << "Enqueue concurrency test failed.";
//        ++ite;
//      }
//    }
//
//    std::vector<uint32b> data;
//    data.resize(num_of_tasks, 0);
//    result = QTest::testEnqueueDequeue(q, data, elapsed_time);
//    ASSERT_TRUE(result) << "The enqueue dequeue operations in multi-thread failed.";
//    ASSERT_EQ(num_of_tasks, q.size());
//    std::cout << "    Multiple prodcons time: " << elapsed_time << " ms" << std::endl;
//    for (auto flag : data) {
//      ASSERT_TRUE(flag) << "Dequeue concurrency test failed.";
//    }
//
//    {
//      const auto& elems = q.data();
//      std::vector<uint64b> indices;
//      indices.resize(elems.size(), 0);
//      std::copy(elems.begin(), elems.end(), indices.begin());
//      std::sort(indices.begin(), indices.end());
//      [[maybe_unused]] auto end = std::unique(indices.begin(), indices.end());
//      auto ite = indices.begin();
//      for (std::size_t index = 0; index < num_of_tasks; ++index) {
//        ASSERT_EQ(index, *ite) << "Enqueue concurrency test failed.";
//        ++ite;
//      }
//    }
//
//    std::fill(data.begin(), data.end(), 0);
//    result = QTest::testDequeue(q, data, elapsed_time);
//    ASSERT_TRUE(result) << "The dequeue operation in multi-thread failed.";
//    ASSERT_EQ(0, q.size());
//    std::cout << "    Multiple consumer time: " << elapsed_time << " ms" << std::endl;
//    for (auto flag : data) {
//      ASSERT_TRUE(flag) << "Dequeue concurrency test failed.";
//    }
//  }
//}
