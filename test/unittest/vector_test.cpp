/*!
  \file vector_test.cpp
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
#include "zisc/vector.hpp"
#include "zisc/zisc_config.hpp"

TEST(VectorTest, ConstructionTest)
{
  {
    constexpr zisc::Vector<int, 3> v{};
    constexpr zisc::uint size = v.size();
    ASSERT_EQ(3, size);
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(0, v[i]);
  }
  {
    constexpr zisc::Vector<int, 3> v{{0, 1, 2}};
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, v[i]);
  }
  {
    constexpr std::array<int, 3> a{{0, 1, 2}};
    constexpr zisc::Vector<int, 3> v{a};
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, v[i]);
  }
  {
    auto make_vector = []()
    {
      zisc::Vector<int, 3> v{};
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
      zisc::Vector<int, 3> v{};
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
      zisc::Vector<int, 3> v1{{0, 1, 2}};
      zisc::Vector<int, 3> v2{v1.data()};
      return v2;
    };
    constexpr auto v = make_vector();
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, v[i]);
  }
}

TEST(VectorTest, OperatorTest)
{
  auto make_vector = [](const int x, const int y, const int z)
  {
    zisc::Vector<int, 3> v{{x, y, z}};
    return v;
  };

  {
    constexpr auto v = -make_vector(0, 1, 2);
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(-i, v[i]);
  }
  {
    constexpr auto v1 = make_vector(0, 1, 2);
    constexpr auto result1 = v1 + v1;
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(2 * i, result1[i]);
    auto add = [](const zisc::Vector<int, 3>& v)
    {
      zisc::Vector<int, 3> vec{};
      vec += v;
      return vec;
    };
    constexpr auto result2 = add(v1);
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, result2[i]);
  }
  {
    constexpr auto v1 = make_vector(0, 1, 2);
    constexpr auto result1 = v1 - v1;
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(0, result1[i]);
    auto sub = [](const zisc::Vector<int, 3>& v)
    {
      zisc::Vector<int, 3> vec{};
      vec -= v;
      return vec;
    };
    constexpr auto result2 = sub(v1);
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(-i, result2[i]);
  }
  {
    constexpr auto v1 = make_vector(0, 1, 2);
    constexpr auto result1 = v1 * 2;
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(2 * i, result1[i]);
    auto mul = [](const int v)
    {
      zisc::Vector<int, 3> vec{{0, 1, 2}};
      vec *= v;
      return vec;
    };
    constexpr auto result2 = mul(2);
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(2 * i, result2[i]);
  }
  {
    constexpr auto v1 = make_vector(0, 2, 4);
    constexpr auto result1 = v1 / 2;
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, result1[i]);
    auto div = [](const int v)
    {
      zisc::Vector<int, 3> vec{{0, 2, 4}};
      vec /= v;
      return vec;
    };
    constexpr auto result2 = div(2);
    for (int i = 0; i < 3; ++i)
      ASSERT_EQ(i, result2[i]);
  }
  {
    constexpr auto v1 = make_vector(0, 1, 2);
    constexpr auto v2 = make_vector(1, 2, 3);
    constexpr bool result1 = v1 == v1;
    constexpr bool result2 = v1 != v1;
    ASSERT_TRUE(result1);
    ASSERT_FALSE(result2);
    constexpr bool result3 = v1 == v2;
    constexpr bool result4 = v1 != v2;
    ASSERT_FALSE(result3);
    ASSERT_TRUE(result4);
  }
  {
    auto make_vec = [](const int x, const int y, const int z, const int w)
    {
      zisc::Vector<int, 4> v{{x, y, z, w}};
      return v;
    };
    constexpr auto v = make_vec(1, 2, 3, 4);
    constexpr auto result = zisc::dot(v, v);
    ASSERT_EQ(30, result);
  }
  {
    constexpr auto v1 = make_vector(1, 0, 0);
    constexpr auto v2 = make_vector(0, 1, 0);
    constexpr auto result = zisc::cross(v1, v2);
    constexpr zisc::Vector<int, 3> expected{{0, 0, 1}};
    ASSERT_EQ(expected, result);
  }
}

TEST(VectorTest, FunctionTest)
{
  {
    constexpr zisc::Vector<double, 3> v{{2.0, 2.0, 2.0}};
    constexpr auto result1 = v.squareNorm();
    constexpr double expected1 = 12.0;
    ASSERT_DOUBLE_EQ(expected1, result1);

    const double expected2 = zisc::sqrt(expected1);
    ASSERT_DOUBLE_EQ(expected2, v.norm());
    ASSERT_DOUBLE_EQ(1.0 / expected2, v.inverseNorm());

    const auto result2 = v.normalized();
    const double expected3 = 2.0 * v.inverseNorm();
    for (int i = 0; i < 3; ++i)
      ASSERT_DOUBLE_EQ(expected3, result2[i]);
  }
}
