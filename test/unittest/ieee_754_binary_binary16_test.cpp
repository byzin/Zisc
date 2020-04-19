/*!
  \file ieee_754_binary_binary16_test.cpp
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
#include "zisc/ieee_754_binary.hpp"
#include "zisc/zisc_config.hpp"

TEST(Ieee754BinaryTest, Binary16BitSizeTest)
{
  using zisc::Binary16;
  ASSERT_EQ(2, sizeof(Binary16));
  constexpr auto sign_mask = Binary16::signBitMask();
  ASSERT_EQ(0b1000'0000'0000'0000u, sign_mask);
  constexpr std::size_t expt_size = Binary16::exponentBitSize();
  ASSERT_EQ(5, expt_size);
  constexpr auto expt_mask = Binary16::exponentBitMask();
  ASSERT_EQ(0b0111'1100'0000'0000u, expt_mask);
  constexpr std::size_t sig_size = Binary16::significandBitSize();
  ASSERT_EQ(10, sig_size);
  constexpr auto sig_mask = Binary16::significandBitMask();
  ASSERT_EQ(0b0000'0011'1111'1111u, sig_mask);
  ASSERT_EQ(16, 1 + expt_size + sig_size);
}

TEST(Ieee754BinaryTest, Binary16LimitsTest)
{
  using zisc::uint16b;
  using zisc::Binary16;
  using Limits = std::numeric_limits<Binary16>;

  {
    constexpr bool is_specialized = Limits::is_specialized;
    ASSERT_TRUE(is_specialized);
  }
  {
    constexpr bool is_signed = Limits::is_signed;
    ASSERT_TRUE(is_signed);
  }
  {
    constexpr bool is_integer = Limits::is_integer;
    ASSERT_FALSE(is_integer);
  }
  {
    constexpr bool is_exact = Limits::is_exact;
    ASSERT_FALSE(is_exact);
  }
  {
    constexpr bool has_infinity = Limits::has_infinity;
    ASSERT_TRUE(has_infinity);
  }
  {
    constexpr bool has_quiet_NaN = Limits::has_quiet_NaN;
    ASSERT_TRUE(has_quiet_NaN);
  }
  {
    constexpr bool has_signaling_NaN = Limits::has_signaling_NaN;
    ASSERT_TRUE(has_signaling_NaN);
  }
  {
    constexpr bool has_denorm = Limits::has_denorm;
    ASSERT_TRUE(has_denorm);
  }
  {
    constexpr bool has_denorm_loss = Limits::has_denorm_loss;
    ASSERT_FALSE(has_denorm_loss);
  }
  {
    constexpr std::float_round_style round_style = Limits::round_style;
    ASSERT_EQ(std::round_to_nearest, round_style);
  }
  {
    constexpr bool is_iec559 = Limits::is_iec559;
    ASSERT_FALSE(is_iec559);
  }
  {
    constexpr bool is_bounded = Limits::is_bounded;
    ASSERT_FALSE(is_bounded);
  }
  {
    constexpr bool is_modulo = Limits::is_modulo;
    ASSERT_FALSE(is_modulo);
  }
  {
    constexpr int digits = Limits::digits;
    ASSERT_EQ(11, digits);
  }
  {
    constexpr int digits10 = Limits::digits10;
    ASSERT_EQ(3, digits10);
  }
  {
    constexpr int max_digits10 = Limits::max_digits10;
    ASSERT_EQ(5, max_digits10);
  }
  {
    constexpr int radix = Limits::radix;
    ASSERT_EQ(2, radix);
  }
  {
    constexpr int min_exponent = Limits::min_exponent;
    ASSERT_EQ(-13, min_exponent);
  }
  {
    constexpr int min_exponent10 = Limits::min_exponent10;
    ASSERT_EQ(-4, min_exponent10);
  }
  {
    constexpr int max_exponent = Limits::max_exponent;
    ASSERT_EQ(16, max_exponent);
  }
  {
    constexpr int max_exponent10 = Limits::max_exponent10;
    ASSERT_EQ(4, max_exponent10);
  }
  {
    constexpr bool traps = Limits::traps;
    ASSERT_TRUE(traps);
  }
  {
    constexpr bool tinyness_before = Limits::tinyness_before;
    ASSERT_FALSE(tinyness_before);
  }
  {
    constexpr uint16b min_ref = 0b0000'0100'0000'0000u;
    constexpr Binary16 min_b = (Limits::min)();
    ASSERT_EQ(min_ref, min_b.bits());
  }
  {
    constexpr uint16b low_ref = 0b1111'1011'1111'1111u;
    constexpr Binary16 low_b = Limits::lowest();
    ASSERT_EQ(low_ref, low_b.bits());
  }
  {
    constexpr uint16b max_ref = 0b0111'1011'1111'1111u;
    constexpr Binary16 max_b = (Limits::max)();
    ASSERT_EQ(max_ref, max_b.bits());
  }
  {
    constexpr uint16b eps_ref = 0b0001'0100'0000'0000u;
    constexpr Binary16 eps_b = Limits::epsilon();
    ASSERT_EQ(eps_ref, eps_b.bits());
  }
  {
    constexpr uint16b round_err_ref = 0b0011'1000'0000'0000u;
    constexpr Binary16 round_err_b = Limits::round_error();
    ASSERT_EQ(round_err_ref, round_err_b.bits());
  }
  {
    constexpr uint16b infinity_ref = 0b0111'1100'0000'0000u;
    constexpr Binary16 infinity_b = Limits::infinity();
    ASSERT_EQ(infinity_ref, infinity_b.bits());
  }
  {
    constexpr uint16b qnan_ref = 0b0111'1110'0000'0000u;
    constexpr Binary16 qnan_b = Limits::quiet_NaN();
    ASSERT_EQ(qnan_ref, qnan_b.bits());
  }
  {
    constexpr uint16b snan_ref = 0b0111'1101'0000'0000u;
    constexpr Binary16 snan_b = Limits::signaling_NaN();
    ASSERT_EQ(snan_ref, snan_b.bits());
  }
  {
    constexpr uint16b denorm_min_ref = 0b0000'0000'0000'0001u;
    constexpr Binary16 denorm_min_b = Limits::denorm_min();
    ASSERT_EQ(denorm_min_ref, denorm_min_b.bits());
  }
}

//template <typename Float>
//struct ValueTest
//{
//  using FloatType = zisc::FloatingPointFromBytes<sizeof(Float)>;
//  static void test()
//  {
//    using zisc::cast;
//
//    EXPECT_EQ(std::numeric_limits<Float>::round_style,
//              std::numeric_limits<FloatType>::round_style)
//        << "Rounding styles aren't same.";
//
//    constexpr Float e[] = {
//      cast<Float>(0),
//      -cast<Float>(0),
//      cast<Float>(1),
//      cast<Float>(-1),
//      std::numeric_limits<Float>::max(),
//      std::numeric_limits<Float>::min(),
//      std::numeric_limits<Float>::lowest(),
//      std::numeric_limits<Float>::epsilon(),
//      std::numeric_limits<Float>::round_error(),
//      std::numeric_limits<Float>::denorm_min(),
//      std::numeric_limits<Float>::infinity(),
//      -std::numeric_limits<Float>::infinity(),
//      std::numeric_limits<Float>::quiet_NaN(),
//      -std::numeric_limits<Float>::quiet_NaN(),
//      std::numeric_limits<Float>::signaling_NaN(),
//      -std::numeric_limits<Float>::signaling_NaN()
//    };
//
//    constexpr FloatType v[] = {
//      FloatType::zero(),
//      -FloatType::zero(),
//      FloatType::one(),
//      -FloatType::one(),
//      std::numeric_limits<FloatType>::max(),
//      std::numeric_limits<FloatType>::min(),
//      std::numeric_limits<FloatType>::lowest(),
//      std::numeric_limits<FloatType>::epsilon(),
//      std::numeric_limits<FloatType>::round_error(),
//      std::numeric_limits<FloatType>::denorm_min(),
//      std::numeric_limits<FloatType>::infinity(),
//      -std::numeric_limits<FloatType>::infinity(),
//      std::numeric_limits<FloatType>::quiet_NaN(),
//      -std::numeric_limits<FloatType>::quiet_NaN(),
//      std::numeric_limits<FloatType>::signaling_NaN(),
//      -std::numeric_limits<FloatType>::signaling_NaN()
//    };
//
//    constexpr std::size_t n = sizeof(e) / sizeof(e[0]);
//
//    // Comparison
//    for (std::size_t i = 0; i < n; ++i) {
//      const auto ei = *zisc::treatAs<const typename FloatType::BitType*>(&e[i]);
//      EXPECT_EQ(ei, v[i].bits()) << "FloatingPoint \"" << e[i] << "\" is wrong.";
//    }
//
//    {
//      constexpr bool r1 = zisc::isZero(v[0]);
//      (void)r1;
//      constexpr bool r2 = zisc::isFinite(v[1]);
//      (void)r2;
//      constexpr bool r3 = zisc::isInf(v[2]);
//      (void)r3;
//      constexpr bool r4 = zisc::isNan(v[3]);
//      (void)r4;
//      constexpr bool r5 = zisc::isNormal(v[4]);
//      (void)r5;
//      constexpr bool r6 = zisc::isSubnormal(v[5]);
//      (void)r6;
//    }
//
//    // Zero
//    for (std::size_t i = 0; i < 2; ++i) {
//      ASSERT_TRUE(zisc::isZero(v[i])) << "The isZero func is wrong.";
//    }
//    for (std::size_t i = 2; i < n; ++i) {
//      ASSERT_FALSE(zisc::isZero(v[i])) << "The isZero func is wrong.";
//    }
//
//    for (std::size_t i = 0; i < n; ++i) {
//      ASSERT_EQ(std::isfinite(v[i].toFloat()), zisc::isFinite(v[i]))
//          << "The isFinite func is wrong.";
//    }
//
//    for (std::size_t i = 0; i < n; ++i) {
//      ASSERT_EQ(std::isinf(v[i].toFloat()), zisc::isInf(v[i]))
//          << "The isInf func is wrong.";
//    }
//
//    for (std::size_t i = 0; i < n; ++i) {
//      ASSERT_EQ(std::isnan(v[i].toFloat()), zisc::isNan(v[i]))
//          << "The isNan func is wrong.";
//    }
//
//    for (std::size_t i = 0; i < n; ++i) {
//      ASSERT_EQ(std::isnormal(v[i].toFloat()), zisc::isNormal(v[i]))
//          << "The isNormal func is wrong.";
//    }
//
//    for (std::size_t i = 0; i < n; ++i) {
//      if( i == 9 )
//        ASSERT_TRUE(zisc::isSubnormal(v[i])) << "The isSubnormal func is wrong.";
//      else
//        ASSERT_FALSE(zisc::isSubnormal(v[i])) << "The isSubnormal func is wrong.";
//    }
//
//    {
//      constexpr bool r1 = v[0] == v[1];
//      (void)r1;
//      constexpr bool r2 = v[0] != v[1];
//      (void)r2;
//      constexpr bool r3 = v[0] < v[1];
//      (void)r3;
//      constexpr bool r4 = v[0] <= v[1];
//      (void)r4;
//      constexpr bool r5 = v[0] > v[1];
//      (void)r5;
//      constexpr bool r6 = v[0] >= v[1];
//      (void)r6;
//    }
//
//    auto comp = [&e, &v](const std::size_t lhsi, const std::size_t rhsi)
//    {
//      ASSERT_EQ(e[lhsi] == e[rhsi], v[lhsi] == v[rhsi])
//        << "FloatingPoint comp " << e[lhsi] << " == " << e[rhsi] << " is wrong.";
//      ASSERT_EQ(e[lhsi] <  e[rhsi], v[lhsi] <  v[rhsi])
//        << "FloatingPoint comp " << e[lhsi] << " <  " << e[rhsi] << " is wrong.";
//      ASSERT_EQ(e[lhsi] <= e[rhsi], v[lhsi] <= v[rhsi])
//        << "FloatingPoint comp " << e[lhsi] << " <= " << e[rhsi] << " is wrong.";
//      ASSERT_EQ(e[lhsi] >  e[rhsi], v[lhsi] >  v[rhsi])
//        << "FloatingPoint comp " << e[lhsi] << " >  " << e[rhsi] << " is wrong.";
//      ASSERT_EQ(e[lhsi] >= e[rhsi], v[lhsi] >= v[rhsi])
//        << "FloatingPoint comp " << e[lhsi] << " >= " << e[rhsi] << " is wrong.";
//    };
//
//    for (std::size_t lhsi = 0; lhsi < n; ++lhsi) {
//      for (std::size_t rhsi = lhsi; rhsi < n; ++rhsi) {
//        comp(lhsi, rhsi);
//      }
//    }
//
//    // Conversion test
//    auto print_conversion_result = []
//    (const char* name, const auto& original, const auto& converted)
//    {
//      using F = typename std::remove_reference_t<decltype(converted)>::FloatType;
//      const F o[2] = {original.toFloat(), cast<F>(0.0)};
//      const F c[2] = {converted.toFloat(), cast<F>(0.0)};
//      std::cout << "## " << name << std::endl;
//      std::cout << std::scientific
//                << std::setprecision(std::numeric_limits<F>::max_digits10);
//      std::cout << "  Original : " << o[0] << std::endl;
//      std::cout << "           : "
//                << std::bitset<64>{*zisc::treatAs<const zisc::uint64b*>(o)}
//                << std::endl;
//      std::cout << "  Converted: " << c[0] << std::endl;
//      std::cout << "           : "
//                << std::bitset<64>{*zisc::treatAs<const zisc::uint64b*>(c)}
//                << std::endl;
//    };
//
//    std::cout << "Float <-> half conversion test" << std::endl;
//    {
//      constexpr auto o = FloatType::zero();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("0", o, c);
//    }
//    {
//      constexpr auto o = FloatType::one();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("1", o, c);
//    }
//    {
//      constexpr auto o = -FloatType::one();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("-1", o, c);
//    }
//    {
//      constexpr auto o = std::numeric_limits<FloatType>::max();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("max", o, c);
//    }
//    {
//      constexpr auto o = std::numeric_limits<FloatType>::min();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("min", o, c);
//    }
//    {
//      constexpr auto o = std::numeric_limits<FloatType>::lowest();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("lowest", o, c);
//    }
//    {
//      constexpr auto o = std::numeric_limits<FloatType>::epsilon();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("epsilon", o, c);
//    }
//    {
//      constexpr auto o = std::numeric_limits<FloatType>::denorm_min();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("denorm_min", o, c);
//    }
//    {
//      constexpr auto o = std::numeric_limits<FloatType>::infinity();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("infinity", o, c);
//    }
//    {
//      constexpr auto o = std::numeric_limits<FloatType>::quiet_NaN();
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("nan", o, c);
//    }
//    {
//      constexpr Float f = (cast<Float>(2.0) - zisc::power<-10>(cast<Float>(2.0))) *
//                          zisc::power<15>(cast<Float>(2.0));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("half max", o, c);
//    }
//    {
//      constexpr Float f = -(cast<Float>(2.0) - zisc::power<-10>(cast<Float>(2.0))) *
//                          zisc::power<15>(cast<Float>(2.0));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("-half max", o, c);
//    }
//    {
//      constexpr Float f = (cast<Float>(2.0) - zisc::power<-10>(cast<Float>(2.0))) *
//                          zisc::power<15>(cast<Float>(2.0)) *
//                          (cast<Float>(1.0005));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("half max x (1.0005)", o, c);
//    }
//    {
//      constexpr Float f = zisc::power<-14>(cast<Float>(2.0));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("half min", o, c);
//    }
//    {
//      constexpr Float f = -zisc::power<-14>(cast<Float>(2.0));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("-half min", o, c);
//    }
//    {
//      constexpr Float f = zisc::power<-14>(cast<Float>(2.0)) *
//                          (cast<Float>(0.9995));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("half min x (0.9995)", o, c);
//    }
//    {
//      constexpr Float f = zisc::power<-24>(cast<Float>(2.0));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("half denorm_min", o, c);
//    }
//    {
//      constexpr Float f = -zisc::power<-24>(cast<Float>(2.0));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("-half denorm_min", o, c);
//    }
//    {
//      constexpr Float f = zisc::power<-24>(cast<Float>(2.0)) *
//                          (cast<Float>(0.9995));
//      constexpr auto o = FloatType::fromFloat(f);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("half denorm_min x (0.9995)", o, c);
//    }
//    {
//      constexpr auto o = FloatType::fromFloat(zisc::kPi<Float>);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("pi", o, c);
//    }
//    {
//      constexpr auto o = FloatType::fromFloat(zisc::kE<Float>);
//      constexpr zisc::HalfFloat h = o;
//      constexpr FloatType c = h;
//      print_conversion_result("euler", o, c);
//    }
//
//    // Arithmetic operation
//    {
//      constexpr zisc::HalfFloat v1{FloatType::fromFloat(cast<Float>(5.0))};
//      constexpr zisc::HalfFloat v2{FloatType::fromFloat(cast<Float>(3.0))};
//      constexpr FloatType result{v1 + v2};
//      constexpr Float r = cast<Float>(result);
//      std::cout << "5 + 3 = " << r << std::endl;
//    }
//    {
//      constexpr zisc::HalfFloat v1{FloatType::fromFloat(cast<Float>(5.0))};
//      constexpr zisc::HalfFloat v2{FloatType::fromFloat(cast<Float>(3.0))};
//      constexpr FloatType result{v1 - v2};
//      constexpr Float r = cast<Float>(result);
//      std::cout << "5 - 3 = " << r << std::endl;
//    }
//    {
//      constexpr zisc::HalfFloat v1{FloatType::fromFloat(cast<Float>(5.0))};
//      constexpr zisc::HalfFloat v2{FloatType::fromFloat(cast<Float>(3.0))};
//      constexpr FloatType result{v1 * v2};
//      constexpr Float r = cast<Float>(result);
//      std::cout << "5 * 3 = " << r << std::endl;
//    }
//    {
//      constexpr zisc::HalfFloat v1{FloatType::fromFloat(cast<Float>(5.0))};
//      constexpr zisc::HalfFloat v2{FloatType::fromFloat(cast<Float>(3.0))};
//      constexpr FloatType result{v1 / v2};
//      constexpr Float r = cast<Float>(result);
//      std::cout << "5 / 3 = " << r << std::endl;
//    }
//    auto pre_increment = [](const auto& value)
//    {
//      zisc::HalfFloat a{FloatType::fromFloat(cast<Float>(value))};
//      const FloatType result{++a};
//      return result;
//    };
//    {
//      constexpr Float expected = cast<Float>(2.0);
//      constexpr auto result = pre_increment(1.0);
//      ASSERT_EQ(expected, result.toFloat()) << "Pre-increment failed.";
//    }
//    auto pre_decrement = [](const auto& value)
//    {
//      zisc::HalfFloat a{FloatType::fromFloat(cast<Float>(value))};
//      const FloatType result{--a};
//      return result;
//    };
//    {
//      constexpr Float expected = cast<Float>(0.0);
//      constexpr auto result = pre_decrement(1.0);
//      ASSERT_EQ(expected, result.toFloat()) << "Pre-decrement failed.";
//    }
//    auto post_increment = [](const auto& value)
//    {
//      zisc::HalfFloat a{FloatType::fromFloat(cast<Float>(value))};
//      const FloatType result{a++};
//      return std::make_tuple(FloatType{a}, result);
//    };
//    {
//      constexpr Float value = cast<Float>(1.0);
//      constexpr Float expected = cast<Float>(2.0);
//      const auto [a, result] = post_increment(1.0);
//      ASSERT_EQ(expected, a.toFloat()) << "Post-increment failed.";
//      ASSERT_EQ(value, result.toFloat()) << "Post-increment failed.";
//    }
//    auto post_decrement = [](const auto& value)
//    {
//      zisc::HalfFloat a{FloatType::fromFloat(cast<Float>(value))};
//      const FloatType result{a--};
//      return std::make_tuple(FloatType{a}, result);
//    };
//    {
//      constexpr Float value = cast<Float>(1.0);
//      constexpr Float expected = cast<Float>(0.0);
//      const auto [a, result] = post_decrement(1.0);
//      ASSERT_EQ(expected, a.toFloat()) << "Post-decrement failed.";
//      ASSERT_EQ(value, result.toFloat()) << "Post-decrement failed.";
//    }
//
//    auto add = [](const auto& lhs, const auto& rhs)
//    {
//      const auto expected = cast<Float>(lhs) + cast<Float>(rhs);
//      auto v1 = FloatType::fromFloat(cast<Float>(lhs));
//      auto v2 = FloatType::fromFloat(cast<Float>(rhs));
//      v1 += v2;
//      return std::make_tuple(expected, v1);
//    };
//    auto subtract = [](const auto& lhs, const auto& rhs)
//    {
//      const auto expected = cast<Float>(lhs) - cast<Float>(rhs);
//      auto v1 = FloatType::fromFloat(cast<Float>(lhs));
//      auto v2 = FloatType::fromFloat(cast<Float>(rhs));
//      v1 -= v2;
//      return std::make_tuple(expected, v1);
//    };
//    auto multiply = [](const auto& lhs, const auto& rhs)
//    {
//      const auto expected = cast<Float>(lhs) * cast<Float>(rhs);
//      auto v1 = FloatType::fromFloat(cast<Float>(lhs));
//      auto v2 = FloatType::fromFloat(cast<Float>(rhs));
//      v1 *= v2;
//      return std::make_tuple(expected, v1);
//    };
//    auto divide = [](const auto& lhs, const auto& rhs)
//    {
//      const auto expected = cast<Float>(lhs) / cast<Float>(rhs);
//      auto v1 = FloatType::fromFloat(cast<Float>(lhs));
//      auto v2 = FloatType::fromFloat(cast<Float>(rhs));
//      v1 /= v2;
//      return std::make_tuple(expected, v1);
//    };
//    {
//      const auto [expected, result] = add(5.0, 3.0);
//      EXPECT_EQ(expected, result.toFloat()) << "Addition \"5 + 3\" failed.";
//    }
//    {
//      const auto [expected, result] = subtract(5.0, 3.0);
//      EXPECT_EQ(expected, result.toFloat()) << "Subtraction \"5 + 3\" failed.";
//    }
//    {
//      const auto [expected, result] = multiply(5.0, 3.0);
//      EXPECT_EQ(expected, result.toFloat()) << "Multiply \"5 + 3\" failed.";
//    }
//    {
//      const auto [expected, result] = divide(5.0, 3.0);
//      EXPECT_EQ(expected, result.toFloat()) << "Divide \"5 + 3\" failed.";
//    }
//  }
//};
//
//TEST(FloatingPointTest, FloatValueTest)
//{
//  ValueTest<float>::test();
//}
//
//TEST(FloatingPointTest, DoubleValueTest)
//{
//  ValueTest<double>::test();
//}
