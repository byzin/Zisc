/*!
  \file ieee_754_binary_binary16_test.cpp
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
#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <fstream>
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

TEST(Ieee754BinaryTest, Half2FloatConstantTest)
{
  using zisc::uint16b;
  using zisc::uint32b;
  using zisc::Binary16;
  using Limits = std::numeric_limits<Binary16>;

  union Float
  {
    Float(const uint32b v) : u_{v} {}
    float f_;
    uint32b u_;
  };

  auto to_float = [](const uint32b u) noexcept
  {
    const Float f{u};
    return f.f_;
  };

  {
    constexpr Binary16 min_b = (Limits::min)();
    constexpr float result = zisc::cast<float>(min_b);
    const float expected = to_float(0x38800000u);
    ASSERT_FLOAT_EQ(expected, result);
  }
  {
    constexpr Binary16 low_b = Limits::lowest();
    constexpr float result = zisc::cast<float>(low_b);
    const float expected = to_float(0xc77fe000u);
    ASSERT_FLOAT_EQ(expected, result);
  }
  {
    constexpr Binary16 max_b = (Limits::max)();
    constexpr float result = zisc::cast<float>(max_b);
    const float expected = to_float(0x477fe000u);
    ASSERT_FLOAT_EQ(expected, result);
  }
  {
    constexpr Binary16 zero_b = Binary16::zero();
    constexpr float result = zisc::cast<float>(zero_b);
    const float expected = to_float(0x0u);
    ASSERT_FLOAT_EQ(expected, result);
  }
  {
    constexpr Binary16 one_b = Binary16::one();
    constexpr float result = zisc::cast<float>(one_b);
    const float expected = to_float(0x3f800000u);
    ASSERT_FLOAT_EQ(expected, result);
  }
  {
    constexpr Binary16 infinity_b = Limits::infinity();
    constexpr float result = zisc::cast<float>(infinity_b);
    const float expected = to_float(0x7f800000u);
    ASSERT_FLOAT_EQ(expected, result);
  }
  {
    constexpr Binary16 infinity_b = -Limits::infinity();
    constexpr float result = zisc::cast<float>(infinity_b);
    const float expected = to_float(0xff800000u);
    ASSERT_FLOAT_EQ(expected, result);
  }
  {
    constexpr Binary16 denorm_min_b = Limits::denorm_min();
    constexpr float result = zisc::cast<float>(denorm_min_b);
    const float expected = to_float(0x33800000u);
    ASSERT_FLOAT_EQ(expected, result);
  }
}

namespace {

template <int n>
void testHalf2Float(const std::string_view reference_file_path)
{
  using zisc::uint16b;
  using zisc::uint32b;
  using zisc::Binary16;
  using zisc::Binary32;

  union Float
  {
    Float() : u_{0} {}
    float f_;
    uint32b u_;
  };

  std::ifstream reference_file{reference_file_path.data()};
  for (int i = 0; i < n; ++i) {
    Float v;
    reference_file >> std::hex >> v.u_;

    const Binary32 f{v.u_};
    const Binary16 h = zisc::cast<Binary16>(f);
    uint16b expected1 = 0;
    reference_file >> std::hex >> expected1;
    ASSERT_EQ(expected1, h.bits());

    const float f2 = zisc::cast<float>(h);
    Float expected2;
    reference_file >> std::hex >> expected2.u_;
    ASSERT_FLOAT_EQ(expected2.f_, f2);
  }
}

} // namespace 

TEST(Ieee754BinaryTest, Half2FloatTest)
{
  ::testHalf2Float<4096>("resources/half_float_reference.txt");
}

TEST(Ieee754BinaryTest, Half2FloatNegativeTest)
{
  ::testHalf2Float<4096>("resources/half_float_negative_reference.txt");
}

TEST(Ieee754BinaryTest, Half2FloatSubnormalTest)
{
  ::testHalf2Float<1024>("resources/half_float_subnormal_reference.txt");
}
