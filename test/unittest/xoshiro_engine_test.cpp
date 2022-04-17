/*!
  \file xoshiro_engine_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <fstream>
#include <string_view>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/random/xoshiro_engine.hpp"

namespace {

template <typename XoshiroEngine>
void testXoshiroEngine(const std::string_view reference_path)
{
  std::ifstream reference_file;
  reference_file.open(reference_path.data());

  using ValueType = typename XoshiroEngine::ValueT;
  XoshiroEngine sampler{};

  constexpr int n = 1024;
  for (int i = 0; i < n; ++i) {
    ValueType expected = 0;
    reference_file >> expected;
    const ValueType r = sampler();
    ASSERT_EQ(expected, r) << "xoshiro[" << i << "] is wrong.";
  }
}

} // namespace

#define XOSHIRO_TEST(engine_type, reference_path) \
    TEST(RandomNumberEngine, engine_type ## Test) \
    { \
      ::testXoshiroEngine<zisc:: engine_type >(reference_path); \
    }

XOSHIRO_TEST(XoshiroPlus128, "resources/xoshiro128_plus_reference.txt")
XOSHIRO_TEST(Xoshiro2Plus128, "resources/xoshiro128_2plus_reference.txt")
XOSHIRO_TEST(Xoshiro2Star128, "resources/xoshiro128_2star_reference.txt")
XOSHIRO_TEST(XoshiroPlus256, "resources/xoshiro256_plus_reference.txt")
XOSHIRO_TEST(Xoshiro2Plus256, "resources/xoshiro256_2plus_reference.txt")
XOSHIRO_TEST(Xoshiro2Star256, "resources/xoshiro256_2star_reference.txt")
