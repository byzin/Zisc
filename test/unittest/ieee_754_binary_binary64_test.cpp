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
#include <random>
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

  auto print_float_bits = [](const std::string_view name, const double f) noexcept
  {
    std::cout << "    " << std::setw(16) << std::setfill(' ') << name << ": "
              << std::bitset<64>{*zisc::reinterp<const zisc::uint64b*>(&f)}
              << std::endl;
  };

  using zisc::cast;

  {
    constexpr double min_f = (FLimits::min)();
    constexpr Binary64 min_b = (Limits::min)();
    constexpr double result = cast<double>(min_b);
    ASSERT_EQ(min_f, result);
  }
  {
    constexpr double low_f = FLimits::lowest();
    constexpr Binary64 low_b = Limits::lowest();
    constexpr double result = cast<double>(low_b);
    ASSERT_EQ(low_f, result);
  }
  {
    constexpr double max_f = (FLimits::max)();
    constexpr Binary64 max_b = (Limits::max)();
    constexpr double result = cast<double>(max_b);
    ASSERT_EQ(max_f, result);
  }
  {
    constexpr double eps_f = FLimits::epsilon();
    constexpr Binary64 eps_b = Limits::epsilon();
    constexpr double result = cast<double>(eps_b);
    ASSERT_EQ(eps_f, result);
  }
  {
    constexpr double round_err_f = FLimits::round_error();
    constexpr Binary64 round_err_b = Limits::round_error();
    constexpr double result = cast<double>(round_err_b);
    ASSERT_EQ(round_err_f, result);
  }
  {
    constexpr double infinity_f = FLimits::infinity();
    constexpr Binary64 infinity_b = Limits::infinity();
    constexpr double result = cast<double>(infinity_b);
    ASSERT_EQ(infinity_f, result);
  }
  {
    const double qnan_f = FLimits::quiet_NaN();
    print_float_bits("quiet_NaN", qnan_f);
    constexpr Binary64 qnan_b = Limits::quiet_NaN();
    const double result = cast<double>(qnan_b);
    EXPECT_TRUE(std::isnan(result));
  }
  {
    const double snan_f = FLimits::signaling_NaN();
    print_float_bits("signaling_NaN", snan_f);
    constexpr Binary64 snan_b = Limits::signaling_NaN();
    const double result = cast<double>(snan_b);
    EXPECT_TRUE(std::isnan(result));
  }
  {
    constexpr double denorm_min_f = FLimits::denorm_min();
    constexpr Binary64 denorm_min_b = Limits::denorm_min();
    constexpr double result = cast<double>(denorm_min_b);
    ASSERT_EQ(denorm_min_f, result);
  }
}

TEST(Ieee754BinaryTest, Double2FloatConstantTest)
{
  using zisc::uint32b;
  using zisc::uint64b;
  using zisc::Binary32;
  using zisc::Binary64;
  using FLimit = std::numeric_limits<float>;
  using DLimit = std::numeric_limits<double>;

  using zisc::cast;

  {
    constexpr double d = 0.0;
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = -0.0;
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = 1.0;
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = -1.0;
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = 100.0;
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = -100.0;
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = DLimit::denorm_min();
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = -DLimit::denorm_min();
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = cast<double>(FLimit::denorm_min());
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = cast<double>(-FLimit::denorm_min());
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = (DLimit::min)();
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = -(DLimit::min)();
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = cast<double>((FLimit::min)());
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = cast<double>(-(FLimit::min)());
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = (DLimit::max)();
    const float f = cast<float>(d);
    const Binary64 b64{d};
    const float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = -(DLimit::max)();
    const float f = cast<float>(d);
    const Binary64 b64{d};
    const float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = cast<double>((FLimit::max)());
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = cast<double>(-(FLimit::max)());
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = DLimit::epsilon();
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = -DLimit::epsilon();
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = cast<double>(FLimit::epsilon());
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = cast<double>(-FLimit::epsilon());
    constexpr float f = cast<float>(d);
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_FLOAT_EQ(f, b32);
  }
  {
    constexpr double d = DLimit::infinity();
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_TRUE(zisc::isInf(b32));
  }
  {
    constexpr double d = -DLimit::infinity();
    constexpr Binary64 b64{d};
    constexpr float b32 = cast<float>(b64);
    ASSERT_TRUE(zisc::isInf(b32));
  }
}

