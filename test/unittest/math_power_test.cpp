/*!
  \file math_power_test.cpp
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
#include "math_test.hpp"
#include "pow_test.hpp"

namespace {

template <int i, int start, int end>
void testPowConstant() noexcept
{
  {
    constexpr float base = 2.0f;
    constexpr float expt = getPowInput<float>(i, end);
    constexpr float y = zisc::pow(base, expt);
    const float expected = zisc::pow(base, expt);
    ASSERT_EQ(expected, y) << "pow(" << base << "," << expt << ") failed.";
  }
  {
    constexpr float base = std::numbers::pi_v<float>;
    constexpr float expt = getPowInput<float>(i, end);
    constexpr float y = zisc::pow(base, expt);
    const float expected = zisc::pow(base, expt);
    ASSERT_EQ(expected, y) << "pow(" << base << "," << expt << ") failed.";
  }
  {
    constexpr float base = 1.0f / std::numbers::pi_v<float>;
    constexpr float expt = getPowInput<float>(i, end);
    constexpr float y = zisc::pow(base, expt);
    const float expected = zisc::pow(base, expt);
    ASSERT_EQ(expected, y) << "pow(" << base << "," << expt << ") failed.";
  }
  if constexpr (i < end)
    testPowConstant<i + 1, start, end>();
}

template <int i, int start, int end, int c>
void testPowIntConstant(const std::vector<float>& expected_list,
                MathTestResult<float>* result) noexcept
{
  constexpr float pi = std::numbers::pi_v<float>;
  constexpr float base = (c == 0) ? 2.0f :
                         (c == 1) ? pi :
                         (c == 2) ? 1.0f / pi :
                         (c == 3) ? -pi :
                                    -1.0f / pi;
  {
    constexpr int expt = i;
    constexpr float y = zisc::pow(base, expt);
    const std::size_t index = zisc::cast<std::size_t>(i - start);
    const float expected = expected_list[index];
    testMathFloat(expected, y, result);
  }
  if constexpr (i < end - 1)
    testPowIntConstant<i + 1, start, end, c>(expected_list, result);
}

} // namespace 

TEST(MathTest, PowTest)
{
  std::ifstream reference_file{"resources/math_powf_reference.txt",
                               std::ios_base::binary};
  zisc::int32b n = 0;
  zisc::BSerializer::read(&n, &reference_file);
  zisc::int32b n_base = 0;
  zisc::BSerializer::read(&n_base, &reference_file);

  std::vector<float> x_list;
  x_list.resize(n);
  zisc::BSerializer::read(x_list.data(), &reference_file, sizeof(float) * n);

  MathTestResult<float> math_result;
  for (int base_index = 0; base_index < n_base; ++base_index) {
    float base = 0.0f;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<float> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(float) * n);
    for (std::size_t i = 0; i < x_list.size(); ++i) {
      const float expt = x_list[i];
      const float y = zisc::pow(base, expt);
      const float expected = expected_list[i];
//      EXPECT_FLOAT_EQ(expected, y) << "pow(" << base  << "," << expt << ") is wrong.";
      testMathFloat(expected, y, &math_result);
    }
  }
  // Special cases
  zisc::int32b n_specials = 0;
  zisc::BSerializer::read(&n_specials, &reference_file);
  for (int spec_index = 0; spec_index < n_specials; ++spec_index) {
    float base = 0.0f;
    zisc::BSerializer::read(&base, &reference_file);
    float expt = 0.0f;
    zisc::BSerializer::read(&expt, &reference_file);
    const float y = zisc::pow(base, expt);
    float expected = 0.0f;
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

TEST(MathTest, PowContantTest)
{
  constexpr int start = -8;
  constexpr int end = 8;
  ::testPowConstant<start, start, end>();
}

TEST(MathTest, PowIntContantTest)
{
  constexpr int start = -8;
  constexpr int end = 8;

  std::ifstream reference_file{"resources/math_powintf_reference.txt",
                               std::ios_base::binary};
  zisc::int32b n = 0;
  zisc::BSerializer::read(&n, &reference_file);
  ASSERT_EQ(end - start, n);
  zisc::int32b n_base = 0;
  zisc::BSerializer::read(&n_base, &reference_file);

  std::vector<float> x_list;
  x_list.resize(n);
  zisc::BSerializer::read(x_list.data(), &reference_file, sizeof(float) * n);

  MathTestResult<float> math_result;
  {
    float base = 0.0f;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<float> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(float) * n);
    ::testPowIntConstant<start, start, end, 0>(expected_list, &math_result);
  }
  {
    float base = 0.0f;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<float> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(float) * n);
    ::testPowIntConstant<start, start, end, 1>(expected_list, &math_result);
  }
  {
    float base = 0.0f;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<float> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(float) * n);
    ::testPowIntConstant<start, start, end, 2>(expected_list, &math_result);
  }
  {
    float base = 0.0f;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<float> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(float) * n);
    ::testPowIntConstant<start, start, end, 3>(expected_list, &math_result);
  }
  {
    float base = 0.0f;
    zisc::BSerializer::read(&base, &reference_file);
    std::vector<float> expected_list;
    expected_list.resize(x_list.size());
    zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(float) * n);
    ::testPowIntConstant<start, start, end, 4>(expected_list, &math_result);
  }

  math_result.print();
  math_result.check("zisc::pow()");
}

//TEST(MathTest, ConstantPowFTest)
//{
//  constexpr int start = -256;
//  constexpr int end = 256;
//  ::PowerTest<start, end>::testPow();
//}

//TEST(ConstMathTest, PowTest)
//{
////  constexpr int start = -126;
////  constexpr int end = 127;
////  PowerTest<end, start>::testPow();
//  FAIL();
//}
//
//TEST(ConstMathTest, SqrtTestF)
//{
//  constexpr int start = 0;
//  constexpr int end = 1024;
//  PowerConstexprTest<float, end, start>::testSqrt();
//}
//
//TEST(ConstMathTest, CbrtTestF)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  PowerConstexprTest<float, end, start>::testCbrt();
//}
//
//TEST(ConstMathTest, ExpTestF)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  PowerConstexprTest<float, end, start>::testExp();
//}
//
//TEST(ConstMathTest, Log2TestF)
//{
//  constexpr int start = 0;
//  constexpr int end = 1024;
//  PowerConstexprTest<float, end, start>::testLog2();
//}
//
//TEST(ConstMathTest, LogTestF)
//{
//  constexpr int start = 0;
//  constexpr int end = 512;
//  PowerConstexprTest<float, end, start>::testLog();
//}
//
//TEST(ConstMathTest, Log10TestF)
//{
//  constexpr int start = 0;
//  constexpr int end = 512;
//  PowerConstexprTest<float, end, start>::testLog10();
//}
