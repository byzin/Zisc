/*!
  \file config_test.cpp
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
#include <string_view>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"

TEST(ConfigTest, ArchitectureTest)
{
  constexpr std::string_view name = zisc::Config::architectureName();
  constexpr auto architecture = zisc::Config::architecture();
  constexpr auto arch = static_cast<zisc::uint>(architecture);
  std::cout << "  Architecture: " << name << " [" << arch << "]." << std::endl;
}
