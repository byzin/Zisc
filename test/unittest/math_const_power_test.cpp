/*!
  \file math_const_power_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/math/math.hpp"
// Test
//#include "math_const_test.hpp"

namespace {

template <int i, int end>
struct PowerTest
{
  static void testPowInt()
  {
    {
      constexpr float b = 0.0f;
      constexpr float result = zisc::pow(b, i);
      const float expected = std::powf(b, i);
      EXPECT_FLOAT_EQ(expected, result) << "pow(" << b << "," << i << ") failed.";
    }
    {
      constexpr float b = zisc::kPi<float> / 2.0f;
      constexpr float result = zisc::pow(b, i);
      const float expected = std::powf(b, i);
      EXPECT_FLOAT_EQ(expected, result) << "pow(" << b << "," << i << ") failed.";
    }
    {
      constexpr float b = zisc::kPi<float> / 4.0f;
      constexpr float result = zisc::pow(b, i);
      const float expected = std::powf(b, i);
      EXPECT_FLOAT_EQ(expected, result) << "pow(" << b << "," << i << ") failed.";
    }
    {
      constexpr float b = -zisc::kPi<float> / 2.0f;
      constexpr float result = zisc::pow(b, i);
      const float expected = std::powf(b, i);
      EXPECT_FLOAT_EQ(expected, result) << "pow(" << b << "," << i << ") failed.";
    }
    if constexpr (i < end)
      PowerTest<i + 1, end>::testPowInt();
  }

  static void testPow()
  {
    constexpr float e = 4.0f * zisc::cast<float>(i) / zisc::cast<float>(end);
    {
      constexpr float b = 0.0f;
      constexpr float result = zisc::pow(b, e);
      const float expected = std::pow(b, e);
      EXPECT_FLOAT_EQ(expected, result) << "pow(" << b << "," << e << ") failed.";
    }
    {
      constexpr float b = zisc::kPi<float> / 2.0f;
      constexpr float result = zisc::pow(b, e);
      const float expected = std::pow(b, e);
      EXPECT_FLOAT_EQ(expected, result) << "pow(" << b << "," << e << ") failed.";
    }
    {
      constexpr float b = zisc::kPi<float> / 4.0f;
      constexpr float result = zisc::pow(b, e);
      const float expected = std::pow(b, e);
      EXPECT_FLOAT_EQ(expected, result) << "pow(" << b << "," << e << ") failed.";
    }
//    {
//      constexpr float b = -zisc::kPi<float> / 2.0f;
//      constexpr float result = zisc::pow(b, e);
//      const float expected = std::pow(b, e);
//      EXPECT_FLOAT_EQ(expected, result) << "pow(" << b << "," << e << ") failed.";
//    }
    if constexpr (i < end)
      PowerTest<i + 1, end>::testPow();
  }
};

} // namespace 

TEST(MathTest, ConstantPowIntFTest)
{
  constexpr int start = -32;
  constexpr int end = 32;
  ::PowerTest<start, end>::testPowInt();
}

TEST(MathTest, ConstantPowFTest)
{
  constexpr int start = -256;
  constexpr int end = 256;
  ::PowerTest<start, end>::testPow();
}

//TEST(ConstMathTest, PowTest)
//{
////  constexpr int start = -126;
////  constexpr int end = 127;
////  PowerTest<end, start>::testPow();
//  FAIL();
//}
//
//TEST(ConstMathTest, SqrtTestF)
//{
//  constexpr int start = 0;
//  constexpr int end = 1024;
//  PowerConstexprTest<float, end, start>::testSqrt();
//}
//
//TEST(ConstMathTest, CbrtTestF)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  PowerConstexprTest<float, end, start>::testCbrt();
//}
//
//TEST(ConstMathTest, ExpTestF)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  PowerConstexprTest<float, end, start>::testExp();
//}
//
//TEST(ConstMathTest, Log2TestF)
//{
//  constexpr int start = 0;
//  constexpr int end = 1024;
//  PowerConstexprTest<float, end, start>::testLog2();
//}
//
//TEST(ConstMathTest, LogTestF)
//{
//  constexpr int start = 0;
//  constexpr int end = 512;
//  PowerConstexprTest<float, end, start>::testLog();
//}
//
//TEST(ConstMathTest, Log10TestF)
//{
//  constexpr int start = 0;
//  constexpr int end = 512;
//  PowerConstexprTest<float, end, start>::testLog10();
//}
