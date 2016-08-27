/*!
  \file dsfmt_engin_test.cpp
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
#include "zisc/dsfmt_engine.hpp"

/*!
  \details
  No detailed.
  */
template <typename Dsfmt>
void testDsfmt(const std::string& reference_file_path)
{
  std::ifstream reference_file;
  reference_file.open(reference_file_path);

  Dsfmt dsfmt;

  constexpr int kNumOfRandoms = 1000;
  double reference_random;
  for (int i = 0; i < kNumOfRandoms; ++i) {
    reference_file >> reference_random;
    ASSERT_DOUBLE_EQ(reference_random, dsfmt());
  }
}

TEST(DsfmtEnginTest, GenerateTest)
{
  testDsfmt<zisc::Dsfmt521>("resources/dsfmt_521_reference");
  testDsfmt<zisc::Dsfmt1279>("resources/dsfmt_1279_reference");
  testDsfmt<zisc::Dsfmt2203>("resources/dsfmt_2203_reference");
  testDsfmt<zisc::Dsfmt4253>("resources/dsfmt_4253_reference");
  testDsfmt<zisc::Dsfmt11213>("resources/dsfmt_11213_reference");
  testDsfmt<zisc::Dsfmt19937>("resources/dsfmt_19937_reference");
  testDsfmt<zisc::Dsfmt44497>("resources/dsfmt_44497_reference");
  testDsfmt<zisc::Dsfmt86243>("resources/dsfmt_86243_reference");
  testDsfmt<zisc::Dsfmt132049>("resources/dsfmt_132049_reference");
  testDsfmt<zisc::Dsfmt216091>("resources/dsfmt_216091_reference");
}
