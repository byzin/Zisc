/*!
  \file math_power_double_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cmath>
#include <fstream>
#include <numbers>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/binary_serializer.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/math/math.hpp"
// Test
#include "basic_test.hpp"
#include "math_test.hpp"
#include "power_test.hpp"

namespace {

template <int i, int start, int end>
void testPowConstant() noexcept
{
  static constexpr double u = zisc::cast<double>(i) / zisc::cast<double>(end);
  {
    constexpr double base = 2.0;
    constexpr double expt = kPowScale<double> * u;
    constexpr double y = zisc::pow(base, expt);
    const double expected = zisc::pow(base, expt);
    ASSERT_EQ(expected, y) << "pow(" << base << "," << expt << ") failed.";
  }
  {
    constexpr double base = std::numbers::pi_v<double>;
    constexpr double expt = kPowScale<double> * u;
    constexpr double y = zisc::pow(base, expt);
    const double expected = zisc::pow(base, expt);
    ASSERT_EQ(expected, y) << "pow(" << base << "," << expt << ") failed.";
  }
  {
    constexpr double base = 1.0 / std::numbers::pi_v<double>;
    constexpr double expt = kPowScale<double> * u;
    constexpr double y = zisc::pow(base, expt);
    const double expected = zisc::pow(base, expt);
    ASSERT_EQ(expected, y) << "pow(" << base << "," << expt << ") failed.";
  }
  if constexpr (i < end)
    testPowConstant<i + 1, start, end>();
}

template <int i, int start, int end, int c>
void testPowIntConstant(const std::vector<double>& expected_list,
                MathTestResult<double>* result) noexcept
{
  constexpr double pi = std::numbers::pi_v<double>;
  constexpr double base = (c == 0) ? 2.0 :
                          (c == 1) ? pi :
                          (c == 2) ? 1.0 / pi :
                          (c == 3) ? -pi :
                                     -1.0 / pi;
  {
    constexpr int expt = i;
    constexpr double y = zisc::pow(base, expt);
    const std::size_t index = zisc::cast<std::size_t>(i - start);
    const double expected = expected_list[index];
    testMathFloat(expected, y, result);
  }
  if constexpr (i < end - 1)
    testPowIntConstant<i + 1, start, end, c>(expected_list, result);
}

}

TEST(MathTest, PowDTest)
{
  std::ifstream reference_file{"resources/math_powd_reference.txt",
                               std::ios_base::binary};
  zisc::int32b n = 0;
  zisc::BSerializer::read(&n, &reference_file);
  zisc::int32b n_base = 0;
  zisc::BSerializer::read(&n_base, &reference_file);

  std::vector<double> x_list;
  x_list.resize(n);
  zisc::BSerializer::read(x_list.data(), &reference_file, sizeof(double) * n);

  MathTestResult<double> math_result;
  for (int base_index = 0; base_index < n_base; ++base_index) {
    double base = 0.0;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<double> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(double) * n);
    for (std::size_t i = 0; i < x_list.size(); ++i) {
      const double expt = x_list[i];
      const double y = zisc::pow(base, expt);
      const double expected = expected_list[i];
//      EXPECT_DOUBLE_EQ(expected, y) << "pow(" << base << "," << expt << ") is wrong.";
      testMathFloat(expected, y, &math_result);
    }
  }
  // Special cases
  zisc::int32b n_specials = 0;
  zisc::BSerializer::read(&n_specials, &reference_file);
  for (int spec_index = 0; spec_index < n_specials; ++spec_index) {
    double base = 0.0;
    zisc::BSerializer::read(&base, &reference_file);
    double expt = 0.0;
    zisc::BSerializer::read(&expt, &reference_file);
    const double y = zisc::pow(base, expt);
    double expected = 0.0;
    zisc::BSerializer::read(&expected, &reference_file);
    if (!testMathFloat(expected, y, &math_result)) {
      std::cout << "  [special] zisc::pow(" << base  << "," << expt << ") is wrong."
                << std::endl
                << "      actual: " << std::scientific << y  << std::endl
                << "    expected: " << std::scientific << expected << std::endl;
    }
  }

  math_result.print();
  math_result.check("zisc::pow()");
}

TEST(MathTest, PowDContantTest)
{
  constexpr int start = -8;
  constexpr int end = 8;
  ::testPowConstant<start, start, end>();
}

TEST(MathTest, PowIntDContantTest)
{
  constexpr int start = -8;
  constexpr int end = 8;

  std::ifstream reference_file{"resources/math_powintd_reference.txt",
                               std::ios_base::binary};
  zisc::int32b n = 0;
  zisc::BSerializer::read(&n, &reference_file);
  ASSERT_EQ(end - start, n);
  zisc::int32b n_base = 0;
  zisc::BSerializer::read(&n_base, &reference_file);

  std::vector<double> x_list;
  x_list.resize(n);
  zisc::BSerializer::read(x_list.data(), &reference_file, sizeof(double) * n);

  MathTestResult<double> math_result;
  {
    double base = 0.0;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<double> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(double) * n);
    ::testPowIntConstant<start, start, end, 0>(expected_list, &math_result);
  }
  {
    double base = 0.0;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<double> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(double) * n);
    ::testPowIntConstant<start, start, end, 1>(expected_list, &math_result);
  }
  {
    double base = 0.0;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<double> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(double) * n);
    ::testPowIntConstant<start, start, end, 2>(expected_list, &math_result);
  }
  {
    double base = 0.0;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<double> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(double) * n);
    ::testPowIntConstant<start, start, end, 3>(expected_list, &math_result);
  }
  {
    double base = 0.0;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<double> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(double) * n);
    ::testPowIntConstant<start, start, end, 4>(expected_list, &math_result);
  }

  math_result.print();
  math_result.check("zisc::pow()");
}

namespace {

template <int i, int start, int end>
void testSqrtConstant() noexcept
{
  static constexpr double u = zisc::cast<double>(i) / zisc::cast<double>(end);
  {
    constexpr double x = makeNormal(u);
    constexpr double y = zisc::sqrt(x);
    const double expected = zisc::sqrt(x);
    ASSERT_EQ(expected, y) << "sqrt(" << x << ") failed.";
  }
  if constexpr (i < end)
    testSqrtConstant<i + 1, start, end>();
}

} // namespace 

TEST(MathTest, SqrtDTest)
{
  auto x_list = loadPositiveXList<double>();
  const std::size_t n = x_list.size();

  std::ifstream reference_file{"resources/math_sqrtd_reference.txt",
                               std::ios_base::binary};
  MathTestResult<double> math_result;
  std::vector<double> expected_list;
  expected_list.resize(x_list.size());
  zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(double) * n);
  for (std::size_t i = 0; i < x_list.size(); ++i) {
    const double x = x_list[i];
    const double y = zisc::sqrt(x);
    const double expected = expected_list[i];
//    EXPECT_DOUBLE_EQ(expected, y) << "sqrt(" << x << ") is wrong.";
    testMathFloat(expected, y, &math_result);
  }
  // Special cases
  zisc::int32b n_specials = 0;
  zisc::BSerializer::read(&n_specials, &reference_file);
  for (int spec_index = 0; spec_index < n_specials; ++spec_index) {
    double x = 0.0;
    zisc::BSerializer::read(&x, &reference_file);
    const double y = zisc::sqrt(x);
    double expected = 0.0;
    zisc::BSerializer::read(&expected, &reference_file);
    if (!testMathFloat(expected, y, &math_result)) {
      std::cout << "  [special] zisc::sqrt(" << x << ") is wrong."
                << std::endl
                << "      actual: " << std::scientific << y  << std::endl
                << "    expected: " << std::scientific << expected << std::endl;
    }
  }

  // Integer
  {
    constexpr double y = zisc::sqrt(100);
    constexpr double expected = zisc::sqrt(100.0);
    ASSERT_EQ(expected, y);
  }

  math_result.print();
  math_result.check("zisc::sqrt()");
}

TEST(MathTest, SqrtSubnormalDTest)
{
  auto x_list = loadPositiveSubnormalXList<double>();
  const std::size_t n = x_list.size();

  std::ifstream reference_file{"resources/math_sqrt_subd_reference.txt",
                               std::ios_base::binary};
  MathTestResult<double> math_result;
  std::vector<double> expected_list;
  expected_list.resize(x_list.size());
  zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(double) * n);
  for (std::size_t i = 0; i < x_list.size(); ++i) {
    const double x = x_list[i];
    const double y = zisc::sqrt(x);
    const double expected = expected_list[i];
//    EXPECT_DOUBLE_EQ(expected, y) << "sqrt(" << x << ") is wrong.";
    testMathFloat(expected, y, &math_result);
  }

  math_result.print();
  math_result.check("zisc::sqrt()");
}

TEST(MathTest, SqrtDContantTest)
{
  constexpr int start = 0;
  constexpr int end = 128;
  ::testSqrtConstant<start, start, end>();
}

//TEST(ConstMathTest, ExpTestD)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  PowerConstexprTest<double, end, start>::testExp();
//}
//
//TEST(ConstMathTest, Log2TestD)
//{
//  constexpr int start = 0;
//  constexpr int end = 1024;
//  PowerConstexprTest<double, end, start>::testLog2();
//}
//
//TEST(ConstMathTest, LogTestD)
//{
//  constexpr int start = 0;
//  constexpr int end = 512;
//  PowerConstexprTest<double, end, start>::testLog();
//}
//
//TEST(ConstMathTest, Log10TestD)
//{
//  constexpr int start = 0;
//  constexpr int end = 512;
//  PowerConstexprTest<double, end, start>::testLog10();
//}
