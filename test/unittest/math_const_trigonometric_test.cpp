/*!
  \file math_const_trigonometric_test.cpp
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
#include "math_const_test.hpp"
#include "zisc/math_const.hpp"
#include "zisc/zisc_config.hpp"

TEST(ConstMathTest, SinTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  TrigConstexprTest<float, end, start>::testSin();
}

TEST(ConstMathTest, CosTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  TrigConstexprTest<float, end, start>::testCos();
}

TEST(ConstMathTest, TanTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  TrigConstexprTest<float, end, start>::testTan();
}

TEST(ConstMathTest, AsinTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  TrigConstexprTest<float, end, start>::testAsin();
}

TEST(ConstMathTest, AcosTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  TrigConstexprTest<float, end, start>::testAcos();
}

TEST(ConstMathTest, AtanTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  TrigConstexprTest<float, end, start>::testAtan();
}
