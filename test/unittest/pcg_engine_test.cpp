/*!
  \file pcg_engine_test.cpp
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
#include <array>
#include <cmath>
#include <fstream>
#include <string>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/random/pcg_engine.hpp"

template <typename PcgEngine>
void testPcgEngine(const std::string& reference_file_path)
{
  std::ifstream reference_file;
  reference_file.open(reference_file_path);

  constexpr typename PcgEngine::SeedType seed = 42;
  PcgEngine pcg{seed};

  constexpr int n = 1024;
  zisc::uint64b reference;
  for (int i = 0; i < n; ++i) {
    reference_file >> reference;
    const auto r = static_cast<typename PcgEngine::ResultType>(reference);
    ASSERT_EQ(r, pcg());
  }

  pcg.setSeed(seed);
  for (int i = 0; i < n; ++i) {
    const float f = pcg.generateFloat(0.0f, 1.0f);
    ASSERT_LE(0.0f, f);
    ASSERT_LT(f, 1.0f);
  }

  pcg.setSeed(seed);
  for (int i = 0; i < n; ++i) {
    const double d = pcg.generateFloat(0.0, 1.0);
    ASSERT_LE(0.0, d);
    ASSERT_LT(d, 1.0);
  }
}

#define PCG_TEST(engine_type, reference_path) \
    TEST(PcgEngineTest, engine_type ## Test) \
    { \
        testPcgEngine<zisc:: engine_type >(reference_path); \
    }

PCG_TEST(PcgLcgXshRs8, "resources/pcg_lcg_xsh_rs_8_reference")
PCG_TEST(PcgLcgXshRs16, "resources/pcg_lcg_xsh_rs_16_reference")
PCG_TEST(PcgLcgXshRs32, "resources/pcg_lcg_xsh_rs_32_reference")
PCG_TEST(PcgMcgXshRs8, "resources/pcg_mcg_xsh_rs_8_reference")
PCG_TEST(PcgMcgXshRs16, "resources/pcg_mcg_xsh_rs_16_reference")
PCG_TEST(PcgMcgXshRs32, "resources/pcg_mcg_xsh_rs_32_reference")
PCG_TEST(PcgLcgXshRr8, "resources/pcg_lcg_xsh_rr_8_reference")
PCG_TEST(PcgLcgXshRr16, "resources/pcg_lcg_xsh_rr_16_reference")
PCG_TEST(PcgLcgXshRr32, "resources/pcg_lcg_xsh_rr_32_reference")
PCG_TEST(PcgMcgXshRr8, "resources/pcg_mcg_xsh_rr_8_reference")
PCG_TEST(PcgMcgXshRr16, "resources/pcg_mcg_xsh_rr_16_reference")
PCG_TEST(PcgMcgXshRr32, "resources/pcg_mcg_xsh_rr_32_reference")
PCG_TEST(PcgLcgRxsMXs8, "resources/pcg_lcg_rxs_m_xs_8_reference")
PCG_TEST(PcgLcgRxsMXs16, "resources/pcg_lcg_rxs_m_xs_16_reference")
PCG_TEST(PcgLcgRxsMXs32, "resources/pcg_lcg_rxs_m_xs_32_reference")
PCG_TEST(PcgLcgRxsMXs64, "resources/pcg_lcg_rxs_m_xs_64_reference")
