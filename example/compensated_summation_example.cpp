/*!
  \file compensated_summation_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <iomanip>
#include <iostream>
// Zisc
#include "zisc/compensated_summation.hpp"

int main()
{
  // CompensatedSummation example
  std::cout << "CompensatedSummation" << std::endl;
  constexpr double big = 1.0;
  constexpr double small = 1.0e-9;
  constexpr int billion = 1'000'000'000;
  std::cout << "Calculate 1.0 + 1.0e-9 * 1'000'000'000" << std::endl;
  {
    double sum = big;
    for (int i = 0; i < billion; ++i)
      sum += small;
    std::cout << "Simple sum: " << std::scientific << std::setprecision(16) 
              << sum << std::endl;
  }
  {
    zisc::CompensatedSummation<double> sum{big};
    for (int i = 0; i < billion; ++i)
      sum.add(small);
    std::cout << "Kahan sum: " << std::scientific << std::setprecision(16) 
              << sum.get() << std::endl;
  }

  return 0;
}
