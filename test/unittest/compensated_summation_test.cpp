/*!
  \file compensated_summation_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/math/math.hpp"

TEST(CompensatedSummationTest, ConstexprSumTest)
{
  constexpr int loop = 1'000'0;
  constexpr double value = 1.0e-4;

  auto calc_simple_sum = [](const int l, const double v)
  {
    double sum = 0.0;
    for (int i = 0; i < l; ++i)
      sum = zisc::Math::add(sum, v);
    return sum;
  };

  auto calc_compensated_sum = [](const int l, const double v)
  {
    double sum = 0.0;
    double compensation = 0.0;
    for (int i = 0; i < l; ++i)
      sum = zisc::Math::add(sum, v, &compensation);
    return sum + compensation;
  };

  // Simple sum
  {
    constexpr double sum = calc_simple_sum(loop, value);
    EXPECT_NE(1.0, sum);
  }
  // Compensated sum
  {
    constexpr double sum = calc_compensated_sum(loop, value);
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
        double sum = i;
        double compensation = 0.0;
        sum = zisc::Math::add(sum, s, &compensation);
        sum = zisc::Math::add(sum, t, &compensation);
        return sum + compensation;
      };
      constexpr double sum = get_sum(init, v1, v2);
      EXPECT_EQ(expected, sum);
    }
  }

  // Large number addition test
  {
    constexpr double large_v = zisc::pow(10.0, 100);
    {
      constexpr double sum = ((1.0 + large_v) + 1.0) - large_v;
      EXPECT_NE(2.0, sum);
    }
    auto get_sum = [](const double a, const double b, const double c, const double d)
    {
      double sum = a;
      double compensation = 0.0;
      sum = zisc::Math::add(sum, b, &compensation);
      sum = zisc::Math::add(sum, c, &compensation);
      sum = zisc::Math::add(sum, d, &compensation);
      return sum + compensation;
    };
    {
      constexpr double sum = get_sum(1.0, large_v, 1.0, -large_v);
      EXPECT_EQ(2.0, sum);
    }
  }
}

TEST(CompensatedSummationTest, SumTest)
{
  auto calc_simple_sum = [](const int loop, const double value)
  {
    double sum = 0.0;
    for (int i = 0; i < loop; ++i)
      sum = zisc::Math::add(sum, value);
    return sum;
  };

  auto calc_compensated_sum = [](const int loop, const double value)
  {
    double sum = 0.0;
    double compensation = 0.0;
    for (int i = 0; i < loop; ++i)
      sum = zisc::Math::add(sum, value, &compensation);
    return sum + compensation;
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
    const int loop = zisc::pow(10, digit);
    const double value = get_value(digit);
    // Simple sum
    {
      const double sum = calc_simple_sum(loop, value);
      EXPECT_NE(1.0, sum);
    }
    // Compensated sum
    {
      const double sum = calc_compensated_sum(loop, value);
      EXPECT_EQ(1.0, sum);
    }
  }
}
