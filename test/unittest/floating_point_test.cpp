/*!
  \file floating_point_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <tuple>
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
    using zisc::cast;

    EXPECT_EQ(std::numeric_limits<Float>::round_style,
              std::numeric_limits<FloatType>::round_style)
        << "Rounding styles aren't same.";

    constexpr Float e[] = {
      cast<Float>(0),
      -cast<Float>(0),
      cast<Float>(1),
      cast<Float>(-1),
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

    {
      constexpr bool r1 = zisc::isZero(v[0]);
      (void)r1;
      constexpr bool r2 = zisc::isFinite(v[1]);
      (void)r2;
      constexpr bool r3 = zisc::isInf(v[2]);
      (void)r3;
      constexpr bool r4 = zisc::isNan(v[3]);
      (void)r4;
      constexpr bool r5 = zisc::isNormal(v[4]);
      (void)r5;
      constexpr bool r6 = zisc::isSubnormal(v[5]);
      (void)r6;
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

    {
      constexpr bool r1 = v[0] == v[1];
      (void)r1;
      constexpr bool r2 = v[0] != v[1];
      (void)r2;
      constexpr bool r3 = v[0] < v[1];
      (void)r3;
      constexpr bool r4 = v[0] <= v[1];
      (void)r4;
      constexpr bool r5 = v[0] > v[1];
      (void)r5;
      constexpr bool r6 = v[0] >= v[1];
      (void)r6;
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
    auto print_conversion_result = []
    (const char* name, const auto& original, const auto& converted)
    {
      using F = typename std::remove_reference_t<decltype(converted)>::FloatType;
      const F o[2] = {original.toFloat(), cast<F>(0.0)};
      const F c[2] = {converted.toFloat(), cast<F>(0.0)};
      std::cout << "## " << name << std::endl;
      std::cout << std::scientific
                << std::setprecision(std::numeric_limits<F>::max_digits10);
      std::cout << "  Original : " << o[0] << std::endl;
      std::cout << "           : "
                << std::bitset<64>{*zisc::treatAs<const zisc::uint64b*>(o)}
                << std::endl;
      std::cout << "  Converted: " << c[0] << std::endl;
      std::cout << "           : "
                << std::bitset<64>{*zisc::treatAs<const zisc::uint64b*>(c)}
                << std::endl;
    };

    std::cout << "Float <-> half conversion test" << std::endl;
    {
      constexpr auto o = FloatType::zero();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("0", o, c);
    }
    {
      constexpr auto o = FloatType::one();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("1", o, c);
    }
    {
      constexpr auto o = -FloatType::one();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("-1", o, c);
    }
    {
      constexpr auto o = std::numeric_limits<FloatType>::max();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("max", o, c);
    }
    {
      constexpr auto o = std::numeric_limits<FloatType>::min();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("min", o, c);
    }
    {
      constexpr auto o = std::numeric_limits<FloatType>::lowest();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("lowest", o, c);
    }
    {
      constexpr auto o = std::numeric_limits<FloatType>::epsilon();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("epsilon", o, c);
    }
    {
      constexpr auto o = std::numeric_limits<FloatType>::denorm_min();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("denorm_min", o, c);
    }
    {
      constexpr auto o = std::numeric_limits<FloatType>::infinity();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("infinity", o, c);
    }
    {
      constexpr auto o = std::numeric_limits<FloatType>::quiet_NaN();
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("nan", o, c);
    }
    {
      constexpr Float f = (cast<Float>(2.0) - zisc::power<-10>(cast<Float>(2.0))) *
                          zisc::power<15>(cast<Float>(2.0));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("half max", o, c);
    }
    {
      constexpr Float f = -(cast<Float>(2.0) - zisc::power<-10>(cast<Float>(2.0))) *
                          zisc::power<15>(cast<Float>(2.0));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("-half max", o, c);
    }
    {
      constexpr Float f = (cast<Float>(2.0) - zisc::power<-10>(cast<Float>(2.0))) *
                          zisc::power<15>(cast<Float>(2.0)) *
                          (cast<Float>(1.0005));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("half max x (1.0005)", o, c);
    }
    {
      constexpr Float f = zisc::power<-14>(cast<Float>(2.0));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("half min", o, c);
    }
    {
      constexpr Float f = -zisc::power<-14>(cast<Float>(2.0));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("-half min", o, c);
    }
    {
      constexpr Float f = zisc::power<-14>(cast<Float>(2.0)) *
                          (cast<Float>(0.9995));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("half min x (0.9995)", o, c);
    }
    {
      constexpr Float f = zisc::power<-24>(cast<Float>(2.0));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("half denorm_min", o, c);
    }
    {
      constexpr Float f = -zisc::power<-24>(cast<Float>(2.0));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("-half denorm_min", o, c);
    }
    {
      constexpr Float f = zisc::power<-24>(cast<Float>(2.0)) *
                          (cast<Float>(0.9995));
      constexpr auto o = FloatType::fromFloat(f);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("half denorm_min x (0.9995)", o, c);
    }
    {
      constexpr auto o = FloatType::fromFloat(zisc::kPi<Float>);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("pi", o, c);
    }
    {
      constexpr auto o = FloatType::fromFloat(zisc::kE<Float>);
      constexpr zisc::HalfFloat h = o;
      constexpr FloatType c = h;
      print_conversion_result("euler", o, c);
    }

    // Arithmetic operation
    {
      constexpr zisc::HalfFloat v1{FloatType::fromFloat(cast<Float>(5.0))};
      constexpr zisc::HalfFloat v2{FloatType::fromFloat(cast<Float>(3.0))};
      constexpr FloatType result{v1 + v2};
      constexpr Float r = cast<Float>(result);
      std::cout << "5 + 3 = " << r << std::endl;
    }
    {
      constexpr zisc::HalfFloat v1{FloatType::fromFloat(cast<Float>(5.0))};
      constexpr zisc::HalfFloat v2{FloatType::fromFloat(cast<Float>(3.0))};
      constexpr FloatType result{v1 - v2};
      constexpr Float r = cast<Float>(result);
      std::cout << "5 - 3 = " << r << std::endl;
    }
    {
      constexpr zisc::HalfFloat v1{FloatType::fromFloat(cast<Float>(5.0))};
      constexpr zisc::HalfFloat v2{FloatType::fromFloat(cast<Float>(3.0))};
      constexpr FloatType result{v1 * v2};
      constexpr Float r = cast<Float>(result);
      std::cout << "5 * 3 = " << r << std::endl;
    }
    {
      constexpr zisc::HalfFloat v1{FloatType::fromFloat(cast<Float>(5.0))};
      constexpr zisc::HalfFloat v2{FloatType::fromFloat(cast<Float>(3.0))};
      constexpr FloatType result{v1 / v2};
      constexpr Float r = cast<Float>(result);
      std::cout << "5 / 3 = " << r << std::endl;
    }
    auto pre_increment = [](const auto& value)
    {
      zisc::HalfFloat a{FloatType::fromFloat(cast<Float>(value))};
      const FloatType result{++a};
      return result;
    };
    {
      constexpr Float expected = cast<Float>(2.0);
      constexpr auto result = pre_increment(1.0);
      ASSERT_EQ(expected, result.toFloat()) << "Pre-increment failed.";
    }
    auto pre_decrement = [](const auto& value)
    {
      zisc::HalfFloat a{FloatType::fromFloat(cast<Float>(value))};
      const FloatType result{--a};
      return result;
    };
    {
      constexpr Float expected = cast<Float>(0.0);
      constexpr auto result = pre_decrement(1.0);
      ASSERT_EQ(expected, result.toFloat()) << "Pre-decrement failed.";
    }
    auto post_increment = [](const auto& value)
    {
      zisc::HalfFloat a{FloatType::fromFloat(cast<Float>(value))};
      const FloatType result{a++};
      return std::make_tuple(FloatType{a}, result);
    };
    {
      constexpr Float value = cast<Float>(1.0);
      constexpr Float expected = cast<Float>(2.0);
      const auto [a, result] = post_increment(1.0);
      ASSERT_EQ(expected, a.toFloat()) << "Post-increment failed.";
      ASSERT_EQ(value, result.toFloat()) << "Post-increment failed.";
    }
    auto post_decrement = [](const auto& value)
    {
      zisc::HalfFloat a{FloatType::fromFloat(cast<Float>(value))};
      const FloatType result{a--};
      return std::make_tuple(FloatType{a}, result);
    };
    {
      constexpr Float value = cast<Float>(1.0);
      constexpr Float expected = cast<Float>(0.0);
      const auto [a, result] = post_decrement(1.0);
      ASSERT_EQ(expected, a.toFloat()) << "Post-decrement failed.";
      ASSERT_EQ(value, result.toFloat()) << "Post-decrement failed.";
    }

    auto add = [](const auto& lhs, const auto& rhs)
    {
      const auto expected = cast<Float>(lhs) + cast<Float>(rhs);
      auto v1 = FloatType::fromFloat(cast<Float>(lhs));
      auto v2 = FloatType::fromFloat(cast<Float>(rhs));
      v1 += v2;
      return std::make_tuple(expected, v1);
    };
    auto subtract = [](const auto& lhs, const auto& rhs)
    {
      const auto expected = cast<Float>(lhs) - cast<Float>(rhs);
      auto v1 = FloatType::fromFloat(cast<Float>(lhs));
      auto v2 = FloatType::fromFloat(cast<Float>(rhs));
      v1 -= v2;
      return std::make_tuple(expected, v1);
    };
    auto multiply = [](const auto& lhs, const auto& rhs)
    {
      const auto expected = cast<Float>(lhs) * cast<Float>(rhs);
      auto v1 = FloatType::fromFloat(cast<Float>(lhs));
      auto v2 = FloatType::fromFloat(cast<Float>(rhs));
      v1 *= v2;
      return std::make_tuple(expected, v1);
    };
    auto divide = [](const auto& lhs, const auto& rhs)
    {
      const auto expected = cast<Float>(lhs) / cast<Float>(rhs);
      auto v1 = FloatType::fromFloat(cast<Float>(lhs));
      auto v2 = FloatType::fromFloat(cast<Float>(rhs));
      v1 /= v2;
      return std::make_tuple(expected, v1);
    };
    {
      const auto [expected, result] = add(5.0, 3.0);
      EXPECT_EQ(expected, result.toFloat()) << "Addition \"5 + 3\" failed.";
    }
    {
      const auto [expected, result] = subtract(5.0, 3.0);
      EXPECT_EQ(expected, result.toFloat()) << "Subtraction \"5 + 3\" failed.";
    }
    {
      const auto [expected, result] = multiply(5.0, 3.0);
      EXPECT_EQ(expected, result.toFloat()) << "Multiply \"5 + 3\" failed.";
    }
    {
      const auto [expected, result] = divide(5.0, 3.0);
      EXPECT_EQ(expected, result.toFloat()) << "Divide \"5 + 3\" failed.";
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
