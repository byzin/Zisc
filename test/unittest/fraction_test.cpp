/*!
  \file fraction_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/fraction.hpp"

TEST(FractionTest, ConstructorTest)
{
  {
    zisc::Fraction32 fraction;
    EXPECT_EQ(0, fraction.numerator());
    EXPECT_EQ(1, fraction.denominator());
  }
  {
    zisc::Fraction32 fraction{1};
    EXPECT_EQ(1, fraction.numerator());
    EXPECT_EQ(1, fraction.denominator());
  }
  {
    zisc::Fraction32 fraction{3, 2};
    EXPECT_EQ(3, fraction.numerator());
    EXPECT_EQ(2, fraction.denominator());
  }
  {
    zisc::Fraction32 fraction{24, 8};
    EXPECT_EQ(3, fraction.numerator());
    EXPECT_EQ(1, fraction.denominator());
  }
  {
    zisc::Fraction32 fraction{6, -4};
    EXPECT_EQ(-3, fraction.numerator());
    EXPECT_EQ(2, fraction.denominator());
  }
}

TEST(FractionTest, OperatorTest)
{
  const zisc::Fraction32 fraction1{3, 2},
                         fraction2{2, 3},
                         fraction3{-2, 3};
  constexpr zisc::Fraction32 fraction4{3, 2 * 2 * 5},
                             fraction5{7, 2 * 3 * 5};
  {
    auto result = fraction1 + fraction2;
    EXPECT_EQ(13, result.numerator());
    EXPECT_EQ(6, result.denominator());
  }
  {
    constexpr auto result = fraction4 + fraction5;
    constexpr auto n = result.numerator();
    constexpr auto d = result.denominator();
    EXPECT_EQ(23, n);
    EXPECT_EQ(60, d);
  }
  {
    auto result = fraction1 - fraction2;
    EXPECT_EQ(5, result.numerator());
    EXPECT_EQ(6, result.denominator());
  }
  {
    constexpr auto result = fraction4 - fraction5;
    EXPECT_EQ(-1, result.numerator());
    EXPECT_EQ(12, result.denominator());
  }
  {
    auto result = fraction1 - fraction3;
    EXPECT_EQ(13, result.numerator());
    EXPECT_EQ(6, result.denominator());
  }
  {
    auto result = fraction1 * fraction2;
    EXPECT_EQ(1, result.numerator());
    EXPECT_EQ(1, result.denominator());
  }
  {
    constexpr auto result = fraction4 * fraction5;
    EXPECT_EQ(7, result.numerator());
    EXPECT_EQ(200, result.denominator());
  }
  {
    auto result = fraction1 / fraction2;
    EXPECT_EQ(9, result.numerator());
    EXPECT_EQ(4, result.denominator());
  }
  {
    constexpr auto result = fraction4 / fraction5;
    EXPECT_EQ(9, result.numerator());
    EXPECT_EQ(14, result.denominator());
  }
  {
    EXPECT_TRUE(fraction1 == fraction1);
    EXPECT_TRUE(fraction1 <= fraction1);
    EXPECT_TRUE(fraction1 >= fraction1);
    EXPECT_FALSE(fraction1 != fraction1);

    EXPECT_FALSE(fraction1 == fraction2);
    EXPECT_TRUE(fraction1 != fraction2);

    EXPECT_TRUE(fraction2 < fraction1);
    EXPECT_TRUE(fraction2 <= fraction1);
    EXPECT_FALSE(fraction1 < fraction2);
    EXPECT_FALSE(fraction1 <= fraction2);

    EXPECT_TRUE(fraction2 > fraction3);
    EXPECT_TRUE(fraction2 >= fraction3);
    EXPECT_FALSE(fraction3 > fraction2);
    EXPECT_FALSE(fraction3 >= fraction2);
  }
  {
    EXPECT_TRUE(fraction2 == (-fraction3));
  }
  // constexpr test
  {
    auto make_fraction = [](const std::int32_t n, const std::int32_t d)
    {
      zisc::Fraction32 fraction{1, 1};
      fraction.numerator() = n;
      fraction.denominator() = d;
      return fraction;
    };
    constexpr auto result = make_fraction(3, 2);
    EXPECT_TRUE(fraction1 == result);
  }
  {
    auto make_fraction = [](const std::int32_t n, const std::int32_t d)
    {
      zisc::Fraction32 fraction{0};
      fraction += zisc::Fraction32{n, d};
      return fraction;
    };
    constexpr auto result = make_fraction(3, 2);
    EXPECT_TRUE(fraction1 == result);
  }
  {
    auto make_fraction = [](const std::int32_t n, const std::int32_t d)
    {
      zisc::Fraction32 fraction{0};
      fraction = zisc::Fraction32{n, d};
      return fraction;
    };
    constexpr auto result = make_fraction(3, 2);
    EXPECT_TRUE(fraction1 == result);
  }
  {
    auto make_fraction = [](const std::int32_t n)
    {
      zisc::Fraction32 fraction{0};
      fraction = n;
      return fraction;
    };
    constexpr auto result = make_fraction(3);
    EXPECT_EQ(3, result.numerator());
    EXPECT_EQ(1, result.denominator());
  }
}
