/*!
  \file string_example.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <iostream>
// Zisc
#include "zisc/string/constant_string.hpp"

int main()
{
  // String example
  constexpr auto message1 = zisc::toString(R"(## String )");
  constexpr auto message2 = zisc::toString(R"(example)");
  std::cout << (message1 + message2).toCStr() << std::endl;

  return 0;
}
