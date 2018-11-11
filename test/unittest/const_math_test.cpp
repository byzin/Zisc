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
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/const_math.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
// Test
#include "test.hpp"

namespace {

template <typename Float>
void printFloat(const Float x)
{
  std::cout << std::scientific
            << std::setprecision(std::numeric_limits<Float>::digits10)
            << x << std::endl;
}

} // namespace 

TEST(ConstMathTest, PiTest)
{
  // Float
  {
    constexpr float pi = zisc::constant::pi<float>();
    EXPECT_FLOAT_EQ(3.141592741f, pi);
    std::cout << "  float pi: ";
    ::printFloat(pi);
  }
  // Double
  {
    constexpr double pi = zisc::constant::pi<double>();
    EXPECT_DOUBLE_EQ(3.14159265358979312, pi);
    std::cout << " double pi: ";
    ::printFloat(pi);
  }
  // Long double
  {
    constexpr long double pi = zisc::constant::pi<long double>();
    EXPECT_DOUBLE_EQ(3.141592653589793238513l, pi);
    std::cout << "ldouble pi: ";
    ::printFloat(pi);
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
struct PowTest
{
  static void test()
  {
    {
      constexpr double result = zisc::constant::pow(2.0, i);
      const double expected = std::pow(2.0, i);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of pow(2, " << i << ") is wrong.";
    }
    PowTest<end, i + 1>::test();
  }
};

template <int end>
struct PowTest<end, end>
{
  static void test()
  {
  }
};

} // namespace

TEST(ConstMathTest, PowTest)
{
  constexpr int start = -126;
  constexpr int end = 127;
  ::PowTest<end, start>::test();
}

namespace {

template <int end, int i>
struct ConstexprTest
{
  static constexpr double zero = 0.0;
  static constexpr double x = zisc::cast<double>(i) / zisc::cast<double>(end);

  static void testSqrt()
  {
    constexpr double n = ::makeNormal(x);
    {
      constexpr double result = zisc::constant::sqrt(n);
      const double expected = std::sqrt(n);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of sqrt(" << n << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testSqrt();
  }

  static void testCbrt()
  {
    constexpr double n = makeNormal(0.85 * x);
    {
      constexpr double result = zisc::constant::cbrt(n);
      const double expected = std::cbrt(n);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of cbrt(" << n << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testCbrt();
  }

  static void testExp()
  {
//    constexpr double n = static_cast<double>(i);
//    {
//      constexpr double result = zisc::constant::exp(n);
//      const double expected = std::exp(n);
//      EXPECT_DOUBLE_EQ(expected, result)
//          << "The result of exp(" << n << ") is wrong.";
//    }
    constexpr double s = zisc::power<4>(x); 
    {
      constexpr double result = zisc::constant::exp(s);
      const double expected = std::exp(s);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of exp(" << s << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testExp();
  }

  static void testLog2()
  {
    constexpr double n = ::makeNormal(x);
    {
      constexpr double result = zisc::constant::log2(n);
      const double expected = std::log2(n);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of log2(" << n << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testLog2();
  }

  static void testLog()
  {
    constexpr double n = ::makeNormal(x);
    {
      constexpr double result = zisc::constant::log(n);
      const double expected = std::log(n);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of log(" << n << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testLog();
  }

  static void testLog10()
  {
    constexpr double n = ::makeNormal(x);
    {
      constexpr double result = zisc::constant::log10(n);
      const double expected = std::log10(n);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of log10(" << n << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testLog10();
  }

  static void testSin()
  {
    {
      constexpr double theta = 0.5 * zisc::kPi<double> * x;
      constexpr double result = zisc::constant::sin(theta);
      const double expected = std::sin(theta);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of sin(" << theta << ") is wrong.";
    }
    {
      constexpr double theta = 2.0 * zisc::kPi<double> * x;
      constexpr double result = zisc::constant::sin(theta);
      const double expected = std::sin(theta);
      const double error = zisc::constant::getUlps<16>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of sin(" << theta << ") is wrong.";
    }
//    {
//      constexpr double theta = 16.0 * zisc::kPi<double> * x;
//      constexpr double result = zisc::constant::sin(theta);
//      const double expected = std::sin(theta);
//      EXPECT_NEAR(expected, result, zisc::constant::getUlps<8>(expected + result))
//          << "The result of sin(" << theta << ") is wrong.";
//    }
    ConstexprTest<end, i + 1>::testSin();
  }

  static void testCos()
  {
    {
      constexpr double y = 0.5 * (x + 1.0);
      constexpr double theta = zisc::kPi<double> * y;
      constexpr double result = zisc::constant::cos(theta);
      const double expected = std::cos(theta);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of cos(" << theta << ") is wrong.";
    }
    {
      constexpr double theta = 2.0 * zisc::kPi<double> * x;
      constexpr double result = zisc::constant::cos(theta);
      const double expected = std::cos(theta);
      const double error = zisc::constant::getUlps<16>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of cos(" << theta << ") is wrong.";
    }
//    {
//      constexpr double theta = 16.0 * zisc::kPi<double> * x;
//      constexpr double result = zisc::constant::cos(theta);
//      const double expected = std::cos(theta);
//      const double error = zisc::constant::getUlps<16>(expected + result);
//      EXPECT_NEAR(expected, result, error)
//          << "The result of cos(" << theta << ") is wrong.";
//    }
    ConstexprTest<end, i + 1>::testCos();
  }

  static void testAsin()
  {
    {
      constexpr double result = zisc::constant::asin(x);
      const double expected = std::asin(x);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of asin(" << x << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testAsin();
  }

  static void testAcos()
  {
    {
      constexpr double result = zisc::constant::acos(x);
      const double expected = std::acos(x);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of acos(" << x << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testAcos();
  }


  static void testAtan()
  {
    {
      constexpr double result = zisc::constant::atan(x);
      const double expected = std::atan(x);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of atan(" << x << ") is wrong.";
    }
    {
      constexpr double n = ::makeNormal(x);
      constexpr double result = zisc::constant::atan(n);
      const double expected = std::atan(n);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of atan(" << n << ") is wrong.";
    }
    ConstexprTest<end, i + 1>::testAtan();
  }
};

template <int end>
struct ConstexprTest<end, end>
{
  static void testSqrt()
  {
    {
      constexpr double x = 1.0;
      constexpr double result = zisc::constant::sqrt(x);
      const double expected = std::sqrt(x);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of sqrt(" << x << ") is wrong.";
    }
  }

  static void testCbrt()
  {
  }

  static void testExp()
  {
    {
      constexpr double x = 0.0;
      constexpr double result = zisc::constant::exp(x);
      const double expected = std::exp(x);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of exp(" << x << ") is wrong.";
    }
    {
      constexpr double x = 1.0;
      constexpr double result = zisc::constant::exp(x);
      const double expected = std::exp(x);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of exp(" << x << ") is wrong.";
    }
  }

  static void testLog2()
  {
    {
      constexpr double x = 1.0;
      constexpr double result = zisc::constant::log2(x);
      const double expected = std::log2(x);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of log2(" << x << ") is wrong.";
    }
    {
      constexpr double x = std::numeric_limits<double>::min();
      constexpr double result = zisc::constant::log2(x);
      const double expected = std::log2(x);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of log2(" << x << ") is wrong.";
    }
    {
      constexpr double x = std::numeric_limits<double>::max();
      constexpr double result = zisc::constant::log2(x);
      const double expected = std::log2(x);
      EXPECT_DOUBLE_EQ(expected, result)
          << "The result of log2(" << x << ") is wrong.";
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
  }

  static void testCos()
  {
  }

  static void testAsin()
  {
    {
      constexpr double x = 1.0;
      constexpr double result = zisc::constant::atan(x);
      const double expected = std::atan(x);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of atan(" << x << ") is wrong.";
    }
  }

  static void testAcos()
  {
    {
      constexpr double x = 1.0;
      constexpr double result = zisc::constant::atan(x);
      const double expected = std::atan(x);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of atan(" << x << ") is wrong.";
    }
  }

  static void testAtan()
  {
    {
      constexpr double x = 1.0;
      constexpr double result = zisc::constant::atan(x);
      const double expected = std::atan(x);
      const double error = zisc::constant::getUlps<8>(expected + result);
      EXPECT_NEAR(expected, result, error)
          << "The result of atan(" << x << ") is wrong.";
    }
  }
};

} // namespace

TEST(ConstMathTest, SqrtTest)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  ::ConstexprTest<end, start>::testSqrt();
}

TEST(ConstMathTest, CbrtTest)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<end, start>::testCbrt();
}

TEST(ConstMathTest, ExpTest)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<end, start>::testExp();
}

TEST(ConstMathTest, Log2Test)
{
  constexpr int start = 0;
  constexpr int end = 1024;
  ::ConstexprTest<end, start>::testLog2();
}

TEST(ConstMathTest, LogTest)
{
  constexpr int start = 0;
  constexpr int end = 512;
  ::ConstexprTest<end, start>::testLog();
}

TEST(ConstMathTest, Log10Test)
{
  constexpr int start = 0;
  constexpr int end = 512;
  ::ConstexprTest<end, start>::testLog10();
}

//TEST(ConstMathTest, SinTest)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  ::ConstexprTest<end, start>::testSin();
//}

//TEST(ConstMathTest, CosTest)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  ::ConstexprTest<end, start>::testCos();
//}

TEST(ConstMathTest, AsinTest)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<end, start>::testAsin();
}

TEST(ConstMathTest, AcosTest)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<end, start>::testAcos();
}

TEST(ConstMathTest, AtanTest)
{
  constexpr int start = -512;
  constexpr int end = 512;
  ::ConstexprTest<end, start>::testAtan();
}
