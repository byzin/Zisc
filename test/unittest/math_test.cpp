/*!
  \file math_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>
#include <type_traits>
#include <tuple>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/math.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

template <typename StlFunc, typename ZiscFunc>
void testTime(const std::string& func_name,
              StlFunc stl_func,
              ZiscFunc zisc_func,
              const int start,
              const int end,
              const int step)
{
  std::cout << func_name << " time test." << std::endl;

  using zisc::cast;
  using std::chrono::duration_cast;

  constexpr int test_count = 10; // Number of tests
  std::cout << "  Number of tests: " << test_count << std::endl;

  auto measure_time = [start, end, step](auto func)
  {
    using Clock = std::chrono::high_resolution_clock;
    volatile double result;
    auto start_time = Clock::now();
    for (volatile int i = start; i < end; i = i + step)
      result = func(i);
    auto end_time = Clock::now();
    return duration_cast<std::chrono::nanoseconds>(end_time - start_time);
  };

  std::chrono::nanoseconds stl_measure_time{0},
                           zisc_measure_time{0};
  for (volatile int n = 0; n < test_count; ++n) {
    // STL test
    {
      auto stl_time = measure_time(stl_func);
      std::cout << "   STL time[" << (n + 1) << "]: " 
          << duration_cast<std::chrono::milliseconds>(stl_time).count() << "ms "
          << "(" << stl_time.count() << " us)." << std::endl;
      stl_measure_time += stl_time;
    }
    // ZISC test
    {
      auto zisc_time = measure_time(zisc_func);
      std::cout << "  ZISC time[" << (n + 1) << "]: " 
          << duration_cast<std::chrono::milliseconds>(zisc_time).count() << "ms "
          << "(" << zisc_time.count() << " us)." << std::endl;
      zisc_measure_time += zisc_time;
    }
  }
  EXPECT_LT(zisc_measure_time.count(), stl_measure_time.count())
      << "zisc::" << func_name << " is slower than std::" << func_name << "."
      << std::endl;
}

template <typename StlFunc, typename ZiscFunc>
void testAccuracy(const std::string& func_name,
                  StlFunc stl_func,
                  ZiscFunc zisc_func,
                  const int start,
                  const int end,
                  const int step, 
                  const double* error = nullptr)
{
  std::cout << func_name << " accuracy test." << std::endl;

  using zisc::cast;

  auto print_progress = [start, end, step](const int i)
  {
    if (((i - start) % ((end - start) / 10)) == 0) {
      const int count = (i - start) / ((end - start) / 10);
      if (count != 0)
        std::cout << "  [" << (count * 10) << "%] " << std::flush;
    }
    if ((i + step) == end)
      std::cout << std::endl;
  };

  for (int i = start; i < end; i = i + step) {
    const auto expected = stl_func(i);
    const auto result = zisc_func(i);
    if (error == nullptr) {
      EXPECT_DOUBLE_EQ(expected, result)
          << "zisc::" << func_name << "(" << i << ") is wrong." << std::endl;
    }
    else {
      EXPECT_NEAR(expected, result, *error)
          << "zisc::" << func_name << "(" << i << ") is wrong." << std::endl;
    }
    print_progress(i);
  }
}

} // namespace

TEST(MathTest, PowerTest)
{
  // Power function test
  ASSERT_EQ(1, zisc::power<0>(2)) << "Power test failed.";
  ASSERT_EQ(2, zisc::power<1>(2)) << "Power test failed.";
  ASSERT_EQ(4, zisc::power<2>(2)) << "Power test failed.";
  ASSERT_EQ(8, zisc::power<3>(2)) << "Power test failed.";
  ASSERT_EQ(16, zisc::power<4>(2)) << "Power test failed.";
  ASSERT_EQ(32, zisc::power<5>(2)) << "Power test failed.";
  // Inverse pow test
  ASSERT_DOUBLE_EQ(0.5, zisc::power<-1>(2.0)) << "Power test failed.";
  ASSERT_DOUBLE_EQ(0.25, zisc::power<-2>(2.0)) << "Power test failed.";
  ASSERT_DOUBLE_EQ(0.125, zisc::power<-3>(2.0)) << "Power test failed.";
}

TEST(MathTest, PowPerformanceTest)
{
  const std::string func_name{"pow"};
  constexpr int start = -63;
  constexpr int end = 63;
  constexpr int step = 1;
  auto stl_func = [](const int i)
  {
    return std::pow(2.0, i);
  };
  auto zisc_func = [](const int i)
  {
    return zisc::power(2.0, i);
  };
  ::testTime(func_name, stl_func, zisc_func, start, end, step);
  ::testAccuracy(func_name, stl_func, zisc_func, start, end, step);
}

TEST(MathTest, SolveCubicOneTest)
{
  constexpr double error = 0.0000001;
  // Example 1
  const auto r1 = zisc::Math::solveCubicOne(6.0, -5.0, -17.0, 6.0);
  EXPECT_NEAR(2.0, r1, error);
  // Example 2
  const auto r2 = zisc::Math::solveCubicOne(1.0, -2.0, -6.0, 4.0);
  EXPECT_NEAR(3.414213562373095, r2, error);
  // Example 3
  const auto r3 = zisc::Math::solveCubicOne(1.0, 1.0, 1.0, -3.0);
  EXPECT_NEAR(1.0, r3, error);
  // Example 4
  const auto r4 = zisc::Math::solveCubicOne(4.0, 3.0, 2.0, 1.0);
  EXPECT_NEAR(-0.605829586188268, r4, error);
  // Example 5
  const auto r5 = zisc::Math::solveCubicOne(1.0, -6.0, 11.0, -6.0);
  EXPECT_NEAR(3.0, r5, error);
  // Example 6
  const auto r6 = zisc::Math::solveCubicOne(1.0, -5.0, 8.0, -4.0);
  EXPECT_NEAR(1.0, r6, error);
  // Example 7
  const auto r7 = zisc::Math::solveCubicOne(1.0, -3.0, 3.0, -1.0);
  EXPECT_NEAR(1.0, r7, error);
}

TEST(MathTest, SolveQuarticTest)
{
  // Example 1
  const auto r1 = zisc::Math::solveQuartic(3.0, 6.0, -123.0, -126.0, 1080.0);
  const auto& a1 = std::get<0>(r1);
  const int n1 = std::get<1>(r1);
  EXPECT_EQ(4, n1);
  EXPECT_DOUBLE_EQ(5.0, a1[0]);
  EXPECT_DOUBLE_EQ(3.0, a1[1]);
  EXPECT_DOUBLE_EQ(-4.0, a1[2]);
  EXPECT_DOUBLE_EQ(-6.0, a1[3]);
  // Example 2
  const auto r2 = zisc::Math::solveQuartic(-20.0, 5.0, 17.0, -29.0, 87.0);
  const auto& a2 = std::get<0>(r2);
  const int n2 = std::get<1>(r2);
  EXPECT_EQ(2, n2);
  EXPECT_FLOAT_EQ(1.48758311033f, static_cast<float>(a2[0]));
  EXPECT_FLOAT_EQ(-1.68200392658f, static_cast<float>(a2[1]));
  // Example 3
  const auto r3 = zisc::Math::solveQuartic(2.0, 2.0, -3.0, -3.0, -4.0);
  const auto& a3 = std::get<0>(r3);
  const int n3 = std::get<1>(r3);
  EXPECT_EQ(2, n3);
  EXPECT_FLOAT_EQ(1.43875f, static_cast<float>(a3[0]));
  EXPECT_FLOAT_EQ(-1.7439762f, static_cast<float>(a3[1]));
  // Example 4 
  const auto r4 = zisc::Math::solveQuartic(16.0, 96.0, -24.0, -664.0, -315.0);
  const auto& a4 = std::get<0>(r4);
  const int n4 = std::get<1>(r4);
  EXPECT_EQ(4, n4);
  EXPECT_DOUBLE_EQ( 5.0 / 2.0, a4[0]);
  EXPECT_DOUBLE_EQ(-1.0 / 2.0, a4[1]);
  EXPECT_DOUBLE_EQ(-7.0 / 2.0, a4[2]);
  EXPECT_DOUBLE_EQ(-9.0 / 2.0, a4[3]);
  // Equation 5
  const auto r5 = zisc::Math::solveQuartic(1.0, -22.0, 152.0, -362.0, 231.0);
  const auto& a5 = std::get<0>(r5);
  const int n5 = std::get<1>(r5);
  ASSERT_EQ(4, n5);
  ASSERT_DOUBLE_EQ(11.0, a5[0]);
  ASSERT_DOUBLE_EQ( 7.0, a5[1]);
  ASSERT_DOUBLE_EQ( 3.0, a5[2]);
  ASSERT_DOUBLE_EQ( 1.0, a5[3]);
  // Equation 3
  const auto r6 = zisc::Math::solveQuartic(1.0, -7.0, 5.0, 31.0, -30.0);
  const auto& a6 = std::get<0>(r6);
  const int n6 = std::get<1>(r6);
  ASSERT_EQ(4, n6);
  ASSERT_DOUBLE_EQ( 5.0, a6[0]);
  ASSERT_DOUBLE_EQ( 3.0, a6[1]);
  ASSERT_DOUBLE_EQ( 1.0, a6[2]);
  ASSERT_DOUBLE_EQ(-2.0, a6[3]);
}

TEST(MathTest, ClassificationTest)
{
  constexpr double v1 = 0.0;
  constexpr double v2 = -0.0;
  constexpr double v3 = std::numeric_limits<double>::max();
  constexpr double v4 = std::numeric_limits<double>::lowest();
  constexpr double v5 = std::numeric_limits<double>::min();
  constexpr double v6 = -std::numeric_limits<double>::min();
  constexpr double v7 = std::numeric_limits<double>::epsilon();
  constexpr double v8 = -std::numeric_limits<double>::epsilon();
  constexpr double v9 = std::numeric_limits<double>::infinity();
  constexpr double v10 = -std::numeric_limits<double>::infinity();
  constexpr double v11 = std::numeric_limits<double>::quiet_NaN();
  constexpr double v12 = std::numeric_limits<double>::signaling_NaN();
  constexpr double v13 = std::numeric_limits<double>::denorm_min();
  constexpr double v14 = -std::numeric_limits<double>::denorm_min();

  {
    constexpr bool r1 = zisc::isFinite(v1);
    ASSERT_TRUE(r1);
    constexpr bool r2 = zisc::isFinite(v2);
    ASSERT_TRUE(r2);
    constexpr bool r3 = zisc::isFinite(v3);
    ASSERT_TRUE(r3);
    constexpr bool r4 = zisc::isFinite(v4);
    ASSERT_TRUE(r4);
    constexpr bool r5 = zisc::isFinite(v5);
    ASSERT_TRUE(r5);
    constexpr bool r6 = zisc::isFinite(v6);
    ASSERT_TRUE(r6);
    constexpr bool r7 = zisc::isFinite(v7);
    ASSERT_TRUE(r7);
    constexpr bool r8 = zisc::isFinite(v8);
    ASSERT_TRUE(r8);
    constexpr bool r9 = zisc::isFinite(v9);
    ASSERT_FALSE(r9);
    constexpr bool r10 = zisc::isFinite(v10);
    ASSERT_FALSE(r10);
    const bool r11 = zisc::isFinite(v11);
    ASSERT_FALSE(r11);
    const bool r12 = zisc::isFinite(v12);
    ASSERT_FALSE(r12);
    constexpr bool r13 = zisc::isFinite(v13);
    ASSERT_TRUE(r13);
    constexpr bool r14 = zisc::isFinite(v14);
    ASSERT_TRUE(r14);
  }

  {
    constexpr bool r1 = zisc::isInf(v1);
    ASSERT_FALSE(r1);
    constexpr bool r2 = zisc::isInf(v2);
    ASSERT_FALSE(r2);
    constexpr bool r3 = zisc::isInf(v3);
    ASSERT_FALSE(r3);
    constexpr bool r4 = zisc::isInf(v4);
    ASSERT_FALSE(r4);
    constexpr bool r5 = zisc::isInf(v5);
    ASSERT_FALSE(r5);
    constexpr bool r6 = zisc::isInf(v6);
    ASSERT_FALSE(r6);
    constexpr bool r7 = zisc::isInf(v7);
    ASSERT_FALSE(r7);
    constexpr bool r8 = zisc::isInf(v8);
    ASSERT_FALSE(r8);
    constexpr bool r9 = zisc::isInf(v9);
    ASSERT_TRUE(r9);
    constexpr bool r10 = zisc::isInf(v10);
    ASSERT_TRUE(r10);
    const bool r11 = zisc::isInf(v11);
    ASSERT_FALSE(r11);
    const bool r12 = zisc::isInf(v12);
    ASSERT_FALSE(r12);
    constexpr bool r13 = zisc::isInf(v13);
    ASSERT_FALSE(r13);
    constexpr bool r14 = zisc::isInf(v14);
    ASSERT_FALSE(r14);
  }

  {
    constexpr bool r1 = zisc::isNan(v1);
    ASSERT_FALSE(r1);
    constexpr bool r2 = zisc::isNan(v2);
    ASSERT_FALSE(r2);
    constexpr bool r3 = zisc::isNan(v3);
    ASSERT_FALSE(r3);
    constexpr bool r4 = zisc::isNan(v4);
    ASSERT_FALSE(r4);
    constexpr bool r5 = zisc::isNan(v5);
    ASSERT_FALSE(r5);
    constexpr bool r6 = zisc::isNan(v6);
    ASSERT_FALSE(r6);
    constexpr bool r7 = zisc::isNan(v7);
    ASSERT_FALSE(r7);
    constexpr bool r8 = zisc::isNan(v8);
    ASSERT_FALSE(r8);
    constexpr bool r9 = zisc::isNan(v9);
    ASSERT_FALSE(r9);
    constexpr bool r10 = zisc::isNan(v10);
    ASSERT_FALSE(r10);
    const bool r11 = zisc::isNan(v11);
    ASSERT_TRUE(r11);
    const bool r12 = zisc::isNan(v12);
    ASSERT_TRUE(r12);
    constexpr bool r13 = zisc::isNan(v13);
    ASSERT_FALSE(r13);
    constexpr bool r14 = zisc::isNan(v14);
    ASSERT_FALSE(r14);
  }

  {
    constexpr bool r1 = zisc::isNormal(v1);
    ASSERT_FALSE(r1);
    constexpr bool r2 = zisc::isNormal(v2);
    ASSERT_FALSE(r2);
    constexpr bool r3 = zisc::isNormal(v3);
    ASSERT_TRUE(r3);
    constexpr bool r4 = zisc::isNormal(v4);
    ASSERT_TRUE(r4);
    constexpr bool r5 = zisc::isNormal(v5);
    ASSERT_TRUE(r5);
    constexpr bool r6 = zisc::isNormal(v6);
    ASSERT_TRUE(r6);
    constexpr bool r7 = zisc::isNormal(v7);
    ASSERT_TRUE(r7);
    constexpr bool r8 = zisc::isNormal(v8);
    ASSERT_TRUE(r8);
    constexpr bool r9 = zisc::isNormal(v9);
    ASSERT_FALSE(r9);
    constexpr bool r10 = zisc::isNormal(v10);
    ASSERT_FALSE(r10);
    const bool r11 = zisc::isNormal(v11);
    ASSERT_FALSE(r11);
    const bool r12 = zisc::isNormal(v12);
    ASSERT_FALSE(r12);
    constexpr bool r13 = zisc::isNormal(v13);
    ASSERT_FALSE(r13);
    constexpr bool r14 = zisc::isNormal(v14);
    ASSERT_FALSE(r14);
  }

  {
    constexpr bool r1 = zisc::isSubnormal(v1);
    ASSERT_FALSE(r1);
    constexpr bool r2 = zisc::isSubnormal(v2);
    ASSERT_FALSE(r2);
    constexpr bool r3 = zisc::isSubnormal(v3);
    ASSERT_FALSE(r3);
    constexpr bool r4 = zisc::isSubnormal(v4);
    ASSERT_FALSE(r4);
    constexpr bool r5 = zisc::isSubnormal(v5);
    ASSERT_FALSE(r5);
    constexpr bool r6 = zisc::isSubnormal(v6);
    ASSERT_FALSE(r6);
    constexpr bool r7 = zisc::isSubnormal(v7);
    ASSERT_FALSE(r7);
    constexpr bool r8 = zisc::isSubnormal(v8);
    ASSERT_FALSE(r8);
    constexpr bool r9 = zisc::isSubnormal(v9);
    ASSERT_FALSE(r9);
    constexpr bool r10 = zisc::isSubnormal(v10);
    ASSERT_FALSE(r10);
    const bool r11 = zisc::isSubnormal(v11);
    ASSERT_FALSE(r11);
    const bool r12 = zisc::isSubnormal(v12);
    ASSERT_FALSE(r12);
    constexpr bool r13 = zisc::isSubnormal(v13);
    ASSERT_TRUE(r13);
    constexpr bool r14 = zisc::isSubnormal(v14);
    ASSERT_TRUE(r14);
  }
}

TEST(MathTest, FmaTest)
{
  volatile const float a = 3.0f;
  volatile const float b = 5.0f;
  volatile const float c = 4.0f;
  std::cout << std::scientific
            << std::setprecision(std::numeric_limits<float>::max_digits10)
            << std::fma(a, b, c) << std::endl;
}
