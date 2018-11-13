/*!
  \file floating_point_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <limits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/floating_point.hpp"
#include "zisc/math.hpp"
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

template <typename Float>
struct ValueTest
{
  using FloatType = zisc::FloatingPointFromBytes<sizeof(Float)>;
  static void test()
  {
    constexpr Float e[] = {
      zisc::cast<Float>(0),
      -zisc::cast<Float>(0),
      zisc::cast<Float>(1),
      zisc::cast<Float>(-1),
      std::numeric_limits<Float>::max(),
      std::numeric_limits<Float>::min(),
      std::numeric_limits<Float>::lowest(),
      std::numeric_limits<Float>::epsilon(),
      std::numeric_limits<Float>::round_error(),
      std::numeric_limits<Float>::denorm_min(),
      std::numeric_limits<Float>::infinity(),
      -std::numeric_limits<Float>::infinity(),
      std::numeric_limits<Float>::quiet_NaN(),
      -std::numeric_limits<Float>::quiet_NaN(),
      std::numeric_limits<Float>::signaling_NaN(),
      -std::numeric_limits<Float>::signaling_NaN()
    };

    constexpr FloatType v[] = {
      FloatType::zero(),
      -FloatType::zero(),
      FloatType::one(),
      -FloatType::one(),
      std::numeric_limits<FloatType>::max(),
      std::numeric_limits<FloatType>::min(),
      std::numeric_limits<FloatType>::lowest(),
      std::numeric_limits<FloatType>::epsilon(),
      std::numeric_limits<FloatType>::round_error(),
      std::numeric_limits<FloatType>::denorm_min(),
      std::numeric_limits<FloatType>::infinity(),
      -std::numeric_limits<FloatType>::infinity(),
      std::numeric_limits<FloatType>::quiet_NaN(),
      -std::numeric_limits<FloatType>::quiet_NaN(),
      std::numeric_limits<FloatType>::signaling_NaN(),
      -std::numeric_limits<FloatType>::signaling_NaN()
    };

    constexpr std::size_t n = sizeof(e) / sizeof(e[0]);

    // Comparison
    for (std::size_t i = 0; i < n; ++i) {
      const auto ei = *zisc::treatAs<const typename FloatType::BitType*>(&e[i]);
      EXPECT_EQ(ei, v[i].bits()) << "FloatingPoint \"" << e[i] << "\" is wrong.";
    }

    // Zero
    for (std::size_t i = 0; i < 2; ++i) {
      ASSERT_TRUE(zisc::isZero(v[i])) << "The isZero func is wrong.";
    }
    for (std::size_t i = 2; i < n; ++i) {
      ASSERT_FALSE(zisc::isZero(v[i])) << "The isZero func is wrong.";
    }

    for (std::size_t i = 0; i < n; ++i) {
      ASSERT_EQ(std::isfinite(v[i].toFloat()), zisc::isFinite(v[i]))
          << "The isFinite func is wrong.";
    }

    for (std::size_t i = 0; i < n; ++i) {
      ASSERT_EQ(std::isinf(v[i].toFloat()), zisc::isInf(v[i]))
          << "The isInf func is wrong.";
    }

    for (std::size_t i = 0; i < n; ++i) {
      ASSERT_EQ(std::isnan(v[i].toFloat()), zisc::isNan(v[i]))
          << "The isNan func is wrong.";
    }

    for (std::size_t i = 0; i < n; ++i) {
      ASSERT_EQ(std::isnormal(v[i].toFloat()), zisc::isNormal(v[i]))
          << "The isNormal func is wrong.";
    }

    for (std::size_t i = 0; i < n; ++i) {
      if( i == 9 )
        ASSERT_TRUE(zisc::isSubnormal(v[i])) << "The isSubnormal func is wrong.";
      else
        ASSERT_FALSE(zisc::isSubnormal(v[i])) << "The isSubnormal func is wrong.";
    }

    auto comp = [&e, &v](const std::size_t lhsi, const std::size_t rhsi)
    {
      ASSERT_EQ(e[lhsi] == e[rhsi], v[lhsi] == v[rhsi])
        << "FloatingPoint comp " << e[lhsi] << " == " << e[rhsi] << " is wrong.";
      ASSERT_EQ(e[lhsi] <  e[rhsi], v[lhsi] <  v[rhsi])
        << "FloatingPoint comp " << e[lhsi] << " <  " << e[rhsi] << " is wrong.";
      ASSERT_EQ(e[lhsi] <= e[rhsi], v[lhsi] <= v[rhsi])
        << "FloatingPoint comp " << e[lhsi] << " <= " << e[rhsi] << " is wrong.";
      ASSERT_EQ(e[lhsi] >  e[rhsi], v[lhsi] >  v[rhsi])
        << "FloatingPoint comp " << e[lhsi] << " >  " << e[rhsi] << " is wrong.";
      ASSERT_EQ(e[lhsi] >= e[rhsi], v[lhsi] >= v[rhsi])
        << "FloatingPoint comp " << e[lhsi] << " >= " << e[rhsi] << " is wrong.";
    };

    for (std::size_t lhsi = 0; lhsi < n; ++lhsi) {
      for (std::size_t rhsi = lhsi; rhsi < n; ++rhsi) {
        comp(lhsi, rhsi);
      }
    }

    // Conversion test
    auto print_conversion_result = [](const auto& original, const auto& converted)
    {
      const auto o = original.toFloat();
      const auto c = converted.toFloat();
      std::cout << "  Original : " << o << std::endl;
      std::cout << "  Converted: " << c << std::endl;
      std::cout << "  Diff     : " << (o - c) << std::endl;
    };

    std::cout << "Float <-> half conversion test" << std::endl;
    {
      constexpr auto o = FloatType::one();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result(o, c);
    }
    {
      const auto o = FloatType::fromFloat(zisc::kPi<Float>);
      const zisc::HalfFloat h = o;
      const FloatType c = h;
      print_conversion_result(o, c);
    }
  }
};

TEST(FloatingPointTest, FloatValueTest)
{
  ValueTest<float>::test();
}

TEST(FloatingPointTest, DoubleValueTest)
{
  ValueTest<double>::test();
}
