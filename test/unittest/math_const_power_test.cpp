/*!
  \file math_const_power_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "math_const_test.hpp"
#include "zisc/math_const.hpp"
#include "zisc/zisc_config.hpp"

TEST(ConstMathTest, PowerTest)
{
  constexpr int start = -126;
  constexpr int end = 127;
  PowerTest<end, start>::testPower();
}

TEST(ConstMathTest, PowTest)
{
//  constexpr int start = -126;
//  constexpr int end = 127;
//  PowerTest<end, start>::testPow();
  FAIL();
}

TEST(ConstMathTest, SqrtTestF)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  PowerConstexprTest<float, end, start>::testSqrt();
}

TEST(ConstMathTest, CbrtTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  PowerConstexprTest<float, end, start>::testCbrt();
}

TEST(ConstMathTest, ExpTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  PowerConstexprTest<float, end, start>::testExp();
}

TEST(ConstMathTest, Log2TestF)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  PowerConstexprTest<float, end, start>::testLog2();
}

TEST(ConstMathTest, LogTestF)
{
  constexpr int start = 0;
  constexpr int end = 512;
  PowerConstexprTest<float, end, start>::testLog();
}

TEST(ConstMathTest, Log10TestF)
{
  constexpr int start = 0;
  constexpr int end = 512;
  PowerConstexprTest<float, end, start>::testLog10();
}
