/*!
  \file error_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
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
  // Log example
  {
    zisc::outputMessage(std::cout, "Log messages example.");
    ZISC_LOG(std::cout, "Zisc version: ",
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

  return 0;
}
