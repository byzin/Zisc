/*!
  \file scalable_circular_queue_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/structure/scalable_circular_queue.hpp"
// Test
#include "concurrent_queue_test.hpp"
#include "queue_test.hpp"

TEST(ScalableCircularQueueTest, LockFreeTest)
{
  {
    const std::atomic<zisc::int32b> v;
    EXPECT_TRUE(v.is_always_lock_free) << "std::atomic<int32_t> isn't lock free.";
  }
  {
    const std::atomic<zisc::uint32b> v;
    EXPECT_TRUE(v.is_always_lock_free) << "std::atomic<uint32_t> isn't lock free.";
  }
  {
    const std::atomic<zisc::int64b> v;
    EXPECT_TRUE(v.is_always_lock_free) << "std::atomic<int64_t> isn't lock free.";
  }
  {
    const std::atomic<zisc::uint64b> v;
    EXPECT_TRUE(v.is_always_lock_free) << "std::atomic<uint64_t> isn't lock free.";
  }
}

TEST(ScalableCircularQueueTest, ConstructorTest)
{
  using Queue = zisc::ScalableCircularQueue<int>;

  zisc::AllocFreeResource mem_resource;
  std::unique_ptr<Queue> q;
  // Test the constructor without size
  {
    Queue q1{&mem_resource};
    q = std::make_unique<Queue>(std::move(q1));
  }
  ASSERT_EQ(1, q->capacity()) << "Constructing of ScalableCircularQueue failed.";

  // test the constructor with power of 2 size
  std::size_t cap = 16;
  {
    Queue q1{cap, &mem_resource};
    q = std::make_unique<Queue>(std::move(q1));
  }
  ASSERT_EQ(cap, q->capacity()) << "Constructing of ScalableCircularQueue failed.";

  // test the constructor with non power of 2 size
  cap = 20;
  {
    *q = Queue{cap, &mem_resource};
  }
  cap = 32;
  ASSERT_EQ(cap, q->capacity()) << "Constructing of ScalableCircularQueue failed.";
}

TEST(ScalableCircularQueueTest, SimpleQueueTest)
{
  using Queue = zisc::ScalableCircularQueue<int>;
  zisc::AllocFreeResource mem_resource;
  Queue q{&mem_resource};
  test::testSimpleBoundedQueue(std::addressof(q));
}

TEST(ScalableCircularQueueTest, MovableValueTest)
{
  using Queue = zisc::ScalableCircularQueue<test::MovableQValue>;
  zisc::AllocFreeResource mem_resource;
  Queue q{&mem_resource};
  test::testMovableValueQueue(std::addressof(q));
}

TEST(ScalableCircularQueueTest, TinyCapacityTest)
{
  using Queue = zisc::ScalableCircularQueue<int>;
  zisc::AllocFreeResource mem_resource;
  Queue q{&mem_resource};
  test::testTinyCapacityQueue(std::addressof(q));
}

TEST(ScalableCircularQueueTest, ConcurrentOperationTest)
{
  constexpr std::size_t num_of_threads = test::QueueTest::kNumOfDefaultThreads;
  constexpr std::size_t num_of_samples = test::QueueTest::kNumOfDefaultSamples;
  constexpr std::size_t num_of_rounds = test::QueueTest::kNumOfDefaultRounds;
  constexpr zisc::uint64b sampler_seed = test::QueueTest::kDefaultSamplerSeed;

  using Queue = zisc::ScalableCircularQueue<zisc::uint64b>;
  zisc::AllocFreeResource mem_resource;
  Queue q{num_of_samples, &mem_resource};

  test::QueueTest::testConcurrentThroughputOp(num_of_threads,
                                              num_of_samples,
                                              num_of_rounds,
                                              sampler_seed,
                                              std::addressof(q));
}

namespace {

void testConcurrentThroughputTime(const std::size_t num_of_threads)
{
  constexpr std::size_t num_of_samples = test::QueueTest::kNumOfDefaultSamples;
  constexpr std::size_t num_of_rounds = test::QueueTest::kNumOfDefaultRounds;
  constexpr zisc::int64b trial_time = test::QueueTest::kDefaultTrialTime;
  constexpr zisc::uint64b sampler_seed = test::QueueTest::kDefaultSamplerSeed;

  using Queue = zisc::ScalableCircularQueue<zisc::uint64b>;
  zisc::AllocFreeResource mem_resource;
  Queue q{num_of_samples, &mem_resource};

  test::QueueTest::testConcurrentThroughputTime(num_of_threads,
                                                num_of_samples,
                                                num_of_rounds,
                                                trial_time,
                                                sampler_seed,
                                                std::addressof(q));
}

} /* namespace */

TEST(ScalableCircularQueueTest, ConcurrentThroughputTest4Threads)
{
  ::testConcurrentThroughputTime(4);
}

TEST(ScalableCircularQueueTest, ConcurrentThroughputTest8Threads)
{
  ::testConcurrentThroughputTime(8);
}

TEST(ScalableCircularQueueTest, ConcurrentThroughputTest16Threads)
{
  ::testConcurrentThroughputTime(16);
}

TEST(ScalableCircularQueueTest, ConcurrentThroughputTest64Threads)
{
  ::testConcurrentThroughputTime(64);
}

TEST(ScalableCircularQueueTest, ConcurrentThroughputTest256Threads)
{
  ::testConcurrentThroughputTime(256);
}

TEST(ScalableCircularQueueTest, ConcurrentThroughputTest)
{
  const std::size_t num_of_threads = std::thread::hardware_concurrency();
  std::cout << "## Number of threads: " << num_of_threads << std::endl;
  ::testConcurrentThroughputTime(num_of_threads);
}
