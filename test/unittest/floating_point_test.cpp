/*!
  \file floating_point_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
#include <cstddef>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/floating_point.hpp"
#include "zisc/zisc_config.hpp"

TEST(FloatingPointTest, BitMaskTest)
{
  // half
  {
    ASSERT_EQ(2, sizeof(zisc::HalfFloat));
    constexpr std::size_t significand_size = zisc::HalfFloat::significandBitSize();
    ASSERT_EQ(10, significand_size);
    constexpr auto significand_mask = zisc::HalfFloat::significandBitMask();
    ASSERT_EQ(0b0000'0011'1111'1111u, significand_mask);
    constexpr std::size_t exponent_size = zisc::HalfFloat::exponentBitSize();
    ASSERT_EQ(5, exponent_size);
    constexpr auto exponent_mask = zisc::HalfFloat::exponentBitMask();
    ASSERT_EQ(0b0111'1100'0000'0000u, exponent_mask);
    constexpr auto sign_mask = zisc::HalfFloat::signBitMask();
    ASSERT_EQ(0b1000'0000'0000'0000u, sign_mask);
    ASSERT_EQ(16, significand_size + exponent_size + 1);
  }
  // float
  {
    ASSERT_EQ(4, sizeof(zisc::SingleFloat));
    constexpr std::size_t significand_size = zisc::SingleFloat::significandBitSize();
    ASSERT_EQ(23, significand_size);
    constexpr auto significand_mask = zisc::SingleFloat::significandBitMask();
    ASSERT_EQ(0b0000'0000'0111'1111'1111'1111'1111'1111u, significand_mask);
    constexpr std::size_t exponent_size = zisc::SingleFloat::exponentBitSize();
    ASSERT_EQ(8, exponent_size);
    constexpr auto exponent_mask = zisc::SingleFloat::exponentBitMask();
    ASSERT_EQ(0b0111'1111'1000'0000'0000'0000'0000'0000u, exponent_mask);
    constexpr auto sign_mask = zisc::SingleFloat::signBitMask();
    ASSERT_EQ(0b1000'0000'0000'0000'0000'0000'0000'0000u, sign_mask);
    ASSERT_EQ(32, significand_size + exponent_size + 1);
  }
  // double
  {
    ASSERT_EQ(8, sizeof(zisc::DoubleFloat));
    constexpr std::size_t significand_size = zisc::DoubleFloat::significandBitSize();
    ASSERT_EQ(52, significand_size);
    constexpr auto significand_mask = zisc::DoubleFloat::significandBitMask();
    ASSERT_EQ(0b0000'0000'0000'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111ull, significand_mask);
    constexpr std::size_t exponent_size = zisc::DoubleFloat::exponentBitSize();
    ASSERT_EQ(11, exponent_size);
    constexpr auto exponent_mask = zisc::DoubleFloat::exponentBitMask();
    ASSERT_EQ(0b0111'1111'1111'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000ull, exponent_mask);
    constexpr auto sign_mask = zisc::DoubleFloat::signBitMask();
    ASSERT_EQ(0b1000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000ull, sign_mask);
    ASSERT_EQ(64, significand_size + exponent_size + 1);
  }
}
