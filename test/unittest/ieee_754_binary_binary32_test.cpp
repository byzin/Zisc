/*!
  \file ieee_754_binary_binary32_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string_view>
#include <tuple>
#include <type_traits>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/bit.hpp"
#include "zisc/ieee_754_binary.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

TEST(Ieee754BinaryTest, FloatBitSizeTest)
{
  using zisc::Binary32;
  static_assert(std::is_trivially_copyable_v<Binary32>,
                "Binary32 isn't trivially copyable.");
  ASSERT_EQ(4, sizeof(Binary32));
  constexpr auto sign_mask = Binary32::signBitMask();
  ASSERT_EQ(0b1000'0000'0000'0000'0000'0000'0000'0000u, sign_mask);
  constexpr std::size_t expt_size = Binary32::exponentBitSize();
  ASSERT_EQ(8, expt_size);
  constexpr auto expt_mask = Binary32::exponentBitMask();
  ASSERT_EQ(0b0111'1111'1000'0000'0000'0000'0000'0000u, expt_mask);
  constexpr std::size_t sig_size = Binary32::significandBitSize();
  ASSERT_EQ(23, sig_size);
  constexpr auto sig_mask = Binary32::significandBitMask();
  ASSERT_EQ(0b0000'0000'0111'1111'1111'1111'1111'1111u, sig_mask);
  ASSERT_EQ(32, 1 + expt_size + sig_size);
}

TEST(Ieee754BinaryTest, FloatLimitsTest)
{
  using zisc::uint32b;
  using zisc::Binary32;
  using Limits = std::numeric_limits<Binary32>;
  using FLimits = std::numeric_limits<float>;

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
    ASSERT_EQ(std::round_to_nearest, round_style);
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
    ASSERT_FALSE(traps);
  }
  {
    constexpr bool tinyness_before = Limits::tinyness_before;
    ASSERT_FALSE(tinyness_before);
  }

  auto print_float_bits = [](const std::string_view name, const float f) noexcept
  {
    const auto u = zisc::bit_cast<zisc::uint32b>(f);
    std::cout << "    " << std::setw(16) << std::setfill(' ') << name << ": "
              << std::bitset<32>{zisc::cast<unsigned long long>(u)}
              << std::endl;
  };

  using zisc::cast;

  {
    constexpr float min_f = (FLimits::min)();
    constexpr Binary32 min_b = (Limits::min)();
    constexpr auto result = cast<float>(min_b);
    ASSERT_EQ(min_f, result);
    ASSERT_TRUE(zisc::isFinite(min_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isNormal(min_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(min_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isInf(min_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNan(min_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isFinite(min_b)) << "Binary classification function failed.";
    ASSERT_TRUE(zisc::isNormal(min_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(min_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isInf(min_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNan(min_b)) << "Binary classification function failed.";
  }
  {
    constexpr float low_f = FLimits::lowest();
    constexpr Binary32 low_b = Limits::lowest();
    constexpr auto result = cast<float>(low_b);
    ASSERT_EQ(low_f, result);
    ASSERT_TRUE(zisc::isFinite(low_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isNormal(low_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(low_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isInf(low_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNan(low_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isFinite(low_b)) << "Binary classification function failed.";
    ASSERT_TRUE(zisc::isNormal(low_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(low_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isInf(low_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNan(low_b)) << "Binary classification function failed.";
  }
  {
    constexpr float max_f = (FLimits::max)();
    constexpr Binary32 max_b = (Limits::max)();
    constexpr auto result = cast<float>(max_b);
    ASSERT_EQ(max_f, result);
    ASSERT_TRUE(zisc::isFinite(max_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isNormal(max_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(max_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isInf(max_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNan(max_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isFinite(max_b)) << "Binary classification function failed.";
    ASSERT_TRUE(zisc::isNormal(max_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(max_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isInf(max_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNan(max_b)) << "Binary classification function failed.";
  }
  {
    constexpr float eps_f = FLimits::epsilon();
    constexpr Binary32 eps_b = Limits::epsilon();
    constexpr auto result = cast<float>(eps_b);
    ASSERT_EQ(eps_f, result);
    ASSERT_TRUE(zisc::isFinite(eps_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isNormal(eps_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(eps_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isInf(eps_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNan(eps_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isFinite(eps_b)) << "Binary classification function failed.";
    ASSERT_TRUE(zisc::isNormal(eps_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(eps_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isInf(eps_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNan(eps_b)) << "Binary classification function failed.";
  }
  {
    constexpr float round_err_f = FLimits::round_error();
    constexpr Binary32 round_err_b = Limits::round_error();
    constexpr auto result = cast<float>(round_err_b);
    ASSERT_EQ(round_err_f, result);
  }
  {
    constexpr float inf_f = FLimits::infinity();
    constexpr Binary32 inf_b = Limits::infinity();
    constexpr auto result = cast<float>(inf_b);
    ASSERT_EQ(inf_f, result);
    ASSERT_FALSE(zisc::isFinite(inf_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNormal(inf_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(inf_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isInf(inf_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNan(inf_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isFinite(inf_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNormal(inf_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(inf_b)) << "Binary classification function failed.";
    ASSERT_TRUE(zisc::isInf(inf_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNan(inf_b)) << "Binary classification function failed.";
  }
  {
    const float qnan_f = FLimits::quiet_NaN();
    print_float_bits("quiet_NaN", qnan_f);
    constexpr Binary32 qnan_b = Limits::quiet_NaN();
    const auto result = cast<float>(qnan_b);
    EXPECT_TRUE(std::isnan(result));
    ASSERT_FALSE(zisc::isFinite(qnan_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNormal(qnan_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(qnan_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isInf(qnan_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isNan(qnan_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isFinite(qnan_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNormal(qnan_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(qnan_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isInf(qnan_b)) << "Binary classification function failed.";
    ASSERT_TRUE(zisc::isNan(qnan_b)) << "Binary classification function failed.";
  }
  {
    const float snan_f = FLimits::signaling_NaN();
    print_float_bits("signaling_NaN", snan_f);
    constexpr Binary32 snan_b = Limits::signaling_NaN();
    const auto result = cast<float>(snan_b);
    EXPECT_TRUE(std::isnan(result));
    ASSERT_FALSE(zisc::isFinite(snan_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNormal(snan_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(snan_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isInf(snan_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isNan(snan_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isFinite(snan_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNormal(snan_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isSubnormal(snan_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isInf(snan_b)) << "Binary classification function failed.";
    ASSERT_TRUE(zisc::isNan(snan_b)) << "Binary classification function failed.";
  }
  {
    constexpr float dmin_f = FLimits::denorm_min();
    constexpr Binary32 dmin_b = Limits::denorm_min();
    constexpr auto result = cast<float>(dmin_b);
    ASSERT_EQ(dmin_f, result);
    ASSERT_TRUE(zisc::isFinite(dmin_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNormal(dmin_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isSubnormal(dmin_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isInf(dmin_f)) << "Float classification function failed.";
    ASSERT_FALSE(zisc::isNan(dmin_f)) << "Float classification function failed.";
    ASSERT_TRUE(zisc::isFinite(dmin_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNormal(dmin_b)) << "Binary classification function failed.";
    ASSERT_TRUE(zisc::isSubnormal(dmin_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isInf(dmin_b)) << "Binary classification function failed.";
    ASSERT_FALSE(zisc::isNan(dmin_b)) << "Binary classification function failed.";
  }
}

TEST(Ieee754BinaryTest, Float2DoubleConstantTest)
{
  using zisc::uint32b;
  using zisc::uint64b;
  using zisc::Binary32;
  using zisc::Binary64;
  using FLimit = std::numeric_limits<float>;

  using zisc::cast;

  {
    constexpr float f = 0.0f;
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = -0.0f;
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = 1.0f;
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = -1.0f;
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = 100.0f;
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = -100.0f;
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = FLimit::denorm_min();
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = -FLimit::denorm_min();
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = (FLimit::min)();
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = -(FLimit::min)();
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = (FLimit::max)();
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = -(FLimit::max)();
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = FLimit::epsilon();
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = -FLimit::epsilon();
    constexpr auto d = cast<double>(f);
    constexpr Binary32 b32{f};
    constexpr auto b64 = cast<double>(b32);
    ASSERT_DOUBLE_EQ(d, b64);
  }
  {
    constexpr float f = FLimit::infinity();
    constexpr Binary32 b32{f};
    constexpr auto b64d = cast<double>(b32);
    ASSERT_TRUE(zisc::isInf(b64d));
  }
  {
    constexpr float f = -FLimit::infinity();
    constexpr Binary32 b32{f};
    constexpr auto b64d = cast<double>(b32);
    ASSERT_TRUE(zisc::isInf(b64d));
  }
}

TEST(Ieee754BinaryTest, Float2DoubleTest)
{
  using zisc::uint32b;
  using zisc::uint64b;
  using zisc::Binary32;
  using zisc::Binary64;
  using Generator = std::uniform_int_distribution<uint32b>;

  auto to_bitset32 = [](const uint32b u)
  {
    const auto ul = zisc::cast<uint64b>(u);
    std::bitset<32> bits{ul};
    return bits;
  };

  constexpr std::size_t n = 1'000'000'000;
  std::size_t num_of_normals = 0;
  std::size_t num_of_subnormals = 0;
  std::size_t num_of_specials = 0;
  std::mt19937 engine{123'456'789};
  Generator generator{0u, (std::numeric_limits<uint32b>::max)()};
  static_assert(sizeof(Generator::result_type) == 4);
  for (std::size_t i = 0; i < n; ++i) {
    const uint32b u = generator(engine);
    const auto f = zisc::bit_cast<float>(u);
    if (zisc::isNormal(f)) {
      ++num_of_normals;
    }
    else if (zisc::isSubnormal(f)) {
      ++num_of_subnormals;
    }
    else {
      ++num_of_specials;
      continue;
    }

    const auto d = zisc::cast<double>(f);
    const Binary32 b32{u};
    {
      const auto b64 = zisc::cast<double>(b32);
      const double expected = d;
      ASSERT_DOUBLE_EQ(expected, b64)
          << f << " (" << to_bitset32(u) << ") conversion failed." << std::endl
          << "    expected: " << expected << std::endl
          << "    binary64: " << b64 << std::endl;
    }
  }
  std::cout << "normal values   : " << num_of_normals << "/" << n << std::endl;
  std::cout << "subnormal values:   " << num_of_subnormals << "/" << n << std::endl;
  std::cout << "special values  :   " << num_of_specials << "/" << n << std::endl;
}

TEST(Ieee754BinaryTest, FloatRelationalConstantTest)
{
  using zisc::uint32b;
  using zisc::Binary32;
  using FLimit = std::numeric_limits<float>;

  {
    constexpr Binary32 lhs{0.0f};
    constexpr Binary32 rhs{0.0f};
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
    constexpr Binary32 lhs{0.0f};
    constexpr Binary32 rhs{-0.0f};
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
    constexpr Binary32 lhs{-0.0f};
    constexpr Binary32 rhs{1.0f};
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
    constexpr Binary32 lhs{-0.0f};
    constexpr Binary32 rhs{(FLimit::max)()};
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
    constexpr Binary32 lhs{0.0f};
    constexpr Binary32 rhs{-(FLimit::max)()};
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
    constexpr Binary32 lhs{-0.0f};
    constexpr Binary32 rhs{-(FLimit::max)()};
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
    constexpr Binary32 lhs{(FLimit::min)()};
    constexpr Binary32 rhs{(FLimit::max)()};
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
    constexpr Binary32 lhs{FLimit::infinity()};
    constexpr Binary32 rhs{FLimit::infinity()};
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
    constexpr Binary32 lhs{FLimit::infinity()};
    constexpr Binary32 rhs{-FLimit::infinity()};
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
    constexpr Binary32 lhs{FLimit::infinity()};
    constexpr Binary32 rhs{(FLimit::max)()};
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

TEST(Ieee754BinaryTest, FloatRelationalTest)
{
  using zisc::uint32b;
  using zisc::uint64b;
  using zisc::Binary32;
  using Generator = std::uniform_int_distribution<uint32b>;

  auto to_bitset32 = [](const float f)
  {
    const auto u = zisc::bit_cast<uint32b>(f);
    const auto ul = zisc::cast<uint64b>(u);
    std::bitset<32> bits{ul};
    return bits;
  };

  constexpr std::size_t n = 1'000'000'000;
  std::size_t num_of_specials = 0;
  std::mt19937 engine{123'456'789};
  Generator generator{0u, (std::numeric_limits<uint32b>::max)()};
  static_assert(sizeof(Generator::result_type) == 4);
  for (std::size_t i = 0; i < n; ++i) {
    const uint32b ul = generator(engine);
    const auto fl = zisc::bit_cast<float>(ul);
    if (!(zisc::isFinite(fl) || zisc::isInf(fl))) {
      ++num_of_specials;
      continue;
    }
    const uint32b ur = generator(engine);
    const auto fr = zisc::bit_cast<float>(ur);
    if (!(zisc::isFinite(fr) || zisc::isInf(fr))) {
      ++num_of_specials;
      continue;
    }

    const Binary32 lhs{fl};
    const Binary32 rhs{fr};
    {
      const bool expected = fl == fr;
      const bool result = lhs == rhs;
      ASSERT_EQ(expected, result)
          << "lhs: " << fl << " (" << to_bitset32(fl) << ")" << std::endl
          << "rhs: " << fr << " (" << to_bitset32(fr) << ")" << std::endl;
    }
    {
      const bool expected = fl != fr;
      const bool result = lhs != rhs;
      ASSERT_EQ(expected, result)
          << "lhs: " << fl << " (" << to_bitset32(fl) << ")" << std::endl
          << "rhs: " << fr << " (" << to_bitset32(fr) << ")" << std::endl;
    }
    {
      const bool expected = fl > fr;
      const bool result = lhs > rhs;
      ASSERT_EQ(expected, result)
          << "lhs: " << fl << " (" << to_bitset32(fl) << ")" << std::endl
          << "rhs: " << fr << " (" << to_bitset32(fr) << ")" << std::endl;
    }
    {
      const bool expected = fl >= fr;
      const bool result = lhs >= rhs;
      ASSERT_EQ(expected, result)
          << "lhs: " << fl << " (" << to_bitset32(fl) << ")" << std::endl
          << "rhs: " << fr << " (" << to_bitset32(fr) << ")" << std::endl;
    }
  }
  std::cout << "special values: " << num_of_specials << "/" << n << std::endl;
}

TEST(Ieee754BinaryTest, FloatArithmeticTest)
{
  using zisc::Binary32;

  const auto x = static_cast<Binary32>(2.0f);
  const auto y = static_cast<Binary32>(2.0f);

  {
    const Binary32 z = x + y;
    ASSERT_FLOAT_EQ(4.0f, static_cast<float>(z));
  }
  {
    const Binary32 z = x - y;
    ASSERT_FLOAT_EQ(0.0f, static_cast<float>(z));
  }
  {
    const Binary32 z = x * y;
    ASSERT_FLOAT_EQ(4.0f, static_cast<float>(z));
  }
  {
    const Binary32 z = x / y;
    ASSERT_FLOAT_EQ(1.0f, static_cast<float>(z));
  }
}
