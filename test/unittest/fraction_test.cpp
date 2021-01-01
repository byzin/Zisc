/*!
  \file fraction_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/math/fraction.hpp"

TEST(FractionTest, ConstructorTest)
{
  {
    zisc::Fraction32 fraction;
    EXPECT_EQ(0, fraction.numer());
    EXPECT_EQ(1, fraction.denom());
  }
  {
    zisc::Fraction32 fraction{1};
    EXPECT_EQ(1, fraction.numer());
    EXPECT_EQ(1, fraction.denom());
  }
  {
    zisc::Fraction32 fraction{3, 2};
    EXPECT_EQ(3, fraction.numer());
    EXPECT_EQ(2, fraction.denom());
  }
  {
    zisc::Fraction32 fraction{24, 8};
    EXPECT_EQ(3, fraction.numer());
    EXPECT_EQ(1, fraction.denom());
  }
  {
    zisc::Fraction32 fraction{6, -4};
    EXPECT_EQ(-3, fraction.numer());
    EXPECT_EQ(2, fraction.denom());
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
    EXPECT_EQ(13, result.numer());
    EXPECT_EQ(6, result.denom());
  }
  {
    constexpr auto result = fraction4 + fraction5;
    constexpr auto n = result.numer();
    constexpr auto d = result.denom();
    EXPECT_EQ(23, n);
    EXPECT_EQ(60, d);
  }
  {
    auto result = fraction1 - fraction2;
    EXPECT_EQ(5, result.numer());
    EXPECT_EQ(6, result.denom());
  }
  {
    constexpr auto result = fraction4 - fraction5;
    EXPECT_EQ(-1, result.numer());
    EXPECT_EQ(12, result.denom());
  }
  {
    auto result = fraction1 - fraction3;
    EXPECT_EQ(13, result.numer());
    EXPECT_EQ(6, result.denom());
  }
  {
    auto result = fraction1 * fraction2;
    EXPECT_EQ(1, result.numer());
    EXPECT_EQ(1, result.denom());
  }
  {
    constexpr auto result = fraction4 * fraction5;
    EXPECT_EQ(7, result.numer());
    EXPECT_EQ(200, result.denom());
  }
  {
    auto result = fraction1 / fraction2;
    EXPECT_EQ(9, result.numer());
    EXPECT_EQ(4, result.denom());
  }
  {
    constexpr auto result = fraction4 / fraction5;
    EXPECT_EQ(9, result.numer());
    EXPECT_EQ(14, result.denom());
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
    constexpr zisc::Fraction32 result{3};
    EXPECT_EQ(3, result.numer());
    EXPECT_EQ(1, result.denom());
  }
  {
    constexpr zisc::Fraction32 result{3, 2};
    EXPECT_TRUE(fraction1 == result);
  }
}
