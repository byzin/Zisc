/*!
  \file point_test.cpp
  \author Sho Ikeda
  
  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <cstdint>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/math.hpp"
#include "zisc/point.hpp"
#include "zisc/vector.hpp"
#include "zisc/zisc_config.hpp"

TEST(PointTest, ConstructionTest)
{
  {
    constexpr zisc::Point<int, 3> v{};
    constexpr zisc::uint size = v.size();
    ASSERT_EQ(3, size);
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(0, v[i]);
  }
  {
    constexpr zisc::Point<int, 3> v{{0, 1, 2}};
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, v[i]);
  }
  {
    constexpr std::array<int, 3> a{{0, 1, 2}};
    constexpr zisc::Point<int, 3> v{a};
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, v[i]);
  }
  {
    auto make_vector = []()
    {
      zisc::Point<int, 3> v{};
      v.set(2, 2);
      return v;
    };
    constexpr auto v = make_vector();
    constexpr auto result = v[2];
    ASSERT_EQ(2, result);
  }
  {
    auto make_vector = []()
    {
      zisc::Point<int, 3> v{};
      v.set({0, 1, 2});
      return v;
    };
    constexpr auto v = make_vector();
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, v[i]);
  }
  {
    auto make_vector = []()
    {
      zisc::Point<int, 3> v1{{0, 1, 2}};
      zisc::Point<int, 3> v2{v1.data()};
      return v2;
    };
    constexpr auto v = make_vector();
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, v[i]);
  }
}

TEST(PointTest, OperatorTest)
{
  auto make_point = [](const int x, const int y, const int z)
  {
    zisc::Point<int, 3> p{{x, y, z}};
    return p;
  };

  auto make_vector = [](const int x, const int y, const int z)
  {
    zisc::Vector<int, 3> v{{x, y, z}};
    return v;
  };

  {
    constexpr auto p1 = make_point(0, 1, 2);
    constexpr auto v1 = make_vector(0, 1, 2);
    constexpr auto result1 = v1 + p1;
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(2 * i, result1[i]);
    constexpr auto result2 = p1 - v1;
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(0, result2[i]);
  }
  {
    constexpr auto p1 = make_vector(0, 1, 2);
    constexpr auto result1 = p1 - p1;
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(0, result1[i]);
  }
}
