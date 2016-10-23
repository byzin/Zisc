/*!
  \file algorithm_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
// Standard C++ library
#include <algorithm>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/arithmetic_array.hpp"
#include "zisc/pcg_engine.hpp"
#include "zisc/stopwatch.hpp"
#include "zisc/utility.hpp"

TEST(AlgorithmTest, BinaryTreeTest)
{
  using zisc::cast;

  constexpr zisc::uint n = 10;
  zisc::ArithmeticArray<double, n> array;
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

TEST(AlgorithmTest, HashTest)
{
  ASSERT_EQ(0x811c9dc5, zisc::toHash32(""))
      << "32bit hash calculation test failed.";
  ASSERT_EQ(0xcbf29ce484222325, zisc::toHash64(""))
      << "64bit hash calculation test failed.";

  ASSERT_EQ(0xe40c292c, zisc::toHash32("a"))
      << "32bit hash calculation test failed.";
  ASSERT_EQ(0xaf63dc4c8601ec8c, zisc::toHash64("a"))
      << "64bit hash calculation test failed.";

  ASSERT_EQ(0xe70c2de5, zisc::toHash32("b"))
      << "32bit hash calculation test failed.";
  ASSERT_EQ(0xaf63df4c8601f1a5, zisc::toHash64("b"))
      << "64bit hash calculation test failed.";

  ASSERT_EQ(0xbf9cf968, zisc::toHash32("foobar"))
      << "32bit hash calculation test failed.";
  ASSERT_EQ(0x85944171f73967e8, zisc::toHash64("foobar"))
      << "64bit hash calculation test failed.";
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
  // Quick sort order: O(n * log(n))
  // Binary radix sort oder: O(64 * n)
  constexpr zisc::uint64 container_size = 2'000'000'00;
  std::vector<zisc::uint64> container;
  container.resize(container_size);
  // Initialize container
  {
    const double n = zisc::cast<double>(container_size);
    std::cout << "  Quick sort average order: n * log(n) = "
              << zisc::cast<zisc::uint64>(n * std::log(n)) << std::endl;
    std::cout << "  Quick sort worst order :       n * n = "
              << (container_size * container_size) << std::endl;
    std::cout << "  Radix sort order:             64 * n = "
              << (64 * container_size) << std::endl;
  }
  zisc::PcgMcgRxsMXs seed_generator{123456789};
  bool radix_is_faster_than_quick = false;
  constexpr int test_loop = 10;
  for (int count = 0; count < test_loop; ++count) {
    std::cout << "Initialize container[" << container_size << "]." << std::endl;
    {
      zisc::PcgMcgRxsMXs sampler{seed_generator()};
      for (zisc::uint i = 0; i < container_size; ++i)
        container[i] = sampler();
    }
    // Measure quick sort time
    std::cout << "Test quick sort:" << std::endl;
    zisc::Stopwatch::Clock::duration quick_sort_time;
    {
      zisc::Stopwatch stopwatch;
      auto container1 = container;
      stopwatch.start();
      std::sort(container1.begin(), container1.end());
      stopwatch.pause();
      quick_sort_time = stopwatch.elapsedTime();
    }
    std::cout << "  " << std::chrono::duration_cast<std::chrono::milliseconds>(quick_sort_time).count() << " ms." << std::endl;
    // Measure radix sort time
    std::cout << "Test radix sort:" << std::endl;
    zisc::Stopwatch::Clock::duration radix_sort_time;
    {
      zisc::Stopwatch stopwatch;
      auto container2 = container;
      stopwatch.start();
      zisc::binaryRadixSort(&container2);
      stopwatch.pause();
      radix_sort_time = stopwatch.elapsedTime();
    }
    std::cout << "  " << std::chrono::duration_cast<std::chrono::milliseconds>(radix_sort_time).count() << " ms." << std::endl;
    if (radix_sort_time < quick_sort_time)
      radix_is_faster_than_quick = true;
  }
  ASSERT_TRUE(radix_is_faster_than_quick)
      << "zisc::binaryRadixSort() is slower than std::sort";
}
