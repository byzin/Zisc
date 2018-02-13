/*!
  \file floating_point_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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
#include "zisc/floating_point_bit.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

void printFloatBit(const double value)
{
  const auto v = zisc::treatAs<const zisc::uint64*>(&value);
  std::bitset<64> float_bit{*v};
  std::cout << std::scientific << value << ": ";
  std::cout << float_bit << std::endl;
}

} // namespace 

TEST(FloatingPointBitTest, getFloatMantissaBitTest)
{
  constexpr float value = 1.2345f;
  /* const auto v = */ zisc::FloatBit::getMantissaBits(value);
//  ::printFloatBit(value);
//  ::printFloatBit(*zisc::treatAs<const float*>(&v));
}

TEST(FloatingPointBitTest, getDoubleMantissaBitTest)
{
  constexpr double value = 1.2345;
  /* const auto v = */ zisc::DoubleBit::getMantissaBits(value);
//  ::printFloatBit(value);
//  ::printFloatBit(*zisc::treatAs<const double*>(&v));
}

TEST(FloatingPointBitTest, getFloatExponentBitTest)
{
  using zisc::cast;
  using zisc::FloatBit;
  {
    constexpr float value = 1.2345f;
    constexpr FloatBit::BitType reference = 0b0111'1111u << 23;
    EXPECT_EQ(reference, FloatBit::getExponentBits(value));
  }
}

TEST(FloatingPointBitTest, getDoubleExponentBitTest)
{
  using zisc::cast;
  using zisc::DoubleBit;
  {
    constexpr double value = 1.2345;
    constexpr DoubleBit::BitType reference = 0b0011'1111'1111ull << 52;
    EXPECT_EQ(reference, DoubleBit::getExponentBits(value));
  }
}

TEST(FloatingPointBitTest, getFloatSignBitTest)
{
  using zisc::FloatBit;
  constexpr float value1 = 1.0f;
  constexpr float value2 = -1.0f;
  EXPECT_EQ(0, FloatBit::getSignBit(value1));
  EXPECT_NE(0, FloatBit::getSignBit(value2));
}

TEST(FloatingPointBitTest, getDoubleSignBitTest)
{
  using zisc::DoubleBit;
  constexpr double value1 = 1.0;
  constexpr double value2 = -1.0;
  EXPECT_EQ(0, DoubleBit::getSignBit(value1));
  EXPECT_NE(0, DoubleBit::getSignBit(value2));
}

