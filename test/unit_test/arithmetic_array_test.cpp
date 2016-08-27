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
  ASSERT_FALSE(non_zero_array.isZeroArray()) << "IsZero test failed.";
  zisc::ArithmeticArray<int, 3> zero_array{0, 0, 0};
  ASSERT_TRUE(zero_array.isZeroArray()) << "IsZero test failed.";

  // Fill test
  zisc::ArithmeticArray<int, 4> filled_array{0, 1, 2, 3};
  filled_array.fill(0);
  ASSERT_TRUE(filled_array.isZeroArray()) << "Fill test failed.";

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
  array3.clamp(0.0, 2.0);
  ASSERT_DOUBLE_EQ(0.0, array3.min()) << "Clamp test failed.";
  ASSERT_DOUBLE_EQ(2.0, array3.max()) << "Clamp test failed.";

  // Sum test
  const zisc::ArithmeticArray<double, 5> a{1.0, 2.0, 3.0, 4.0, 5.0};
  ASSERT_DOUBLE_EQ(15.0, a.sum());
  const zisc::ArithmeticArray<int, 5> b{1, 2, 3, 4, 5};
  ASSERT_EQ(15, b.sum());
}
