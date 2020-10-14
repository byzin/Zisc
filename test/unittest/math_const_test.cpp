/*!
  \file math_const_test.cpp
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
#include <limits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/math/math.hpp"

//TEST(ConstMathTest, PrintConstantValues)
//{
//  using zisc::cast;
//  std::cout << "## float" << std::endl;
//  printFloat("float max", std::numeric_limits<float>::max());
//  printFloat("float min", std::numeric_limits<float>::min());
//  printFloat("float denormmax", std::numeric_limits<float>::min() - std::numeric_limits<float>::denorm_min());
//  printFloat("float denormmin", std::numeric_limits<float>::denorm_min());
//  printFloat("float epsilon", std::numeric_limits<float>::epsilon());
//  printFloat("float e", std::exp(1.0f));
//  printFloat("float zisc e", zisc::constant::exp(1.0f));
//  printFloat("float ld2f e", cast<float>(std::exp(1.0l)));
//  printFloat("float inv e", 1.0f / std::exp(1.0f));
//  printFloat("float zisc inv e", 1.0f / zisc::constant::exp(1.0f));
//  printFloat("float ld2f inv e", cast<float>(1.0l / std::exp(1.0l)));
//  printFloat("float log2E", std::log2(std::exp(1.0f)));
//  printFloat("float zisc log2E", zisc::constant::log2(zisc::constant::exp(1.0f)));
//  printFloat("float ld2f log2E", cast<float>(std::log2(std::exp(1.0l))));
//  printFloat("float inv log2E", 1.0f / std::log2(std::exp(1.0f)));
//  printFloat("float zisc inv log2E", 1.0f / zisc::constant::log2(zisc::constant::exp(1.0f)));
//  printFloat("float ld2f inv log2E", cast<float>(1.0l / std::log2(std::exp(1.0l))));
//  printFloat("float log10E", std::log10(std::exp(1.0f)));
//  printFloat("float zisc log10E", zisc::constant::log10(zisc::constant::exp(1.0f)));
//  printFloat("float ld2f log10E", cast<float>(std::log10(std::exp(1.0l))));
//  printFloat("float inv log10E", 1.0f / std::log10(std::exp(1.0f)));
//  printFloat("float zisc inv log10E", 1.0f / zisc::constant::log10(zisc::constant::exp(1.0f)));
//  printFloat("float ld2f inv log10E", cast<float>(1.0l / std::log10(std::exp(1.0l))));
//  printFloat("float logE2", std::log(2.0f));
//  printFloat("float zisc logE2", zisc::constant::log(2.0f));
//  printFloat("float ld2f logE2", cast<float>(std::log(2.0l)));
//  printFloat("float inv logE2", 1.0f / std::log(2.0f));
//  printFloat("float zisc inv logE2", 1.0f / zisc::constant::log(2.0f));
//  printFloat("float ld2f inv logE2", cast<float>(1.0l / std::log(2.0l)));
//  printFloat("float logE10", std::log(10.0f));
//  printFloat("float zisc logE10", zisc::constant::log(10.0f));
//  printFloat("float ld2f logE10", cast<float>(std::log(10.0l)));
//  printFloat("float inv logE10", 1.0f / std::log(10.0f));
//  printFloat("float zisc inv logE10", 1.0f / zisc::constant::log(10.0f));
//  printFloat("float ld2f inv logE10", cast<float>(1.0l / std::log(10.0l)));
//  printFloat("float pi", 4.0f * std::atan(1.0f));
//  printFloat("float zisc pi", zisc::constant::Math::pi<float>());
//  printFloat("float ld2f pi", cast<float>(4.0l * std::atan(1.0l)));
//  printFloat("float pi/2", 2.0f * std::atan(1.0f));
//  printFloat("float zisc pi/2", 0.5f * zisc::constant::Math::pi<float>());
//  printFloat("float ld2f pi/2", cast<float>(2.0l * std::atan(1.0l)));
//  printFloat("float pi/4", std::atan(1.0f));
//  printFloat("float zisc pi/4", 0.25f * zisc::constant::Math::pi<float>());
//  printFloat("float ld2f pi/4", cast<float>(std::atan(1.0l)));
//  printFloat("float 1/pi", 1.0f / (4.0f * std::atan(1.0f)));
//  printFloat("float zisc 1/pi", 1.0f / zisc::constant::Math::pi<float>());
//  printFloat("float ld2f 1/pi", cast<float>(1.0l / (4.0l * std::atan(1.0l))));
//  printFloat("float 2/pi", 1.0f / (2.0f * std::atan(1.0f)));
//  printFloat("float zisc 2/pi", 2.0f / zisc::constant::Math::pi<float>());
//  printFloat("float ld2f 2/pi", cast<float>(1.0l / (2.0l * std::atan(1.0l))));
//  printFloat("float 2/sqrt(pi)", 2.0f / std::sqrt((4.0f * std::atan(1.0f))));
//  printFloat("float zisc 2/sqrt(pi)", 2.0f / zisc::constant::sqrt(zisc::constant::Math::pi<float>()));
//  printFloat("float ld2f 2/sqrt(pi)", cast<float>(2.0l / std::sqrt((4.0l * std::atan(1.0l)))));
//  printFloat("float sqrt(2)", std::sqrt(2.0f));
//  printFloat("float zisc sqrt(2)", zisc::constant::sqrt(2.0f));
//  printFloat("float ld2f sqrt(2)", cast<float>(std::sqrt(2.0l)));
//  printFloat("float 1/sqrt(2)", 1.0f / std::sqrt(2.0f));
//  printFloat("float zisc 1/sqrt(2)", 1.0f / zisc::constant::sqrt(2.0f));
//  printFloat("float ld2f 1/sqrt(2)", cast<float>(1.0l / std::sqrt(2.0l)));
//  printFloat("float sqrt(3)", std::sqrt(3.0f));
//  printFloat("float zisc sqrt(3)", zisc::constant::sqrt(3.0f));
//  printFloat("float ld2f sqrt(3)", cast<float>(std::sqrt(3.0l)));
//  printFloat("float 1/sqrt(3)", 1.0f / std::sqrt(3.0f));
//  printFloat("float zisc 1/sqrt(3)", 1.0f / zisc::constant::sqrt(3.0f));
//  printFloat("float ld2f 1/sqrt(3)", cast<float>(1.0l / std::sqrt(3.0l)));
//
//  std::cout << "## double" << std::endl;
//  printFloat("double max", std::numeric_limits<double>::max());
//  printFloat("double min", std::numeric_limits<double>::min());
//  printFloat("double denormmax", std::numeric_limits<double>::min() - std::numeric_limits<double>::denorm_min());
//  printFloat("double denormmin", std::numeric_limits<double>::denorm_min());
//  printFloat("double epsilon", std::numeric_limits<double>::epsilon());
//  printFloat("double e", std::exp(1.0));
//  printFloat("double zisc e", zisc::constant::exp(1.0));
//  printFloat("double ld2f e", cast<double>(std::exp(1.0l)));
//  printFloat("double inv e", 1.0 / std::exp(1.0));
//  printFloat("double zisc inv e", 1.0 / zisc::constant::exp(1.0));
//  printFloat("double ld2f inv e", cast<double>(1.0l / std::exp(1.0l)));
//  printFloat("double log2E", std::log2(std::exp(1.0)));
//  printFloat("double zisc log2E", zisc::constant::log2(zisc::constant::exp(1.0)));
//  printFloat("double ld2f log2E", cast<double>(std::log2(std::exp(1.0l))));
//  printFloat("double inv log2E", 1.0 / std::log2(std::exp(1.0)));
//  printFloat("double zisc inv log2E", 1.0 / zisc::constant::log2(zisc::constant::exp(1.0)));
//  printFloat("double ld2f inv log2E", cast<double>(1.0l / std::log2(std::exp(1.0l))));
//  printFloat("double log10E", std::log10(std::exp(1.0)));
//  printFloat("double zisc log10E", zisc::constant::log10(zisc::constant::exp(1.0)));
//  printFloat("double ld2f log10E", cast<double>(std::log10(std::exp(1.0l))));
//  printFloat("double inv log10E", 1.0 / std::log10(std::exp(1.0)));
//  printFloat("double zisc inv log10E", 1.0 / zisc::constant::log10(zisc::constant::exp(1.0)));
//  printFloat("double ld2f inv log10E", cast<double>(1.0l / std::log10(std::exp(1.0l))));
//  printFloat("double logE2", std::log(2.0));
//  printFloat("double zisc logE2", zisc::constant::log(2.0));
//  printFloat("double ld2f logE2", cast<double>(std::log(2.0l)));
//  printFloat("double inv logE2", 1.0 / std::log(2.0));
//  printFloat("double zisc inv logE2", 1.0 / zisc::constant::log(2.0));
//  printFloat("double ld2f inv logE2", cast<double>(1.0l / std::log(2.0l)));
//  printFloat("double logE10", std::log(10.0));
//  printFloat("double zisc logE10", zisc::constant::log(10.0));
//  printFloat("double ld2f logE10", cast<double>(std::log(10.0l)));
//  printFloat("double inv logE10", 1.0 / std::log(10.0));
//  printFloat("double zisc inv logE10", 1.0 / zisc::constant::log(10.0));
//  printFloat("double ld2f inv logE10", cast<double>(1.0l / std::log(10.0l)));
//  printFloat("double pi", 4.0 * std::atan(1.0));
//  printFloat("double zisc pi", zisc::constant::Math::pi<double>());
//  printFloat("double ld2f pi", cast<double>(4.0l * std::atan(1.0l)));
//  printFloat("double pi/2", 2.0 * std::atan(1.0));
//  printFloat("double zisc pi/2", 0.5 * zisc::constant::Math::pi<double>());
//  printFloat("double ld2f pi/2", cast<double>(2.0l * std::atan(1.0l)));
//  printFloat("double pi/4", std::atan(1.0));
//  printFloat("double zisc pi/4", 0.25 * zisc::constant::Math::pi<double>());
//  printFloat("double ld2f pi/4", cast<double>(std::atan(1.0l)));
//  printFloat("double 1/pi", 1.0 / (4.0 * std::atan(1.0)));
//  printFloat("double zisc 1/pi", 1.0 / zisc::constant::Math::pi<double>());
//  printFloat("double ld2f 1/pi", cast<double>(1.0l / (4.0l * std::atan(1.0l))));
//  printFloat("double 2/pi", 1.0 / (2.0 * std::atan(1.0)));
//  printFloat("double zisc 2/pi", 2.0 / zisc::constant::Math::pi<double>());
//  printFloat("double ld2f 2/pi", cast<double>(1.0l / (2.0l * std::atan(1.0l))));
//  printFloat("double 2/sqrt(pi)", 2.0 / std::sqrt((4.0 * std::atan(1.0))));
//  printFloat("double zisc 2/sqrt(pi)", 2.0 / zisc::constant::sqrt(zisc::constant::Math::pi<double>()));
//  printFloat("double ld2f 2/sqrt(pi)", cast<double>(2.0l / std::sqrt((4.0l * std::atan(1.0l)))));
//  printFloat("double sqrt(2)", std::sqrt(2.0));
//  printFloat("double zisc sqrt(2)", zisc::constant::sqrt(2.0));
//  printFloat("double ld2f sqrt(2)", cast<double>(std::sqrt(2.0l)));
//  printFloat("double 1/sqrt(2)", 1.0 / std::sqrt(2.0));
//  printFloat("double zisc 1/sqrt(2)", 1.0 / zisc::constant::sqrt(2.0));
//  printFloat("double ld2f 1/sqrt(2)", cast<double>(1.0l / std::sqrt(2.0l)));
//  printFloat("double sqrt(3)", std::sqrt(3.0));
//  printFloat("double zisc sqrt(3)", zisc::constant::sqrt(3.0));
//  printFloat("double ld2f sqrt(3)", cast<double>(std::sqrt(3.0l)));
//  printFloat("double 1/sqrt(3)", 1.0 / std::sqrt(3.0));
//  printFloat("double zisc 1/sqrt(3)", 1.0 / zisc::constant::sqrt(3.0));
//  printFloat("double ld2f 1/sqrt(3)", cast<double>(1.0l / std::sqrt(3.0l)));
//}

TEST(ConstMathTest, PiTest)
{
  // Float
  {
    constexpr float pi = zisc::kPi<float>;
    EXPECT_FLOAT_EQ(3.141592741f, pi);
  }
  // Double
  {
    constexpr double pi = zisc::kPi<double>;
    EXPECT_DOUBLE_EQ(3.14159265358979312, pi);
  }
  // Long double
  {
    constexpr long double pi = zisc::kPi<long double>;
    EXPECT_EQ(3.141592653589793238513l, pi);
  }
}

//TEST(ConstMathTest, AbsTest)
//{
//  {
//    constexpr int value = zisc::abs(3);
//    EXPECT_EQ(3, value) << "The result of abs(3) is wrong.";
//  }
//  {
//    constexpr int value = zisc::abs(-1);
//    EXPECT_EQ(1, value) << "The result of abs(-1) is wrong.";
//  }
//  {
//    constexpr double value = zisc::abs(-2.5);
//    EXPECT_DOUBLE_EQ(2.5, value) << "The result of abs(-2.5) is wrong.";
//  }
//}
//
//TEST(ConstMathTest, GcdTest)
//{
//  {
//    constexpr int result = zisc::constant::gcd(54, 24);
//    EXPECT_EQ(6, result) << "The result of gcd(54, 24) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::gcd(42, 56);
//    EXPECT_EQ(14, result) << "The result of gcd(42, 56) is wrong.";
//  }
//}
//
//TEST(ConstMathTest, FactorialTest)
//{
//  {
//    constexpr int result = zisc::constant::factorial(0);
//    EXPECT_EQ(1, result) << "The result of factorial(0) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::factorial(1);
//    EXPECT_EQ(1, result) << "The result of factorial(1) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::factorial(5);
//    EXPECT_EQ(120, result) << "The result of factorial(5) is wrong.";
//  }
//}
//
//TEST(ConstMathTest, ModTest)
//{
//  {
//    constexpr int result = zisc::constant::mod<1>(10);
//    EXPECT_EQ(0, result) << "The result of mod<1>(10) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<2>(10);
//    EXPECT_EQ(0, result) << "The result of mod<2>(10) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<2>(9);
//    EXPECT_EQ(1, result) << "The result of mod<2>(9) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<3>(10);
//    EXPECT_EQ(1, result) << "The result of mod<3>(10) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<3>(9);
//    EXPECT_EQ(0, result) << "The result of mod<3>(9) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<4>(12);
//    EXPECT_EQ(0, result) << "The result of mod<4>(12) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<4>(15);
//    EXPECT_EQ(3, result) << "The result of mod<4>(15) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<8>(5);
//    EXPECT_EQ(5, result) << "The result of mod<8>(5) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<8>(8);
//    EXPECT_EQ(0, result) << "The result of mod<8>(8) is wrong.";
//  }
//  {
//    constexpr int result = zisc::constant::mod<8>(13);
//    EXPECT_EQ(5, result) << "The result of mod<8>(13) is wrong.";
//  }
//}
//
//TEST(ConstMathTest, FrLdTestF)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  ConstexprTest<float, end, start>::testFrLdexp();
//}
//
//TEST(ConstMathTest, FrLdTestD)
//{
//  constexpr int start = -512;
//  constexpr int end = 512;
//  ConstexprTest<double, end, start>::testFrLdexp();
//}
