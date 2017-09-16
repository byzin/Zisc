/*!
  \file fraction_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/fraction.hpp"

TEST(FractionTest, ConstructorTest)
{
  {
    zisc::Fraction32 fraction;
    ASSERT_EQ(0, fraction.numerator());
    ASSERT_EQ(1, fraction.denominator());
  }
  {
    zisc::Fraction32 fraction{1};
    ASSERT_EQ(1, fraction.numerator());
    ASSERT_EQ(1, fraction.denominator());
  }
  {
    zisc::Fraction32 fraction{3, 2};
    ASSERT_EQ(3, fraction.numerator());
    ASSERT_EQ(2, fraction.denominator());
  }
  {
    zisc::Fraction32 fraction{24, 8};
    ASSERT_EQ(3, fraction.numerator());
    ASSERT_EQ(1, fraction.denominator());
  }
  {
    zisc::Fraction32 fraction{6, -4};
    ASSERT_EQ(-3, fraction.numerator());
    ASSERT_EQ(2, fraction.denominator());
  }
}

TEST(FractionTest, OperatorTest)
{
  zisc::Fraction32 fraction1{3, 2},
                   fraction2{2, 3},
                   fraction3{-2, 3};
  {
    auto result = fraction1 + fraction2;
    ASSERT_EQ(13, result.numerator());
    ASSERT_EQ(6, result.denominator());
  }
  {
    auto result = fraction1 - fraction2;
    ASSERT_EQ(5, result.numerator());
    ASSERT_EQ(6, result.denominator());
  }
  {
    auto result = fraction1 - fraction3;
    ASSERT_EQ(13, result.numerator());
    ASSERT_EQ(6, result.denominator());
  }
  {
    auto result = fraction1 * fraction2;
    ASSERT_EQ(1, result.numerator());
    ASSERT_EQ(1, result.denominator());
  }
  {
    auto result = fraction1 / fraction2;
    ASSERT_EQ(9, result.numerator());
    ASSERT_EQ(4, result.denominator());
  }
  {
    ASSERT_TRUE(fraction1 == fraction1);
    ASSERT_FALSE(fraction1 == fraction2);
    ASSERT_TRUE(fraction2 < fraction1);
    ASSERT_TRUE(fraction3 < fraction2);
  }
}
