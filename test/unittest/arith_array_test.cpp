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
      ASSERT_EQ(0, array[i]);
  }
  {
    auto make_array = []()
    {
      zisc::ArithArray<zisc::uint, 4> array{{0, 1, 2, 3}};
      return array;
    };
    constexpr auto array = make_array();
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(i, array[i]);
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
      ASSERT_EQ(2, array[i]);
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
      ASSERT_EQ(i, array[i]);
  }
  {
    auto make_array = []()
    {
      std::array<zisc::uint, 4> a{{0, 1, 2, 3}};
      return zisc::ArithArray<zisc::uint, 4>{a};
    };
    constexpr auto array = make_array();
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(i, array[i]);
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
      ASSERT_EQ(-i, result[i]);
  }
  {
    constexpr auto v = make_array({0, 1, 2, 3});
    constexpr auto result1 = v + v;
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(i * 2, result1[i]);

    auto add = [](const zisc::ArithArray<zisc::uint, 4>& input)
    {
      zisc::ArithArray<zisc::uint, 4> array{{1, 1, 1, 1}};
      array += input;
      return array;
    };
    constexpr auto result2 = add(v);
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(i + 1, result2[i]);
  }
  {
    constexpr auto v = make_array({0, 1, 2, 3});
    constexpr auto result1 = v - v;
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(0, result1[i]);
    auto sub = [](const zisc::ArithArray<zisc::uint, 4>& input)
    {
      zisc::ArithArray<zisc::uint, 4> array{{4, 4, 4, 4}};
      array -= input;
      return array;
    };
    constexpr auto result2 = sub(v);
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(4 - i, result2[i]);
  }
  {
    constexpr auto v = make_array({0, 1, 2, 3});
    constexpr auto result1 = v * v;
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(i * i, result1[i]);
    auto mul = [](const zisc::ArithArray<zisc::uint, 4>& input)
    {
      zisc::ArithArray<zisc::uint, 4> array{{2, 2, 2, 2}};
      array *= input;
      return array;
    };
    constexpr auto result2 = mul(v);
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(2 * i, result2[i]);
    constexpr auto result3 = v * 2u;
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(2 * i, result3[i]);
  }
  {
    constexpr auto v = make_array({1, 2, 3, 4});
    constexpr auto result1 = v / v;
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(1, result1[i]);
    auto div = [](const zisc::ArithArray<zisc::uint, 4>& input)
    {
      zisc::ArithArray<zisc::uint, 4> array{{12, 12, 12, 12}};
      array /= input;
      return array;
    };
    constexpr auto result2 = div(v);
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(12 / (i + 1), result2[i]);
    constexpr zisc::ArithArray<zisc::uint, 4> v2{{12, 12, 12, 12}};
    constexpr auto result3 = v2 / 2u;
    for (zisc::uint i = 0; i < 4; ++i)
      ASSERT_EQ(6, result3[i]);
  }
  {
    constexpr auto v1 = make_array({1, 2, 3, 4});
    constexpr auto v2 = make_array({2, 3, 4, 5});
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
    constexpr auto v = make_array({0, 1, 2, 3});
    const auto result = zisc::dot(v, v);
    ASSERT_EQ(14, result);
  }
  {
    constexpr zisc::ArithArray<int, 3> v1{{1, 0, 0}};
    constexpr zisc::ArithArray<int, 3> v2{{0, 1, 0}};
    constexpr auto result = zisc::cross(v1, v2);
    constexpr zisc::ArithArray<int, 3> expected{{0, 0, 1}};
    ASSERT_EQ(expected, result);
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
    ASSERT_DOUBLE_EQ(0.0, v[0]);
    ASSERT_DOUBLE_EQ(0.0, v[1]);
    ASSERT_DOUBLE_EQ(0.5, v[2]);
    ASSERT_DOUBLE_EQ(1.0, v[3]);

    constexpr auto result1 = v.isAllInBounds(0.0, 1.0);
    ASSERT_FALSE(result1);
    constexpr auto result2 = v.isAllInBounds(0.0, 1.0 + std::numeric_limits<double>::epsilon());
    ASSERT_TRUE(result2);
    constexpr auto result3 = v.isAllInClosedBounds(0.0, 1.0);
    ASSERT_TRUE(result3);
  }
  {
    constexpr zisc::ArithArray<double, 4> v1{};
    constexpr bool result1 = v1.isAllZero();
    ASSERT_TRUE(result1);
    constexpr zisc::ArithArray<double, 4> v2{{0.0, 1.0, 2.0, 3.0}};
    constexpr bool result2 = v2.isAllZero();
    ASSERT_FALSE(result2);
  }
  {
    zisc::ArithArray<double, 3> v{};

    ASSERT_TRUE(v.hasValue(0.0));
    ASSERT_FALSE(v.hasValue(1.0));

    ASSERT_FALSE(v.hasInf());
    ASSERT_FALSE(v.hasNan());
    v[1] = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(v.hasInf());
    v[1] = std::numeric_limits<double>::quiet_NaN();
    ASSERT_TRUE(v.hasNan());
  }
  {
    constexpr zisc::ArithArray<int, 6> v{{-1, 0, 1, 2, 3, 4}};
    constexpr auto sum = v.sum();
    ASSERT_EQ(9, sum);
    constexpr auto m = v.max();
    ASSERT_EQ(4, m);
    constexpr auto n = v.min();
    ASSERT_EQ(-1, n);
  }
  {
    constexpr zisc::ArithArray<double, 4> v{{1.0, 1.0, 1.0, 1.0}};
    constexpr auto sum = v.sum();
    ASSERT_DOUBLE_EQ(4.0, sum);
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
    ASSERT_EQ(expected, sum);
  }
  {
    int sum = 0;
    for (const auto value : array)
      sum += value;
    ASSERT_EQ(expected, sum);
  }
}
