/*!
  \file arith_array_test.cpp
  \author zin
  */

// GoogleTest
#include "gtest/gtest.h"
// Standard C++ library
#include <array>
#include <initializer_list>
#include <type_traits>
// Zisc
#include "zisc/arith_array.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

TEST(ArithArrayTest, ConstructionTest)
{
  {
    constexpr zisc::ArithArray<zisc::uint, 4> array{};
    constexpr auto size = array.size();
    ASSERT_EQ(4, size);
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(0, array[i]);
  }
  {
    auto make_array = []()
    {
      zisc::ArithArray<zisc::uint, 4> array{{0, 1, 2, 3}};
      return array;
    };
    constexpr auto array = make_array();
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(i, array[i]);
  }
  {
    auto make_array = []()
    {
      zisc::ArithArray<zisc::uint, 4> array{};
      array.fill(2);
      return array;
    };
    constexpr auto array = make_array();
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(2, array[i]);
  }
  {
    auto make_array = []()
    {
      zisc::ArithArray<zisc::uint, 4> array{};
      array.set({0, 1, 2, 3});
      array.set(3, 3);
      return array;
    };
    constexpr auto array = make_array();
    static_assert(array[3] == 3);
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(i, array[i]);
  }
  {
    auto make_array = []()
    {
      std::array<zisc::uint, 4> a{{0, 1, 2, 3}};
      return zisc::ArithArray<zisc::uint, 4>{a};
    };
    constexpr auto array = make_array();
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(i, array[i]);
  }
}

TEST(ArithArrayTest, OperatorTest)
{
  auto make_array = [](std::initializer_list<zisc::uint> init_list)
  {
    zisc::ArithArray<zisc::uint, 4> array{init_list};
    return array;
  };

  {
    constexpr zisc::ArithArray<int, 4> v{{0, 1, 2, 3}};
    constexpr auto result = -v;
    for (int i = 0; i < 4; ++i)
      EXPECT_EQ(-i, result[i]);
  }
  {
    constexpr auto v = make_array({0, 1, 2, 3});
    constexpr auto result1 = v + v;
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(i * 2, result1[i]);

    auto add = [](const zisc::ArithArray<zisc::uint, 4>& input)
    {
      zisc::ArithArray<zisc::uint, 4> array{{1, 1, 1, 1}};
      array += input;
      return array;
    };
    constexpr auto result2 = add(v);
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(i + 1, result2[i]);
  }
  {
    constexpr auto v = make_array({0, 1, 2, 3});
    constexpr auto result1 = v - v;
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(0, result1[i]);
    auto sub = [](const zisc::ArithArray<zisc::uint, 4>& input)
    {
      zisc::ArithArray<zisc::uint, 4> array{{4, 4, 4, 4}};
      array -= input;
      return array;
    };
    constexpr auto result2 = sub(v);
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(4 - i, result2[i]);
  }
  {
    constexpr auto v = make_array({0, 1, 2, 3});
    constexpr auto result1 = v * v;
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(i * i, result1[i]);
    auto mul = [](const zisc::ArithArray<zisc::uint, 4>& input)
    {
      zisc::ArithArray<zisc::uint, 4> array{{2, 2, 2, 2}};
      array *= input;
      return array;
    };
    constexpr auto result2 = mul(v);
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(2 * i, result2[i]);
    constexpr auto result3 = v * 2u;
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(2 * i, result3[i]);
  }
  {
    constexpr auto v = make_array({1, 2, 3, 4});
    constexpr auto result1 = v / v;
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(1, result1[i]);
    auto div = [](const zisc::ArithArray<zisc::uint, 4>& input)
    {
      zisc::ArithArray<zisc::uint, 4> array{{12, 12, 12, 12}};
      array /= input;
      return array;
    };
    constexpr auto result2 = div(v);
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(12 / (i + 1), result2[i]);
    constexpr zisc::ArithArray<zisc::uint, 4> v2{{12, 12, 12, 12}};
    constexpr auto result3 = v2 / 2u;
    for (zisc::uint i = 0; i < 4; ++i)
      EXPECT_EQ(6, result3[i]);
  }
  {
    constexpr auto v1 = make_array({1, 2, 3, 4});
    constexpr auto v2 = make_array({2, 3, 4, 5});
    constexpr bool result1 = v1 == v1;
    constexpr bool result2 = v1 != v1;
    EXPECT_TRUE(result1);
    EXPECT_FALSE(result2);
    constexpr bool result3 = v1 == v2;
    constexpr bool result4 = v1 != v2;
    EXPECT_FALSE(result3);
    EXPECT_TRUE(result4);
  }
  {
    constexpr auto v = make_array({0, 1, 2, 3});
    const auto result = zisc::dot(v, v);
    EXPECT_EQ(14, result);
  }
  {
    constexpr zisc::ArithArray<int, 3> v1{{1, 0, 0}};
    constexpr zisc::ArithArray<int, 3> v2{{0, 1, 0}};
    constexpr auto result = zisc::cross(v1, v2);
    constexpr zisc::ArithArray<int, 3> expected{{0, 0, 1}};
    EXPECT_EQ(expected, result);
  }
}

