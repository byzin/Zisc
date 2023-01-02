/*!
  \file csv_example.cpp
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
#include <sstream>
#include <string_view>
// Zisc
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/string/csv.hpp"

int main()
{
  using Csv = zisc::Csv<int, double, bool, std::string_view>;
  // CSV example
  std::cout << "## CSV example" << std::endl;
  std::istringstream csv_string{R"(1, 3.14, true, "Example1")"};
  std::cout << "  CSV text: " << csv_string.str() << std::endl;
  zisc::AllocFreeResource mem_resource;
  Csv csv{&mem_resource};
  csv.append(csv_string);
  const auto& record = csv.record(0);
  std::cout << "  1: " << std::get<0>(record) << std::endl;
  std::cout << "  2: " << std::get<1>(record) << std::endl;
  std::cout << "  3: " << std::get<2>(record) << std::endl;
  std::cout << "  4: " << std::get<3>(record) << std::endl;

  return 0;
}
