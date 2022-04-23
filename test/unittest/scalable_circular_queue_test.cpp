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
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <system_error>
#include <thread>
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
#include "zisc/memory/simple_memory_resource.hpp"
#include "zisc/structure/scalable_circular_queue.hpp"
// Test
#include "concurrent_queue_test.hpp"
#include "queue_test.hpp"

TEST(ScalableCircularQueueTest, LockFreeTest)
{
  {
    std::atomic<zisc::int32b> v;
    EXPECT_TRUE(v.is_always_lock_free) << "std::atomic<int32_t> isn't lock free.";
  }
  {
    std::atomic<zisc::uint32b> v;
    EXPECT_TRUE(v.is_always_lock_free) << "std::atomic<uint32_t> isn't lock free.";
  }
  {
    std::atomic<zisc::int64b> v;
    EXPECT_TRUE(v.is_always_lock_free) << "std::atomic<int64_t> isn't lock free.";
  }
  {
    std::atomic<zisc::uint64b> v;
    EXPECT_TRUE(v.is_always_lock_free) << "std::atomic<uint64_t> isn't lock free.";
  }
}

TEST(ScalableCircularQueueTest, ConstructorTest)
{
  using Queue = zisc::ScalableCircularQueue<int>;

  zisc::SimpleMemoryResource mem_resource;
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
    Queue q1{cap, &mem_resource};
    q = std::make_unique<Queue>(std::move(q1));
  }
  cap = 32;
  ASSERT_EQ(cap, q->capacity()) << "Constructing of ScalableCircularQueue failed.";
}

TEST(ScalableCircularQueueTest, SimpleQueueTest)
{
  using Queue = zisc::ScalableCircularQueue<int>;
  zisc::SimpleMemoryResource mem_resource;
  Queue q{&mem_resource};
  test::testSimpleBoundedQueue(std::addressof(q));
}

TEST(ScalableCircularQueueTest, MovableValueTest)
{
  using Queue = zisc::ScalableCircularQueue<test::MovableQValue>;
  zisc::SimpleMemoryResource mem_resource;
  Queue q{&mem_resource};
  test::testMovableValueQueue(std::addressof(q));
}

TEST(ScalableCircularQueueTest, TinyCapacityTest)
{
  using Queue = zisc::ScalableCircularQueue<int>;
  zisc::SimpleMemoryResource mem_resource;
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
  zisc::SimpleMemoryResource mem_resource;
  Queue q{num_of_samples, &mem_resource};

  test::QueueTest::testConcurrentThroughputOp(num_of_threads,
                                              num_of_samples,
                                              num_of_rounds,
                                              sampler_seed,
                                              std::addressof(q));
}

TEST(ScalableCircularQueueTest, ConcurrentThroughputTest)
{
  constexpr std::size_t num_of_threads = test::QueueTest::kNumOfDefaultThreads;
  constexpr std::size_t num_of_samples = test::QueueTest::kNumOfDefaultSamples;
  constexpr std::size_t num_of_rounds = test::QueueTest::kNumOfDefaultRounds;
  constexpr zisc::int64b trial_time = test::QueueTest::kDefaultTrialTime;
  constexpr zisc::uint64b sampler_seed = test::QueueTest::kDefaultSamplerSeed;

  using Queue = zisc::ScalableCircularQueue<zisc::uint64b>;
  zisc::SimpleMemoryResource mem_resource;
  Queue q{num_of_samples, &mem_resource};

  test::QueueTest::testConcurrentThroughputTime(num_of_threads,
                                                num_of_samples,
                                                num_of_rounds,
                                                trial_time,
                                                sampler_seed,
                                                std::addressof(q));
}
