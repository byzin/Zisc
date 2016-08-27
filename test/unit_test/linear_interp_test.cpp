/*!
  \file linear_interp_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/linear_interp.hpp"
#include "zisc/utility.hpp"

TEST(LinearInterpTest, InterpolateTest)
{
  using zisc::cast;

  zisc::LinearInterp<double> interp;
  for (int i = 0; i < 11; ++i)
    interp.add(cast<double>(i), cast<double>(i * i));

  // Exist method test
  for (int i = 0; i < 11; ++i)
    ASSERT_TRUE(interp.exists(cast<double>(i))) << "Exist check test failed.";
  ASSERT_FALSE(interp.exists(-1.0)) << "Exist check test failed.";
  ASSERT_FALSE(interp.exists(5.5)) << "Exist check test failed.";
  ASSERT_FALSE(interp.exists(11.0)) << "Exist check test failed.";

  // Interpolate method test
  ASSERT_DOUBLE_EQ(0.0, interp(0.0)) << "Interpolation test failed.";
  ASSERT_DOUBLE_EQ(25.0, interp(5.0)) << "Interpolation test failed.";
  ASSERT_DOUBLE_EQ(100.0, interp(10.0)) << "Interpolation test failed.";
  ASSERT_DOUBLE_EQ((2.0 * 2.0 + 3.0 * 3.0) / 2.0, interp(2.5))
      << "Interpolation test failed.";
}
