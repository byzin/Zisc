/*!
  \file cumulative_distribution_function_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>
// Zisc
#include "zisc/cumulative_distribution_function.hpp"

int main()
{
  std::cout << "## CumulativeDistributionFunction example" << std::endl;

  const std::size_t n = 10;

  std::vector<int> value_list;
  std::vector<double> pdf_list;
  value_list.resize(n);
  pdf_list.resize(n);
  for (int i = 0; i < static_cast<int>(n); ++i) {
    value_list[i] = i;
    pdf_list[i] = 0.1;
  }

  using Cdf = zisc::CumulativeDistributionFunction<int, double>;
  const Cdf cdf{std::move(value_list), std::move(pdf_list)};

  std::cout << "P^{-1}(0.5) == " << cdf.inverseFunction(0.5) << std::endl;

  return 0;
}
