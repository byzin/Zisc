/*!
  \file xoroshiro128_plus_engine_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <string>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/split_mix64_engine.hpp"
#include "zisc/xoroshiro128_plus_engine.hpp"

namespace {

void testXoroshiro128Plus()
{
  zisc::Xoroshiro128PlusEngine xoroshiro;
  // Initialize
  {
    zisc::SplitMix64Engine split_mix{static_cast<zisc::uint64>(0x0ddc0ffeebadf00dull)};
    const auto s0 = split_mix();
    const auto s1 = split_mix();
    xoroshiro.setSeed(s0, s1);
  }

  // Test
  constexpr int n = 10;
  zisc::uint64 expected[n] = {
      11814330020949985800ull,
      11817088786836023749ull,
      1654166990350674155ull,
      14112748191344281834ull,
      4288295283113472773ull,
      8391955421631067594ull,
      168274855724945977ull,
      2815117763357611551ull,
      12187186948608395331ull,
      10629044371437376348ull};
  for (int i = 0; i < n; ++i) {
    ASSERT_EQ(expected[i], xoroshiro())
        << "The implementation of xoroshiro128+ is wrong.";
  }
}

void testFloatDistribution()
{
  zisc::Xoroshiro128PlusEngine xoroshiro;
  // Initialize
  {
    zisc::SplitMix64Engine split_mix{static_cast<zisc::uint64>(0x0ddc0ffeebadf00dull)};
    const auto s0 = split_mix();
    const auto s1 = split_mix();
    xoroshiro.setSeed(s0, s1);
  }

  constexpr int n = 1'000'000'00;
  constexpr int section = 20;
  constexpr int expected = n / section;
  constexpr int error = 6000;

  std::array<int, section> result;
  result.fill(0);
  for (int i = 0; i < n; ++i) {
    const double u = static_cast<double>(xoroshiro.generate01());
    ASSERT_LE(0.0, u) << "The u is less than 0";
    ASSERT_GT(1.0, u) << "The u is greater than 1";
    const int index = static_cast<int>(static_cast<double>(section) * u);
    ++result[index];
  }
  for (int i = 0; i < section; ++i) {
    const int diff = std::abs(expected - result[i]);
    ASSERT_GT(error, diff)
        << "Float isn't distributed uniformly: section[" << i << "]";
  }
}

} // namespace

TEST(Xoroshiro128PlusEngineTest, GenerateTest)
{
  constexpr std::size_t s = sizeof(zisc::Xoroshiro128PlusEngine);
  static_assert(s == 16, "The size of xoroshiro128+ is wrong.");
  constexpr std::size_t a = alignof(zisc::Xoroshiro128PlusEngine);
  static_assert(a == 8, "The alignment of xoroshiro128+ is wrong.");

  ::testXoroshiro128Plus();
  ::testFloatDistribution();
}
