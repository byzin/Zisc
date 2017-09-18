/*!
  \file floating_point_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <bitset>
#include <iomanip>
#include <iostream>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/floating_point.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

void printFloatBit(const double value);

void printFloatBit(const double value)
{
  const auto v = zisc::treatAs<const zisc::uint64*>(&value);
  std::bitset<64> float_bit{*v};
  std::cout << std::scientific << value << ": ";
  std::cout << float_bit << std::endl;
}

TEST(FloatingPointTest, getFloatMantissaBitTest)
{
  constexpr float value = 1.2345f;
  /* const auto v = */ zisc::FloatBit::getMantissaBits(value);
//  printFloatBit(value);
//  printFloatBit(*zisc::treatAs<const float*>(&v));
}

TEST(FloatingPointTest, getDoubleMantissaBitTest)
{
  constexpr double value = 1.2345;
  /* const auto v = */ zisc::DoubleBit::getMantissaBits(value);
//  printFloatBit(value);
//  printFloatBit(*zisc::treatAs<const double*>(&v));
}

TEST(FloatingPointTest, getFloatExponentBitTest)
{
  using zisc::cast;
  using zisc::FloatBit;
  {
    constexpr float value = 1.2345f;
    constexpr FloatBit::BitType reference = 0b0111'1111u << 23;
    ASSERT_EQ(reference, FloatBit::getExponentBits(value));
  }
}

TEST(FloatingPointTest, getDoubleExponentBitTest)
{
  using zisc::cast;
  using zisc::DoubleBit;
  {
    constexpr double value = 1.2345;
    constexpr DoubleBit::BitType reference = 0b0011'1111'1111ull << 52;
    ASSERT_EQ(reference, DoubleBit::getExponentBits(value));
  }
}

TEST(FloatingPointTest, getFloatSignBitTest)
{
  using zisc::FloatBit;
  constexpr float value1 = 1.0f;
  constexpr float value2 = -1.0f;
  ASSERT_EQ(0, FloatBit::getSignBit(value1));
  ASSERT_NE(0, FloatBit::getSignBit(value2));
}

TEST(FloatingPointTest, getDoubleSignBitTest)
{
  using zisc::DoubleBit;
  constexpr double value1 = 1.0;
  constexpr double value2 = -1.0;
  ASSERT_EQ(0, DoubleBit::getSignBit(value1));
  ASSERT_NE(0, DoubleBit::getSignBit(value2));
}

TEST(FloatingPointTest, getHalfFloatExponentBitTest)
{
  auto half_float_exponent = [](const double value, const zisc::int64 expected)
  {
    using zisc::DoubleBit;
    auto exponent = DoubleBit::getExponentBits(value);
    exponent = DoubleBit::halfExponentBits(exponent);
    const zisc::int64 actual = zisc::cast<zisc::int64>(exponent >> 52) - 1023;
    ASSERT_EQ(expected, actual) << "Float: " << value << " half exponent test failed.";
  };
  half_float_exponent(0.0625, -2);
  half_float_exponent(0.125, -1);
  half_float_exponent(0.25, -1);
  half_float_exponent(0.5, 0);
  half_float_exponent(1.0, 0);
  half_float_exponent(2.0, 0);
  half_float_exponent(4.0, 1);
  half_float_exponent(8.0, 1);
  half_float_exponent(16.0, 2);
  half_float_exponent(32.0, 2);
  half_float_exponent(64.0, 3);
//  half_float_exponent(0.0, 0);
}

TEST(FloatingPointTest, isPositiveExponentFloatTest)
{
  using zisc::FloatBit;
  ASSERT_FALSE(FloatBit::isPositiveExponent(0.0625f));
  ASSERT_FALSE(FloatBit::isPositiveExponent(0.125f));
  ASSERT_FALSE(FloatBit::isPositiveExponent(0.25f));
  ASSERT_FALSE(FloatBit::isPositiveExponent(0.5f));
  ASSERT_FALSE(FloatBit::isPositiveExponent(1.0f));
  ASSERT_TRUE(FloatBit::isPositiveExponent(2.0f));
  ASSERT_TRUE(FloatBit::isPositiveExponent(4.0f));
  ASSERT_TRUE(FloatBit::isPositiveExponent(8.0f));
  ASSERT_TRUE(FloatBit::isPositiveExponent(16.0f));
  ASSERT_TRUE(FloatBit::isPositiveExponent(32.0f));
  ASSERT_TRUE(FloatBit::isPositiveExponent(64.0f));
}

