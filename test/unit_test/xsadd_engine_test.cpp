/*!
  \file xsadd_engine_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/xsadd_engine.hpp"

TEST(XsaddEngineTest, GenerateTest)
{
  std::ifstream reference_file;
  reference_file.open("resources/xsadd_reference");

  zisc::XsaddEngine xsadd{1234};
  constexpr std::size_t kNumOfRandoms = 40;

  std::uint32_t reference_random1;
  for (std::size_t i = 0; i < kNumOfRandoms; ++i) {
    reference_file >> reference_random1;
    ASSERT_EQ(reference_random1, xsadd.generate());
  }

  float reference_random2;
  for (std::size_t i = 0; i < kNumOfRandoms; ++i) {
    reference_file >> reference_random2;
//    ASSERT_FLOAT_EQ(reference_random2, xsadd.generateFloat());
  }

  xsadd.setSeed(std::vector<std::uint32_t>{0x0a, 0x0b, 0x0c, 0x0d});
  std::string reference_random3;
  for (std::size_t i = 0; i < kNumOfRandoms; ++i) {
    reference_file >> reference_random3;
    const std::uint32_t expect = 
        static_cast<std::uint32_t>(std::stoul(reference_random3, nullptr, 16));
    ASSERT_EQ(expect, xsadd.generate());
  }

//  double reference_random4;
//  for (std::size_t i = 0; i < kNumOfRandoms; ++i) {
//    reference_file >> reference_random4;
//    ASSERT_DOUBLE_EQ(reference_random4, xsadd.generateFloat());
//  }
}
