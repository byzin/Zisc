/*!
  \file ieee_754_binary_binary32_test.cpp
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
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

TEST(Ieee754BinaryTest, Binary64BitSizeTest)
{
  using zisc::Binary64;
  ASSERT_EQ(8, sizeof(Binary64));
  constexpr auto sign_mask = Binary64::signBitMask();
  ASSERT_EQ(0b1000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000ull, sign_mask);
  constexpr std::size_t expt_size = Binary64::exponentBitSize();
  ASSERT_EQ(11, expt_size);
  constexpr auto expt_mask = Binary64::exponentBitMask();
  ASSERT_EQ(0b0111'1111'1111'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000ull, expt_mask);
  constexpr std::size_t sig_size = Binary64::significandBitSize();
  ASSERT_EQ(52, sig_size);
  constexpr auto sig_mask = Binary64::significandBitMask();
  ASSERT_EQ(0b0000'0000'0000'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111ull, sig_mask);
  ASSERT_EQ(64, 1 + expt_size + sig_size);
}

TEST(Ieee754BinaryTest, Binary64LimitsTest)
{
  using zisc::uint64b;
  using zisc::Binary64;
  using Limits = std::numeric_limits<Binary64>;
  using FLimits = std::numeric_limits<double>;

  {
    constexpr bool is_specialized = Limits::is_specialized;
    ASSERT_TRUE(is_specialized);
  }
  {
    constexpr bool is_signed = Limits::is_signed;
    ASSERT_EQ(FLimits::is_signed, is_signed);
  }
  {
    constexpr bool is_integer = Limits::is_integer;
    ASSERT_EQ(FLimits::is_integer, is_integer);
  }
  {
    constexpr bool is_exact = Limits::is_exact;
    ASSERT_EQ(FLimits::is_exact, is_exact);
  }
  {
    constexpr bool has_infinity = Limits::has_infinity;
    ASSERT_EQ(FLimits::has_infinity, has_infinity);
  }
  {
    constexpr bool has_quiet_NaN = Limits::has_quiet_NaN;
    ASSERT_EQ(FLimits::has_quiet_NaN, has_quiet_NaN);
  }
  {
    constexpr bool has_signaling_NaN = Limits::has_signaling_NaN;
    ASSERT_EQ(FLimits::has_signaling_NaN, has_signaling_NaN);
  }
  {
    constexpr bool has_denorm = Limits::has_denorm;
    ASSERT_EQ(FLimits::has_denorm, has_denorm);
  }
  {
    constexpr bool has_denorm_loss = Limits::has_denorm_loss;
    ASSERT_EQ(FLimits::has_denorm_loss, has_denorm_loss);
  }
  {
    constexpr std::float_round_style round_style = Limits::round_style;
    ASSERT_EQ(FLimits::round_style, round_style);
  }
  {
    constexpr bool is_iec559 = Limits::is_iec559;
    ASSERT_EQ(FLimits::is_iec559, is_iec559);
  }
  {
    constexpr bool is_bounded = Limits::is_bounded;
    ASSERT_FALSE(is_bounded);
  }
  {
    constexpr bool is_modulo = Limits::is_modulo;
    ASSERT_EQ(FLimits::is_modulo, is_modulo);
  }
  {
    constexpr int digits = Limits::digits;
    ASSERT_EQ(FLimits::digits, digits);
  }
  {
    constexpr int digits10 = Limits::digits10;
    ASSERT_EQ(FLimits::digits10, digits10);
  }
  {
    constexpr int max_digits10 = Limits::max_digits10;
    ASSERT_EQ(FLimits::max_digits10, max_digits10);
  }
  {
    constexpr int radix = Limits::radix;
    ASSERT_EQ(FLimits::radix, radix);
  }
  {
    constexpr int min_exponent = Limits::min_exponent;
    ASSERT_EQ(FLimits::min_exponent, min_exponent);
  }
  {
    constexpr int min_exponent10 = Limits::min_exponent10;
    ASSERT_EQ(FLimits::min_exponent10, min_exponent10);
  }
  {
    constexpr int max_exponent = Limits::max_exponent;
    ASSERT_EQ(FLimits::max_exponent, max_exponent);
  }
  {
    constexpr int max_exponent10 = Limits::max_exponent10;
    ASSERT_EQ(FLimits::max_exponent10, max_exponent10);
  }
  {
    constexpr bool traps = Limits::traps;
    ASSERT_TRUE(traps);
  }
  {
    constexpr bool tinyness_before = Limits::tinyness_before;
    ASSERT_EQ(FLimits::tinyness_before, tinyness_before);
  }

  auto as_uint = [](const double f) noexcept
  {
    return *zisc::treatAs<const uint64b*>(&f);
  };

  auto print_float_bits = [](const std::string_view name, const double f) noexcept
  {
    std::cout << "    " << std::setw(16) << std::setfill(' ') << name << ": "
              << std::bitset<64>{*zisc::treatAs<const zisc::uint64b*>(&f)}
              << std::endl;
  };


  {
    constexpr double min_f = (FLimits::min)();
    const uint64b min_ref = as_uint(min_f);
    constexpr Binary64 min_b = (Limits::min)();
    ASSERT_EQ(min_ref, min_b.bits());
    ASSERT_FLOAT_EQ(min_f, zisc::castBinary<double>(min_b));

    constexpr uint64b min_bits = Binary64::makeBits(min_f);
    ASSERT_EQ(min_ref, min_bits);
  }
  {
    constexpr double low_f = FLimits::lowest();
    const uint64b low_ref = as_uint(low_f);
    constexpr Binary64 low_b = Limits::lowest();
    ASSERT_EQ(low_ref, low_b.bits());
    ASSERT_FLOAT_EQ(low_f, zisc::castBinary<double>(low_b));

    constexpr uint64b low_bits = Binary64::makeBits(low_f);
    ASSERT_EQ(low_ref, low_bits);
  }
  {
    constexpr double max_f = (FLimits::max)();
    const uint64b max_ref = as_uint(max_f);
    constexpr Binary64 max_b = (Limits::max)();
    ASSERT_EQ(max_ref, max_b.bits());
    ASSERT_FLOAT_EQ(max_f, zisc::castBinary<double>(max_b));

    constexpr uint64b max_bits = Binary64::makeBits(max_f);
    ASSERT_EQ(max_ref, max_bits);
  }
  {
    constexpr double eps_f = FLimits::epsilon();
    const uint64b eps_ref = as_uint(eps_f);
    constexpr Binary64 eps_b = Limits::epsilon();
    ASSERT_EQ(eps_ref, eps_b.bits());
    ASSERT_FLOAT_EQ(eps_f, zisc::castBinary<double>(eps_b));

    constexpr uint64b eps_bits = Binary64::makeBits(eps_f);
    ASSERT_EQ(eps_ref, eps_bits);
  }
  {
    constexpr double round_err_f = FLimits::round_error();
    const uint64b round_err_ref = as_uint(round_err_f);
    constexpr Binary64 round_err_b = Limits::round_error();
    ASSERT_EQ(round_err_ref, round_err_b.bits());
    ASSERT_FLOAT_EQ(round_err_f, zisc::castBinary<double>(round_err_b));

    constexpr uint64b round_err_bits = Binary64::makeBits(round_err_f);
    ASSERT_EQ(round_err_ref, round_err_bits);
  }
  {
    constexpr double infinity_f = FLimits::infinity();
    const uint64b infinity_ref = as_uint(infinity_f);
    constexpr Binary64 infinity_b = Limits::infinity();
    ASSERT_EQ(infinity_ref, infinity_b.bits());
    ASSERT_FLOAT_EQ(infinity_f, zisc::castBinary<double>(infinity_b));

    constexpr uint64b infinity_bits = Binary64::makeBits(infinity_f);
    ASSERT_EQ(infinity_ref, infinity_bits);
  }
  {
    const double qnan_f = FLimits::quiet_NaN();
    print_float_bits("quiet_NaN", qnan_f);
    const uint64b qnan_ref = as_uint(qnan_f);
    constexpr Binary64 qnan_b = Limits::quiet_NaN();
    EXPECT_EQ(qnan_ref, qnan_b.bits());
    std::isnan(zisc::castBinary<double>(qnan_b));

    const uint64b qnan_bits = Binary64::makeBits(qnan_f);
    EXPECT_EQ(qnan_ref, qnan_bits);
  }
  {
    const double snan_f = FLimits::signaling_NaN();
    print_float_bits("signaling_NaN", snan_f);
    const uint64b snan_ref = as_uint(snan_f);
    constexpr Binary64 snan_b = Limits::signaling_NaN();
    EXPECT_EQ(snan_ref, snan_b.bits());
    std::isnan(zisc::castBinary<double>(snan_b));

//    const uint64b snan_bits = Binary64::makeBits(snan_f);
//    EXPECT_EQ(snan_ref, snan_bits);
  }
  {
    constexpr double denorm_min_f = FLimits::denorm_min();
    const uint64b denorm_min_ref = as_uint(denorm_min_f);
    constexpr Binary64 denorm_min_b = Limits::denorm_min();
    ASSERT_EQ(denorm_min_ref, denorm_min_b.bits());
    ASSERT_FLOAT_EQ(denorm_min_f, zisc::castBinary<double>(denorm_min_b));

    constexpr uint64b denorm_min_bits = Binary64::makeBits(denorm_min_f);
    ASSERT_EQ(denorm_min_ref, denorm_min_bits);
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
