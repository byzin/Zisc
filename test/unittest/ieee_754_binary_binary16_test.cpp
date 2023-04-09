/*!
  \file ieee_754_binary_binary16_test.cpp
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
#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <tuple>
#include <type_traits>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/binary_serializer.hpp"
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/ieee_754_binary.hpp"
#include "zisc/zisc_config.hpp"
// Test
#include "math_test.hpp"

TEST(Ieee754BinaryTest, HalfBitSizeTest)
{
  using zisc::Binary16;
  static_assert(std::is_trivially_copyable_v<Binary16>,
                "Binary16 isn't trivially copyable.");
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

TEST(Ieee754BinaryTest, HalfLimitsTest)
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
    ASSERT_FALSE(traps);
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
  using Half = zisc::Binary16;
  using Limits = std::numeric_limits<Half>;
  using FloatBits = decltype(toBitset(1.0f));

  {
    constexpr Half min_b = (Limits::min)();
    constexpr float result = zisc::cast<float>(min_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0x38800000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half low_b = Limits::lowest();
    constexpr float result = zisc::cast<float>(low_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0xc77fe000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half max_b = (Limits::max)();
    constexpr float result = zisc::cast<float>(max_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0x477fe000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half max_b = Limits::epsilon();
    constexpr float result = zisc::cast<float>(max_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0x3a800000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half max_b = Limits::round_error();
    constexpr float result = zisc::cast<float>(max_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0x3f000000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half zero_b = Half::zero();
    constexpr float result = zisc::cast<float>(zero_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0x0ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half one_b = Half::one();
    constexpr float result = zisc::cast<float>(one_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0x3f800000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half infinity_b = Limits::infinity();
    constexpr float result = zisc::cast<float>(infinity_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0x7f800000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half infinity_b = -Limits::infinity();
    constexpr float result = zisc::cast<float>(infinity_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0xff800000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
  {
    constexpr Half denorm_min_b = Limits::denorm_min();
    constexpr float result = zisc::cast<float>(denorm_min_b);
    const auto r_bits = toBitset(result);
    const auto expected = FloatBits{0x33800000ull};
    ASSERT_EQ(expected, r_bits) << "Converting half to float failed.";
  }
}

namespace {

/*!
  \details No detailed description

  \tparam Source No description.
  \tparam Dest No description.
  \param [in] x_list No description.
  \param [in] ref_file_path No description.
  \param [in] src_label No description.
  \param [in] dst_label No description.
  */
template <zisc::TriviallyCopyable Source, zisc::TriviallyCopyable Dest>
void testHalf(const std::vector<Source>& x_list,
              const std::string_view ref_file_path,
              const std::string_view src_label,
              const std::string_view dst_label)
{
  using zisc::cast;

  const std::size_t n = x_list.size();

  // Load reference values
  std::ifstream reference_file{ref_file_path.data(), std::ios_base::binary};
  ASSERT_TRUE(reference_file.is_open()) << "Reference file not found: '"
                                        << ref_file_path.data() << "'.";
  std::vector<Dest> expected_list;
  expected_list.resize(n);
  zisc::BSerializer::read(expected_list.data(), &reference_file, sizeof(Dest) * n);

  // Test
  for (std::size_t i = 0; i < n; ++i) {
    const Source x = x_list[i];
    const auto y = cast<Dest>(x);
    const auto y_bits = toBitset(y);
    const Dest expected = expected_list[i];
    const auto e_bits = toBitset(expected);
    ASSERT_EQ(e_bits, y_bits)
        << "Converting " << src_label << " to " << dst_label << " failed.";
  }

  // Special cases
  const auto n_specials = readValue<zisc::int32b>(&reference_file);
  for (int spec_index = 0; spec_index < n_specials; ++spec_index) {
    const auto x = readValue<Source>(&reference_file);
    const auto y = cast<Dest>(x);
    const auto y_bits = toBitset(y);
    const auto expected = readValue<Dest>(&reference_file);
    const auto e_bits = toBitset(expected);
    ASSERT_EQ(e_bits, y_bits)
        << "Converting " << src_label << " to " << dst_label << " failed.";
  }
}

} /* namespace */


TEST(Ieee754BinaryTest, Half2FloatTest)
{
  using Half = zisc::Binary16;
  // Load input values
  auto x_list = loadAllHalfXList<Half>();
  // Set reference file path
  const std::string_view ref_file_path = "resources/math_half2float_reference.bin";

  ::testHalf<Half, float>(x_list, ref_file_path, "half", "float");
}

TEST(Ieee754BinaryTest, Half2FloatSubnormalTest)
{
  using Half = zisc::Binary16;
  // Load input values
  auto x_list = loadAllHalfSubnormalXList<Half>();
  // Set reference file path
  const std::string_view ref_file_path = "resources/math_half2float_subh_reference.bin";

  ::testHalf<Half, float>(x_list, ref_file_path, "half", "float");
}

TEST(Ieee754BinaryTest, Float2HalfTest)
{
  using Half = zisc::Binary16;
  // Load input values
  auto x_list = loadAllHalfXList<float>();
  // Set reference file path
  const std::string_view ref_file_path = "resources/math_float2half_reference.bin";

  ::testHalf<float, Half>(x_list, ref_file_path, "float", "half");
}

