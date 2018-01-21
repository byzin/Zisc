/*!
  \file sip_hash_engine_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <cstdint>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/sip_hash_engine.hpp"
#include "zisc/zisc_config.hpp"

template <typename Type>
void testSipHash(const std::string& reference_file_path)
{
  std::ifstream reference_file;
  reference_file.open(reference_file_path);

  constexpr std::size_t n = 64;
  constexpr std::size_t hash_size = sizeof(Type);

  auto get_input_list = []()
  {
    std::array<zisc::uint8, n> inputs{};
    for (std::size_t i = 0; i < n; ++i)
      inputs[i] = static_cast<zisc::uint8>(i);
    return inputs;
  };

  auto to_uint = [](const std::array<zisc::uint8, hash_size> outputs)
  {
    Type result = 0;
    for (std::size_t i = 0; i < hash_size; ++i)
      result = result | (static_cast<Type>(outputs[i]) << (8 * i));
    return result;
  };

  constexpr auto input_list = get_input_list();

  for (std::size_t i = 0; i < n; ++i) {
    std::array<zisc::uint8, hash_size> output_list;
    for (std::size_t b = 0; b < hash_size; ++b) {
      zisc::uint expected = 0;
      reference_file >> std::hex >> expected;
      output_list[b] = static_cast<zisc::uint8>(expected);
    }

    const Type result = zisc::SipHashEngine<Type>::hash(input_list.data(), i);
    const Type expected = to_uint(output_list);
    ASSERT_EQ(expected, result) << "Hash test[" << i << "] failed.";
  }
}

TEST(SipHashEngineTest, 32BitHashTest)
{
  // Hash function test
  testSipHash<zisc::uint32>("resources/sip_hash_32_reference");
  // constexpr test
  using SipHash = zisc::SipHash32;
  {
    constexpr char seed[] = "32bit hash test";
    [[maybe_unused]] constexpr auto result = SipHash::hash(seed);
  }
  {
    constexpr std::array<zisc::uint8, 4> seed0{{1, 2, 3, 4}};
    constexpr auto expected = SipHash::hash(seed0.data(), 4);

    constexpr char seed1[] = {1, 2, 3, 4, '\0'};
    constexpr auto result1 = SipHash::hash(seed1);
    ASSERT_EQ(expected, result1) << "Char array hash function test failed.";

    constexpr zisc::uint32 seed2 = 0x04030201;
    constexpr auto result2 = SipHash::hash(seed2);
    ASSERT_EQ(expected, result2) << "Uint array hash function test failed.";
  }
}

TEST(SipHashEngineTest, 64BitHashTest)
{
  // Hash function test
  testSipHash<zisc::uint64>("resources/sip_hash_64_reference");
  // constexpr test
  using SipHash = zisc::SipHash64;
  {
    constexpr char seed[] = "64bit hash test";
    [[maybe_unused]] constexpr auto result = SipHash::hash(seed);
  }
  {
    constexpr std::array<zisc::uint8, 8> seed0{{1, 2, 3, 4, 5, 6, 7, 8}};
    constexpr auto expected = SipHash::hash(seed0.data(), 8);

    constexpr char seed1[] = {1, 2, 3, 4, 5, 6, 7, 8, '\0'};
    constexpr auto result1 = SipHash::hash(seed1);
    ASSERT_EQ(expected, result1) << "Char array hash function test failed.";

    constexpr zisc::uint64 seed2 = 0x0807060504030201;
    constexpr auto result2 = SipHash::hash(seed2);
    ASSERT_EQ(expected, result2) << "Uint array hash function test failed.";
  }
}

