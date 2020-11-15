/*!
  \file main.cpp
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
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string_view>
// Zisc
#include "zisc/binary_serializer.hpp"
// Test
#include "pow_test.hpp"

int main(int /* argc */, char** /* argv */)
{
  auto compress_file = [](std::string_view file_path) noexcept
  {
    char command[1024] = "";
    std::sprintf(command, "tar -Jcvf %s.tar.xz %s", file_path.data(),
                                                    file_path.data());
    std::system(command);
  };

  // Pow test
  {
    std::string_view file_path{"math_powf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testPowF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_powd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testPowD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_powintf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testPowIntF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_powintd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testPowIntD(&output);
    output.close();
    compress_file(file_path);
  }

  return 0;
}
