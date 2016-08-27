/*!
  \file point_test.cpp
  \author zin
  */

// Standard C++ library
#include <type_traits>
#include <memory>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/point.hpp"
#include "zisc/type_traits.hpp"
#include "zisc/vector.hpp"
#include "zisc/utility.hpp"

template <typename Float>
void testPoint3()
{
  static_assert(zisc::kIsFloat<Float>, "Float isn't floating point type.");
  using zisc::cast;

  zisc::Point<Float, 3> point{0.0, 1.0, 2.0};
  zisc::Vector<Float, 3> vector{1.0, 2.0, 3.0};

  // Addition test
  auto addition = point + vector;
  for (int i = 0; i < 3; ++i) {
    const double value = cast<double>(i) * 2.0 + 1.0;
    ASSERT_DOUBLE_EQ(value, addition[i]) << "Addition test failed.";
  }
  // Subtraction test
  auto subtraction = point - vector;
  for (int i = 0; i < 3; ++i) {
    ASSERT_DOUBLE_EQ(-1.0, subtraction[i]) << "Subtraction test failed.";
  }
  // Subtraction point test
  zisc::Point<Float, 3> point2{3.0, 0.0, -3.0};
  auto direction = point2 - point;
  ASSERT_DOUBLE_EQ(3.0, direction[0])
      << "Subtraction point test failed.";
  ASSERT_DOUBLE_EQ(-1.0, direction[1])
      << "Subtraction point test failed.";
  ASSERT_DOUBLE_EQ(-5.0, direction[2])
      << "Subtraction point test failed.";
}

TEST(PointTest, FloatTest)
{
  testPoint3<float>();
}

TEST(PointTest, DoubleTest)
{
  testPoint3<double>();
}
