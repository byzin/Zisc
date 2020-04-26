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

TEST(Ieee754BinaryTest, Double2FloatConstantTest)
{
  using zisc::uint32b;
  using zisc::uint64b;
  using zisc::Binary32;
  using zisc::Binary64;
  using FLimit = std::numeric_limits<float>;
  using DLimit = std::numeric_limits<double>;

  {
    constexpr double d = 0.0;
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
//  {
//    constexpr double d = -0.0;
//    constexpr float f = zisc::cast<float>(d);
//    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
//    {
//      const float data = f;
//      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
//      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
//      ASSERT_EQ(expected, b32.bits());
//    }
//    {
//      constexpr float b32 = zisc::castBinary<float>(b64);
//      ASSERT_FLOAT_EQ(f, b32);
//    }
//  }
  {
    constexpr double d = 1.0;
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = -1.0;
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = 100.0;
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = -100.0;
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = DLimit::denorm_min();
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = -DLimit::denorm_min();
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = zisc::cast<double>(FLimit::denorm_min());
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = zisc::cast<double>(-FLimit::denorm_min());
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = DLimit::min();
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = -DLimit::min();
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = zisc::cast<double>(FLimit::min());
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = zisc::cast<double>(-FLimit::min());
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = DLimit::max();
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = -DLimit::max();
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = zisc::cast<double>(FLimit::max());
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = zisc::cast<double>(-FLimit::max());
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = DLimit::epsilon();
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = -DLimit::epsilon();
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = zisc::cast<double>(FLimit::epsilon());
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = zisc::cast<double>(-FLimit::epsilon());
    constexpr float f = zisc::cast<float>(d);
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    {
      const float data = f;
      const uint32b expected = *zisc::treatAs<const uint32b*>(&data);
      constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
      ASSERT_EQ(expected, b32.bits());
    }
    {
      constexpr float b32 = zisc::castBinary<float>(b64);
      ASSERT_FLOAT_EQ(f, b32);
    }
  }
  {
    constexpr double d = DLimit::infinity();
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
    ASSERT_TRUE(zisc::isInf(b32));
    constexpr float b32f = zisc::castBinary<float>(b64);
    ASSERT_TRUE(zisc::isInf(b32f));
  }
  {
    constexpr double d = -DLimit::infinity();
    constexpr Binary64 b64 = zisc::castBinary<Binary64>(d);
    constexpr Binary32 b32 = zisc::castBinary<Binary32>(b64);
    ASSERT_TRUE(zisc::isInf(b32));
    constexpr float b32f = zisc::castBinary<float>(b64);
    ASSERT_TRUE(zisc::isInf(b32f));
  }
}

TEST(Ieee754BinaryTest, Double2FloatTest)
{
  using zisc::uint32b;
  using zisc::uint64b;
  using zisc::Binary32;
  using zisc::Binary64;
  using Generator = std::uniform_int_distribution<uint64b>;

  auto to_bitset32 = [](const uint32b u)
  {
    uint64b ul = zisc::cast<uint64b>(u);
    std::bitset<32> bits{ul};
    return bits;
  };

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
  Generator generator{0u, std::numeric_limits<uint64b>::max()};
  static_assert(sizeof(Generator::result_type) == 8);
  for (std::size_t i = 0; i < n; ++i) {
    const uint64b u = generator(engine);
    const double d = *zisc::treatAs<const double*>(&u);
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
      const Binary32 b32 = zisc::castBinary<Binary32>(b64);
      const uint32b expected = *zisc::treatAs<const uint32b*>(&f);
      ASSERT_EQ(expected, b32.bits())
          << f << " (" << to_bitset64(u) << ") conversion failed." << std::endl
          << "    expected: " << to_bitset32(expected) << std::endl
          << "    binary32: " << to_bitset32(b32.bits()) << std::endl;
    }
    {
      const float b32 = zisc::castBinary<float>(b64);
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(0.0);
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(0.0);
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(0.0);
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(-0.0);
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(-0.0);
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(1.0);
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(-0.0);
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(FLimit::max());
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(0.0);
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(-FLimit::max());
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(-0.0);
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(-FLimit::max());
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(FLimit::min());
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(FLimit::max());
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(FLimit::infinity());
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(FLimit::infinity());
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(FLimit::infinity());
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(-FLimit::infinity());
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
    constexpr Binary64 lhs = zisc::castBinary<Binary64>(FLimit::infinity());
    constexpr Binary64 rhs = zisc::castBinary<Binary64>(FLimit::max());
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
    const uint64b ul = *zisc::treatAs<const uint64b*>(&d);
    std::bitset<64> bits{ul};
    return bits;
  };

  constexpr std::size_t n = 1'000'000'000;
  std::mt19937_64 engine{123'456'789'0ull};
  Generator generator{0u, std::numeric_limits<uint64b>::max()};
  static_assert(sizeof(Generator::result_type) == 8);
  for (std::size_t i = 0; i < n; ++i) {
    const uint64b ul = generator(engine);
    const double fl = *zisc::treatAs<const double*>(&ul);
    if (!(zisc::isFinite(fl) || zisc::isInf(fl)))
      continue;
    const uint64b ur = generator(engine);
    const double fr = *zisc::treatAs<const double*>(&ur);
    if (!(zisc::isFinite(fr) || zisc::isInf(fr)))
      continue;

    const Binary64 lhs = zisc::castBinary<Binary64>(fl);
    const Binary64 rhs = zisc::castBinary<Binary64>(fr);
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
}
