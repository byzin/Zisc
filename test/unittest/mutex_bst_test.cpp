/*!
  \file mutex_bst_test.cpp
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
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/structure/map.hpp"
#include "zisc/structure/mutex_bst.hpp"
// Test
#include "concurrent_map_test.hpp"
#include "map_test.hpp"

TEST(MutexBstTest, ConstructorTest)
{
  using Map = zisc::MutexBst<int>;

  zisc::AllocFreeResource mem_resource;
  std::unique_ptr<Map> map;

  // test the map with default capacity
  {
    Map map1{&mem_resource};
    map = std::make_unique<Map>(std::move(map1));
  }
  ASSERT_EQ(1, map->capacity()) << "Constructing of MutexBst failed.";

  // test the map with power of 2 size
  std::size_t cap = 16;
  {
    Map map1{cap, &mem_resource};
    map = std::make_unique<Map>(std::move(map1));
  }
  ASSERT_EQ(cap, map->capacity()) << "Constructing of MutexBst failed.";

  // test the map with non power of 2 size
  cap = 20;
  {
    *map = Map{cap, &mem_resource};
  }
  ASSERT_EQ(cap, map->capacity()) << "Constructing of MutexBst failed.";
}

TEST(MutexBstTest, SimpleMapTest)
{
  using Map = zisc::MutexBst<int>;
  zisc::AllocFreeResource mem_resource;
  Map map{&mem_resource};
  test::testSimpleBoundedMap(std::addressof(map));
}

TEST(MutexBstTest, MovableValueTest)
{
  using Map = zisc::MutexBst<int, test::MovableMValue>;
  zisc::AllocFreeResource mem_resource;
  Map map{&mem_resource};
  test::testMovableValueMap(std::addressof(map));
}

TEST(MutexBstTest, TinyCapacityTest)
{
  using Map = zisc::MutexBst<int>;
  zisc::AllocFreeResource mem_resource;
  Map map{&mem_resource};
  test::testTinyCapacityMap(std::addressof(map));
}

//TEST(MutexBstTest, ConcurrentOperationTest1)
//{
//  constexpr std::size_t num_of_threads = test::MapTest::kNumOfDefaultThreads;
//  constexpr std::size_t num_of_samples = test::MapTest::kNumOfDefaultSamples;
//  constexpr std::size_t num_of_keys = test::MapTest::kNumOfDefaultKeys;
//  constexpr std::size_t num_of_rounds = test::MapTest::kNumOfDefaultRounds;
//  constexpr zisc::uint64b sampler_seed = test::MapTest::kDefaultSamplerSeed;
//  constexpr bool use_sparse = false;
//  constexpr bool use_zipfian = false;
//  constexpr double zipfian_param = 0.0;
//
//  using Map = zisc::MutexBst<zisc::uint64b>;
//  zisc::AllocFreeResource mem_resource;
//  Map map{num_of_samples, &mem_resource};
//  test::MapTest::testConcurrentThroughputOp(num_of_threads,
//                                            num_of_samples,
//                                            num_of_keys,
//                                            num_of_rounds,
//                                            sampler_seed,
//                                            use_sparse,
//                                            use_zipfian,
//                                            zipfian_param,
//                                            std::addressof(map));
//}

//TEST(MutexBstTest, ConcurrentOperationTest2)
//{
//  constexpr std::size_t num_of_threads = test::MapTest::kNumOfDefaultThreads;
//  constexpr std::size_t num_of_samples = test::MapTest::kNumOfDefaultSamples;
//  constexpr std::size_t num_of_keys = test::MapTest::kNumOfDefaultKeys;
//  constexpr std::size_t num_of_rounds = test::MapTest::kNumOfDefaultRounds;
//  constexpr zisc::uint64b sampler_seed = test::MapTest::kDefaultSamplerSeed;
//  constexpr bool use_sparse = true;
//  constexpr bool use_zipfian = true;
//  constexpr double zipfian_param = 0.9;
//
//  using Map = zisc::MutexBst<zisc::uint64b>;
//  zisc::AllocFreeResource mem_resource;
//  Map map{num_of_samples, &mem_resource};
//  test::MapTest::testConcurrentThroughputOp(num_of_threads,
//                                            num_of_samples,
//                                            num_of_keys,
//                                            num_of_rounds,
//                                            sampler_seed,
//                                            use_sparse,
//                                            use_zipfian,
//                                            zipfian_param,
//                                            std::addressof(map));
//}

namespace {

void testConcurrentThroughputTime1(const std::size_t num_of_threads)
{
  constexpr std::size_t num_of_samples = test::MapTest::kNumOfDefaultSamples;
  constexpr std::size_t num_of_keys = test::MapTest::kNumOfDefaultKeys;
  constexpr std::size_t num_of_rounds = test::MapTest::kNumOfDefaultRounds;
  constexpr std::size_t update_percent = test::MapTest::kDefaultUpdatePercent;
  constexpr zisc::int64b trial_time = test::MapTest::kDefaultTrialTime;
  constexpr zisc::uint64b sampler_seed = test::MapTest::kDefaultSamplerSeed;
  constexpr bool use_sparse = false;
  constexpr bool use_zipfian = false;
  constexpr double zipfian_param = 0.0;

  using Map = zisc::MutexBst<zisc::uint64b>;
  zisc::AllocFreeResource mem_resource;
  Map map{num_of_samples, &mem_resource};
  test::MapTest::testConcurrentThroughputTime(num_of_threads,
                                              num_of_samples,
                                              num_of_keys,
                                              num_of_rounds,
                                              update_percent,
                                              trial_time,
                                              sampler_seed,
                                              use_sparse,
                                              use_zipfian,
                                              zipfian_param,
                                              std::addressof(map));
}

void testConcurrentThroughputTime2(const std::size_t num_of_threads)
{
  constexpr std::size_t num_of_samples = test::MapTest::kNumOfDefaultSamples;
  constexpr std::size_t num_of_keys = test::MapTest::kNumOfDefaultKeys;
  constexpr std::size_t num_of_rounds = test::MapTest::kNumOfDefaultRounds;
  constexpr std::size_t update_percent = test::MapTest::kDefaultUpdatePercent;
  constexpr zisc::int64b trial_time = test::MapTest::kDefaultTrialTime;
  constexpr zisc::uint64b sampler_seed = test::MapTest::kDefaultSamplerSeed;
  constexpr bool use_sparse = true;
  constexpr bool use_zipfian = true;
  constexpr double zipfian_param = 0.9;

  using Map = zisc::MutexBst<zisc::uint64b>;
  zisc::AllocFreeResource mem_resource;
  Map map{num_of_samples, &mem_resource};
  test::MapTest::testConcurrentThroughputTime(num_of_threads,
                                              num_of_samples,
                                              num_of_keys,
                                              num_of_rounds,
                                              update_percent,
                                              trial_time,
                                              sampler_seed,
                                              use_sparse,
                                              use_zipfian,
                                              zipfian_param,
                                              std::addressof(map));
}

} /* namespace */

