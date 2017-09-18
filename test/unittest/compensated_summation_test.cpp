/*!
  \file compensated_summation_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/compensated_summation.hpp"

TEST(CompensatedSummationTest, SumTest)
{
  constexpr int loop = 1'000'000'000;
  constexpr double value = 1.0e-9;
  // Normal sum
  {
    double sum = 0.0;
    for (int i = 0; i < loop; ++i)
      sum += value;
    ASSERT_NE(1.0, sum);
  }
  // Compensated sum
  {
    zisc::CompensatedSummation<double> sum;
    sum.set(0.0);
    for (int i = 0; i < loop; ++i)
      sum.add(value);
    ASSERT_EQ(1.0, sum.get());
  }
}
