/*!
  \file math_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <tuple>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/math.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

template <typename MeasureFunc, typename StlFunc, typename ZiscFunc> inline
void testMathTime(const zisc::int64 max_n,
                  MeasureFunc measure_time_func,
                  StlFunc stl_func,
                  ZiscFunc zisc_func)
{
  using zisc::cast;
  using zisc::int64;
  using std::chrono::duration_cast;

  constexpr int64 loop = 10;
  std::cout << "Loop count: " << loop << std::endl;

  std::chrono::nanoseconds stl_measure_time{0},
                           zisc_measure_time{0};
  for (int64 n = 0; n < loop; ++n) {
    {
      auto stl_time = measure_time_func(max_n, stl_func);
      std::cout << " STL time[" << (n + 1) << "]: " 
          << duration_cast<std::chrono::milliseconds>(stl_time).count() << "ms "
          << "(" << stl_time.count() << " us)." << std::endl;
      stl_measure_time += stl_time;
    }
    {
      auto zisc_time = measure_time_func(max_n, zisc_func);
      std::cout << "ZISC time[" << (n + 1) << "]: " 
          << duration_cast<std::chrono::milliseconds>(zisc_time).count() << "ms "
          << "(" << zisc_time.count() << " us)." << std::endl;
      zisc_measure_time += zisc_time;
    }
  }
  ASSERT_LT(zisc_measure_time.count(), stl_measure_time.count());
}

template <typename StlFunc, typename ZiscFunc>
void testMathAccuracy(const char* func_name,
                      const zisc::int64 start,
                      const zisc::int64 end,
                      const zisc::int64 interval,
                      const double k,
                      StlFunc stl_func,
                      ZiscFunc zisc_func,
                      const double* error = nullptr)
{
  using zisc::cast;
  using zisc::int64;
  const int64 threshold = (end - start) / (interval * 10);
  int64 count = threshold;
  int64 percentage = 0;
  for (int64 i = start; i < end; i = i + interval) {
    const volatile double value = k * (cast<double>(i) / cast<double>(end));
    const auto expected = stl_func(value);
    const auto actual = zisc_func(value);
    if (error == nullptr) {
      EXPECT_DOUBLE_EQ(expected, actual) 
          << func_name << "(" << value << ") is failed.";
    }
    else {
      EXPECT_NEAR(expected, actual, *error) 
          << func_name << "(" << value << ") is failed.";
    }
    --count;
    if (count == 0) {
      percentage += 10;
      std::cout << "[" << percentage << "%] " << std::flush;
      count = threshold;
    }
  }
  std::cout << std::endl;
}

} // namespace

TEST(MathTest, piTest)
{
  // float 
  {
    const float reference = 4.0f * std::atan(1.0f);
    constexpr float pi = zisc::kPi<float>;
    ASSERT_FLOAT_EQ(reference, pi);
    ASSERT_FLOAT_EQ(std::sin(0.25f * pi), std::cos(0.25f * pi));
  }
  // double
  {
    const double reference = 4.0 * std::atan(1.0);
    constexpr double pi = zisc::kPi<double>;
    ASSERT_DOUBLE_EQ(reference, pi);
    ASSERT_DOUBLE_EQ(std::sin(0.25 * pi), std::cos(0.25 * pi));
  }
}

TEST(MathTest, absTest)
{
  ASSERT_EQ(1, zisc::abs(1));
  ASSERT_EQ(1, zisc::abs(-1));
}

TEST(MathTest, gcdTest)
{
  auto test_gcd = [](const int a, const int b, const int gcd)
  {
    ASSERT_EQ(gcd, zisc::gcd(a, b));
    ASSERT_EQ(gcd, zisc::gcd(b, a));
  };
  test_gcd(1, 0, 1);
  test_gcd(40, 4, 4);
  test_gcd(53, 21, 1);
}

TEST(MathTest, powerTest)
{
  // Power function test
  ASSERT_EQ(1, zisc::power<0>(2)) << "Power test failed.";
  ASSERT_EQ(2, zisc::power<1>(2)) << "Power test failed.";
  ASSERT_EQ(4, zisc::power<2>(2)) << "Power test failed.";
  ASSERT_EQ(8, zisc::power<3>(2)) << "Power test failed.";
  ASSERT_EQ(16, zisc::power<4>(2)) << "Power test failed.";
  ASSERT_EQ(32, zisc::power<5>(2)) << "Power test failed.";

  //
  ASSERT_DOUBLE_EQ(0.25, zisc::power(2.0, -2));
  ASSERT_DOUBLE_EQ(0.5, zisc::power(2.0, -1));
  ASSERT_DOUBLE_EQ(1.0, zisc::power(2.0, 0));
  ASSERT_DOUBLE_EQ(2.0, zisc::power(2.0, 1));
  ASSERT_DOUBLE_EQ(4.0, zisc::power(2.0, 2));
  ASSERT_DOUBLE_EQ(8.0, zisc::power(2.0, 3));
}

TEST(MathTest, powTest)
{
  ASSERT_DOUBLE_EQ(0.25, zisc::pow(2.0, -2.0));
  ASSERT_DOUBLE_EQ(0.5, zisc::pow(2.0, -1.0));
  ASSERT_DOUBLE_EQ(1.0, zisc::pow(2.0, 0.0));
  ASSERT_DOUBLE_EQ(2.0, zisc::pow(2.0, 1.0));
  ASSERT_DOUBLE_EQ(4.0, zisc::pow(2.0, 2.0));
  ASSERT_DOUBLE_EQ(8.0, zisc::pow(2.0, 3.0));
}

TEST(MathTest, squareRootTest)
{
  constexpr double root2 = zisc::squareRoot(2.0);
  ASSERT_DOUBLE_EQ(std::sqrt(2.0), root2);
  ASSERT_DOUBLE_EQ(std::sqrt(3.0), zisc::squareRoot(3.0));
  ASSERT_DOUBLE_EQ(std::sqrt(1000.0), zisc::squareRoot(1000.0));
  constexpr double pi = zisc::kPi<double>;
  ASSERT_DOUBLE_EQ(std::sqrt(pi), zisc::squareRoot(pi));
}

TEST(MathTest, cbrtTest)
{
  ASSERT_DOUBLE_EQ(0.0, zisc::cbrt(0.0));
  ASSERT_DOUBLE_EQ(1.0, zisc::cbrt(1.0));
  ASSERT_DOUBLE_EQ(1.2599210498948732, zisc::cbrt(2.0));
  ASSERT_DOUBLE_EQ(1.4422495703074083, zisc::cbrt(3.0));
  ASSERT_DOUBLE_EQ(1.5874010519681994, zisc::cbrt(4.0));
  ASSERT_DOUBLE_EQ(2.0, zisc::cbrt(8.0));
}

TEST(MathTest, expTest)
{
  ASSERT_DOUBLE_EQ(1.0, zisc::exp(0.0));
  ASSERT_DOUBLE_EQ(zisc::kE<double>, zisc::exp(1.0));
  ASSERT_DOUBLE_EQ(zisc::power<2>(zisc::kE<double>), zisc::exp(2.0));
  ASSERT_DOUBLE_EQ(zisc::power<3>(zisc::kE<double>), zisc::exp(3.0));
  ASSERT_DOUBLE_EQ(zisc::power<4>(zisc::kE<double>), zisc::exp(4.0));
  ASSERT_DOUBLE_EQ(zisc::power<5>(zisc::kE<double>), zisc::exp(5.0));
}

TEST(MathTest, SolveCubicOneTest)
{
  constexpr double error = 0.0000001;
  // Example 1
  const auto r1 = zisc::solveCubicOne(6.0, -5.0, -17.0, 6.0);
  EXPECT_NEAR(2.0, r1, error);
  // Example 2
  const auto r2 = zisc::solveCubicOne(1.0, -2.0, -6.0, 4.0);
  EXPECT_NEAR(3.414213562373095, r2, error);
  // Example 3
  const auto r3 = zisc::solveCubicOne(1.0, 1.0, 1.0, -3.0);
  EXPECT_NEAR(1.0, r3, error);
  // Example 4
  const auto r4 = zisc::solveCubicOne(4.0, 3.0, 2.0, 1.0);
  EXPECT_NEAR(-0.605829586188268, r4, error);
  // Example 5
  const auto r5 = zisc::solveCubicOne(1.0, -6.0, 11.0, -6.0);
  EXPECT_NEAR(3.0, r5, error);
  // Example 6
  const auto r6 = zisc::solveCubicOne(1.0, -5.0, 8.0, -4.0);
  EXPECT_NEAR(1.0, r6, error);
  // Example 7
  const auto r7 = zisc::solveCubicOne(1.0, -3.0, 3.0, -1.0);
  EXPECT_NEAR(1.0, r7, error);
}

TEST(MathTest, SolveQuarticTest)
{
  // Example 1
  const auto r1 = zisc::solveQuartic(3.0, 6.0, -123.0, -126.0, 1080.0);
  const auto& a1 = std::get<0>(r1);
  const int n1 = std::get<1>(r1);
  EXPECT_EQ(4, n1);
  EXPECT_DOUBLE_EQ(5.0, a1[0]);
  EXPECT_DOUBLE_EQ(3.0, a1[1]);
  EXPECT_DOUBLE_EQ(-4.0, a1[2]);
  EXPECT_DOUBLE_EQ(-6.0, a1[3]);
  // Example 2
  const auto r2 = zisc::solveQuartic(-20.0, 5.0, 17.0, -29.0, 87.0);
  const auto& a2 = std::get<0>(r2);
  const int n2 = std::get<1>(r2);
  EXPECT_EQ(2, n2);
  EXPECT_FLOAT_EQ(1.48758311033f, static_cast<float>(a2[0]));
  EXPECT_FLOAT_EQ(-1.68200392658f, static_cast<float>(a2[1]));
  // Example 3
  const auto r3 = zisc::solveQuartic(2.0, 2.0, -3.0, -3.0, -4.0);
  const auto& a3 = std::get<0>(r3);
  const int n3 = std::get<1>(r3);
  EXPECT_EQ(2, n3);
  EXPECT_FLOAT_EQ(1.43875f, static_cast<float>(a3[0]));
  EXPECT_FLOAT_EQ(-1.7439762f, static_cast<float>(a3[1]));
  // Example 4 
  const auto r4 = zisc::solveQuartic(16.0, 96.0, -24.0, -664.0, -315.0);
  const auto& a4 = std::get<0>(r4);
  const int n4 = std::get<1>(r4);
  EXPECT_EQ(4, n4);
  EXPECT_DOUBLE_EQ( 5.0 / 2.0, a4[0]);
  EXPECT_DOUBLE_EQ(-1.0 / 2.0, a4[1]);
  EXPECT_DOUBLE_EQ(-7.0 / 2.0, a4[2]);
  EXPECT_DOUBLE_EQ(-9.0 / 2.0, a4[3]);
  // Equation 5
  const auto r5 = zisc::solveQuartic(1.0, -22.0, 152.0, -362.0, 231.0);
  const auto& a5 = std::get<0>(r5);
  const int n5 = std::get<1>(r5);
  ASSERT_EQ(4, n5);
  ASSERT_DOUBLE_EQ(11.0, a5[0]);
  ASSERT_DOUBLE_EQ( 7.0, a5[1]);
  ASSERT_DOUBLE_EQ( 3.0, a5[2]);
  ASSERT_DOUBLE_EQ( 1.0, a5[3]);
  // Equation 3
  const auto r6 = zisc::solveQuartic(1.0, -7.0, 5.0, 31.0, -30.0);
  const auto& a6 = std::get<0>(r6);
  const int n6 = std::get<1>(r6);
  ASSERT_EQ(4, n6);
  ASSERT_DOUBLE_EQ( 5.0, a6[0]);
  ASSERT_DOUBLE_EQ( 3.0, a6[1]);
  ASSERT_DOUBLE_EQ( 1.0, a6[2]);
  ASSERT_DOUBLE_EQ(-2.0, a6[3]);
}

#if defined(ZISC_MATH_EFFICIENT_POWER)

TEST(MathTest, sqrtTaylorCoefficientTest)
{
  {
    constexpr auto k = zisc::inner::TaylorCoefficient<1, double>::sqrt();
    ASSERT_DOUBLE_EQ(1.0 / 2.0, k);
  }
  {
    constexpr auto k = zisc::inner::TaylorCoefficient<2, double>::sqrt();
    ASSERT_DOUBLE_EQ(-1.0 / 8.0, k);
  }
  {
    constexpr auto k = zisc::inner::TaylorCoefficient<3, double>::sqrt();
    ASSERT_DOUBLE_EQ(1.0 / 16.0, k);
  }
  {
    constexpr auto k = zisc::inner::TaylorCoefficient<4, double>::sqrt();
    ASSERT_DOUBLE_EQ(-5.0 / 128.0, k);
  }
}

TEST(MathTest, sqrtLongRangeComputationTimeTest)
{
  using zisc::cast;
  using zisc::int64;
  using Clock = std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  constexpr int64 max_n = 100'000'000'000;
  constexpr int64 interval = 10;

  auto stl_func = [](const double value) {return std::sqrt(value);};
  auto zisc_func = [](const double value) {return zisc::sqrt(value);};
  auto measure_time_func = [](const int64 max_num, auto func)
  {
    auto start = Clock::now();
    for (int64 v = 0; v < max_num; v = v + interval) {
      const volatile double value = cast<double>(v);
      func(value);
    }
    auto end = Clock::now();
    return duration_cast<std::chrono::nanoseconds>(end - start);
  };
  ::testMathTime(max_n, measure_time_func, stl_func, zisc_func);
}

TEST(MathTest, sqrtSmallRangeComputationTimeTest)
{
  using zisc::cast;
  using zisc::int64;
  using Clock = std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  constexpr int64 max_n = 1'000'000'000;

  auto stl_func = [](const double value){return std::sqrt(value);};
  auto zisc_func = [](const double value){return zisc::sqrt(value);};
  auto measure_time_func = [](const int64 max_num, auto func)
  {
    auto start = Clock::now();
    for (int64 i = 1; i < max_num; ++i) {
      const volatile double value = cast<double>(i) / cast<double>(max_num);
      func(value);
    }
    auto end = Clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };
  ::testMathTime(max_n, measure_time_func, stl_func, zisc_func);
}

TEST(MathTest, sqrtLongRangeAccuracyTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = 1; 
  constexpr int64 end = 1000'000'000'000;
  constexpr int64 interval = 1000;
  constexpr double k = cast<double>(end);
  auto stl_func = [](const double value){return std::sqrt(value);};
  auto zisc_func = [](const double value){return zisc::sqrt(value);};
  ::testMathAccuracy("sqrt", start, end, interval, k, stl_func, zisc_func);
}

TEST(MathTest, sqrtLongRangeConvergenceTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = 1; 
  constexpr int64 end = 1000'000'000'000;
  constexpr int64 interval = 1000;
  constexpr double k = cast<double>(end);
  auto zisc1_func = [](const double value){return zisc::inner::sqrt<4>(value);};
  auto zisc2_func = [](const double value){return zisc::inner::sqrt<3>(value);};
  ::testMathAccuracy("sqrt", start, end, interval, k, zisc1_func, zisc2_func);
}

TEST(MathTest, sqrtSmallRangeAccuracyTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = 1; 
  constexpr int64 end = 1'000'000'000;
  constexpr int64 interval = 10;
  constexpr double k = 1.0;
  auto stl_func = [](const double value){return std::sqrt(value);};
  auto zisc_func = [](const double value){return zisc::sqrt(value);};
  ::testMathAccuracy("sqrt", start, end, interval, k, stl_func, zisc_func);
}

TEST(MathTest, sqrtSmallRangeConvergenceTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = 1; 
  constexpr int64 end = 1'000'000'000;
  constexpr int64 interval = 10;
  constexpr double k = 1.0;
  auto zisc1_func = [](const double value){return zisc::inner::sqrt<4>(value);};
  auto zisc2_func = [](const double value){return zisc::inner::sqrt<3>(value);};
  ::testMathAccuracy("sqrt", start, end, interval, k, zisc1_func, zisc2_func);
}

#endif // ZISC_MATH_EFFICIENT_POWER

#if defined(ZISC_MATH_EFFICIENT_TRIGONOMETRIC)

TEST(MathTest, sinTaylorCoefficientTest)
{
  using zisc::cast;
  {
    constexpr double v = zisc::inner::TaylorCoefficient<1, double>::sin();
    constexpr double reference = -1.0 / cast<double>(zisc::factorial(3));
    ASSERT_DOUBLE_EQ(reference, v);
  }
  {
    constexpr double v = zisc::inner::TaylorCoefficient<2, double>::sin();
    constexpr double reference = 1.0 / cast<double>(zisc::factorial(5));
    ASSERT_DOUBLE_EQ(reference, v);
  }
  {
    constexpr double v = zisc::inner::TaylorCoefficient<3, double>::sin();
    constexpr double reference = -1.0 / cast<double>(zisc::factorial(7));
    ASSERT_DOUBLE_EQ(reference, v);
  }
  {
    constexpr double v = zisc::inner::TaylorCoefficient<4, double>::sin();
    constexpr double reference = 1.0 / cast<double>(zisc::factorial(9));
    ASSERT_DOUBLE_EQ(reference, v);
  }
}

TEST(MathTest, sinComputationTimeTest)
{
  using zisc::cast;
  using zisc::int64;
  using Clock = std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  constexpr int64 max_n = 1'000'000'000;

  auto stl_func = [](const double value) {return std::sin(value);};
  auto zisc_func = [](const double value) {return zisc::sin(value);};
  auto measure_time_func = [](const int64 max_num, auto func)
  {
    auto start = Clock::now();
    for (int64 i = -max_num; i < max_num; ++i) {
      constexpr double k = 0.5 * zisc::kPi<double>;
      const volatile double theta = k * (cast<double>(i) / cast<double>(max_num));
      func(theta);
    }
    auto end = Clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };
  ::testMathTime(max_n, measure_time_func, stl_func, zisc_func);
}

TEST(MathTest, sinAccuracyTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = -1000'000'000; 
  constexpr int64 end = -start;
  constexpr int64 interval = 1;
  constexpr double k = zisc::kPi<double>;
  constexpr double error = 0.000'000'000'000'001;
  auto stl_func = [](const double theta){return std::sin(theta);};
  auto zisc_func = [](const double theta){return zisc::sin(theta);};
  ::testMathAccuracy("sin", start, end, interval, k, stl_func, zisc_func, &error);
}

TEST(MathTest, cosAccuracyTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = -1000'000'000; 
  constexpr int64 end = -start;
  constexpr int64 interval = 1;
  constexpr double k = zisc::kPi<double>;
  constexpr double error = 0.000'000'000'000'001;
  auto stl_func = [](const double theta){return std::cos(theta);};
  auto zisc_func = [](const double theta){return zisc::cos(theta);};
  ::testMathAccuracy("cos", start, end, interval, k, stl_func, zisc_func, &error);
}

TEST(MathTest, tanComputationTimeTest)
{
  using zisc::cast;
  using zisc::int64;
  using Clock = std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  constexpr int64 max_n = 1'000'000'000;

  auto stl_func = [](const double value) {return std::tan(value);};
  auto zisc_func = [](const double value) {return zisc::tan(value);};
  auto measure_time_func = [](const int64 max_num, auto func)
  {
    auto start = Clock::now();
    for (int64 i = 1; i < max_num; ++i) {
      constexpr double k = 0.5 * zisc::kPi<double>;
      const volatile double theta = k * (cast<double>(i) / cast<double>(max_num));
      func(theta);
    }
    auto end = Clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };
  ::testMathTime(max_n, measure_time_func, stl_func, zisc_func);
}

TEST(MathTest, tanAccuracyTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = 0;
  constexpr int64 end = 1000'000'000; 
  constexpr int64 interval = 1;
//  constexpr double k = 0.5 * zisc::kPi<double>;
  constexpr double k = 0.45 * zisc::kPi<double>;
  constexpr double error = 0.000'000'000'000'01;
  auto stl_func = [](const double theta)
  {
    return (theta != 0.0) ? std::tan(theta) : 0.0;
  };
  auto zisc_func = [](const double theta)
  {
    return (theta != 0.0) ? zisc::tan(theta) : 0.0;
  };
  ::testMathAccuracy("tan", start, end, interval, k, stl_func, zisc_func, &error);
}

TEST(MathTest, arcsinComputationTimeTest)
{
  using zisc::cast;
  using zisc::int64;
  using Clock = std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  constexpr int64 max_n = 1'000'000'000;

  auto stl_func = [](const double value) {return std::asin(value);};
  auto zisc_func = [](const double value) {return zisc::asin(value);};
  auto measure_time_func = [](const int64 max_num, auto func)
  {
    auto start = Clock::now();
    for (int64 i = -max_num; i < max_num; ++i) {
      const volatile double x = cast<double>(i) / cast<double>(max_num);
      func(x);
    }
    auto end = Clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };
  ::testMathTime(max_n, measure_time_func, stl_func, zisc_func);
}

TEST(MathTest, arcsinAccuracyTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = -100'000'000; 
  constexpr int64 end = -start;
  constexpr int64 interval = 1;
  constexpr double k = 1.0;
  constexpr double error = 0.000'000'000'000'1;
  auto stl_func = [](const double value) {return std::asin(value);};
  auto zisc_func = [](const double value) {return zisc::asin(value);};
  ::testMathAccuracy("arcsin", start, end, interval, k, stl_func, zisc_func, &error);
}

TEST(MathTest, arccosComputationTimeTest)
{
  using zisc::cast;
  using zisc::int64;
  using Clock = std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  constexpr int64 max_n = 1'000'000'000;

  auto stl_func = [](const double value) {return std::acos(value);};
  auto zisc_func = [](const double value) {return zisc::acos(value);};
  auto measure_time_func = [](const int64 max_num, auto func)
  {
    auto start = Clock::now();
    for (int64 i = -max_num; i < max_num; ++i) {
      const volatile double x = cast<double>(i) / cast<double>(max_num);
      func(x);
    }
    auto end = Clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };
  ::testMathTime(max_n, measure_time_func, stl_func, zisc_func);
}

TEST(MathTest, arccosAccuracyTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = -100'000'000; 
  constexpr int64 end = -start;
  constexpr int64 interval = 1;
  constexpr double k = 1.0;
  constexpr double error = 0.000'000'000'000'1;
  auto stl_func = [](const double value) {return std::acos(value);};
  auto zisc_func = [](const double value) {return zisc::acos(value);};
  ::testMathAccuracy("arccos", start, end, interval, k, stl_func, zisc_func, &error);
}

TEST(MathTest, arctanTaylorCoefficientTest)
{
  using zisc::cast;
  {
    constexpr double v = zisc::inner::TaylorCoefficient<1, double>::arctan();
    constexpr double reference = -1.0 / 3.0;
    ASSERT_DOUBLE_EQ(reference, v);
  }
  {
    constexpr double v = zisc::inner::TaylorCoefficient<2, double>::arctan();
    constexpr double reference = 1.0 / 5.0;
    ASSERT_DOUBLE_EQ(reference, v);
  }
  {
    constexpr double v = zisc::inner::TaylorCoefficient<3, double>::arctan();
    constexpr double reference = -1.0 / 7.0;
    ASSERT_DOUBLE_EQ(reference, v);
  }
  {
    constexpr double v = zisc::inner::TaylorCoefficient<4, double>::arctan();
    constexpr double reference = 1.0 / 9.0;
    ASSERT_DOUBLE_EQ(reference, v);
  }
}

TEST(MathTest, arctanComputationTimeTest)
{
  using zisc::cast;
  using zisc::int64;
  using Clock = std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  constexpr int64 max_n = 1'000'000'000;

  auto stl_func = [](const double value) {return std::atan(value);};
  auto zisc_func = [](const double value) {return zisc::atan(value);};
  auto measure_time_func = [](const int64 max_num, auto func)
  {
    auto start = Clock::now();
    for (int64 i = -max_num; i < max_num; ++i) {
      if (i == 0)
        continue;
      constexpr double k = 0.5 * zisc::kPi<double>;
      const volatile double x = k * (cast<double>(i) / cast<double>(max_num));
      func(x);
    }
    auto end = Clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  };
  ::testMathTime(max_n, measure_time_func, stl_func, zisc_func);
}

TEST(MathTest, arctanAccuracyTest)
{
  using zisc::cast;
  using zisc::int64;
  constexpr int64 start = -100'000'000; 
  constexpr int64 end = -start;
  constexpr int64 interval = 1;
  constexpr double k = 0.5 * zisc::kPi<double>;
  constexpr double error = 0.000'000'000'000'1;
  auto stl_func = [](const double value)
  {
    return (value != 0.0) ? std::atan(value) : 0.0;
  };
  auto zisc_func = [](const double value)
  {
    return (value != 0.0) ? zisc::atan(value) : 0.0;
  };
  ::testMathAccuracy("arctan", start, end, interval, k, stl_func, zisc_func, &error);
}

#endif // ZISC_MATH_EFFICIENT_TRIGONOMETRIC
