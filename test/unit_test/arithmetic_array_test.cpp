/*!
  \file arithmetic_array_test.cpp
  \author zin
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/arithmetic_array.hpp"
#include "zisc/utility.hpp"

TEST(ArithmeticArrayTest, Array3Test)
{
  using zisc::cast;

  zisc::ArithmeticArray<double, 3> array1;
  double value;

  // Constructor test
  for (int i = 0; i < 3; ++i)
    ASSERT_DOUBLE_EQ(0.0, array1[0]) << "Constructor test failed.";

  // Set test
  for (zisc::uint i = 0; i < 3; ++i) {
    array1.set(i, cast<double>(i));
    ASSERT_DOUBLE_EQ(cast<double>(i), array1[i]);
  }

  zisc::ArithmeticArray<double, 3> array2{4.0, 5.0, 6.0};

  // IsZero test
  zisc::ArithmeticArray<int, 3> non_zero_array{1, 2, 3};
  ASSERT_FALSE(non_zero_array.isAllZero()) << "IsZero test failed.";
  zisc::ArithmeticArray<int, 3> zero_array{0, 0, 0};
  ASSERT_TRUE(zero_array.isAllZero()) << "IsZero test failed.";

  // Fill test
  zisc::ArithmeticArray<int, 4> filled_array{0, 1, 2, 3};
  filled_array.fill(0);
  ASSERT_TRUE(filled_array.isAllZero()) << "Fill test failed.";

  // Addition operation test
  auto addition = array1 + array2;
  for (int i = 0; i < 3; ++i) {
    value = 4.0 + 2.0 * cast<double>(i);
    ASSERT_DOUBLE_EQ(value, addition[i]) << "Addition test failed.";
  }

  // Subtraction operation test
  auto subtraction = array1 - array2;
  for (int i = 0; i < 3; ++i)
    ASSERT_DOUBLE_EQ(-4.0, subtraction[i]) << "Subtraction test failed.";

  // Multiplication operation test
  auto multiplication = array1 * array2;
  ASSERT_DOUBLE_EQ(0.0, multiplication[0]) << "Multiplication test failed.";
  ASSERT_DOUBLE_EQ(5.0, multiplication[1]) << "Multiplication test failed.";
  ASSERT_DOUBLE_EQ(12.0, multiplication[2]) << "Multiplication test failed.";

  // Dot product test
  const double dot = zisc::dot(array1, array2);
  ASSERT_DOUBLE_EQ(17.0, dot) << "Dot product test failed.";

  // Division test
  auto division = 1.0 / array2;
  for (int i = 0; i < 3; ++i) {
    value = 1.0 / cast<double>(4 + i);
    ASSERT_DOUBLE_EQ(value, division[i]) << "Division test failed.";
  }

  // Get maximum and minimum elements test
  zisc::ArithmeticArray<double, 4> array3{-3.0, 2.0, -1.0, 0.0},
                                   array4{3.0, -2.0, 1.0, 0.0};
  auto max_array = maxElements(array3, array4);
  auto min_array = minElements(array3, array4);
  for (int i = 0; i < 4; ++i) {
    ASSERT_DOUBLE_EQ(cast<double>(3 - i), max_array[i]) 
        << "Get max elements test failed.";
    ASSERT_DOUBLE_EQ(cast<double>(-3 + i), min_array[i])
        << "Get min elements test failed.";
  }

  // Clamp test
  array3.clampAll(0.0, 2.0);
  ASSERT_DOUBLE_EQ(0.0, array3.min()) << "Clamp test failed.";
  ASSERT_DOUBLE_EQ(2.0, array3.max()) << "Clamp test failed.";
  ASSERT_TRUE(array3.isAllInBounds(0.0, 2.0000001));

  // Sum test
  const zisc::ArithmeticArray<double, 5> a{1.0, 2.0, 3.0, 4.0, 5.0};
  ASSERT_DOUBLE_EQ(15.0, a.sum());
  const zisc::ArithmeticArray<int, 5> b{1, 2, 3, 4, 5};
  ASSERT_EQ(15, b.sum());

  ASSERT_FALSE(a.hasValue(0.0));
  ASSERT_TRUE(a.hasValue(1.0));
}

TEST(ArithmeticArrayTest, IteratorTest)
{
  const zisc::ArithmeticArray<int, 5> array{1, 2, 3, 4, 5};
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

namespace  {

constexpr zisc::ArithmeticArray<int, 3> makeConstArray1()
{
  zisc::ArithmeticArray<int, 3> array;
  for (zisc::uint i = 0; i < array.size(); ++i)
    array[i] = i + 1;
  return array;
}

constexpr zisc::ArithmeticArray<int, 3> makeConstArray2()
{
  zisc::ArithmeticArray<int, 3> a{3, 2, 1};
  zisc::ArithmeticArray<int, 3> array{a};
  return array;
}

constexpr int sum(const zisc::ArithmeticArray<int, 3>& array)
{
  int value = 0;
  for (auto i = array.begin(); i != array.end(); ++i)
    value += *i;
  return value;
}

constexpr zisc::ArithmeticArray<int, 3> clamp()
{
  auto array = makeConstArray1();
  array.clampAll(0, 2);
  return array;
}

} // namespace

TEST(ArithmeticArrayTest, ConstexprTest)
{
  constexpr auto array1 = ::makeConstArray1();
  constexpr auto array2 = ::makeConstArray2();
  // Constructor
  {
    for (zisc::uint i = 0; i < array1.size(); ++i) {
      const int expected = i + 1;
      ASSERT_EQ(expected, array1[i]);
    }
  }
  {
    for (zisc::uint i = 0; i < array2.size(); ++i) {
      const int expected = 3 - i;
      ASSERT_EQ(expected, array2[i]);
    }
  }
  // Iterator
  {
    constexpr int s = ::sum(array1);
    ASSERT_EQ(6, s);
  }
  // Arithmetic operations
  {
    constexpr auto result = array1 + array2;
    ASSERT_EQ(4, result[2]);
  }
  {
    constexpr auto result = array1 - array2;
    ASSERT_EQ(2, result[2]);
  }
  {
    constexpr auto result = array1 * array2;
    ASSERT_EQ(4, result[1]);
  }
  {
    constexpr auto result = array1 / array2;
    ASSERT_EQ(1, result[1]);
  }
  // Clamp
  {
    constexpr auto array = ::clamp();
    ASSERT_EQ(2, array[2]);
  }
  // Minmax
  {
    constexpr auto min_array = zisc::minElements(array1, array2);
    ASSERT_EQ(1, min_array[0]);
    ASSERT_EQ(1, min_array[2]);
    constexpr auto max_array = zisc::maxElements(array1, array2);
    ASSERT_EQ(3, max_array[0]);
    ASSERT_EQ(3, max_array[2]);
  }
  // Value check
  {
    constexpr zisc::ArithmeticArray<int, 3> a;
    static_assert(a.isAllZero(), "The array constructor is wrong.");
    static_assert(array1.isAllInBounds(1, 4), "The array initialization is wrong.");
    static_assert(array1.isAllInClosedBounds(1, 3), "The array initialization is wrong.");
  }
}