TEST(Ieee754BinaryTest, Double2FloatTest)
{
  using zisc::uint32b;
  using zisc::uint64b;
  using zisc::Binary32;
  using zisc::Binary64;
  using Generator = std::uniform_int_distribution<uint64b>;

  auto to_bitset64 = [](const uint64b u)
  {
    std::bitset<64> bits{u};
    return bits;
  };

  constexpr std::size_t n = 1'000'000'000;
  std::size_t num_of_normals = 0;
  std::size_t num_of_subnormals = 0;
  std::size_t num_of_specials = 0;
  std::mt19937_64 engine{123'456'789'0ull};
  Generator generator{0u, (std::numeric_limits<uint64b>::max)()};
  static_assert(sizeof(Generator::result_type) == 8);
  for (std::size_t i = 0; i < n; ++i) {
    const uint64b u = generator(engine);
    const double d = *zisc::reinterp<const double*>(&u);
    if (zisc::isNormal(d)) {
      ++num_of_normals;
    }
    else if (zisc::isSubnormal(d)) {
      ++num_of_subnormals;
    }
    else {
      ++num_of_specials;
      continue;
    }

    const float f = zisc::cast<float>(d);
    const Binary64 b64{u};
    {
      const float b32 = zisc::cast<float>(b64);
      const float expected = f;
      ASSERT_FLOAT_EQ(expected, b32)
          << f << " (" << to_bitset64(u) << ") conversion failed." << std::endl
          << "    expected: " << expected << std::endl
          << "    binary32: " << b32 << std::endl;
    }
  }
  std::cout << "normal values   : " << num_of_normals << "/" << n << std::endl;
  std::cout << "subnormal values:    " << num_of_subnormals << "/" << n << std::endl;
  std::cout << "special values  :    " << num_of_specials << "/" << n << std::endl;
}

TEST(Ieee754BinaryTest, DoubleRelationalConstantTest)
{
  using zisc::uint64b;
  using zisc::Binary64;
  using FLimit = std::numeric_limits<float>;

  {
    constexpr Binary64 lhs{0.0};
    constexpr Binary64 rhs{0.0};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_TRUE(result);
    }
  }
  {
    constexpr Binary64 lhs{0.0};
    constexpr Binary64 rhs{-0.0};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_TRUE(result);
    }
  }
  {
    constexpr Binary64 lhs{-0.0};
    constexpr Binary64 rhs{1.0};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_FALSE(result);
    }
  }
  {
    constexpr Binary64 lhs{-0.0};
    constexpr Binary64 rhs{(FLimit::max)()};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_FALSE(result);
    }
  }
  {
    constexpr Binary64 lhs{0.0};
    constexpr Binary64 rhs{-(FLimit::max)()};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_TRUE(result);
    }
  }
  {
    constexpr Binary64 lhs{-0.0};
    constexpr Binary64 rhs{-(FLimit::max)()};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_TRUE(result);
    }
  }
  {
    constexpr Binary64 lhs{(FLimit::min)()};
    constexpr Binary64 rhs{(FLimit::max)()};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_FALSE(result);
    }
  }
  {
    constexpr Binary64 lhs{FLimit::infinity()};
    constexpr Binary64 rhs{FLimit::infinity()};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_FALSE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_TRUE(result);
    }
  }
  {
    constexpr Binary64 lhs{FLimit::infinity()};
    constexpr Binary64 rhs{-FLimit::infinity()};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_TRUE(result);
    }
  }
  {
    constexpr Binary64 lhs{FLimit::infinity()};
    constexpr Binary64 rhs{(FLimit::max)()};
    {
      constexpr bool result = lhs != rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs > rhs;
      ASSERT_TRUE(result);
    }
    {
      constexpr bool result = lhs >= rhs;
      ASSERT_TRUE(result);
    }
  }
}

TEST(Ieee754BinaryTest, DoubleRelationalTest)
{
  using zisc::uint64b;
  using zisc::Binary64;
  using Generator = std::uniform_int_distribution<uint64b>;

  auto to_bitset64 = [](const double d)
  {
    const uint64b ul = *zisc::reinterp<const uint64b*>(&d);
    std::bitset<64> bits{ul};
    return bits;
  };

  constexpr std::size_t n = 1'000'000'000;
  std::size_t num_of_specials = 0;
  std::mt19937_64 engine{123'456'789'0ull};
  Generator generator{0u, std::numeric_limits<uint64b>::max()};
  static_assert(sizeof(Generator::result_type) == 8);
  for (std::size_t i = 0; i < n; ++i) {
    const uint64b ul = generator(engine);
    const double fl = *zisc::reinterp<const double*>(&ul);
    if (!(zisc::isFinite(fl) || zisc::isInf(fl))) {
      ++num_of_specials;
      continue;
    }
    const uint64b ur = generator(engine);
    const double fr = *zisc::reinterp<const double*>(&ur);
    if (!(zisc::isFinite(fr) || zisc::isInf(fr))) {
      ++num_of_specials;
      continue;
    }

    const Binary64 lhs{fl};
    const Binary64 rhs{fr};
    {
      const bool expected = fl == fr;
      const bool result = lhs == rhs;
      ASSERT_EQ(expected, result)
          << "lhs: " << fl << " (" << to_bitset64(fl) << ")" << std::endl
          << "rhs: " << fr << " (" << to_bitset64(fr) << ")" << std::endl;
    }
    {
      const bool expected = fl != fr;
      const bool result = lhs != rhs;
      ASSERT_EQ(expected, result)
          << "lhs: " << fl << " (" << to_bitset64(fl) << ")" << std::endl
          << "rhs: " << fr << " (" << to_bitset64(fr) << ")" << std::endl;
    }
    {
      const bool expected = fl > fr;
      const bool result = lhs > rhs;
      ASSERT_EQ(expected, result)
          << "lhs: " << fl << " (" << to_bitset64(fl) << ")" << std::endl
          << "rhs: " << fr << " (" << to_bitset64(fr) << ")" << std::endl;
    }
    {
      const bool expected = fl >= fr;
      const bool result = lhs >= rhs;
      ASSERT_EQ(expected, result)
          << "lhs: " << fl << " (" << to_bitset64(fl) << ")" << std::endl
          << "rhs: " << fr << " (" << to_bitset64(fr) << ")" << std::endl;
    }
  }
  std::cout << "special values: " << num_of_specials << "/" << n << std::endl;
}
