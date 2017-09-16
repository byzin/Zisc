/*!
  \file cumulative_distribution_function_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <iostream>
// Zisc
#include "zisc/cumulative_distribution_function.hpp"

int main()
{
  std::cout << "## CumulativeDistributionFunction example" << std::endl;
  std::array<int, 10> value{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
  std::array<double, 10> pdf;
  pdf.fill(0.1);

  zisc::CumulativeDistributionFunction<int, double> cdf{value.begin(), value.end(), 
                                                        pdf.begin(), pdf.end()};
  std::cout << "P^{-1}(0.5) == " << cdf.inverseFunction(0.5) << std::endl;

  return 0;
}