TEST(ArithArrayTest, FunctionTest)
{
  {
    auto make_array = []()
    {
      zisc::ArithArray<double, 4> array{{-1.0, 0.0, 0.5, 2.0}};
      array.clampAll(0.0, 1.0);
      return array;
    };
    constexpr auto v = make_array();
    EXPECT_DOUBLE_EQ(0.0, v[0]);
    EXPECT_DOUBLE_EQ(0.0, v[1]);
    EXPECT_DOUBLE_EQ(0.5, v[2]);
    EXPECT_DOUBLE_EQ(1.0, v[3]);

    constexpr auto result1 = v.isAllInBounds(0.0, 1.0);
    EXPECT_FALSE(result1);
    constexpr auto result2 = v.isAllInBounds(0.0, 1.0 + std::numeric_limits<double>::epsilon());
    EXPECT_TRUE(result2);
    constexpr auto result3 = v.isAllInClosedBounds(0.0, 1.0);
    EXPECT_TRUE(result3);
  }
  {
    constexpr zisc::ArithArray<double, 4> v1{};
    constexpr bool result1 = v1.isAllZero();
    EXPECT_TRUE(result1);
    constexpr zisc::ArithArray<double, 4> v2{{0.0, 1.0, 2.0, 3.0}};
    constexpr bool result2 = v2.isAllZero();
    EXPECT_FALSE(result2);
  }
  {
    auto make_array = [](const double d)
    {
      zisc::ArithArray<double, 3> v{{0.0, d, -0.0}};
      return v;
    };

    constexpr auto v1 = make_array(0.0);
    constexpr auto v2 = make_array(2.0);
    constexpr auto v3 = make_array(std::numeric_limits<double>::infinity());
    constexpr auto v4 = make_array(-std::numeric_limits<double>::infinity());
    constexpr auto v5 = make_array(std::numeric_limits<double>::quiet_NaN());
    constexpr auto v6 = make_array(std::numeric_limits<double>::signaling_NaN());
    constexpr auto v7 = make_array(std::numeric_limits<double>::denorm_min());

    // Value
    {
      constexpr bool r1 = v1.hasValue(0.0);
      EXPECT_TRUE(r1);
      constexpr bool r2 = v1.hasValue(1.0);
      EXPECT_FALSE(r2);
    }
    // Inf
    {
      constexpr bool r1 = v1.hasInf();
      EXPECT_FALSE(r1);
      constexpr bool r2 = v2.hasInf();
      EXPECT_FALSE(r2);
      constexpr bool r3 = v3.hasInf();
      EXPECT_TRUE(r3);
      constexpr bool r4 = v4.hasInf();
      EXPECT_TRUE(r4);
      const bool r5 = v5.hasInf();
      EXPECT_FALSE(r5);
      const bool r6 = v6.hasInf();
      EXPECT_FALSE(r6);
      constexpr bool r7 = v7.hasInf();
      EXPECT_FALSE(r7);
    }
    // Inf
    {
      constexpr bool r1 = v1.hasNan();
      EXPECT_FALSE(r1);
      constexpr bool r2 = v2.hasNan();
      EXPECT_FALSE(r2);
      constexpr bool r3 = v3.hasNan();
      EXPECT_FALSE(r3);
      constexpr bool r4 = v4.hasNan();
      EXPECT_FALSE(r4);
      const bool r5 = v5.hasNan();
      EXPECT_TRUE(r5);
      const bool r6 = v6.hasNan();
      EXPECT_TRUE(r6);
      constexpr bool r7 = v7.hasNan();
      EXPECT_FALSE(r7);
    }
    // Subnormal
    {
      constexpr bool r1 = v1.hasSubnormal();
      EXPECT_FALSE(r1);
      constexpr bool r2 = v2.hasSubnormal();
      EXPECT_FALSE(r2);
      constexpr bool r3 = v3.hasSubnormal();
      EXPECT_FALSE(r3);
      constexpr bool r4 = v4.hasSubnormal();
      EXPECT_FALSE(r4);
      const bool r5 = v5.hasSubnormal();
      EXPECT_FALSE(r5);
      const bool r6 = v6.hasSubnormal();
      EXPECT_FALSE(r6);
      constexpr bool r7 = v7.hasSubnormal();
      EXPECT_TRUE(r7);
    }
  }
  {
    constexpr zisc::ArithArray<int, 6> v{{-1, 0, 1, 2, 3, 4}};
    constexpr auto sum = v.sum();
    EXPECT_EQ(9, sum);
    constexpr auto m = v.max();
    EXPECT_EQ(4, m);
    constexpr auto n = v.min();
    EXPECT_EQ(-1, n);
  }
  {
    constexpr zisc::ArithArray<double, 4> v{{1.0, 1.0, 1.0, 1.0}};
    constexpr auto sum = v.sum();
    EXPECT_DOUBLE_EQ(4.0, sum);
  }
}

TEST(ArithArrayTest, IteratorTest)
{
  const zisc::ArithArray<int, 5> array{1, 2, 3, 4, 5};
  const int expected = 15;
  {
    int sum = 0;
    for (auto i = array.begin(); i != array.end(); ++i)
      sum += *i;
    EXPECT_EQ(expected, sum);
  }
  {
    int sum = 0;
    for (const auto value : array)
      sum += value;
    EXPECT_EQ(expected, sum);
  }
}