TEST(FloatingPointBitTest, getHalfFloatExponentBitTest)
{
  auto half_float_exponent = [](const double value, const zisc::int64 expected)
  {
    using zisc::DoubleBit;
    auto exponent = DoubleBit::getExponentBits(value);
    exponent = DoubleBit::halfExponentBits(exponent);
    const zisc::int64 actual = zisc::cast<zisc::int64>(exponent >> 52) - 1023;
    EXPECT_EQ(expected, actual) << "Float: " << value << " half exponent test failed.";
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

TEST(FloatingPointBitTest, isPositiveExponentFloatTest)
{
  using zisc::FloatBit;
  EXPECT_FALSE(FloatBit::isPositiveExponent(0.0625f));
  EXPECT_FALSE(FloatBit::isPositiveExponent(0.125f));
  EXPECT_FALSE(FloatBit::isPositiveExponent(0.25f));
  EXPECT_FALSE(FloatBit::isPositiveExponent(0.5f));
  EXPECT_FALSE(FloatBit::isPositiveExponent(1.0f));
  EXPECT_TRUE(FloatBit::isPositiveExponent(2.0f));
  EXPECT_TRUE(FloatBit::isPositiveExponent(4.0f));
  EXPECT_TRUE(FloatBit::isPositiveExponent(8.0f));
  EXPECT_TRUE(FloatBit::isPositiveExponent(16.0f));
  EXPECT_TRUE(FloatBit::isPositiveExponent(32.0f));
  EXPECT_TRUE(FloatBit::isPositiveExponent(64.0f));
}

TEST(FloatingPointBitTest, isPositiveExponentDoubleTest)
{
  using zisc::DoubleBit;
  EXPECT_FALSE(DoubleBit::isPositiveExponent(0.0625));
  EXPECT_FALSE(DoubleBit::isPositiveExponent(0.125));
  EXPECT_FALSE(DoubleBit::isPositiveExponent(0.25));
  EXPECT_FALSE(DoubleBit::isPositiveExponent(0.5));
  EXPECT_FALSE(DoubleBit::isPositiveExponent(1.0));
  EXPECT_TRUE(DoubleBit::isPositiveExponent(2.0));
  EXPECT_TRUE(DoubleBit::isPositiveExponent(4.0));
  EXPECT_TRUE(DoubleBit::isPositiveExponent(8.0));
  EXPECT_TRUE(DoubleBit::isPositiveExponent(16.0));
  EXPECT_TRUE(DoubleBit::isPositiveExponent(32.0));
  EXPECT_TRUE(DoubleBit::isPositiveExponent(64.0));
}

TEST(FloatingPointBitTest, isOddExponentFloatTest)
{
  using zisc::FloatBit;
  EXPECT_FALSE(FloatBit::isOddExponent(0.0625f));
  EXPECT_TRUE(FloatBit::isOddExponent(0.125f));
  EXPECT_FALSE(FloatBit::isOddExponent(0.25f));
  EXPECT_TRUE(FloatBit::isOddExponent(0.5f));
  EXPECT_FALSE(FloatBit::isOddExponent(1.0f));
  EXPECT_TRUE(FloatBit::isOddExponent(2.0f));
  EXPECT_FALSE(FloatBit::isOddExponent(4.0f));
  EXPECT_TRUE(FloatBit::isOddExponent(8.0f));
  EXPECT_FALSE(FloatBit::isOddExponent(16.0f));
  EXPECT_TRUE(FloatBit::isOddExponent(32.0f));
  EXPECT_FALSE(FloatBit::isOddExponent(64.0f));
}

TEST(FloatingPointBitTest, isOddExponentDoubleTest)
{
  using zisc::DoubleBit;
  EXPECT_FALSE(DoubleBit::isOddExponent(0.0625));
  EXPECT_TRUE(DoubleBit::isOddExponent(0.125));
  EXPECT_FALSE(DoubleBit::isOddExponent(0.25));
  EXPECT_TRUE(DoubleBit::isOddExponent(0.5));
  EXPECT_FALSE(DoubleBit::isOddExponent(1.0));
  EXPECT_TRUE(DoubleBit::isOddExponent(2.0));
  EXPECT_FALSE(DoubleBit::isOddExponent(4.0));
  EXPECT_TRUE(DoubleBit::isOddExponent(8.0));
  EXPECT_FALSE(DoubleBit::isOddExponent(16.0));
  EXPECT_TRUE(DoubleBit::isOddExponent(32.0));
  EXPECT_FALSE(DoubleBit::isOddExponent(64.0));
}

TEST(FloatingPointBitTest, makeFloatTest)
{
  using zisc::FloatBit;
  {
    constexpr float value = 1.0f;
    const auto e = FloatBit::getExponentBits(value);
    const auto m = FloatBit::getMantissaBits(value);
    const auto v = FloatBit::makeFloat(m, e);
    EXPECT_FLOAT_EQ(value, v);
  }
}

TEST(FloatingPointBitTest, makeDoubleTest)
{
  using zisc::DoubleBit;
  {
    constexpr double value = 1.0;
    const auto e = DoubleBit::getExponentBits(value);
    const auto m = DoubleBit::getMantissaBits(value);
    const auto v = DoubleBit::makeFloat(m, e);
    EXPECT_DOUBLE_EQ(value, v);
  }
}

TEST(FloatingPointBitTest, FloatMapTest)
{
  using zisc::FloatBit;
  {
    constexpr zisc::uint32 x = 0;
    constexpr float result = FloatBit::mapTo01Float(x);
    EXPECT_EQ(0.0f, result);
  }
  {
    constexpr zisc::uint32 x = std::numeric_limits<zisc::uint32>::max() / 2;
    constexpr float result = FloatBit::mapTo01Float(x);
    EXPECT_FLOAT_EQ(0.5f, result);
  }
  {
    constexpr zisc::uint32 x = std::numeric_limits<zisc::uint32>::max();
    constexpr float result = FloatBit::mapTo01Float(x);
    EXPECT_FLOAT_EQ(1.0f, result);
    EXPECT_GT(1.0f, result);
  }
}

TEST(FloatingPointBitTest, DoubleMapTest)
{
  using zisc::DoubleBit;
  {
    constexpr zisc::uint64 x = 0;
    constexpr double result = DoubleBit::mapTo01Float(x);
    EXPECT_EQ(0.0, result);
  }
  {
    constexpr zisc::uint64 x = std::numeric_limits<zisc::uint64>::max() / 2;
    constexpr double result = DoubleBit::mapTo01Float(x);
    EXPECT_DOUBLE_EQ(0.5, result);
  }
  {
    constexpr zisc::uint64 x = std::numeric_limits<zisc::uint64>::max();
    constexpr double result = DoubleBit::mapTo01Float(x);
    EXPECT_DOUBLE_EQ(1.0, result);
    EXPECT_GT(1.0, result);
  }
}
