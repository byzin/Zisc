/*!
  \file const_math_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifdef ZISC_CONSTEXPR_MATH_TEST

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

namespace {

template <typename Float>
void printFloat(const Float x)
{
  std::cout << std::fixed
            << std::setprecision(std::numeric_limits<Float>::max_digits10)
            << x << std::endl;
}

} // namespace 

TEST(ConstMathTest, PiTest)
{
  // Float
  {
    constexpr float pi = zisc::constant::pi<float>();
    EXPECT_LT(3.14f, pi);
    std::cout << "  float pi: ";
    ::printFloat(pi);
  }
  // Double
  {
    constexpr double pi = zisc::constant::pi<double>();
    EXPECT_LT(3.14, pi);
    std::cout << " double pi: ";
    ::printFloat(pi);
  }
  // Long double
  {
    constexpr long double pi = zisc::constant::pi<long double>();
    EXPECT_LT(3.14l, pi);
    std::cout << "ldouble pi: ";
    ::printFloat(pi);
  }
}

TEST(ConstMathTest, AbsTest)
{
  {
    constexpr int value = zisc::constant::abs(3);
    EXPECT_EQ(3, value) << "The result of abs(3) is wrong.";
  }
  {
    constexpr int value = zisc::constant::abs(-1);
    EXPECT_EQ(1, value) << "The result of abs(-1) is wrong.";
  }
  {
    constexpr double value = zisc::constant::abs(-2.5);
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

template <int scale, int end, int step, int i>
struct PowLoopTest
{
  static void test()
  {
    constexpr double result = zisc::constant::pow(2.0, i);
    const double expected = std::pow(2.0, i);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of pow(2, " << i << ") is wrong.";
    PowLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct PowLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct PowTest
{
  static void test()
  {
    constexpr int start = -100;
    constexpr int end = 100;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 1;
      PowLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, PowTest)
{
  ::PowTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct SqrtLoopTest
{
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    static_assert(0.0 <= x, "The x is minus.");
    constexpr double result = zisc::constant::sqrt(x);
    const double expected = std::sqrt(x);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of sqrt(" << x << ") is wrong.";
    SqrtLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct SqrtLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct SqrtTest
{
  static void test()
  {
    constexpr int start = 0;
    constexpr int end = 20000;
    constexpr int step = 100;
    // Loop test
    {
      constexpr int scale = 20000;
      SqrtLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      SqrtLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, SqrtTest)
{
  ::SqrtTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct CbrtLoopTest
{
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::cbrt(x);
    const double expected = std::cbrt(x);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of cbrt(" << x << ") is wrong.";
    CbrtLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct CbrtLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct CbrtTest
{
  static void test()
  {
    constexpr int start = -10000;
    constexpr int end = 10000;
    constexpr int step = 100;
    // Loop test
    {
      constexpr int scale = 10000;
      CbrtLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      CbrtLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, CbrtTest)
{
  ::CbrtTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct ExpLoopTest
{
  static constexpr double error = 1.0e-14;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::exp(x);
    const double expected = std::exp(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of exp(" << x << ") is wrong.";
    ExpLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct ExpLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct ExpTest
{
  static void test()
  {
    {
      constexpr double result = zisc::constant::exp(1.0);
      const double expected = std::exp(1.0);
      EXPECT_DOUBLE_EQ(expected, result) << "The result of exp(1) is wrong.";
    }
    constexpr int start = -100;
    constexpr int end = 100;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 100;
      ExpLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      ExpLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, ExponentialTest)
{
  ::ExpTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct LogLoopTest
{
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::log(x);
    const double expected = std::log(x);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of log(" << x << ") is wrong.";
    LogLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct LogLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct LogTest
{
  static void test()
  {
    constexpr int step = 100;
    constexpr int start = step;
    constexpr int end = 20000;
    // Loop test
    {
      constexpr int scale = 20000;
      LogLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      LogLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace inner

TEST(ConstMathTest, LogTest)
{
  ::LogTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct Log2LoopTest
{
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::log2(x);
    const double expected = std::log2(x);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of log2(" << x << ") is wrong.";
    Log2LoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct Log2LoopTest<scale, end, step, end>
{
  static void test() {}
};

struct Log2Test
{
  static void test()
  {
    constexpr int step = 100;
    constexpr int start = step;
    constexpr int end = 20000;
    // Loop test
    {
      constexpr int scale = 20000;
      Log2LoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      Log2LoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace inner

TEST(ConstMathTest, Log2Test)
{
  ::Log2Test::test();
}

namespace {

template <int scale, int end, int step, int i>
struct Log10LoopTest
{
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::log10(x);
    const double expected = std::log10(x);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of log10(" << x << ") is wrong.";
    Log10LoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct Log10LoopTest<scale, end, step, end>
{
  static void test() {}
};

struct Log10Test
{
  static void test()
  {
    constexpr int step = 100;
    constexpr int start = step;
    constexpr int end = 20000;
    // Loop test
    {
      constexpr int scale = 20000;
      Log10LoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      Log10LoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace inner

TEST(ConstMathTest, Log10Test)
{
  ::Log10Test::test();

}

namespace {

template <int scale, int end, int step, int i>
struct SinLoopTest
{
  static constexpr double error = 1.0e-12;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = zisc::constant::pi<double>() * t;
    constexpr double result = zisc::constant::sin(x);
    const double expected = std::sin(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of sin(" << x << ") is wrong.";
    SinLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct SinLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct SinTest
{
  static void test()
  {
    constexpr int start = -100;
    constexpr int end = 100;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 100;
      SinLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      SinLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, SinTest)
{
  ::SinTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct CosLoopTest
{
  static constexpr double error = 1.0e-12;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = zisc::constant::pi<double>() * t;
    constexpr double result = zisc::constant::cos(x);
    const double expected = std::cos(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of cos(" << x << ") is wrong.";
    CosLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct CosLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct CosTest
{
  static void test()
  {
    constexpr int start = 0;
    constexpr int end = 200;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 200;
      CosLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      CosLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, CosTest)
{
  ::CosTest::test();
}

namespace {

template <int scale, int step, int i>
struct TanLoopTest
{
  static constexpr double error = 1.0e-12;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = 0.5 * zisc::constant::pi<double>() * t;
    constexpr double result = zisc::constant::tan(x);
    const double expected = std::tan(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of tan(" << x << ") is wrong.";
    TanLoopTest<scale, step, i + step>::test();
  }
};

template <int scale, int step>
struct TanLoopTest<scale, step, scale>
{
  static void test() {}
};

struct TanTest
{
  static void test()
  {
    constexpr int start = 0;
    constexpr int end = 200;
    constexpr int step = 1;
    // Loop test
    {
      TanLoopTest<end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, TanTest)
{
  ::TanTest::test();
}

namespace {

template <int scale, int i>
struct ArcSinLoopTest
{
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    static_assert(zisc::isInBounds(x, -1.0, 1.0), "The x isn't [-1, 1).");
    constexpr double result = zisc::constant::asin(x);
    const double expected = std::asin(x);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of asin(" << x << ") is wrong.";
    ArcSinLoopTest<scale, i + 1>::test();
  }
};

template <int scale>
struct ArcSinLoopTest<scale, scale>
{
  static void test() {}
};

struct ArcSinTest
{
  static void test()
  {
    constexpr int start = -100;
    constexpr int end = 100;
    // loop test
    {
      ArcSinLoopTest<end, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, ArcSinTest)
{
  ::ArcSinTest::test();
}

namespace {

template <int scale, int i>
struct ArcCosLoopTest
{
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    static_assert(zisc::isInBounds(x, -1.0, 1.0), "The x isn't [-1, 1).");
    constexpr double result = zisc::constant::acos(x);
    const double expected = std::acos(x);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of acos(" << x << ") is wrong.";
    ArcCosLoopTest<scale, i + 1>::test();
  }
};

template <int scale>
struct ArcCosLoopTest<scale, scale>
{
  static void test() {}
};

struct ArcCosTest
{
  static void test()
  {
    constexpr int start = -100;
    constexpr int end = 100;
    // loop test
    {
      ArcCosLoopTest<end, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, ArcCosTest)
{
  ::ArcCosTest::test();
}

namespace {

template <int scale, int i>
struct ArcTanLoopTest
{
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = 10000.0 * t;
    constexpr double result = zisc::constant::atan(x);
    const double expected = std::atan(x);
    EXPECT_DOUBLE_EQ(expected, result)
        << "The result of atan(" << x << ") is wrong.";
    ArcTanLoopTest<scale, i + 1>::test();
  }
};

template <int scale>
struct ArcTanLoopTest<scale, scale>
{
  static void test() {}
};

struct ArcTanTest
{
  static void test()
  {
    constexpr int start = -100;
    constexpr int end = 100;
    // loop test
    {
      ArcTanLoopTest<end, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, ArcTanTest)
{
  ::ArcTanTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct SinhLoopTest
{
  static constexpr double error = 1.0e-14;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::sinh(x);
    const double expected = std::sinh(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of sinh(" << x << ") is wrong.";
    SinhLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct SinhLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct SinhTest
{
  static void test()
  {
    constexpr int start = -100;
    constexpr int end = 100;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 100;
      SinhLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      SinhLoopTest<scale, end, step, start>::test();
    }
  }
};

}

TEST(ConstMathTest, SinhTest)
{
  ::SinhTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct CoshLoopTest
{
  static constexpr double error = 1.0e-14;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::cosh(x);
    const double expected = std::cosh(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of cosh(" << x << ") is wrong.";
    CoshLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct CoshLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct CoshTest
{
  static void test()
  {
    constexpr int start = -100;
    constexpr int end = 100;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 100;
      CoshLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      CoshLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, CoshTest)
{
  ::CoshTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct TanhLoopTest
{
  static constexpr double error = 1.0e-14;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::tanh(x);
    const double expected = std::tanh(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of tanh(" << x << ") is wrong.";
    TanhLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct TanhLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct TanhTest
{
  static void test()
  {
    constexpr int start = -100;
    constexpr int end = 100;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 100;
      TanhLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      TanhLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, TanhTest)
{
  ::TanhTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct ArcSinhLoopTest
{
  static constexpr double error = 1.0e-14;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::asinh(x);
    const double expected = std::asinh(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of asinh(" << x << ") is wrong.";
    ArcSinhLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct ArcSinhLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct ArcSinhTest
{
  static void test()
  {
    constexpr int start = -10000;
    constexpr int end = 10000;
    constexpr int step = 100;
    // Loop test
    {
      constexpr int scale = 10000;
      ArcSinhLoopTest<scale, end, step, start>::test();
    }
    // Long range loop test
    {
      constexpr int scale = 1;
      ArcSinhLoopTest<scale, end, step, start>::test();
    }
  }
};

}

TEST(ConstMathTest, ArcSinhTest)
{
  ::ArcSinhTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct ArcCoshLoopTest
{
  static constexpr double error = 1.0e-14;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::acosh(x);
    const double expected = std::acosh(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of acosh(" << x << ") is wrong.";
    ArcCoshLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct ArcCoshLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct ArcCoshTest
{
  static void test()
  {
    constexpr int start = 1;
    constexpr int end = 200;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 1;
      ArcCoshLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, ArcCoshTest)
{
  ::ArcCoshTest::test();
}

namespace {

template <int scale, int end, int step, int i>
struct ArcTanhLoopTest
{
  static constexpr double error = 1.0e-14;
  static void test()
  {
    constexpr double t = zisc::cast<double>(i) / zisc::cast<double>(scale);
    constexpr double x = t;
    constexpr double result = zisc::constant::atanh(x);
    const double expected = std::atanh(x);
    EXPECT_NEAR(expected, result, error)
        << "The result of atanh(" << x << ") is wrong.";
    TanhLoopTest<scale, end, step, i + step>::test();
  }
};

template <int scale, int end, int step>
struct ArcTanhLoopTest<scale, end, step, end>
{
  static void test() {}
};

struct ArcTanhTest
{
  static void test()
  {
    constexpr int start = -99;
    constexpr int end = 99;
    constexpr int step = 1;
    // Loop test
    {
      constexpr int scale = 100;
      ArcTanhLoopTest<scale, end, step, start>::test();
    }
  }
};

} // namespace

TEST(ConstMathTest, ArcTanhTest)
{
  ::ArcTanhTest::test();
}

#endif // ZISC_CONSTEXPR_MATH_TEST
