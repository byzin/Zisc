/*!
  \file unit_example.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <iostream>
// Zisc
#include "zisc/math/unit.hpp"

int main()
{
  // Unit example
  std::cout << "## Unit example" << std::endl;
  constexpr double degree = 45.0;
  std::cout << degree << " degree is " << zisc::toRadian(degree) << " rad" << std::endl;
  constexpr double rad = 3.141592;
  std::cout << rad << " rad is " << zisc::toAngle(rad) << " degree" << std::endl;

  return 0;
}
