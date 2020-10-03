/*!
  \file algorithm_test.cpp
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
#include <algorithm>
#include <bitset>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/arith_array.hpp"
#include "zisc/stopwatch.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/random/pcg_engine.hpp"

namespace {

std::vector<zisc::uint64b> makeSortedArray(const zisc::uint64b n,
                                           zisc::PcgLcgRxsMXs64& rng_engine) noexcept
{
  // Make a sorted array
  std::vector<zisc::uint64b> tmp;
  tmp.resize(n);
  for (zisc::uint64b i = 0; i < n; ++i) {
    zisc::uint64b v = rng_engine.generate();
    v = zisc::clamp(v,
                    std::numeric_limits<zisc::uint64b>::min() + 1,
                    std::numeric_limits<zisc::uint64b>::max() - 1);
    tmp[i] = v;
  }
  std::sort(tmp.begin(), tmp.end());

  // Make a unique array
  std::vector<zisc::uint64b> array;
  std::unique_copy(tmp.begin(), tmp.end(), std::back_inserter(array));

  return array;
}

}

TEST(AlgorithmTest, clampTest)
{
  constexpr double min_value = -1.0;
  constexpr double max_value = 1.0;
  {
    constexpr double value = -2.0;
    ASSERT_EQ(min_value, zisc::clamp(value, min_value, max_value));
  }
  {
    constexpr double value = 0.0;
    ASSERT_EQ(value, zisc::clamp(value, min_value, max_value));
  }
  {
    constexpr double value = 2.0;
    ASSERT_EQ(max_value, zisc::clamp(value, min_value, max_value));
  }
}

TEST(AlgorithmTest, isNegativeTest)
{
  {
    constexpr int value = -1;
    ASSERT_TRUE(zisc::isNegative(value));
  }
  {
    constexpr int value = 0;
    ASSERT_FALSE(zisc::isNegative(value));
  }
  {
    constexpr int value = 1;
    ASSERT_FALSE(zisc::isNegative(value));
  }
}

TEST(AlgorithmTest, isOddTest)
{
  {
    constexpr int value = 1;
    ASSERT_TRUE(zisc::isOdd(value));
  }
  {
    constexpr int value = 2;
    ASSERT_FALSE(zisc::isOdd(value));
  }
}

TEST(AlgorithmTest, isInBoundsTest)
{
  // (0, 1) test
  {
    auto is_in_bounds = [](const double value)
    {
      return zisc::isInOpenBounds(value, 0.0, 1.0);
    };
    ASSERT_FALSE(is_in_bounds(-1.0));
    ASSERT_FALSE(is_in_bounds(0.0));
    ASSERT_TRUE(is_in_bounds(0.5));
    ASSERT_FALSE(is_in_bounds(1.0));
    ASSERT_FALSE(is_in_bounds(2.0));
  }
  // [0, 1) test
  {
    auto is_in_bounds = [](const double value)
    {
      return zisc::isInBounds(value, 0.0, 1.0);
    };
    ASSERT_FALSE(is_in_bounds(-1.0));
    ASSERT_TRUE(is_in_bounds(0.0));
    ASSERT_TRUE(is_in_bounds(0.5));
    ASSERT_FALSE(is_in_bounds(1.0));
    ASSERT_FALSE(is_in_bounds(2.0));
  }
  // [0, 1] test
  {
    auto is_in_bounds = [](const double value)
    {
      return zisc::isInClosedBounds(value, 0.0, 1.0);
    };
    ASSERT_FALSE(is_in_bounds(-1.0));
    ASSERT_TRUE(is_in_bounds(0.0));
    ASSERT_TRUE(is_in_bounds(0.5));
    ASSERT_TRUE(is_in_bounds(1.0));
    ASSERT_FALSE(is_in_bounds(2.0));
  }
}

TEST(AlgorithmTest, minMaxTest)
{
  constexpr int v1 = -1;
  constexpr int v2 = 1;
  ASSERT_EQ(v1, zisc::min(v1, v2));
  ASSERT_EQ(v1, zisc::min(v2, v1));
  ASSERT_EQ(v2, zisc::max(v1, v2));
  ASSERT_EQ(v2, zisc::max(v2, v1));
}

TEST(AlgorithmTest, GetExponentTest)
{
  {
    constexpr zisc::uint v = 8;
    constexpr auto result = zisc::Algorithm::getExponent(v);
    constexpr zisc::uint expected = 3;
    ASSERT_EQ(expected, result);
  }
  {
    constexpr int v = 100;
    constexpr auto result = zisc::Algorithm::getExponent(v);
    constexpr int expected = 6;
    ASSERT_EQ(expected, result);
  }
  {
    constexpr std::size_t size = sizeof(zisc::uint8b) * 8;
    for (std::size_t bit = 0; bit < size; ++bit) {
      const zisc::uint8b v = zisc::cast<zisc::uint8b>(zisc::cast<zisc::uint8b>(0b1) << bit);
      const auto result = zisc::Algorithm::getExponent(v);
      ASSERT_EQ(bit, result);
    }
  }
  {
    constexpr std::size_t size = sizeof(zisc::uint16b) * 8;
    for (std::size_t bit = 0; bit < size; ++bit) {
      const zisc::uint16b v = zisc::cast<zisc::uint16b>(zisc::cast<zisc::uint16b>(0b1) << bit);
      const auto result = zisc::Algorithm::getExponent(v);
      ASSERT_EQ(bit, result);
    }
  }
  {
    constexpr std::size_t size = sizeof(zisc::uint32b) * 8;
    for (std::size_t bit = 0; bit < size; ++bit) {
      const zisc::uint32b v = zisc::cast<zisc::uint32b>(0b1) << bit;
      const auto result = zisc::Algorithm::getExponent(v);
      ASSERT_EQ(bit, result);
    }
  }
  {
    constexpr std::size_t size = sizeof(zisc::uint64b) * 8;
    for (std::size_t bit = 0; bit < size; ++bit) {
      const zisc::uint64b v = zisc::cast<zisc::uint64b>(0b1) << bit;
      const auto result = zisc::Algorithm::getExponent(v);
      ASSERT_EQ(bit, result);
    }
  }
}

TEST(AlgorithmTest, RoundUpToPowerOf2Test)
{
  {
    constexpr zisc::uint64b v = 1;
    constexpr auto result = zisc::Algorithm::roundUpToPowOf2(v);
    constexpr zisc::uint64b expected = 1;
    ASSERT_EQ(expected, result);
  }
  {
    constexpr int v = 3;
    constexpr auto result = zisc::Algorithm::roundUpToPowOf2(v);
    constexpr int expected = 4;
    ASSERT_EQ(expected, result);
  }
  {
    constexpr zisc::uint16b v = 100;
    constexpr auto result = zisc::Algorithm::roundUpToPowOf2(v);
    constexpr zisc::uint16b expected = 128;
    ASSERT_EQ(expected, result);
  }
  {
    constexpr zisc::uint v = 1000;
    constexpr auto result = zisc::Algorithm::roundUpToPowOf2(v);
    constexpr zisc::uint expected = 1024;
    ASSERT_EQ(expected, result);
  }
  {
    constexpr zisc::uint64b v = 30'000;
    constexpr auto result = zisc::Algorithm::roundUpToPowOf2(v);
    constexpr zisc::uint64b expected = 32'768;
    ASSERT_EQ(expected, result);
  }
}

TEST(AlgorithmTest, ClzFallbackTest)
{
  using zisc::uint32b;
  const auto get_expected = [](zisc::uint32b x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    uint32b c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr uint32b msb = zisc::cast<uint32b>(0b1u << (size - 1));
      if (x & msb)
        break;
      x = x << 1;
      ++c;
    }
    return c;
  };

  constexpr char error_message[] = "zisc clz is wrong.";
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::min();
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
  {
    constexpr uint32b x = 0b0000'0000'0000'0000'0000'0000'0000'0001u;
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::max();
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
  {
    constexpr uint32b x = 0b0011'0101'1100'0000'1001'1010'0100'0001u;
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
  {
    constexpr uint32b x = 0b0000'0000'0000'0000'1001'1010'0100'0001u;
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
  {
    constexpr uint32b x = 0b0000'0000'0000'0010'0000'0000'0000'0000u;
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Clz8Test)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using ResultT =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<ResultT>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  using UInt = zisc::uint8b;
  const auto get_expected = [](UInt x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    UInt c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr UInt msb = zisc::cast<UInt>(0b1u << (size - 1));
      if (x & msb)
        break;
      x = zisc::cast<UInt>(x << 1);
      ++c;
    }
    return c;
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc clz is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const UInt x = zisc::cast<UInt>(rng() & 0b1111'1111u);
    const UInt expected = get_expected(x);
    const UInt result = zisc::Algorithm::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Clz8FallbackTest)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using ResultT =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<ResultT>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  using UInt = zisc::uint8b;
  const auto get_expected = [](UInt x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    UInt c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr UInt msb = zisc::cast<UInt>(0b1u << (size - 1));
      if (x & msb)
        break;
      x = zisc::cast<UInt>(x << 1);
      ++c;
    }
    return c;
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc clz is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const UInt x = zisc::cast<UInt>(rng() & 0b1111'1111u);
    const UInt expected = get_expected(x);
    const UInt result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Clz16Test)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using ResultT =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<ResultT>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  using UInt = zisc::uint16b;
  const auto get_expected = [](UInt x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    UInt c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr UInt msb = zisc::cast<UInt>(0b1u << (size - 1));
      if (x & msb)
        break;
      x = zisc::cast<UInt>(x << 1);
      ++c;
    }
    return c;
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const UInt x = zisc::cast<UInt>(rng() & 0b1111'1111'1111'1111u);
    const UInt expected = get_expected(x);
    const UInt result = zisc::Algorithm::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Clz16FallbackTest)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using ResultT =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<ResultT>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  using UInt = zisc::uint16b;
  const auto get_expected = [](UInt x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    UInt c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr UInt msb = zisc::cast<UInt>(0b1u << (size - 1));
      if (x & msb)
        break;
      x = zisc::cast<UInt>(x << 1);
      ++c;
    }
    return c;
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const UInt x = zisc::cast<UInt>(rng() & 0b1111'1111'1111'1111u);
    const UInt expected = get_expected(x);
    const UInt result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Clz32Test)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using ResultT =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<ResultT>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  using UInt = zisc::uint32b;
  const auto get_expected = [](UInt x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    UInt c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr UInt msb = zisc::cast<UInt>(0b1u << (size - 1));
      if (x & msb)
        break;
      x = zisc::cast<UInt>(x << 1);
      ++c;
    }
    return c;
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const UInt x = zisc::cast<UInt>(rng());
    const UInt expected = get_expected(x);
    const UInt result = zisc::Algorithm::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Clz32FallbackTest)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using ResultT =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<ResultT>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  using UInt = zisc::uint32b;
  const auto get_expected = [](UInt x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    UInt c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr UInt msb = zisc::cast<UInt>(0b1u << (size - 1));
      if (x & msb)
        break;
      x = zisc::cast<UInt>(x << 1);
      ++c;
    }
    return c;
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const UInt x = zisc::cast<UInt>(rng());
    const UInt expected = get_expected(x);
    const UInt result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Clz64Test)
{
  std::mt19937_64 rng;
  rng.seed(123'456'789u);
  using ResultT =  std::mt19937_64::result_type;
  static_assert(std::is_unsigned_v<ResultT>, "The result type isn't unsigned.");
  static_assert(std::mt19937_64::max() == std::numeric_limits<zisc::uint64b>::max());
  static_assert(std::mt19937_64::min() == std::numeric_limits<zisc::uint64b>::min());

  using UInt = zisc::uint64b;
  const auto get_expected = [](UInt x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    UInt c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr UInt msb = zisc::cast<UInt>(0b1ull << (size - 1));
      if (x & msb)
        break;
      x = zisc::cast<UInt>(x << 1);
      ++c;
    }
    return c;
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const UInt x = zisc::cast<UInt>(rng());
    const UInt expected = get_expected(x);
    const UInt result = zisc::Algorithm::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Clz64FallbackTest)
{
  std::mt19937_64 rng;
  rng.seed(123'456'789u);
  using ResultT =  std::mt19937_64::result_type;
  static_assert(std::is_unsigned_v<ResultT>, "The result type isn't unsigned.");
  static_assert(std::mt19937_64::max() == std::numeric_limits<zisc::uint64b>::max());
  static_assert(std::mt19937_64::min() == std::numeric_limits<zisc::uint64b>::min());

  using UInt = zisc::uint64b;
  const auto get_expected = [](UInt x)
  {
    constexpr std::size_t size = 8 * sizeof(x);
    UInt c = 0;
    for (std::size_t index = 0; index < size; ++index) {
      constexpr UInt msb = zisc::cast<UInt>(0b1ull << (size - 1));
      if (x & msb)
        break;
      x = zisc::cast<UInt>(x << 1);
      ++c;
    }
    return c;
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const UInt x = zisc::cast<UInt>(rng());
    const UInt expected = get_expected(x);
    const UInt result = zisc::Algorithm::Zisc::clz(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, PopcountFallbackTest)
{
  const auto get_expected = [](const zisc::uint32b x)
  {
    std::bitset<32> bits{x};
    return zisc::cast<zisc::uint32b>(bits.count());
  };

  using zisc::uint32b;
  constexpr char error_message[] = "zisc popcount is wrong.";
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::min();
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::max();
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
  {
    constexpr uint32b x = 0b0011'0101'1100'0000'1001'1010'0100'0001u;
    const uint32b expected = get_expected(x);
    constexpr uint32b result = zisc::Algorithm::Zisc::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Popcount8Test)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using UInt =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<UInt>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  const auto get_expected = [](const zisc::uint8b x)
  {
    std::bitset<8> bits{x};
    return zisc::cast<zisc::uint8b>(bits.count());
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const auto x = zisc::cast<zisc::uint8b>(rng() & 0b1111'1111u);
    const auto expected = get_expected(x);
    const auto result = zisc::Algorithm::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Popcount8FallbackTest)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using UInt =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<UInt>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  const auto get_expected = [](const zisc::uint8b x)
  {
    std::bitset<8> bits{x};
    return zisc::cast<zisc::uint8b>(bits.count());
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const auto x = zisc::cast<zisc::uint8b>(rng() & 0b1111'1111u);
    const auto expected = get_expected(x);
    const auto result = zisc::Algorithm::Zisc::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Popcount16Test)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using UInt =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<UInt>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  const auto get_expected = [](const zisc::uint16b x)
  {
    std::bitset<16> bits{x};
    return zisc::cast<zisc::uint16b>(bits.count());
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const auto x = zisc::cast<zisc::uint16b>(rng() & 0b1111'1111'1111'1111u);
    const auto expected = get_expected(x);
    const auto result = zisc::Algorithm::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Popcount16FallbackTest)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using UInt =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<UInt>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  const auto get_expected = [](const zisc::uint16b x)
  {
    std::bitset<16> bits{x};
    return zisc::cast<zisc::uint16b>(bits.count());
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const auto x = zisc::cast<zisc::uint16b>(rng() & 0b1111'1111'1111'1111u);
    const auto expected = get_expected(x);
    const auto result = zisc::Algorithm::Zisc::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Popcount32Test)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using UInt =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<UInt>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  const auto get_expected = [](const zisc::uint32b x)
  {
    std::bitset<32> bits{x};
    return zisc::cast<zisc::uint32b>(bits.count());
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const auto x = zisc::cast<zisc::uint32b>(rng());
    const auto expected = get_expected(x);
    const auto result = zisc::Algorithm::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Popcount32FallbackTest)
{
  std::mt19937 rng;
  rng.seed(123'456'789u);
  using UInt =  std::mt19937::result_type;
  static_assert(std::is_unsigned_v<UInt>, "The result type isn't unsigned.");
  static_assert(std::mt19937::max() == std::numeric_limits<zisc::uint32b>::max());
  static_assert(std::mt19937::min() == std::numeric_limits<zisc::uint32b>::min());

  const auto get_expected = [](const zisc::uint32b x)
  {
    std::bitset<32> bits{x};
    return zisc::cast<zisc::uint32b>(bits.count());
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const auto x = zisc::cast<zisc::uint32b>(rng());
    const auto expected = get_expected(x);
    const auto result = zisc::Algorithm::Zisc::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Popcount64Test)
{
  std::mt19937_64 rng;
  rng.seed(123'456'789u);
  using UInt =  std::mt19937_64::result_type;
  static_assert(std::is_unsigned_v<UInt>, "The result type isn't unsigned.");
  static_assert(std::mt19937_64::max() == std::numeric_limits<zisc::uint64b>::max());
  static_assert(std::mt19937_64::min() == std::numeric_limits<zisc::uint64b>::min());

  const auto get_expected = [](const zisc::uint64b x)
  {
    std::bitset<64> bits{x};
    return zisc::cast<zisc::uint64b>(bits.count());
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const auto x = zisc::cast<zisc::uint64b>(rng());
    const auto expected = get_expected(x);
    const auto result = zisc::Algorithm::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, Popcount64FallbackTest)
{
  std::mt19937_64 rng;
  rng.seed(123'456'789u);
  using UInt =  std::mt19937_64::result_type;
  static_assert(std::is_unsigned_v<UInt>, "The result type isn't unsigned.");
  static_assert(std::mt19937_64::max() == std::numeric_limits<zisc::uint64b>::max());
  static_assert(std::mt19937_64::min() == std::numeric_limits<zisc::uint64b>::min());

  const auto get_expected = [](const zisc::uint64b x)
  {
    std::bitset<64> bits{x};
    return zisc::cast<zisc::uint64b>(bits.count());
  };

  constexpr std::size_t n = 100'000'000;
  constexpr char error_message[] = "zisc popcount is wrong.";
  for (std::size_t i = 0; i < n; ++i) {
    const auto x = zisc::cast<zisc::uint64b>(rng());
    const auto expected = get_expected(x);
    const auto result = zisc::Algorithm::Zisc::popcount(x);
    ASSERT_EQ(expected, result) << error_message
        << " x = " << x << ", expected = " << expected << ", result = " << result;
  }
}

TEST(AlgorithmTest, BinaryTreeTest)
{
  using zisc::cast;

  constexpr zisc::uint n = 10;
  zisc::ArithArray<double, n> array;
  for (zisc::uint i = 0; i < n; ++i)
    array[i] = cast<double>(i);
  zisc::Algorithm::toBinaryTree(array.begin(), array.end());

  auto position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 0.5);
  EXPECT_DOUBLE_EQ(0.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 1.5);
  EXPECT_DOUBLE_EQ(1.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 2.5);
  EXPECT_DOUBLE_EQ(2.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 6.5);
  EXPECT_DOUBLE_EQ(6.0, *position);
  
  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 5.5);
  EXPECT_DOUBLE_EQ(5.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), cast<double>(n));
  EXPECT_DOUBLE_EQ(cast<double>(n - 1), *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 7.0);
  EXPECT_DOUBLE_EQ(7.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 9.0);
  EXPECT_DOUBLE_EQ(9.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 2.0);
  EXPECT_DOUBLE_EQ(2.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 3.0);
  EXPECT_DOUBLE_EQ(3.0, *position);

  position = zisc::Algorithm::searchBinaryTree(array.begin(), array.end(), 6.0);
  EXPECT_DOUBLE_EQ(6.0, *position);
}

TEST(AlgorithmTest, BinaryTreeSearchTest)
{
  zisc::PcgLcgRxsMXs64 rng_engine{123456789};

  constexpr zisc::uint max_number = 200;
  for (zisc::uint number = 0; number < max_number; ++number) {
    zisc::uint64b n = zisc::cast<zisc::uint64b>(rng_engine(1.0, 1.0e6));

    // Make a sorted array
    auto array = ::makeSortedArray(n, rng_engine);
    n = zisc::cast<zisc::uint64b>(array.size());

    std::cout << "Test" << std::setw(4) << number << ": n = " << n << std::endl;

    // Make a binary tree
    auto tree = array;
    zisc::Algorithm::Zisc::toBinaryTree(tree.begin(), tree.end());

    for (zisc::uint64b i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64b>(c);
          // STL
          const auto expected = zisc::Algorithm::Stl::searchBinaryTree(array.begin(),
                                                                       array.end(),
                                                                       v);
          ASSERT_TRUE(expected != array.end()) << "The binary search failed.";
          // ZISC
          const auto actual = zisc::Algorithm::Zisc::searchBinaryTree(tree.begin(),
                                                                      tree.end(), 
                                                                      v);
          ASSERT_EQ(*expected, *actual) << "The binary tree is wrong.";
        }
      }
    }
  }
}

TEST(AlgorithmTest, BinaryTreeSearchPerformanceTest)
{
  zisc::PcgLcgRxsMXs64 rng_engine{123456789};

  constexpr zisc::uint max_number = 200;
  auto stl_time = zisc::Stopwatch::Clock::duration::zero();
  auto zisc_time = stl_time;
  for (zisc::uint number = 0; number < max_number; ++number) {
    zisc::uint64b n = zisc::cast<zisc::uint64b>(rng_engine(1.0, 1.0e6));

    // Make a sorted array
    auto array = ::makeSortedArray(n, rng_engine);
    n = zisc::cast<zisc::uint64b>(array.size());

    std::cout << "Test" << std::setw(4) << number << ": n = " << n << std::endl;

    // Make a binary tree
    auto tree = array;
    zisc::Algorithm::Zisc::toBinaryTree(tree.begin(), tree.end());

    zisc::Stopwatch stopwatch;
    stopwatch.start();
    for (zisc::uint64b i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64b>(c);
          // STL
          [[maybe_unused]] volatile auto result = 
              zisc::Algorithm::Stl::searchBinaryTree(array.begin(), array.end(), v);
        }
      }
    }
    stl_time += stopwatch.elapsedTime();
    stopwatch.stop();
    stopwatch.start();
    for (zisc::uint64b i = 0; i < n; ++i) {
      for (int c = 0; c < 3; ++c) {
        if (!(((i == 0) && (c == 2)) || ((i == n - 1) && (c == 0)))) {
          const auto v = (array[i] + 1) - zisc::cast<zisc::uint64b>(c);
          // Zisc
          [[maybe_unused]] volatile auto result = 
              zisc::Algorithm::Zisc::searchBinaryTree(tree.begin(), tree.end(), v);
        }
      }
    }
    zisc_time += stopwatch.elapsedTime();
  }
  {
    const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(stl_time);
    std::cout << " STL time: " << t.count() << " ms" << std::endl;
  }
  {
    const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(zisc_time);
    std::cout << "Zisc time: " << t.count() << " ms" << std::endl;
  }
  ASSERT_GT(stl_time, zisc_time) << "The zisc binary tree is slower than the stl.";
}
