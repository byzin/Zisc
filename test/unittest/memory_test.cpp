/*!
  \file memory_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <cstddef>
#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
// Googletest
#include "googletest.hpp"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/math/unit_multiple.hpp"
#include "zisc/memory/memory.hpp"

TEST(MemoryTest, SystemMemoryStatsTest)
{
  const auto system_stats = zisc::Memory::retrieveSystemStats();

  std::cout << std::defaultfloat << std::endl;
  using zisc::cast;
  using Arith = typename zisc::ByteUnit::Type;
  {
    constexpr std::size_t m = std::numeric_limits<std::size_t>::max();
    ASSERT_NE(system_stats.totalPhysicalMemory(), m) << "Unsupported platform.";
  }
  {
    const Arith b = zisc::cast<Arith>(system_stats.totalPhysicalMemory());
    const zisc::ByteUnit bytes{b};
    const auto mb = bytes.cast<zisc::MebiUnit::exponent()>();
    std::cout << "## Total physical memory size: " << cast<double>(mb.value())
              << " MB" << std::endl;
    EXPECT_GT(b, 0) << "The platform has no physical memory";
  }
  {
    const Arith b = zisc::cast<Arith>(system_stats.availablePhysicalMemory());
    const zisc::ByteUnit bytes{b};
    const auto mb = bytes.cast<zisc::MebiUnit::exponent()>();
    std::cout << "## Free  physical memory size: " << cast<double>(mb.value())
              << " MB" << std::endl;
  }
  {
    const Arith b = zisc::cast<Arith>(system_stats.totalVirtualMemory());
    const zisc::ByteUnit bytes{b};
    const auto mb = bytes.cast<zisc::MebiUnit::exponent()>();
    std::cout << "## Total virtual memory size: " << cast<double>(mb.value())
              << " MB" << std::endl;
  }
  {
    const Arith b = zisc::cast<Arith>(system_stats.availableVirtualMemory());
    const zisc::ByteUnit bytes{b};
    const auto mb = bytes.cast<zisc::MebiUnit::exponent()>();
    std::cout << "## Free  virtual memory size: " << cast<double>(mb.value())
              << " MB" << std::endl;
  }
}

TEST(MemoryTest, AlignmentTest)
{
  alignas(16) std::array<float, 4> storage{{0.0f, 1.0f, 2.0f, 3.0f}};
  {
    ASSERT_TRUE(zisc::Memory::isAligned(&storage, 16)) << "zisc::isAligned() failed.";
    ASSERT_FALSE(zisc::Memory::isAligned(&storage[1], 16)) << "zisc::isAligned() failed.";
  }
  {
    const auto* ptr1 = storage.data();
    const auto* ptr2 = zisc::assume_aligned<16>(ptr1);
    ASSERT_EQ(ptr1, ptr2) << "zisc::assume_aligned() failed.";
  }
}

TEST(MemoryTest, AllocateTest)
{
  auto test_alloc = [](const std::size_t s)
  {
    // Allocation test
    void* mem = zisc::aligned_alloc(s, s);
    EXPECT_TRUE(mem != nullptr)
        << "zisc::aligned_alloc(" << s << "," << s << ") failed.";
    if (mem == nullptr)
      return;
    // Alignment test
    EXPECT_TRUE(zisc::Memory::isAligned(mem, s))
        << "zisc::aligned_alloc(" << s << "," << s << ") failed.";
    // Size test
    zisc::uint8b* data = reinterpret_cast<zisc::uint8b*>(mem);
    for (std::size_t i = 0; i < s; ++i)
      data[i] = (std::numeric_limits<zisc::uint8b>::max)();

    zisc::free(mem);
  };
  test_alloc(1);
  test_alloc(4);
  test_alloc(16);
  test_alloc(64);
  test_alloc(1024);
  test_alloc(4096);
}
