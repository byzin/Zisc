/*!
  \file error_example.cpp
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
                        zisc::Config::versionMajor(), ".",
                        zisc::Config::versionMinor(), ".",
                        zisc::Config::versionPatch());
  }
  // Assert example
  {
    zisc::outputMessage(std::cout, "Assert example.");
    ZISC_ASSERT(false, "Zisc version: ",
                       zisc::Config::versionMajor(), ".",
                       zisc::Config::versionMinor(), ".",
                       zisc::Config::versionPatch());
  }

  return 0;
}