TEST(FloatingPointTest, isPositiveExponentDoubleTest)
{
  using zisc::DoubleBit;
  ASSERT_FALSE(DoubleBit::isPositiveExponent(0.0625));
  ASSERT_FALSE(DoubleBit::isPositiveExponent(0.125));
  ASSERT_FALSE(DoubleBit::isPositiveExponent(0.25));
  ASSERT_FALSE(DoubleBit::isPositiveExponent(0.5));
  ASSERT_FALSE(DoubleBit::isPositiveExponent(1.0));
  ASSERT_TRUE(DoubleBit::isPositiveExponent(2.0));
  ASSERT_TRUE(DoubleBit::isPositiveExponent(4.0));
  ASSERT_TRUE(DoubleBit::isPositiveExponent(8.0));
  ASSERT_TRUE(DoubleBit::isPositiveExponent(16.0));
  ASSERT_TRUE(DoubleBit::isPositiveExponent(32.0));
  ASSERT_TRUE(DoubleBit::isPositiveExponent(64.0));
}

TEST(FloatingPointTest, isOddExponentFloatTest)
{
  using zisc::FloatBit;
  ASSERT_FALSE(FloatBit::isOddExponent(0.0625f));
  ASSERT_TRUE(FloatBit::isOddExponent(0.125f));
  ASSERT_FALSE(FloatBit::isOddExponent(0.25f));
  ASSERT_TRUE(FloatBit::isOddExponent(0.5f));
  ASSERT_FALSE(FloatBit::isOddExponent(1.0f));
  ASSERT_TRUE(FloatBit::isOddExponent(2.0f));
  ASSERT_FALSE(FloatBit::isOddExponent(4.0f));
  ASSERT_TRUE(FloatBit::isOddExponent(8.0f));
  ASSERT_FALSE(FloatBit::isOddExponent(16.0f));
  ASSERT_TRUE(FloatBit::isOddExponent(32.0f));
  ASSERT_FALSE(FloatBit::isOddExponent(64.0f));
}

TEST(FloatingPointTest, isOddExponentDoubleTest)
{
  using zisc::DoubleBit;
  ASSERT_FALSE(DoubleBit::isOddExponent(0.0625));
  ASSERT_TRUE(DoubleBit::isOddExponent(0.125));
  ASSERT_FALSE(DoubleBit::isOddExponent(0.25));
  ASSERT_TRUE(DoubleBit::isOddExponent(0.5));
  ASSERT_FALSE(DoubleBit::isOddExponent(1.0));
  ASSERT_TRUE(DoubleBit::isOddExponent(2.0));
  ASSERT_FALSE(DoubleBit::isOddExponent(4.0));
  ASSERT_TRUE(DoubleBit::isOddExponent(8.0));
  ASSERT_FALSE(DoubleBit::isOddExponent(16.0));
  ASSERT_TRUE(DoubleBit::isOddExponent(32.0));
  ASSERT_FALSE(DoubleBit::isOddExponent(64.0));
}

TEST(FloatingPointTest, makeFloatTest)
{
  using zisc::FloatBit;
  {
    constexpr float value = 1.0f;
    const auto e = FloatBit::getExponentBits(value);
    const auto m = FloatBit::getMantissaBits(value);
    const auto v = FloatBit::makeFloat(m, e);
    ASSERT_FLOAT_EQ(value, v);
  }
}

TEST(FloatingPointTest, makeDoubleTest)
{
  using zisc::DoubleBit;
  {
    constexpr double value = 1.0;
    const auto e = DoubleBit::getExponentBits(value);
    const auto m = DoubleBit::getMantissaBits(value);
    const auto v = DoubleBit::makeFloat(m, e);
    ASSERT_DOUBLE_EQ(value, v);
  }
}
