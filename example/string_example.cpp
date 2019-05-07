/*!
  \file string_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <iostream>
// Zisc
#include "zisc/string.hpp"

int main()
{
  // String example
  constexpr auto message1 = zisc::toString(R"(## String )");
  constexpr auto message2 = zisc::toString(R"(example)");
  std::cout << (message1 + message2).toCString() << std::endl;

  return 0;
}
