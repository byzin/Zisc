/*!
  \file pcg_engine_test.cpp
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
#include "zisc/random/pcg_engine.hpp"

template <typename PcgEngine>
void testPcgEngine(const std::string_view reference_path)
{
  std::ifstream reference_file;
  reference_file.open(reference_path.data());

  using ValueType = typename PcgEngine::ValueT;
  constexpr ValueType seed = 42;
  PcgEngine sampler{seed};

  constexpr int n = 1024;
  for (int i = 0; i < n; ++i) {
    ValueType expected = 0;
    reference_file >> expected;
    const ValueType r = sampler();
    ASSERT_EQ(expected, r) << "pcg[" << i << "] is wrong.";
  }
}

#define PCG_TEST(engine_type, reference_path) \
    TEST(RandomNumberEngine, engine_type ## Test) \
    { \
      ::testPcgEngine<zisc:: engine_type >(reference_path); \
    }

PCG_TEST(PcgLcgRxsMXs32, "resources/pcg_lcg_rxs_m_xs_32_reference.txt")
PCG_TEST(PcgLcgRxsMXs64, "resources/pcg_lcg_rxs_m_xs_64_reference.txt")