TEST(Ieee754BinaryTest, Float2HalfSubnormalTest)
{
  using Half = zisc::Binary16;
  // Load input values
  auto x_list = loadAllHalfSubnormalXList<float>();
  // Set reference file path
  const std::string_view ref_file_path = "resources/math_float2half_subf_reference.bin";

  ::testHalf<float, Half>(x_list, ref_file_path, "float", "half");
}

TEST(Ieee754BinaryTest, HalfArithmeticTest)
{
  using HalfT = zisc::Binary16;

  // Load reference values
  const std::string_view ref_file_path = "resources/half_arithmetic_reference.bin";
  std::ifstream reference_file{ref_file_path.data(), std::ios_base::binary};
  ASSERT_TRUE(reference_file.is_open()) << "Reference file not found: `"
                                        << ref_file_path.data() << "'.";

  std::size_t n = 0;
  zisc::BSerializer::read(&n, &reference_file);
  std::cout << "  n = " << n << std::endl;

  // Make a normal list
  std::vector<HalfT> normal_list;
  normal_list.resize(2 * n);
  zisc::BSerializer::read(normal_list.data(), &reference_file, normal_list.size() * sizeof(HalfT));
  // Subnormal test
  std::vector<HalfT> subnormal_list;
  subnormal_list.resize(2 * n);
  zisc::BSerializer::read(subnormal_list.data(), &reference_file, subnormal_list.size() * sizeof(HalfT));

  enum class OpType
  {
    kAddition,
    kSubtraction,
    kMultiplication,
    kDivision
  };

  auto test = [](const std::vector<HalfT>& in_list,
                 const std::vector<zisc::uint16b>& expected_list,
                 const OpType type,
                 const std::string_view op_name) noexcept
  {
    std::cout << "  " << op_name << std::endl;
    for (std::size_t i = 0; i < expected_list.size(); ++i) {
      HalfT x = in_list[2 * i + 0];
      const HalfT y = in_list[2 * i + 1];
      {
        HalfT result{};
        switch (type) {
         case OpType::kAddition:
          result = x + y;
          break;
         case OpType::kSubtraction:
          result = x - y;
          break;
         case OpType::kMultiplication:
          result = x * y;
          break;
         case OpType::kDivision:
          result = x / y;
          break;
         default:
          break;
        }
        const zisc::uint16b expected = expected_list[i];
        const auto bresult = zisc::bit_cast<zisc::uint16b>(result);
        ASSERT_EQ(expected, bresult)
            << "half '" << op_name << "' failed."
            << std::scientific << " x=" << static_cast<float>(x) << ", y=" << static_cast<float>(y)
            << std::endl;
      }
      {
        switch (type) {
         case OpType::kAddition:
          x += y;
          break;
         case OpType::kSubtraction:
          x -= y;
          break;
         case OpType::kMultiplication:
          x *= y;
          break;
         case OpType::kDivision:
          x /= y;
          break;
         default:
          break;
        }
        const HalfT& result = x;
        const zisc::uint16b expected = expected_list[i];
        const auto bresult = zisc::bit_cast<zisc::uint16b>(result);
        ASSERT_EQ(expected, bresult)
            << "half '" << op_name << "' failed."
            << std::scientific << " x=" << static_cast<float>(x) << ", y=" << static_cast<float>(y)
            << std::endl;
      }
    }
  };

  std::vector<zisc::uint16b> expected_list;
  expected_list.resize(n);

  zisc::BSerializer::read(expected_list.data(), &reference_file, expected_list.size() * sizeof(HalfT));
  test(normal_list, expected_list, OpType::kAddition, "normal addition");

  zisc::BSerializer::read(expected_list.data(), &reference_file, expected_list.size() * sizeof(HalfT));
  test(normal_list, expected_list, OpType::kSubtraction, "normal subtraction");

  zisc::BSerializer::read(expected_list.data(), &reference_file, expected_list.size() * sizeof(HalfT));
  test(normal_list, expected_list, OpType::kMultiplication, "normal multiplication");

  zisc::BSerializer::read(expected_list.data(), &reference_file, expected_list.size() * sizeof(HalfT));
  test(normal_list, expected_list, OpType::kDivision, "normal division");

  zisc::BSerializer::read(expected_list.data(), &reference_file, expected_list.size() * sizeof(HalfT));
  test(subnormal_list, expected_list, OpType::kAddition, "subnormal addition");

  zisc::BSerializer::read(expected_list.data(), &reference_file, expected_list.size() * sizeof(HalfT));
  test(subnormal_list, expected_list, OpType::kSubtraction, "subnormal subtraction");

  zisc::BSerializer::read(expected_list.data(), &reference_file, expected_list.size() * sizeof(HalfT));
  test(subnormal_list, expected_list, OpType::kMultiplication, "subnormal multiplication");

  zisc::BSerializer::read(expected_list.data(), &reference_file, expected_list.size() * sizeof(HalfT));
  test(subnormal_list, expected_list, OpType::kDivision, "subnormal division");
}
