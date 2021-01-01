/*!
  \file math_const_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_CONST_TEST_HPP
#define ZISC_MATH_CONST_TEST_HPP

// Standard C++ library
#include <cmath>
//#include <iostream>
//#include <iomanip>
//#include <limits>
//#include <tuple>
#include <type_traits>
//#include <utility>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
//#include "zisc/ieee_754_binary.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/math/math.hpp"
//// Test
//#include "test.hpp"

template <int i, int end>
struct PowerTest
{
  static void testPowInt()
  {
    {
      constexpr double b = 0.0;
      constexpr double result = zisc::pow(b, i);
      const double expected = std::pow(b, i);
      EXPECT_DOUBLE_EQ(expected, result) << "pow(" << b << "," << i << ") failed.";
    }
    {
      constexpr double b = zisc::kPi<double> / 2.0;
      constexpr double result = zisc::pow(b, i);
      const double expected = std::pow(b, i);
      EXPECT_DOUBLE_EQ(expected, result) << "pow(" << b << "," << i << ") failed.";
    }
    {
      constexpr double b = zisc::kPi<double> / 4.0;
      constexpr double result = zisc::pow(b, i);
      const double expected = std::pow(b, i);
      EXPECT_DOUBLE_EQ(expected, result) << "pow(" << b << "," << i << ") failed.";
    }
    {
      constexpr double b = -zisc::kPi<double> / 2.0;
      constexpr double result = zisc::pow(b, i);
      const double expected = std::pow(b, i);
      EXPECT_DOUBLE_EQ(expected, result) << "pow(" << b << "," << i << ") failed.";
    }
    if constexpr (i < end)
      PowerTest<i + 1, end>::testPowInt();
  }
};

//template <typename Float>
//void printFloat(const char* message, const Float x)
//{
//  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;
//  std::cout << std::setfill(' ') << std::setw(24) << message << ": "
//            << std::scientific
//            << std::setprecision(std::numeric_limits<Float>::max_digits10)
//            << x;
//  if (sizeof(Binary) == 4)
//    std::cout << "f";
//  std::cout << std::endl;
//  const Binary data{x};
//  std::cout << "                     hex: " << std::hex << data.bits() << std::endl;
//  std::cout << std::dec;
//}
//
//template <int end, int i>
//struct PowerTest
//{
//  static void testPower()
//  {
//    {
//      constexpr double result = zisc::constant::power(2.0, i);
//      const double expected = std::pow(2.0, i);
//      EXPECT_DOUBLE_EQ(expected, result) << "power(2, " << i << ") is wrong.";
//    }
//    PowerTest<end, i + 1>::testPower();
//  }
//
//  static void testPow()
//  {
//    {
//      constexpr double result = zisc::constant::pow(2.0, zisc::cast<double>(i));
//      const double expected = std::pow(2.0, i);
//      EXPECT_DOUBLE_EQ(expected, result) << "pow(2, " << i << ") is wrong.";
//    }
//    {
//      constexpr double base = zisc::constant::Math::pi<double>() / 2.0;
//      constexpr double result = zisc::constant::pow(base, zisc::cast<double>(i));
//      const double expected = std::pow(base, i);
//      EXPECT_DOUBLE_EQ(expected, result) << "pow(pi/2, " << i << ") is wrong.";
//    }
//    PowerTest<end, i + 1>::testPow();
//  }
//};
//
//template <int end>
//struct PowerTest<end, end>
//{
//  static void testPower()
//  {
//  }
//
//  static void testPow()
//  {
//  }
//};
//
//#define expectFloatEq(expected, result, message) \
//  if constexpr (std::is_same_v<float, Float>) \
//  { \
//    EXPECT_FLOAT_EQ(expected, result) << message; \
//  } \
//  else \
//  { \
//    EXPECT_DOUBLE_EQ(expected, result) << message; \
//  }
//
//template <typename Float, int end, int i>
//struct ConstexprTest
//{
//  static_assert(std::is_floating_point_v<Float>, "The Float isn't floating point.");
//  static constexpr Float zero = zisc::cast<Float>(0.0);
//  static constexpr Float x = zisc::cast<Float>(i) / zisc::cast<Float>(end);
//  static constexpr Float n = ::makeNormal(x);
////  static constexpr Float d = ::makeSubnormal(x);
//
//  static void testFrLdexp()
//  {
//    auto run_frexp = [](const Float f)
//    {
//      int e = 0;
//      const Float m = zisc::constant::frexp(f, &e);
//      return std::make_pair(m, e);
//    };
//
//    // Normal
//    {
//      constexpr auto result = run_frexp(n);
//      constexpr Float result_m = result.first;
//      constexpr int result_e = result.second;
//      int expected_e = 0;
//      const Float expected_m = std::frexp(n, &expected_e);
//      EXPECT_EQ(expected_e, result_e) << "frexp(" << n << ") is wrong.";
//      EXPECT_EQ(expected_m, result_m) << "frexp(" << n << ") is wrong.";
//
//      constexpr Float result_f = zisc::constant::ldexp(result_m, result_e);
//      const Float expected_f = std::ldexp(result_m, result_e);
//      expectFloatEq(expected_f, result_f, "ldexp(" << n << ") is wrong.")
//    }
//    // Subnormal
////    {
////      constexpr auto result = run_frexp(d);
////      constexpr Float result_m = result.first;
////      constexpr int result_e = result.second;
////      int expected_e = 0;
////      const Float expected_m = std::frexp(d, &expected_e);
////      EXPECT_EQ(expected_e, result_e) << "frexp(" << d << ") is wrong.";
////      EXPECT_EQ(expected_m, result_m) << "frexp(" << d << ") is wrong.";
////
////      constexpr Float result_f = zisc::constant::ldexp(result_m, result_e);
////      const Float expected_f = std::ldexp(result_m, result_e);
////      EXPECT_FLOAT_EQ(expected_f, result_f) << "ldexp(" << d << ") is wrong.";
////    }
//    ConstexprTest<Float, end, i + 1>::testFrLdexp();
//  }
//};
//
//template <typename Float, int end>
//struct ConstexprTest<Float, end, end>
//{
//  static void testFrLdexp()
//  {
//    auto run_frexp = [](const Float f)
//    {
//      int e = 0;
//      const Float m = zisc::constant::frexp(f, &e);
//      return std::make_pair(m, e);
//    };
//
//    // Zero
//    {
//      constexpr Float n = zisc::cast<Float>(0.0);
//
//      constexpr auto result = run_frexp(n);
//      constexpr int result_e = result.second;
//      int expected_e = 0;
//      constexpr Float result_m = result.first;
//      const Float expected_m = std::frexp(n, &expected_e);
//      EXPECT_EQ(expected_e, result_e) << "frexp(" << n << ") is wrong.";
//      expectFloatEq(expected_m, result_m, "frexp(" << n << ") is wrong.")
//
//      constexpr Float result_f = zisc::constant::ldexp(result_m, result_e);
//      const Float expected_f = std::ldexp(result_m, result_e);
//      expectFloatEq(expected_f, result_f, "ldexp(" << n << ") is wrong.")
//    }
//    // Inf
//    {
//      constexpr Float n = std::numeric_limits<Float>::infinity();
//
//      constexpr auto result = run_frexp(n);
//      constexpr Float result_m = result.first;
//      EXPECT_TRUE(std::isinf(result_m)) << "frexp(inf) is wrong.";
//
//      constexpr Float result_f = zisc::constant::ldexp(result_m, 1);
//      EXPECT_TRUE(std::isinf(result_f)) << "frexp(inf) is wrong.";
//    }
//    // Nan 
//    {
//      constexpr Float n = std::numeric_limits<Float>::quiet_NaN();
//
//      const auto result = run_frexp(n);
//      const Float result_m = result.first;
//      EXPECT_TRUE(std::isnan(result_m)) << "frexp(NaN) is wrong.";
//
//      const Float result_f = zisc::constant::ldexp(result_m, 1);
//      EXPECT_TRUE(std::isnan(result_f)) << "frexp(NaN) is wrong.";
//    }
//  }
//};
//
//template <typename Float, int end, int i>
//struct PowerConstexprTest
//{
//  static_assert(std::is_floating_point_v<Float>, "The Float isn't floating point.");
//  static constexpr Float zero = zisc::cast<Float>(0.0);
//  static constexpr Float x = zisc::cast<Float>(i) / zisc::cast<Float>(end);
//  static constexpr Float n = ::makeNormal(x);
////  static constexpr Float d = ::makeSubnormal(x);
//
//  static void testSqrt()
//  {
//    // Normal
//    {
//      constexpr Float result = zisc::constant::sqrt(n);
//      const Float expected = std::sqrt(n);
//      expectFloatEq(expected, result, "sqrt(" << n << ") is wrong.")
//    }
//    PowerConstexprTest<Float, end, i + 1>::testSqrt();
//  }
//
//  static void testCbrt()
//  {
//    // Normal
//    {
//      static constexpr Float l = ::makeNormal(zisc::cast<Float>(0.85) * x);
//      constexpr Float result = zisc::constant::cbrt(l);
//      const Float expected = std::cbrt(l);
//      expectFloatEq(expected, result, "cbrt(" << l << ") is wrong.")
//    }
//    PowerConstexprTest<Float, end, i + 1>::testCbrt();
//  }
//
//  static void testExp()
//  {
//    constexpr Float l = zisc::cast<Float>(0.1) * zisc::cast<Float>(i);
//    {
//      constexpr Float result = zisc::constant::exp(l);
//      const Float expected = std::exp(l);
//      expectFloatEq(expected, result, "exp(" << l << ") is wrong.")
//    }
//    constexpr Float s = zisc::power<4>(x); 
//    {
//      constexpr Float result = zisc::constant::exp(s);
//      const Float expected = std::exp(s);
//      expectFloatEq(expected, result, "exp(" << s << ") is wrong.")
//    }
//    PowerConstexprTest<Float, end, i + 1>::testExp();
//  }
//
//  static void testLog2()
//  {
//    // Normal
//    {
//      constexpr Float result = zisc::constant::log2(n);
//      const Float expected = std::log2(n);
//      expectFloatEq(expected, result, "log2(" << n << ") is wrong.")
//    }
//    PowerConstexprTest<Float, end, i + 1>::testLog2();
//  }
//
//  static void testLog()
//  {
//    // Normal
//    {
//      constexpr Float result = zisc::constant::log(n);
//      const Float expected = std::log(n);
//      expectFloatEq(expected, result, "log(" << n << ") is wrong.")
//    }
//    PowerConstexprTest<Float, end, i + 1>::testLog();
//  }
//
//  static void testLog10()
//  {
//    // Normal
//    {
//      constexpr Float result = zisc::constant::log10(n);
//      const Float expected = std::log10(n);
//      expectFloatEq(expected, result, "log10(" << n << ") is wrong.")
//    }
//    PowerConstexprTest<Float, end, i + 1>::testLog10();
//  }
//};
//
//template <typename Float, int end>
//struct PowerConstexprTest<Float, end, end>
//{
//  static void testSqrt()
//  {
//    {
//      constexpr Float x = zisc::cast<Float>(1.0);
//      constexpr Float result = zisc::constant::sqrt(x);
//      const Float expected = std::sqrt(x);
//      expectFloatEq(expected, result, "sqrt(" << x << ") is wrong.")
//    }
//  }
//
//  static void testCbrt()
//  {
//  }
//
//  static void testExp()
//  {
//    {
//      constexpr Float x = zisc::cast<Float>(0.0);
//      constexpr Float result = zisc::constant::exp(x);
//      const Float expected = std::exp(x);
//      expectFloatEq(expected, result, "exp(" << x << ") is wrong.")
//    }
//    {
//      constexpr Float x = zisc::cast<Float>(1.0);
//      constexpr Float result = zisc::constant::exp(x);
//      const Float expected = std::exp(x);
//      expectFloatEq(expected, result, "exp(" << x << ") is wrong.")
//    }
//  }
//
//  static void testLog2()
//  {
//    {
//      constexpr Float x = zisc::cast<Float>(1.0);
//      constexpr Float result = zisc::constant::log2(x);
//      const Float expected = std::log2(x);
//      expectFloatEq(expected, result, "log2(" << x << ") is wrong.")
//    }
//    {
//      constexpr Float x = std::numeric_limits<Float>::min();
//      constexpr Float result = zisc::constant::log2(x);
//      const Float expected = std::log2(x);
//      expectFloatEq(expected, result, "log2(" << x << ") is wrong.")
//    }
//    {
//      constexpr Float x = std::numeric_limits<Float>::max();
//      constexpr Float result = zisc::constant::log2(x);
//      const Float expected = std::log2(x);
//      expectFloatEq(expected, result, "log2(" << x << ") is wrong.")
//    }
//  }
//
//  static void testLog()
//  {
//  }
//
//  static void testLog10()
//  {
//  }
//};
//
//template <typename Float, int end, int i>
//struct TrigConstexprTest
//{
//  static_assert(std::is_floating_point_v<Float>, "The Float isn't floating point.");
//  static constexpr Float zero = zisc::cast<Float>(0.0);
//  static constexpr Float x = zisc::cast<Float>(i) / zisc::cast<Float>(end);
//  static constexpr Float n = ::makeNormal(x);
////  static constexpr Float d = ::makeSubnormal(x);
//
//  static void testSin()
//  {
//    constexpr Float pi = zisc::constant::Math::pi<Float>();
//    {
//      constexpr Float theta = zisc::cast<Float>(0.5) * pi * x;
//      constexpr Float result = zisc::constant::sin(theta);
//      const Float expected = std::sin(theta);
//      expectFloatEq(expected, result, "sin(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(2.0) * pi * x;
//      constexpr Float result = zisc::constant::sin(theta);
//      const Float expected = std::sin(theta);
//      expectFloatEq(expected, result, "sin(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(16.0) * pi * x;
//      constexpr Float result = zisc::constant::sin(theta);
//      const Float expected = std::sin(theta);
//      expectFloatEq(expected, result, "sin(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    TrigConstexprTest<Float, end, i + 1>::testSin();
//  }
//
//  static void testCos()
//  {
//    constexpr Float pi = zisc::constant::Math::pi<Float>();
//    {
//      constexpr Float theta = zisc::cast<Float>(0.5) * pi * x;
//      constexpr Float result = zisc::constant::cos(theta);
//      const Float expected = std::cos(theta);
//      expectFloatEq(expected, result, "cos(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(2.0) * pi * x;
//      constexpr Float result = zisc::constant::cos(theta);
//      const Float expected = std::cos(theta);
//      expectFloatEq(expected, result, "cos(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(16.0) * pi * x;
//      constexpr Float result = zisc::constant::cos(theta);
//      const Float expected = std::cos(theta);
//      expectFloatEq(expected, result, "cos(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    TrigConstexprTest<Float, end, i + 1>::testCos();
//  }
//
//  static void testTan()
//  {
//    constexpr Float pi = zisc::constant::Math::pi<Float>();
//    {
//      constexpr Float theta = zisc::cast<Float>(0.5) * pi * x;
//      constexpr Float result = zisc::constant::tan(theta);
//      const Float expected = std::tan(theta);
//      expectFloatEq(expected, result, "tan(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(2.0) * pi * x;
//      constexpr Float result = zisc::constant::tan(theta);
//      const Float expected = std::tan(theta);
//      expectFloatEq(expected, result, "tan(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(16.0) * pi * x;
//      constexpr Float result = zisc::constant::tan(theta);
//      const Float expected = std::tan(theta);
//      expectFloatEq(expected, result, "tan(" << (theta / pi) << " x pi" << ") is wrong.")
//    }
//    TrigConstexprTest<Float, end, i + 1>::testTan();
//  }
//
//  static void testAsin()
//  {
//    {
//      constexpr Float result = zisc::constant::asin(x);
//      const Float expected = std::asin(x);
//      expectFloatEq(expected, result, "asin(" << x << ") is wrong.")
//    }
//    TrigConstexprTest<Float, end, i + 1>::testAsin();
//  }
//
//  static void testAcos()
//  {
//    {
//      constexpr Float result = zisc::constant::acos(x);
//      const Float expected = std::acos(x);
//      expectFloatEq(expected, result, "acos(" << x << ") is wrong.")
//    }
//    TrigConstexprTest<Float, end, i + 1>::testAcos();
//  }
//
//  static void testAtan()
//  {
//    {
//      constexpr Float result = zisc::constant::atan(x);
//      const Float expected = std::atan(x);
//      expectFloatEq(expected, result, "atan(" << x << ") is wrong.")
//    }
//    {
//      constexpr Float result = zisc::constant::atan(n);
//      const Float expected = std::atan(n);
//      expectFloatEq(expected, result, "atan(" << n << ") is wrong.")
//    }
//    TrigConstexprTest<Float, end, i + 1>::testAtan();
//  }
//};
//
//template <typename Float, int end>
//struct TrigConstexprTest<Float, end, end>
//{
//  static void testSin()
//  {
//    constexpr Float pi = zisc::constant::Math::pi<Float>();
//    {
//      constexpr Float theta = zisc::cast<Float>(0.0);
//      constexpr Float result = zisc::constant::sin(theta);
//      const Float expected = std::sin(theta);
//      expectFloatEq(expected, result, "sin(" << theta << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(0.25) * pi;
//      constexpr Float result = zisc::constant::sin(theta);
//      const Float expected = std::sin(theta);
//      expectFloatEq(expected, result, "sin(" << theta << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(0.5) * pi;
//      constexpr Float result = zisc::constant::sin(theta);
//      const Float expected = std::sin(theta);
//      expectFloatEq(expected, result, "sin(" << theta << ") is wrong.")
//    }
//  }
//
//  static void testCos()
//  {
//    constexpr Float pi = zisc::constant::Math::pi<Float>();
//    {
//      constexpr Float theta = zisc::cast<Float>(0.0);
//      constexpr Float result = zisc::constant::cos(theta);
//      const Float expected = std::cos(theta);
//      expectFloatEq(expected, result, "cos(" << theta << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(0.25) * pi;
//      constexpr Float result = zisc::constant::cos(theta);
//      const Float expected = std::cos(theta);
//      expectFloatEq(expected, result, "cos(" << theta << ") is wrong.")
//    }
//    {
//      constexpr Float theta = zisc::cast<Float>(0.5) * pi;
//      constexpr Float result = zisc::constant::cos(theta);
//      const Float expected = std::cos(theta);
//      expectFloatEq(expected, result, "cos(" << theta << ") is wrong.")
//    }
//  }
//
//  static void testTan()
//  {
//  }
//
//  static void testAsin()
//  {
//    {
//      constexpr Float x = zisc::cast<Float>(1.0);
//      constexpr Float result = zisc::constant::asin(x);
//      const Float expected = std::asin(x);
//      expectFloatEq(expected, result, "asin(" << x << ") is wrong.")
//    }
//  }
//
//  static void testAcos()
//  {
//    {
//      constexpr Float x = zisc::cast<Float>(1.0);
//      constexpr Float result = zisc::constant::acos(x);
//      const Float expected = std::acos(x);
//      expectFloatEq(expected, result, "acos(" << x << ") is wrong.")
//    }
//  }
//
//  static void testAtan()
//  {
//    {
//      constexpr Float x = zisc::cast<Float>(1.0);
//      constexpr Float result = zisc::constant::atan(x);
//      const Float expected = std::atan(x);
//      expectFloatEq(expected, result, "atan(" << x << ") is wrong.")
//    }
//  }
//};

#endif // ZISC_MATH_CONST_TEST_HPP
