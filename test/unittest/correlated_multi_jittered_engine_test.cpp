/*!
  \file correlated_multi_jittered_engine_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
#include <fstream>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/correlated_multi_jittered_engine.hpp"
#include "zisc/fnv_1a_hash_engine.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

template <typename CmjEngine>
void testCmjEngine(const std::string& reference_file_path)
{
  std::ifstream reference_file;
  reference_file.open(reference_file_path);

  std::uint32_t seed = 42;
  auto update_engine = [&seed](std::uint32_t& s)
  {
    if (CmjEngine::isEndOfPeriod(s++)) {
      ++seed;
      s = 0;
    }
  };

  constexpr std::uint32_t n = 1024;
  for (std::uint32_t sample = 0, s = 0; sample < n; ++sample) {
    const std::uint32_t p = zisc::Fnv1aHash32::hash(seed);

    // Generate a 1D random number
    const auto d = CmjEngine::template generate1D<double>(s, p);
    ASSERT_LE(0.0, d);
    ASSERT_LT(d, 1.0);

    // Generate a 2D random number
    const auto xy = CmjEngine::template generate2D<double>(s, p);
    ASSERT_LE(0.0, xy[0]);
    ASSERT_LT(xy[0], 1.0);
    ASSERT_LE(0.0, xy[1]);
    ASSERT_LT(xy[1], 1.0);

    update_engine(s);

    static_assert(sizeof(double) == 8);
    const std::uint64_t d_value = *zisc::treatAs<const std::uint64_t*>(&d);
    const std::uint64_t x_value = *zisc::treatAs<const std::uint64_t*>(&xy[0]);
    const std::uint64_t y_value = *zisc::treatAs<const std::uint64_t*>(&xy[1]);

    std::uint64_t reference;
    reference_file >> reference;
    ASSERT_EQ(reference, d_value);
    reference_file >> reference;
    ASSERT_EQ(reference, x_value);
    reference_file >> reference;
    ASSERT_EQ(reference, y_value);
  }
}

#define CMJ_TEST(engine_type, reference_path) \
    TEST(CmjEngineTest, engine_type ## Test) \
    { \
        testCmjEngine<zisc:: engine_type >(reference_path); \
    }

CMJ_TEST(CmjN64, "resources/cmj_n64_reference")
CMJ_TEST(CmjN81, "resources/cmj_n81_reference")
