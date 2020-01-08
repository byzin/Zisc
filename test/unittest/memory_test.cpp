/*!
  \file memory_test.cpp
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
#include <iomanip>
#include <limits>
// Googletest
#include "gtest/gtest.h"
// Zisc
#include "zisc/memory.hpp"
#include "zisc/unit_multiple.hpp"
#include "zisc/zisc_config.hpp"

TEST(MemoryTest, SystemMemoryStatsTest)
{
  const auto system_stats = zisc::Memory::retrieveSystemStats();

  std::cout << std::defaultfloat << std::endl;
  using Arith = typename zisc::ByteUnit::ArithmeticType;
  {
    constexpr std::size_t m = std::numeric_limits<std::size_t>::max();
    ASSERT_NE(system_stats.physicalMemoryTotal(), m) << "Unsupported platform.";
  }
  {
    const Arith b = zisc::cast<Arith>(system_stats.physicalMemoryTotal());
    const zisc::ByteUnit bytes{b};
    const auto mb = bytes.representedAs<zisc::MebiUnit::exponent()>();
    std::cout << "## Total physical memory size: " << mb.value().toFloat<double>()
              << " ms" << std::endl;
    EXPECT_GT(b, 0) << "The platform has no physical memory";
  }
  {
    const Arith b = zisc::cast<Arith>(system_stats.physicalMemoryFree());
    const zisc::ByteUnit bytes{b};
    const auto mb = bytes.representedAs<zisc::MebiUnit::exponent()>();
    std::cout << "## Free  physical memory size: " << mb.value().toFloat<double>()
              << " ms" << std::endl;
  }
  {
    const Arith b = zisc::cast<Arith>(system_stats.virtualMemoryTotal());
    const zisc::ByteUnit bytes{b};
    const auto mb = bytes.representedAs<zisc::MebiUnit::exponent()>();
    std::cout << "## Total virtual memory size: " << mb.value().toFloat<double>()
              << " ms" << std::endl;
  }
  {
    const Arith b = zisc::cast<Arith>(system_stats.virtualMemoryFree());
    const zisc::ByteUnit bytes{b};
    const auto mb = bytes.representedAs<zisc::MebiUnit::exponent()>();
    std::cout << "## Free  virtual memory size: " << mb.value().toFloat<double>()
              << " ms" << std::endl;
  }
}
