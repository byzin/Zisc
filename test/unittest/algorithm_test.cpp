/*!
  \file algorithm_test.cpp
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
#include <algorithm>
#include <type_traits>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

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

TEST(AlgorithmTest, signbitTest)
{
  {
    constexpr int value = -1;
    ASSERT_TRUE(zisc::signbit(value));
  }
  {
    constexpr int value = 0;
    ASSERT_FALSE(zisc::signbit(value));
  }
  {
    constexpr int value = 1;
    ASSERT_FALSE(zisc::signbit(value));
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
