/*!
  \file compensated_summation_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/compensated_summation.hpp"
#include "zisc/math.hpp"

TEST(CompensatedSummationTest, ConstexprSumTest)
{
  constexpr int loop = 1'000'0;
  constexpr double value = 1.0e-4;

  auto calc_normal_sum = []()
  {
    double sum = 0.0;
    for (int i = 0; i < loop; ++i)
      sum += value;
    return sum;
  };

  auto calc_compensated_sum = []()
  {
    zisc::CompensatedSummation<double> sum{0.0};
    for (int i = 0; i < loop; ++i)
      sum.add(value);
    return sum.get();
  };

  // Normal sum
  {
    constexpr double sum = calc_normal_sum();
    EXPECT_NE(1.0, sum);
  }
  // Compensated sum
  {
    constexpr double sum = calc_compensated_sum();
    EXPECT_EQ(1.0, sum);
  }

  // 
  {
    constexpr double init = 1.0e+4;
    constexpr double v1 = 3.14159;
    constexpr double v2 = 2.71828;
    constexpr double expected = init + (v1 + v2);
    // Naive summation
    {
      constexpr double sum = (init + v1) + v2;
      EXPECT_NE(expected, sum);
    }
    // Compensated summation
    {
      auto get_sum = [](const double i, const double s, const double t)
      {
        zisc::CompensatedSummation<double> sum{0.0};
        sum.set(i);
        sum.add({s, t});
        return sum.get();
      };
      constexpr double sum = get_sum(init, v1, v2);
      EXPECT_EQ(expected, sum);
    }
  }

  // Large number addition test
  {
    constexpr double large_v = zisc::power<100>(10.0);
    constexpr double sum1 = ((1.0 + large_v) + 1.0) - large_v;
    EXPECT_NE(2.0, sum1);
    constexpr zisc::CompensatedSummation<double> sum2{{1.0, large_v, 1.0, -large_v}};
    EXPECT_EQ(2.0, sum2.get());
  }
}

TEST(CompensatedSummationTest, SumTest)
{
  auto calc_normal_sum = [](const int loop, const double value)
  {
    double sum = 0.0;
    for (int i = 0; i < loop; ++i)
      sum += value;
    return sum;
  };

  auto calc_compensated_sum = [](const int loop, const double value)
  {
    zisc::CompensatedSummation<double> sum{0.0};
    for (int i = 0; i < loop; ++i)
      sum.add(value);
    return sum.get();
  };

  auto get_value = [](const int digit)
  {
    double value = 0;
    switch (digit) {
     case 1:
      value = 1.0e-1;
      break;
     case 2:
      value = 1.0e-2;
      break;
     case 3:
      value = 1.0e-3;
      break;
     case 4:
      value = 1.0e-4;
      break;
     case 5:
      value = 1.0e-5;
      break;
     case 6:
      value = 1.0e-6;
      break;
     case 7:
      value = 1.0e-7;
      break;
     case 8:
      value = 1.0e-8;
      break;
     case 9:
      value = 1.0e-9;
      break;
     case 10:
      value = 1.0e-10;
      break;
     default:
      break;
    }
    return value;
  };

  for (int digit = 1; digit < 10; ++digit) {
    const int loop = zisc::power(10, digit);
    const double value = get_value(digit);
    // Normal sum
    {
      const double sum = calc_normal_sum(loop, value);
      EXPECT_NE(1.0, sum);
    }
    // Compensated sum
    {
      const double sum = calc_compensated_sum(loop, value);
      EXPECT_EQ(1.0, sum);
    }
  }
}
