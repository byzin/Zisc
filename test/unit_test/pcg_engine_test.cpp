/*!
  \file pcg_engine_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <fstream>
#include <string>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/pcg_engine.hpp"

template <typename PcgEngine>
void testPcgEngine(const std::string& reference_file_path)
{
  std::ifstream reference_file;
  reference_file.open(reference_file_path);

  PcgEngine pcg;

  constexpr int n = 1000;
  typename PcgEngine::ResultType reference;
  for (int i = 0; i < n; ++i) {
    reference_file >> reference;
    ASSERT_EQ(reference, pcg());
  }
}

TEST(PcgEngineTest, GenerateTest)
{
  testPcgEngine<zisc::PcgLcgXshRr>("resources/pcg_lcg_xsh_rr_reference");
  testPcgEngine<zisc::PcgMcgXshRr>("resources/pcg_mcg_xsh_rr_reference");
  testPcgEngine<zisc::PcgLcgRxsMXs>("resources/pcg_lcg_rxs_m_xs_reference");
  testPcgEngine<zisc::PcgMcgRxsMXs>("resources/pcg_mcg_rxs_m_xs_reference");
}
