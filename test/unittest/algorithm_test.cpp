/*!
  \file algorithm_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
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

std::vector<zisc::uint64b> makeSortedArray(const zisc::uint64b n,
                                           zisc::PcgLcgRxsMXs64& rng_engine) noexcept
{
  // Make a sorted array
  std::vector<zisc::uint64b> tmp;
  tmp.resize(n);
  for (zisc::uint64b i = 0; i < n; ++i) {
    zisc::uint64b v = rng_engine.generate();
    v = zisc::clamp(v,
                    std::numeric_limits<zisc::uint64b>::min() + 1,
                    std::numeric_limits<zisc::uint64b>::max() - 1);
    tmp[i] = v;
  }
  std::sort(tmp.begin(), tmp.end());

  // Make a unique array
  std::vector<zisc::uint64b> array;
  std::unique_copy(tmp.begin(), tmp.end(), std::back_inserter(array));

  return array;
}

}

TEST(AlgorithmTest, clampTest)
{
  constexpr double min_value = -1.0;
  constexpr double max_value = 1.0;
  {
    constexpr double value = -2.0;
    ASSERT_EQ(min_value, zisc::clamp(value, min_value, max_value));
  }
  {
    constexpr double value = 0.0;
    ASSERT_EQ(value, zisc::clamp(value, min_value, max_value));
  }
  {
    constexpr double value = 2.0;
    ASSERT_EQ(max_value, zisc::clamp(value, min_value, max_value));
  }
}

TEST(AlgorithmTest, isNegativeTest)
{
  {
    constexpr int value = -1;
    ASSERT_TRUE(zisc::isNegative(value));
  }
  {
    constexpr int value = 0;
    ASSERT_FALSE(zisc::isNegative(value));
  }
  {
    constexpr int value = 1;
    ASSERT_FALSE(zisc::isNegative(value));
  }
}

TEST(AlgorithmTest, isOddTest)
{
  {
    constexpr int value = 1;
    ASSERT_TRUE(zisc::isOdd(value));
  }
  {
    constexpr int value = 2;
    ASSERT_FALSE(zisc::isOdd(value));
  }
}

TEST(AlgorithmTest, isInBoundsTest)
{
  // (0, 1) test
  {
    auto is_in_bounds = [](const double value)
    {
      return zisc::isInOpenBounds(value, 0.0, 1.0);
    };
    ASSERT_FALSE(is_in_bounds(-1.0));
    ASSERT_FALSE(is_in_bounds(0.0));
    ASSERT_TRUE(is_in_bounds(0.5));
    ASSERT_FALSE(is_in_bounds(1.0));
    ASSERT_FALSE(is_in_bounds(2.0));
  }
  // [0, 1) test
  {
    auto is_in_bounds = [](const double value)
    {
      return zisc::isInBounds(value, 0.0, 1.0);
    };
    ASSERT_FALSE(is_in_bounds(-1.0));
    ASSERT_TRUE(is_in_bounds(0.0));
    ASSERT_TRUE(is_in_bounds(0.5));
    ASSERT_FALSE(is_in_bounds(1.0));
    ASSERT_FALSE(is_in_bounds(2.0));
  }
  // [0, 1] test
  {
    auto is_in_bounds = [](const double value)
    {
      return zisc::isInClosedBounds(value, 0.0, 1.0);
    };
    ASSERT_FALSE(is_in_bounds(-1.0));
    ASSERT_TRUE(is_in_bounds(0.0));
    ASSERT_TRUE(is_in_bounds(0.5));
    ASSERT_TRUE(is_in_bounds(1.0));
    ASSERT_FALSE(is_in_bounds(2.0));
  }
}

TEST(AlgorithmTest, minMaxTest)
{
  constexpr int v1 = -1;
  constexpr int v2 = 1;
  ASSERT_EQ(v1, zisc::min(v1, v2));
  ASSERT_EQ(v1, zisc::min(v2, v1));
  ASSERT_EQ(v2, zisc::max(v1, v2));
  ASSERT_EQ(v2, zisc::max(v2, v1));
}

TEST(AlgorithmTest, BinaryTreeTest)
{
  using zisc::cast;

  constexpr zisc::uint n = 10;
  zisc::ArithArray<double, n> array;
  for (zisc::uint i = 0; i < n; ++i)
    array[i] = cast<double>(i);
  zisc::Algorithm::toBinaryTree(array.begin(), array.end());

  auto position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 0.5);
  EXPECT_DOUBLE_EQ(0.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 1.5);
  EXPECT_DOUBLE_EQ(1.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 2.5);
  EXPECT_DOUBLE_EQ(2.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 6.5);
  EXPECT_DOUBLE_EQ(6.0, *position);
  
  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 5.5);
  EXPECT_DOUBLE_EQ(5.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), cast<double>(n));
  EXPECT_DOUBLE_EQ(cast<double>(n - 1), *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 7.0);
  EXPECT_DOUBLE_EQ(7.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 9.0);
  EXPECT_DOUBLE_EQ(9.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 2.0);
  EXPECT_DOUBLE_EQ(2.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 3.0);
  EXPECT_DOUBLE_EQ(3.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 6.0);
  EXPECT_DOUBLE_EQ(6.0, *position);
}

TEST(AlgorithmTest, BinaryTreeSearchTest)
{
  zisc::PcgLcgRxsMXs64 rng_engine{123456789};

  constexpr zisc::uint max_number = 200;
  for (zisc::uint number = 0; number < max_number; ++number) {
    zisc::uint64b n = zisc::cast<zisc::uint64b>(rng_engine(1.0, 1.0e6));

    // Make a sorted array
    auto array = ::makeSortedArray(n, rng_engine);
    n = zisc::cast<zisc::uint64b>(array.size());

    std::cout << "Test" << std::setw(4) << number << ": n = " << n << std::endl;

    // Make a binary tree
    auto tree = array;
    zisc::Algorithm::Zisc::toBinaryTree(tree.begin(), tree.end());

    for (zisc::uint64b i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64b>(c);
          // STL
          const auto expected = zisc::Algorithm::Stl::searchBinaryTree(array.begin(),
                                                                       array.end(),
                                                                       v);
          ASSERT_TRUE(expected != array.end()) << "The binary search failed.";
          // ZISC
          const auto actual = zisc::Algorithm::Zisc::searchBinaryTree(tree.begin(),
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
  zisc::PcgLcgRxsMXs64 rng_engine{123456789};

  constexpr zisc::uint max_number = 200;
  auto stl_time = zisc::Stopwatch::Clock::duration::zero();
  auto zisc_time = stl_time;
  for (zisc::uint number = 0; number < max_number; ++number) {
    zisc::uint64b n = zisc::cast<zisc::uint64b>(rng_engine(1.0, 1.0e6));

    // Make a sorted array
    auto array = ::makeSortedArray(n, rng_engine);
    n = zisc::cast<zisc::uint64b>(array.size());

    std::cout << "Test" << std::setw(4) << number << ": n = " << n << std::endl;

    // Make a binary tree
    auto tree = array;
    zisc::Algorithm::Zisc::toBinaryTree(tree.begin(), tree.end());

    zisc::Stopwatch stopwatch;
    stopwatch.start();
    for (zisc::uint64b i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64b>(c);
          // STL
          [[maybe_unused]] volatile auto result = 
              zisc::Algorithm::Stl::searchBinaryTree(array.begin(), array.end(), v);
        }
      }
    }
    stl_time += stopwatch.elapsedTime();
    stopwatch.stop();
    stopwatch.start();
    for (zisc::uint64b i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64b>(c);
          // Zisc
          [[maybe_unused]] volatile auto result = 
              zisc::Algorithm::Zisc::searchBinaryTree(tree.begin(), tree.end(), v);
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
