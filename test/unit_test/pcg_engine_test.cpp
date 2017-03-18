/*!
  \file pcg_engine_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
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

template <typename PcgEngine>
void testFloatDistribution()
{
  PcgEngine pcg;

  constexpr int n = 1'000'000'00;
  constexpr int section = 20;
  constexpr int expected = n / section;
  constexpr int error = 5000;

  std::array<int, section> result;
  result.fill(0);
  for (int i = 0; i < n; ++i) {
    const double u = static_cast<double>(pcg.generate01());
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

TEST(PcgEngineTest, GenerateTest)
{
  testPcgEngine<zisc::PcgLcgXshRr>("resources/pcg_lcg_xsh_rr_reference");
  testPcgEngine<zisc::PcgMcgXshRr>("resources/pcg_mcg_xsh_rr_reference");
  testPcgEngine<zisc::PcgLcgRxsMXs>("resources/pcg_lcg_rxs_m_xs_reference");
  testPcgEngine<zisc::PcgMcgRxsMXs>("resources/pcg_mcg_rxs_m_xs_reference");
  testFloatDistribution<zisc::PcgMcgRxsMXs32>();
  testFloatDistribution<zisc::PcgMcgRxsMXs>();
}
