/*!
  \file compensated_summation_example.cpp
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
#include <iomanip>
#include <iostream>
#include <limits>
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
  constexpr int digit = std::numeric_limits<double>::max_digits10;
  {
    double sum = big;
    for (int i = 0; i < billion; ++i)
      sum += small;
    std::cout << "Simple sum: " << std::scientific << std::setprecision(digit) 
              << sum << std::endl;
  }
  {
    zisc::CompensatedSummation<double> sum{big};
    for (int i = 0; i < billion; ++i)
      sum.add(small);
    std::cout << "Kahan sum: " << std::scientific << std::setprecision(digit) 
              << sum.get() << std::endl;
  }

  return 0;
}
