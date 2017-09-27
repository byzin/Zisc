/*!
  \file cumulative_distribution_function_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/cumulative_distribution_function.hpp"
#include "zisc/utility.hpp"

TEST(CumulativeDistributionFunctionTest, InverseFunctionTest)
{
  using zisc::cast;

  constexpr std::size_t n = 5;

  // Initialization
  using Cdf = zisc::CumulativeDistributionFunction<int, double>;

  std::vector<int> x_list{{0, 1, 2, 3, 4}};
  std::vector<double> y_list{{0.2, 0.3, 0.0, 0.3, 0.2}};
  auto cdf = std::make_unique<Cdf>(std::move(x_list), std::move(y_list));

  // Test
  constexpr int loop = 1000;
  const std::array<int, n> reference_list{{200, 300, 0, 300, 200}};
  std::array<int, n> count_list;
  count_list.fill(0);
  for (int i = 0; i < loop; ++i) {
    const auto y = cast<double>(i) / cast<double>(loop);
    const auto x = cast<decltype(count_list)::size_type>(cdf->inverseFunction(y));
    ++count_list[x];
  }
  for (int i = 0; i < static_cast<int>(n); ++i) {
    EXPECT_EQ(reference_list[i], count_list[i])
        << "The inverseFunction test failed.";
  }
}