TEST(MutexBstTest, ConcurrentThroughput1Test4Threads)
{
  ::testConcurrentThroughputTime1(4);
}

TEST(MutexBstTest, ConcurrentThroughput2Test4Threads)
{
  ::testConcurrentThroughputTime2(4);
}

TEST(MutexBstTest, ConcurrentThroughput1Test8Threads)
{
  ::testConcurrentThroughputTime1(8);
}

TEST(MutexBstTest, ConcurrentThroughput2Test8Threads)
{
  ::testConcurrentThroughputTime2(8);
}

TEST(MutexBstTest, ConcurrentThroughput1Test16Threads)
{
  ::testConcurrentThroughputTime1(16);
}

TEST(MutexBstTest, ConcurrentThroughput2Test16Threads)
{
  ::testConcurrentThroughputTime2(16);
}

TEST(MutexBstTest, ConcurrentThroughput1Test64Threads)
{
  ::testConcurrentThroughputTime1(64);
}

TEST(MutexBstTest, ConcurrentThroughput2Test64Threads)
{
  ::testConcurrentThroughputTime2(64);
}

TEST(MutexBstTest, ConcurrentThroughput1Test256Threads)
{
  ::testConcurrentThroughputTime1(256);
}

TEST(MutexBstTest, ConcurrentThroughput2Test256Threads)
{
  ::testConcurrentThroughputTime2(256);
}

TEST(MutexBstTest, ConcurrentThroughput1TestThreads)
{
  const std::size_t num_of_threads = std::thread::hardware_concurrency();
  std::cout << "## Number of threads: " << num_of_threads << std::endl;
  ::testConcurrentThroughputTime1(num_of_threads);
}

TEST(MutexBstTest, ConcurrentThroughput2TestThreads)
{
  const std::size_t num_of_threads = std::thread::hardware_concurrency();
  std::cout << "## Number of threads: " << num_of_threads << std::endl;
  ::testConcurrentThroughputTime2(num_of_threads);
}
