/*!
  \file algorithm_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/arith_array.hpp"
#include "zisc/pcg_engine.hpp"
#include "zisc/stopwatch.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

std::vector<zisc::uint64> makeSortedArray(const zisc::uint64 n,
                                          zisc::PcgMcgRxsMXs& rng_engine) noexcept
{
  // Make a sorted array
  std::vector<zisc::uint64> tmp;
  tmp.resize(n);
  for (zisc::uint64 i = 0; i < n; ++i) {
    zisc::uint64 v = rng_engine.generate();
    v = zisc::clamp(v,
                    std::numeric_limits<zisc::uint64>::min() + 1,
                    std::numeric_limits<zisc::uint64>::max() - 1);
    tmp[i] = v;
  }
  std::sort(tmp.begin(), tmp.end());

  // Make a unique array
  std::vector<zisc::uint64> array;
  std::unique_copy(tmp.begin(), tmp.end(), std::back_inserter(array));

  return array;
}

}

TEST(AlgorithmTest, BinaryTreeTest)
{
  using zisc::cast;

  constexpr zisc::uint n = 10;
  zisc::ArithArray<double, n> array;
  for (zisc::uint i = 0; i < n; ++i)
    array[i] = cast<double>(i);
  zisc::toBinaryTree(array.begin(), array.end());

  auto position = zisc::searchBinaryTree(array.begin(), array.end(), 0.5);
  ASSERT_DOUBLE_EQ(0.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 1.5);
  ASSERT_DOUBLE_EQ(1.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 2.5);
  ASSERT_DOUBLE_EQ(2.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 6.5);
  ASSERT_DOUBLE_EQ(6.0, *position);
  
  position = zisc::searchBinaryTree(array.begin(), array.end(), 5.5);
  ASSERT_DOUBLE_EQ(5.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), cast<double>(n));
  ASSERT_DOUBLE_EQ(cast<double>(n - 1), *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 7.0);
  ASSERT_DOUBLE_EQ(7.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 9.0);
  ASSERT_DOUBLE_EQ(9.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 2.0);
  ASSERT_DOUBLE_EQ(2.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 3.0);
  ASSERT_DOUBLE_EQ(3.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 6.0);
  ASSERT_DOUBLE_EQ(6.0, *position);
}

TEST(AlgorithmTest, BinaryTreeSearchTest)
{
  zisc::PcgMcgRxsMXs rng_engine{123456789};

  constexpr zisc::uint max_number = 200;
  for (zisc::uint number = 0; number < max_number; ++number) {
    zisc::uint64 n = zisc::cast<zisc::uint64>(rng_engine(1.0, 1.0e6));

    // Make a sorted array
    auto array = ::makeSortedArray(n, rng_engine);
    n = zisc::cast<zisc::uint64>(array.size());

    std::cout << "Test" << std::setw(4) << number << ": n = " << n << std::endl;

    // Make a binary tree
    auto tree = array;
    {
      auto tmp = tree;
      zisc::zisc_implementation::toBinaryTree(tmp.begin(), tmp.end(), 0, tree.begin());
    }

    for (zisc::uint64 i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64>(c);
          // STL
          const auto expected = 
              zisc::stl_implementation::searchBinaryTree(array.begin(),
                                                         array.end(),
                                                         v);
          ASSERT_TRUE(expected != array.end()) << "The binary search failed.";
          // ZISC
          const auto actual =
              zisc::zisc_implementation::searchBinaryTree(tree.begin(),
                                                          tree.end(), 
                                                          v);
          ASSERT_EQ(*expected, *actual) << "The binary tree is wrong.";
        }
      }
    }
  }
}

TEST(AlgorithmTest, BinaryTreeSearchPerformanceTest)
{
  zisc::PcgMcgRxsMXs rng_engine{123456789};

  constexpr zisc::uint max_number = 200;
  auto stl_time = zisc::Stopwatch::Clock::duration::zero();
  auto zisc_time = stl_time;
  for (zisc::uint number = 0; number < max_number; ++number) {
    zisc::uint64 n = zisc::cast<zisc::uint64>(rng_engine(1.0, 1.0e6));

    // Make a sorted array
    auto array = ::makeSortedArray(n, rng_engine);
    n = zisc::cast<zisc::uint64>(array.size());

    std::cout << "Test" << std::setw(4) << number << ": n = " << n << std::endl;

    // Make a binary tree
    auto tree = array;
    {
      auto tmp = tree;
      zisc::zisc_implementation::toBinaryTree(tmp.begin(), tmp.end(), 0, tree.begin());
    }

    zisc::Stopwatch stopwatch;
    stopwatch.start();
    for (zisc::uint64 i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64>(c);
          // STL
          [[maybe_unused]] volatile auto result = 
              zisc::stl_implementation::searchBinaryTree(array.begin(),
                                                         array.end(),
                                                         v);
        }
      }
    }
    stl_time += stopwatch.elapsedTime();
    stopwatch.stop();
    stopwatch.start();
    for (zisc::uint64 i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64>(c);
          // Zisc
          [[maybe_unused]] volatile auto result = 
              zisc::zisc_implementation::searchBinaryTree(tree.begin(),
                                                          tree.end(), 
                                                          v);
        }
      }
    }
    zisc_time += stopwatch.elapsedTime();
  }
  {
    const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(stl_time);
    std::cout << " STL time: " << t.count() << " ms" << std::endl;
  }
  {
    const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(zisc_time);
    std::cout << "Zisc time: " << t.count() << " ms" << std::endl;
  }
  ASSERT_GT(stl_time, zisc_time) << "The zisc binary tree is slower than the stl.";
}

TEST(AlgorithmTest, BinaryRadixSortTest)
{
  std::vector<zisc::uint> array = {{10, 40, 2, 9, 1024, 200, 0, 642}};
  zisc::binaryRadixSort(&array);

  auto reference_array = array;
  std::sort(reference_array.begin(), reference_array.end());
  for (zisc::uint i = 0; i < array.size(); ++i) {
    ASSERT_EQ(reference_array[i], array[i]);
  }
}

TEST(AlgorithmTest, BinaryRadixSortPerformanceTest)
{
//  // Quick sort order: O(n * log(n))
//  // Binary radix sort oder: O(64 * n)
//  constexpr zisc::uint64 container_size = 2'000'000'00;
//  std::vector<zisc::uint64> container;
//  container.resize(container_size);
//  // Initialize container
//  {
//    const double n = zisc::cast<double>(container_size);
//    std::cout << "  Quick sort average order: n * log(n) = "
//              << zisc::cast<zisc::uint64>(n * std::log(n)) << std::endl;
//    std::cout << "  Quick sort worst order :       n * n = "
//              << (container_size * container_size) << std::endl;
//    std::cout << "  Radix sort order:             64 * n = "
//              << (64 * container_size) << std::endl;
//  }
//  zisc::PcgMcgRxsMXs seed_generator{123456789};
//  bool radix_is_faster_than_quick = false;
//  constexpr int test_loop = 10;
//  for (int count = 0; count < test_loop; ++count) {
//    std::cout << "Initialize container[" << container_size << "]." << std::endl;
//    {
//      zisc::PcgMcgRxsMXs sampler{seed_generator()};
//      for (zisc::uint i = 0; i < container_size; ++i)
//        container[i] = sampler();
//    }
//    // Measure quick sort time
//    std::cout << "Test quick sort:" << std::endl;
//    zisc::Stopwatch::Clock::duration quick_sort_time;
//    {
//      zisc::Stopwatch stopwatch;
//      auto container1 = container;
//      stopwatch.start();
//      std::sort(container1.begin(), container1.end());
//      stopwatch.pause();
//      quick_sort_time = stopwatch.elapsedTime();
//    }
//    std::cout << "  " << std::chrono::duration_cast<std::chrono::milliseconds>(quick_sort_time).count() << " ms." << std::endl;
//    // Measure radix sort time
//    std::cout << "Test radix sort:" << std::endl;
//    zisc::Stopwatch::Clock::duration radix_sort_time;
//    {
//      zisc::Stopwatch stopwatch;
//      auto container2 = container;
//      stopwatch.start();
//      zisc::binaryRadixSort(&container2);
//      stopwatch.pause();
//      radix_sort_time = stopwatch.elapsedTime();
//    }
//    std::cout << "  " << std::chrono::duration_cast<std::chrono::milliseconds>(radix_sort_time).count() << " ms." << std::endl;
//    if (radix_sort_time < quick_sort_time)
//      radix_is_faster_than_quick = true;
//  }
//  ASSERT_TRUE(radix_is_faster_than_quick)
//      << "zisc::binaryRadixSort() is slower than std::sort";
}
