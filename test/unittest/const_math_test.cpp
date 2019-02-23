/*!
  \file const_math_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include <tuple>
#include <utility>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/const_math.hpp"
#include "zisc/floating_point.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
// Test
#include "test.hpp"

namespace {

template <typename Float>
void printFloat(const char* message, const Float x)
{
  using FloatData = zisc::FloatingPointFromBytes<sizeof(Float)>;
  std::cout << std::setfill(' ') << std::setw(24) << message << ": "
            << std::scientific
            << std::setprecision(std::numeric_limits<Float>::max_digits10)
            << x;
  if (sizeof(FloatData) == 4)
    std::cout << "f";
  std::cout << std::endl;
  const auto data = FloatData::fromFloat(x);
  std::cout << "                     hex: " << std::hex << data.bits() << std::endl;
}

} // namespace 

TEST(ConstMathTest, PrintConstantValues)
{
  using zisc::cast;
  std::cout << "## float" << std::endl;
  ::printFloat("float max", std::numeric_limits<float>::max());
  ::printFloat("float min", std::numeric_limits<float>::min());
  ::printFloat("float denormmax", std::numeric_limits<float>::min() - std::numeric_limits<float>::denorm_min());
  ::printFloat("float denormmin", std::numeric_limits<float>::denorm_min());
  ::printFloat("float epsilon", std::numeric_limits<float>::epsilon());
  ::printFloat("float e", std::exp(1.0f));
  ::printFloat("float zisc e", zisc::constant::exp(1.0f));
  ::printFloat("float ld2f e", cast<float>(std::exp(1.0l)));
  ::printFloat("float inv e", 1.0f / std::exp(1.0f));
  ::printFloat("float zisc inv e", 1.0f / zisc::constant::exp(1.0f));
  ::printFloat("float ld2f inv e", cast<float>(1.0l / std::exp(1.0l)));
  ::printFloat("float log2E", std::log2(std::exp(1.0f)));
  ::printFloat("float zisc log2E", zisc::constant::log2(zisc::constant::exp(1.0f)));
  ::printFloat("float ld2f log2E", cast<float>(std::log2(std::exp(1.0l))));
  ::printFloat("float inv log2E", 1.0f / std::log2(std::exp(1.0f)));
  ::printFloat("float zisc inv log2E", 1.0f / zisc::constant::log2(zisc::constant::exp(1.0f)));
  ::printFloat("float ld2f inv log2E", cast<float>(1.0l / std::log2(std::exp(1.0l))));
  ::printFloat("float log10E", std::log10(std::exp(1.0f)));
  ::printFloat("float zisc log10E", zisc::constant::log10(zisc::constant::exp(1.0f)));
  ::printFloat("float ld2f log10E", cast<float>(std::log10(std::exp(1.0l))));
  ::printFloat("float inv log10E", 1.0f / std::log10(std::exp(1.0f)));
  ::printFloat("float zisc inv log10E", 1.0f / zisc::constant::log10(zisc::constant::exp(1.0f)));
  ::printFloat("float ld2f inv log10E", cast<float>(1.0l / std::log10(std::exp(1.0l))));
  ::printFloat("float logE2", std::log(2.0f));
  ::printFloat("float zisc logE2", zisc::constant::log(2.0f));
  ::printFloat("float ld2f logE2", cast<float>(std::log(2.0l)));
  ::printFloat("float inv logE2", 1.0f / std::log(2.0f));
  ::printFloat("float zisc inv logE2", 1.0f / zisc::constant::log(2.0f));
  ::printFloat("float ld2f inv logE2", cast<float>(1.0l / std::log(2.0l)));
  ::printFloat("float logE10", std::log(10.0f));
  ::printFloat("float zisc logE10", zisc::constant::log(10.0f));
  ::printFloat("float ld2f logE10", cast<float>(std::log(10.0l)));
  ::printFloat("float inv logE10", 1.0f / std::log(10.0f));
  ::printFloat("float zisc inv logE10", 1.0f / zisc::constant::log(10.0f));
  ::printFloat("float ld2f inv logE10", cast<float>(1.0l / std::log(10.0l)));
  ::printFloat("float pi", 4.0f * std::atan(1.0f));
  ::printFloat("float zisc pi", zisc::constant::pi<float>());
  ::printFloat("float ld2f pi", cast<float>(4.0l * std::atan(1.0l)));
  ::printFloat("float pi/2", 2.0f * std::atan(1.0f));
  ::printFloat("float zisc pi/2", 0.5f * zisc::constant::pi<float>());
  ::printFloat("float ld2f pi/2", cast<float>(2.0l * std::atan(1.0l)));
  ::printFloat("float pi/4", std::atan(1.0f));
  ::printFloat("float zisc pi/4", 0.25f * zisc::constant::pi<float>());
  ::printFloat("float ld2f pi/4", cast<float>(std::atan(1.0l)));
  ::printFloat("float 1/pi", 1.0f / (4.0f * std::atan(1.0f)));
  ::printFloat("float zisc 1/pi", 1.0f / zisc::constant::pi<float>());
  ::printFloat("float ld2f 1/pi", cast<float>(1.0l / (4.0l * std::atan(1.0l))));
  ::printFloat("float 2/pi", 1.0f / (2.0f * std::atan(1.0f)));
  ::printFloat("float zisc 2/pi", 2.0f / zisc::constant::pi<float>());
  ::printFloat("float ld2f 2/pi", cast<float>(1.0l / (2.0l * std::atan(1.0l))));
  ::printFloat("float 2/sqrt(pi)", 2.0f / std::sqrt((4.0f * std::atan(1.0f))));
  ::printFloat("float zisc 2/sqrt(pi)", 2.0f / zisc::constant::sqrt(zisc::constant::pi<float>()));
  ::printFloat("float ld2f 2/sqrt(pi)", cast<float>(2.0l / std::sqrt((4.0l * std::atan(1.0l)))));
  ::printFloat("float sqrt(2)", std::sqrt(2.0f));
  ::printFloat("float zisc sqrt(2)", zisc::constant::sqrt(2.0f));
  ::printFloat("float ld2f sqrt(2)", cast<float>(std::sqrt(2.0l)));
  ::printFloat("float 1/sqrt(2)", 1.0f / std::sqrt(2.0f));
  ::printFloat("float zisc 1/sqrt(2)", 1.0f / zisc::constant::sqrt(2.0f));
  ::printFloat("float ld2f 1/sqrt(2)", cast<float>(1.0l / std::sqrt(2.0l)));
  ::printFloat("float sqrt(3)", std::sqrt(3.0f));
  ::printFloat("float zisc sqrt(3)", zisc::constant::sqrt(3.0f));
  ::printFloat("float ld2f sqrt(3)", cast<float>(std::sqrt(3.0l)));
  ::printFloat("float 1/sqrt(3)", 1.0f / std::sqrt(3.0f));
  ::printFloat("float zisc 1/sqrt(3)", 1.0f / zisc::constant::sqrt(3.0f));
  ::printFloat("float ld2f 1/sqrt(3)", cast<float>(1.0l / std::sqrt(3.0l)));

  std::cout << "## double" << std::endl;
  ::printFloat("double max", std::numeric_limits<double>::max());
  ::printFloat("double min", std::numeric_limits<double>::min());
  ::printFloat("double denormmax", std::numeric_limits<double>::min() - std::numeric_limits<double>::denorm_min());
  ::printFloat("double denormmin", std::numeric_limits<double>::denorm_min());
  ::printFloat("double epsilon", std::numeric_limits<double>::epsilon());
  ::printFloat("double e", std::exp(1.0));
  ::printFloat("double zisc e", zisc::constant::exp(1.0));
  ::printFloat("double ld2f e", cast<double>(std::exp(1.0l)));
  ::printFloat("double inv e", 1.0 / std::exp(1.0));
  ::printFloat("double zisc inv e", 1.0 / zisc::constant::exp(1.0));
  ::printFloat("double ld2f inv e", cast<double>(1.0l / std::exp(1.0l)));
  ::printFloat("double log2E", std::log2(std::exp(1.0)));
  ::printFloat("double zisc log2E", zisc::constant::log2(zisc::constant::exp(1.0)));
  ::printFloat("double ld2f log2E", cast<double>(std::log2(std::exp(1.0l))));
  ::printFloat("double inv log2E", 1.0 / std::log2(std::exp(1.0)));
  ::printFloat("double zisc inv log2E", 1.0 / zisc::constant::log2(zisc::constant::exp(1.0)));
  ::printFloat("double ld2f inv log2E", cast<double>(1.0l / std::log2(std::exp(1.0l))));
  ::printFloat("double log10E", std::log10(std::exp(1.0)));
  ::printFloat("double zisc log10E", zisc::constant::log10(zisc::constant::exp(1.0)));
  ::printFloat("double ld2f log10E", cast<double>(std::log10(std::exp(1.0l))));
  ::printFloat("double inv log10E", 1.0 / std::log10(std::exp(1.0)));
  ::printFloat("double zisc inv log10E", 1.0 / zisc::constant::log10(zisc::constant::exp(1.0)));
  ::printFloat("double ld2f inv log10E", cast<double>(1.0l / std::log10(std::exp(1.0l))));
  ::printFloat("double logE2", std::log(2.0));
  ::printFloat("double zisc logE2", zisc::constant::log(2.0));
  ::printFloat("double ld2f logE2", cast<double>(std::log(2.0l)));
  ::printFloat("double inv logE2", 1.0 / std::log(2.0));
  ::printFloat("double zisc inv logE2", 1.0 / zisc::constant::log(2.0));
  ::printFloat("double ld2f inv logE2", cast<double>(1.0l / std::log(2.0l)));
  ::printFloat("double logE10", std::log(10.0));
  ::printFloat("double zisc logE10", zisc::constant::log(10.0));
  ::printFloat("double ld2f logE10", cast<double>(std::log(10.0l)));
  ::printFloat("double inv logE10", 1.0 / std::log(10.0));
  ::printFloat("double zisc inv logE10", 1.0 / zisc::constant::log(10.0));
  ::printFloat("double ld2f inv logE10", cast<double>(1.0l / std::log(10.0l)));
  ::printFloat("double pi", 4.0 * std::atan(1.0));
  ::printFloat("double zisc pi", zisc::constant::pi<double>());
  ::printFloat("double ld2f pi", cast<double>(4.0l * std::atan(1.0l)));
  ::printFloat("double pi/2", 2.0 * std::atan(1.0));
  ::printFloat("double zisc pi/2", 0.5 * zisc::constant::pi<double>());
  ::printFloat("double ld2f pi/2", cast<double>(2.0l * std::atan(1.0l)));
  ::printFloat("double pi/4", std::atan(1.0));
  ::printFloat("double zisc pi/4", 0.25 * zisc::constant::pi<double>());
  ::printFloat("double ld2f pi/4", cast<double>(std::atan(1.0l)));
  ::printFloat("double 1/pi", 1.0 / (4.0 * std::atan(1.0)));
  ::printFloat("double zisc 1/pi", 1.0 / zisc::constant::pi<double>());
  ::printFloat("double ld2f 1/pi", cast<double>(1.0l / (4.0l * std::atan(1.0l))));
  ::printFloat("double 2/pi", 1.0 / (2.0 * std::atan(1.0)));
  ::printFloat("double zisc 2/pi", 2.0 / zisc::constant::pi<double>());
  ::printFloat("double ld2f 2/pi", cast<double>(1.0l / (2.0l * std::atan(1.0l))));
  ::printFloat("double 2/sqrt(pi)", 2.0 / std::sqrt((4.0 * std::atan(1.0))));
  ::printFloat("double zisc 2/sqrt(pi)", 2.0 / zisc::constant::sqrt(zisc::constant::pi<double>()));
  ::printFloat("double ld2f 2/sqrt(pi)", cast<double>(2.0l / std::sqrt((4.0l * std::atan(1.0l)))));
  ::printFloat("double sqrt(2)", std::sqrt(2.0));
  ::printFloat("double zisc sqrt(2)", zisc::constant::sqrt(2.0));
  ::printFloat("double ld2f sqrt(2)", cast<double>(std::sqrt(2.0l)));
  ::printFloat("double 1/sqrt(2)", 1.0 / std::sqrt(2.0));
  ::printFloat("double zisc 1/sqrt(2)", 1.0 / zisc::constant::sqrt(2.0));
  ::printFloat("double ld2f 1/sqrt(2)", cast<double>(1.0l / std::sqrt(2.0l)));
  ::printFloat("double sqrt(3)", std::sqrt(3.0));
  ::printFloat("double zisc sqrt(3)", zisc::constant::sqrt(3.0));
  ::printFloat("double ld2f sqrt(3)", cast<double>(std::sqrt(3.0l)));
  ::printFloat("double 1/sqrt(3)", 1.0 / std::sqrt(3.0));
  ::printFloat("double zisc 1/sqrt(3)", 1.0 / zisc::constant::sqrt(3.0));
  ::printFloat("double ld2f 1/sqrt(3)", cast<double>(1.0l / std::sqrt(3.0l)));
}

TEST(ConstMathTest, PiTest)
{
  // Float
  {
    constexpr float pi = zisc::constant::pi<float>();
    EXPECT_FLOAT_EQ(3.141592741f, pi);
  }
  // Double
  {
    constexpr double pi = zisc::constant::pi<double>();
    EXPECT_DOUBLE_EQ(3.14159265358979312, pi);
  }
  // Long double
  {
    constexpr long double pi = zisc::constant::pi<long double>();
    EXPECT_DOUBLE_EQ(3.141592653589793238513l, pi);
  }
}

TEST(ConstMathTest, AbsTest)
{
  {
    constexpr int value = zisc::abs(3);
    EXPECT_EQ(3, value) << "The result of abs(3) is wrong.";
  }
  {
    constexpr int value = zisc::abs(-1);
    EXPECT_EQ(1, value) << "The result of abs(-1) is wrong.";
  }
  {
    constexpr double value = zisc::abs(-2.5);
    EXPECT_DOUBLE_EQ(2.5, value) << "The result of abs(-2.5) is wrong.";
  }
}

TEST(ConstMathTest, GcdTest)
{
  {
    constexpr int result = zisc::constant::gcd(54, 24);
    EXPECT_EQ(6, result) << "The result of gcd(54, 24) is wrong.";
  }
  {
    constexpr int result = zisc::constant::gcd(42, 56);
    EXPECT_EQ(14, result) << "The result of gcd(42, 56) is wrong.";
  }
}

TEST(ConstMathTest, FactorialTest)
{
  {
    constexpr int result = zisc::constant::factorial(0);
    EXPECT_EQ(1, result) << "The result of factorial(0) is wrong.";
  }
  {
    constexpr int result = zisc::constant::factorial(1);
    EXPECT_EQ(1, result) << "The result of factorial(1) is wrong.";
  }
  {
    constexpr int result = zisc::constant::factorial(5);
    EXPECT_EQ(120, result) << "The result of factorial(5) is wrong.";
  }
}

TEST(ConstMathTest, ModTest)
{
  {
    constexpr int result = zisc::constant::mod<1>(10);
    EXPECT_EQ(0, result) << "The result of mod<1>(10) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<2>(10);
    EXPECT_EQ(0, result) << "The result of mod<2>(10) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<2>(9);
    EXPECT_EQ(1, result) << "The result of mod<2>(9) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<3>(10);
    EXPECT_EQ(1, result) << "The result of mod<3>(10) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<3>(9);
    EXPECT_EQ(0, result) << "The result of mod<3>(9) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<4>(12);
    EXPECT_EQ(0, result) << "The result of mod<4>(12) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<4>(15);
    EXPECT_EQ(3, result) << "The result of mod<4>(15) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<8>(5);
    EXPECT_EQ(5, result) << "The result of mod<8>(5) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<8>(8);
    EXPECT_EQ(0, result) << "The result of mod<8>(8) is wrong.";
  }
  {
    constexpr int result = zisc::constant::mod<8>(13);
    EXPECT_EQ(5, result) << "The result of mod<8>(13) is wrong.";
  }
}

namespace {

template <int end, int i>
struct PowerTest
{
  static void testPower()
  {
    {
      constexpr double result = zisc::constant::power(2.0, i);
      const double expected = std::pow(2.0, i);
      EXPECT_DOUBLE_EQ(expected, result) << "power(2, " << i << ") is wrong.";
    }
    PowerTest<end, i + 1>::testPower();
  }

  static void testPow()
  {
    {
      constexpr double result = zisc::constant::pow(2.0, zisc::cast<double>(i));
      const double expected = std::pow(2.0, i);
      EXPECT_DOUBLE_EQ(expected, result) << "pow(2, " << i << ") is wrong.";
    }
    {
      constexpr double base = zisc::constant::pi<double>() / 2.0;
      constexpr double result = zisc::constant::pow(base, zisc::cast<double>(i));
      const double expected = std::pow(base, i);
      EXPECT_DOUBLE_EQ(expected, result) << "pow(pi/2, " << i << ") is wrong.";
    }
    PowerTest<end, i + 1>::testPow();
  }
};

template <int end>
struct PowerTest<end, end>
{
  static void testPower()
  {
  }

  static void testPow()
  {
  }
};

} // namespace

TEST(ConstMathTest, PowerTest)
{
  constexpr int start = -126;
  constexpr int end = 127;
  ::PowerTest<end, start>::testPower();
}

TEST(ConstMathTest, PowTest)
{
//  constexpr int start = -126;
//  constexpr int end = 127;
//  ::PowerTest<end, start>::testPow();
  FAIL();
}

namespace {

#define expectFloatEq(expected, result, message) \
  if constexpr (std::is_same_v<float, Float>) \
  { \
    EXPECT_FLOAT_EQ(expected, result) << message; \
  } \
  else \
  { \
    EXPECT_DOUBLE_EQ(expected, result) << message; \
  }

template <typename Float, int end, int i>
struct ConstexprTest
{
  static_assert(std::is_floating_point_v<Float>, "The Float isn't floating point.");
  static constexpr Float zero = zisc::cast<Float>(0.0);
  static constexpr Float x = zisc::cast<Float>(i) / zisc::cast<Float>(end);
  static constexpr Float n = ::makeNormal(x);
//  static constexpr Float d = ::makeSubnormal(x);

  static void testFrLdexp()
  {
    auto run_frexp = [](const Float f)
    {
      int e = 0;
      const Float m = zisc::constant::frexp(f, &e);
      return std::make_pair(m, e);
    };

    // Normal
    {
      constexpr auto result = run_frexp(n);
      constexpr Float result_m = result.first;
      constexpr int result_e = result.second;
      int expected_e = 0;
      const Float expected_m = std::frexp(n, &expected_e);
      EXPECT_EQ(expected_e, result_e) << "frexp(" << n << ") is wrong.";
      EXPECT_EQ(expected_m, result_m) << "frexp(" << n << ") is wrong.";

      constexpr Float result_f = zisc::constant::ldexp(result_m, result_e);
      const Float expected_f = std::ldexp(result_m, result_e);
      expectFloatEq(expected_f, result_f, "ldexp(" << n << ") is wrong.");
    }
    // Subnormal
//    {
//      constexpr auto result = run_frexp(d);
//      constexpr Float result_m = result.first;
//      constexpr int result_e = result.second;
//      int expected_e = 0;
//      const Float expected_m = std::frexp(d, &expected_e);
//      EXPECT_EQ(expected_e, result_e) << "frexp(" << d << ") is wrong.";
//      EXPECT_EQ(expected_m, result_m) << "frexp(" << d << ") is wrong.";
//
//      constexpr Float result_f = zisc::constant::ldexp(result_m, result_e);
//      const Float expected_f = std::ldexp(result_m, result_e);
//      EXPECT_FLOAT_EQ(expected_f, result_f) << "ldexp(" << d << ") is wrong.";
//    }
    ConstexprTest<Float, end, i + 1>::testFrLdexp();
  }

  static void testSqrt()
  {
    // Normal
    {
      constexpr Float result = zisc::constant::sqrt(n);
      const Float expected = std::sqrt(n);
      expectFloatEq(expected, result, "sqrt(" << n << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testSqrt();
  }

  static void testCbrt()
  {
    // Normal
    {
      static constexpr Float l = ::makeNormal(zisc::cast<Float>(0.85) * x);
      constexpr Float result = zisc::constant::cbrt(l);
      const Float expected = std::cbrt(l);
      expectFloatEq(expected, result, "cbrt(" << l << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testCbrt();
  }

  static void testExp()
  {
    constexpr Float l = zisc::cast<Float>(0.1) * zisc::cast<Float>(i);
    {
      constexpr Float result = zisc::constant::exp(l);
      const Float expected = std::exp(l);
      expectFloatEq(expected, result, "exp(" << l << ") is wrong.");
    }
    constexpr Float s = zisc::power<4>(x); 
    {
      constexpr Float result = zisc::constant::exp(s);
      const Float expected = std::exp(s);
      expectFloatEq(expected, result, "exp(" << s << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testExp();
  }

  static void testLog2()
  {
    // Normal
    {
      constexpr Float result = zisc::constant::log2(n);
      const Float expected = std::log2(n);
      expectFloatEq(expected, result, "log2(" << n << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testLog2();
  }

  static void testLog()
  {
    // Normal
    {
      constexpr Float result = zisc::constant::log(n);
      const Float expected = std::log(n);
      expectFloatEq(expected, result, "log(" << n << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testLog();
  }

  static void testLog10()
  {
    // Normal
    {
      constexpr Float result = zisc::constant::log10(n);
      const Float expected = std::log10(n);
      expectFloatEq(expected, result, "log10(" << n << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testLog10();
  }

  static void testSin()
  {
    constexpr Float pi = zisc::constant::pi<Float>();
    {
      constexpr Float theta = zisc::cast<Float>(0.5) * pi * x;
      constexpr Float result = zisc::constant::sin(theta);
      const Float expected = std::sin(theta);
      expectFloatEq(expected, result, "sin(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(2.0) * pi * x;
      constexpr Float result = zisc::constant::sin(theta);
      const Float expected = std::sin(theta);
      expectFloatEq(expected, result, "sin(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(16.0) * pi * x;
      constexpr Float result = zisc::constant::sin(theta);
      const Float expected = std::sin(theta);
      expectFloatEq(expected, result, "sin(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testSin();
  }

  static void testCos()
  {
    constexpr Float pi = zisc::constant::pi<Float>();
    {
      constexpr Float theta = zisc::cast<Float>(0.5) * pi * x;
      constexpr Float result = zisc::constant::cos(theta);
      const Float expected = std::cos(theta);
      expectFloatEq(expected, result, "cos(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(2.0) * pi * x;
      constexpr Float result = zisc::constant::cos(theta);
      const Float expected = std::cos(theta);
      expectFloatEq(expected, result, "cos(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(16.0) * pi * x;
      constexpr Float result = zisc::constant::cos(theta);
      const Float expected = std::cos(theta);
      expectFloatEq(expected, result, "cos(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testCos();
  }

  static void testTan()
  {
    constexpr Float pi = zisc::constant::pi<Float>();
    {
      constexpr Float theta = zisc::cast<Float>(0.5) * pi * x;
      constexpr Float result = zisc::constant::tan(theta);
      const Float expected = std::tan(theta);
      expectFloatEq(expected, result, "tan(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(2.0) * pi * x;
      constexpr Float result = zisc::constant::tan(theta);
      const Float expected = std::tan(theta);
      expectFloatEq(expected, result, "tan(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(16.0) * pi * x;
      constexpr Float result = zisc::constant::tan(theta);
      const Float expected = std::tan(theta);
      expectFloatEq(expected, result, "tan(" << (theta / pi) << " x pi" << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testTan();
  }

  static void testAsin()
  {
    {
      constexpr Float result = zisc::constant::asin(x);
      const Float expected = std::asin(x);
      expectFloatEq(expected, result, "asin(" << x << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testAsin();
  }

  static void testAcos()
  {
    {
      constexpr Float result = zisc::constant::acos(x);
      const Float expected = std::acos(x);
      expectFloatEq(expected, result, "acos(" << x << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testAcos();
  }

  static void testAtan()
  {
    {
      constexpr Float result = zisc::constant::atan(x);
      const Float expected = std::atan(x);
      expectFloatEq(expected, result, "atan(" << x << ") is wrong.");
    }
    {
      constexpr Float result = zisc::constant::atan(n);
      const Float expected = std::atan(n);
      expectFloatEq(expected, result, "atan(" << n << ") is wrong.");
    }
    ConstexprTest<Float, end, i + 1>::testAtan();
  }
};

template <typename Float, int end>
struct ConstexprTest<Float, end, end>
{
  static void testFrLdexp()
  {
    auto run_frexp = [](const Float f)
    {
      int e = 0;
      const Float m = zisc::constant::frexp(f, &e);
      return std::make_pair(m, e);
    };

    // Zero
    {
      constexpr Float n = zisc::cast<Float>(0.0);

      constexpr auto result = run_frexp(n);
      constexpr Float result_m = result.first;
      constexpr int result_e = result.second;
      int expected_e = 0;
      const Float expected_m = std::frexp(n, &expected_e);
      expectFloatEq(expected_e, result_e, "frexp(" << n << ") is wrong.");
      expectFloatEq(expected_m, result_m, "frexp(" << n << ") is wrong.");

      constexpr Float result_f = zisc::constant::ldexp(result_m, result_e);
      const Float expected_f = std::ldexp(result_m, result_e);
      expectFloatEq(expected_f, result_f, "ldexp(" << n << ") is wrong.");
    }
    // Inf
    {
      constexpr Float n = std::numeric_limits<Float>::infinity();

      constexpr auto result = run_frexp(n);
      constexpr Float result_m = result.first;
      EXPECT_TRUE(std::isinf(result_m)) << "frexp(inf) is wrong.";

      constexpr Float result_f = zisc::constant::ldexp(result_m, 1);
      EXPECT_TRUE(std::isinf(result_f)) << "frexp(inf) is wrong.";
    }
    // Nan 
    {
      constexpr Float n = std::numeric_limits<Float>::quiet_NaN();

      constexpr auto result = run_frexp(n);
      constexpr Float result_m = result.first;
      EXPECT_TRUE(std::isnan(result_m)) << "frexp(NaN) is wrong.";

      constexpr Float result_f = zisc::constant::ldexp(result_m, 1);
      EXPECT_TRUE(std::isnan(result_f)) << "frexp(NaN) is wrong.";
    }
  }

  static void testSqrt()
  {
    {
      constexpr Float x = zisc::cast<Float>(1.0);
      constexpr Float result = zisc::constant::sqrt(x);
      const Float expected = std::sqrt(x);
      expectFloatEq(expected, result, "sqrt(" << x << ") is wrong.");
    }
  }

  static void testCbrt()
  {
  }

  static void testExp()
  {
    {
      constexpr Float x = zisc::cast<Float>(0.0);
      constexpr Float result = zisc::constant::exp(x);
      const Float expected = std::exp(x);
      expectFloatEq(expected, result, "exp(" << x << ") is wrong.");
    }
    {
      constexpr Float x = zisc::cast<Float>(1.0);
      constexpr Float result = zisc::constant::exp(x);
      const Float expected = std::exp(x);
      expectFloatEq(expected, result, "exp(" << x << ") is wrong.");
    }
  }

  static void testLog2()
  {
    {
      constexpr Float x = zisc::cast<Float>(1.0);
      constexpr Float result = zisc::constant::log2(x);
      const Float expected = std::log2(x);
      expectFloatEq(expected, result, "log2(" << x << ") is wrong.");
    }
    {
      constexpr Float x = std::numeric_limits<Float>::min();
      constexpr Float result = zisc::constant::log2(x);
      const Float expected = std::log2(x);
      expectFloatEq(expected, result, "log2(" << x << ") is wrong.");
    }
    {
      constexpr Float x = std::numeric_limits<Float>::max();
      constexpr Float result = zisc::constant::log2(x);
      const Float expected = std::log2(x);
      expectFloatEq(expected, result, "log2(" << x << ") is wrong.");
    }
  }

  static void testLog()
  {
  }

  static void testLog10()
  {
  }

  static void testSin()
  {
    constexpr Float pi = zisc::constant::pi<Float>();
    {
      constexpr Float theta = zisc::cast<Float>(0.0);
      constexpr Float result = zisc::constant::sin(theta);
      const Float expected = std::sin(theta);
      expectFloatEq(expected, result, "sin(" << theta << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(0.25) * pi;
      constexpr Float result = zisc::constant::sin(theta);
      const Float expected = std::sin(theta);
      expectFloatEq(expected, result, "sin(" << theta << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(0.5) * pi;
      constexpr Float result = zisc::constant::sin(theta);
      const Float expected = std::sin(theta);
      expectFloatEq(expected, result, "sin(" << theta << ") is wrong.");
    }
  }

  static void testCos()
  {
    constexpr Float pi = zisc::constant::pi<Float>();
    {
      constexpr Float theta = zisc::cast<Float>(0.0);
      constexpr Float result = zisc::constant::cos(theta);
      const Float expected = std::cos(theta);
      expectFloatEq(expected, result, "cos(" << theta << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(0.25) * pi;
      constexpr Float result = zisc::constant::cos(theta);
      const Float expected = std::cos(theta);
      expectFloatEq(expected, result, "cos(" << theta << ") is wrong.");
    }
    {
      constexpr Float theta = zisc::cast<Float>(0.5) * pi;
      constexpr Float result = zisc::constant::cos(theta);
      const Float expected = std::cos(theta);
      expectFloatEq(expected, result, "cos(" << theta << ") is wrong.");
    }
  }

  static void testTan()
  {
  }

  static void testAsin()
  {
    {
      constexpr Float x = zisc::cast<Float>(1.0);
      constexpr Float result = zisc::constant::asin(x);
      const Float expected = std::asin(x);
      expectFloatEq(expected, result, "asin(" << x << ") is wrong.");
    }
  }

  static void testAcos()
  {
    {
      constexpr Float x = zisc::cast<Float>(1.0);
      constexpr Float result = zisc::constant::acos(x);
      const Float expected = std::acos(x);
      expectFloatEq(expected, result, "acos(" << x << ") is wrong.");
    }
  }

  static void testAtan()
  {
    {
      constexpr Float x = zisc::cast<Float>(1.0);
      constexpr Float result = zisc::constant::atan(x);
      const Float expected = std::atan(x);
      expectFloatEq(expected, result, "atan(" << x << ") is wrong.");
    }
  }
};

} // namespace

TEST(ConstMathTest, SqrtTestF)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  ::ConstexprTest<float, end, start>::testSqrt();
}

TEST(ConstMathTest, SqrtTestD)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  ::ConstexprTest<double, end, start>::testSqrt();
}

TEST(ConstMathTest, CbrtTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testCbrt();
}

TEST(ConstMathTest, CbrtTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testCbrt();
}

TEST(ConstMathTest, ExpTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testExp();
}

TEST(ConstMathTest, ExpTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testExp();
}

TEST(ConstMathTest, Log2TestF)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  ::ConstexprTest<float, end, start>::testLog2();
}

TEST(ConstMathTest, Log2TestD)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  ::ConstexprTest<double, end, start>::testLog2();
}

TEST(ConstMathTest, LogTestF)
{
  constexpr int start = 0;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testLog();
}

TEST(ConstMathTest, LogTestD)
{
  constexpr int start = 0;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testLog();
}

TEST(ConstMathTest, Log10TestF)
{
  constexpr int start = 0;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testLog10();
}

TEST(ConstMathTest, Log10TestD)
{
  constexpr int start = 0;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testLog10();
}

TEST(ConstMathTest, SinTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testSin();
}

TEST(ConstMathTest, SinTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testSin();
}

TEST(ConstMathTest, CosTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testCos();
}

TEST(ConstMathTest, CosTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testCos();
}

TEST(ConstMathTest, TanTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testTan();
}

TEST(ConstMathTest, TanTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testTan();
}

TEST(ConstMathTest, AsinTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testAsin();
}

TEST(ConstMathTest, AsinTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testAsin();
}

TEST(ConstMathTest, AcosTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testAcos();
}

TEST(ConstMathTest, AcosTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testAcos();
}

TEST(ConstMathTest, AtanTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testAtan();
}

TEST(ConstMathTest, AtanTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testAtan();
}

TEST(ConstMathTest, FrLdTestF)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<float, end, start>::testFrLdexp();
}

TEST(ConstMathTest, FrLdTestD)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<double, end, start>::testFrLdexp();
}
