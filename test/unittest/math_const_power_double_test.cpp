/*!
  \file math_const_power_double_test.cpp
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

TEST(ConstMathTest, SqrtTestD)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  PowerConstexprTest<double, end, start>::testSqrt();
}

TEST(ConstMathTest, CbrtTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  PowerConstexprTest<double, end, start>::testCbrt();
}

TEST(ConstMathTest, ExpTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  PowerConstexprTest<double, end, start>::testExp();
}

TEST(ConstMathTest, Log2TestD)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  PowerConstexprTest<double, end, start>::testLog2();
}

TEST(ConstMathTest, LogTestD)
{
  constexpr int start = 0;
  constexpr int end = 512;
  PowerConstexprTest<double, end, start>::testLog();
}

TEST(ConstMathTest, Log10TestD)
{
  constexpr int start = 0;
  constexpr int end = 512;
  PowerConstexprTest<double, end, start>::testLog10();
}
