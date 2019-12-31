/*!
  \file error_example.cpp
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
#include <iostream>
// Zisc
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"

int main()
{
  // Output messages example
  {
    zisc::outputMessage(std::cout, "Output messages example.");
    zisc::outputMessage(std::cout);
    zisc::outputMessage(std::cout,
                        "Zisc version: ",
                        kZiscVersionMajor, ".",
                        kZiscVersionMinor, ".",
                        kZiscVersionPatch);
  }
  // Assert example
  {
    zisc::outputMessage(std::cout, "Assert example.");
    ZISC_ASSERT(false, "Zisc version: ",
                       kZiscVersionMajor, ".",
                       kZiscVersionMinor, ".",
                       kZiscVersionPatch);
  }
  // Static assert example
  {
    constexpr int value = 10;
    ZISC_STATIC_ASSERT(value == 10, "The value isn't 10.");
  }

  return 0;
}
