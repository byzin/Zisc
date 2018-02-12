/*!
  \file utility_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/utility.hpp"

TEST(UtilityTest, clampTest)
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

TEST(UtilityTest, isNegativeTest)
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

TEST(UtilityTest, isOddTest)
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

TEST(UtilityTest, isInBoundsTest)
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
      return zisc::isInLClosedROpenBounds(value, 0.0, 1.0);
    };
    ASSERT_FALSE(is_in_bounds(-1.0));
    ASSERT_TRUE(is_in_bounds(0.0));
    ASSERT_TRUE(is_in_bounds(0.5));
    ASSERT_FALSE(is_in_bounds(1.0));
    ASSERT_FALSE(is_in_bounds(2.0));
  }
  // (0, 1] test
  {
    auto is_in_bounds = [](const double value)
    {
      return zisc::isInLOpenRClosedBounds(value, 0.0, 1.0);
    };
    ASSERT_FALSE(is_in_bounds(-1.0));
    ASSERT_FALSE(is_in_bounds(0.0));
    ASSERT_TRUE(is_in_bounds(0.5));
    ASSERT_TRUE(is_in_bounds(1.0));
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

TEST(UtilityTest, minMaxTest)
{
  constexpr int v1 = -1;
  constexpr int v2 = 1;
  ASSERT_EQ(v1, zisc::min(v1, v2));
  ASSERT_EQ(v1, zisc::min(v2, v1));
  ASSERT_EQ(v2, zisc::max(v1, v2));
  ASSERT_EQ(v2, zisc::max(v2, v1));
}
