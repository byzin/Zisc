/*!
  \file simple_memory_resource_test.cpp
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
#include <array>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/memory/simple_memory_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace {

struct Data1
{
  std::array<zisc::uint64b, 10> value_;
};

}

TEST(SimpleMemoryResourceTest, MemoryAllocationTest)
{
  zisc::SimpleMemoryResource mem_resource;
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << "SimpleMemoryResource initialization failed.";
  ASSERT_FALSE(mem_resource.peakMemoryUsage())
      << "SimpleMemoryResource initialization failed.";

  using zisc::uint8b;
  using zisc::uint16b;
  using zisc::uint32b;
  using zisc::uint64b;

  constexpr std::size_t header_s = sizeof(zisc::SimpleMemoryResource::Header);
  std::size_t total = 0;
  std::size_t peak = 0;

  auto p1 = mem_resource.allocate(sizeof(uint8b), std::alignment_of_v<uint8b>);
  total += sizeof(uint8b) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p2 = mem_resource.allocate(sizeof(uint16b), std::alignment_of_v<uint16b>);
  total += sizeof(uint16b) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p3 = mem_resource.allocate(sizeof(::Data1), std::alignment_of_v<::Data1>);
  total += sizeof(::Data1) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  mem_resource.deallocate(p3, sizeof(::Data1), std::alignment_of_v<::Data1>);
  total -= sizeof(::Data1) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p4 = mem_resource.allocate(sizeof(uint32b), std::alignment_of_v<uint32b>);
  total += sizeof(uint32b) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p5 = mem_resource.allocate(sizeof(uint64b), std::alignment_of_v<uint64b>);
  total += sizeof(uint64b) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  p3 = mem_resource.allocate(sizeof(::Data1), std::alignment_of_v<::Data1>);
  total += sizeof(::Data1) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p6 = mem_resource.allocate(sizeof(long double),
                                  std::alignment_of_v<long double>);
  total += sizeof(long double) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  mem_resource.deallocate(p1, sizeof(uint8b), std::alignment_of_v<uint8b>);
  mem_resource.deallocate(p2, sizeof(uint16b), std::alignment_of_v<uint16b>);
  mem_resource.deallocate(p3, sizeof(::Data1), std::alignment_of_v<::Data1>);
  mem_resource.deallocate(p4, sizeof(uint32b), std::alignment_of_v<uint32b>);
  mem_resource.deallocate(p5, sizeof(uint64b), std::alignment_of_v<uint64b>);
  mem_resource.deallocate(p6, sizeof(long double), std::alignment_of_v<long double>);

  {
    zisc::SimpleMemoryResource mem_resource1{std::move(mem_resource)};
    zisc::SimpleMemoryResource mem_resource2;
    mem_resource2 = std::move(mem_resource1);
    ASSERT_FALSE(mem_resource2.totalMemoryUsage());
    ASSERT_GE(mem_resource2.peakMemoryUsage(), peak);
  }
}

TEST(SimpleMemoryResourceTest, ResourceComparisonTest)
{
  zisc::SimpleMemoryResource mem_resource1;
  zisc::SimpleMemoryResource mem_resource2;

  ASSERT_TRUE(mem_resource1.is_equal(mem_resource1));
  ASSERT_FALSE(mem_resource1.is_equal(mem_resource2));
}

TEST(SimpleMemoryResourceTest, AlignmentTest)
{
  zisc::SimpleMemoryResource mem_resource;
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << "SimpleMemoryResource initialization failed.";
  ASSERT_FALSE(mem_resource.peakMemoryUsage())
      << "SimpleMemoryResource initialization failed.";

  auto test_allocation = [&mem_resource](const std::size_t alignment)
  {
    std::size_t size = 3 * alignment;
    void* ptr = mem_resource.allocate(size, alignment);
    {
      void* p = ptr;
      ASSERT_EQ(std::align(alignment, size, p, size), ptr)
          << "Allocation with " << alignment << " alignment is failed.";
      ASSERT_EQ(p, ptr)
          << "Allocation with " << alignment << " alignment is failed.";
    }
    mem_resource.deallocate(ptr, alignment, alignment);
    ASSERT_FALSE(mem_resource.totalMemoryUsage())
        << "Allocation with " << alignment << " alignment is failed.";
  };

  // 1 byte alignment
  {
    constexpr std::size_t alignment = 1;
    test_allocation(alignment);
  }
  // 2 bytes alignment
  {
    constexpr std::size_t alignment = 2;
    test_allocation(alignment);
  }
  // 4 bytes alignment
  {
    constexpr std::size_t alignment = 4;
    test_allocation(alignment);
  }
  // 8 bytes alignment
  {
    constexpr std::size_t alignment = 8;
    test_allocation(alignment);
  }
  // std::max_align_t
  {
    constexpr std::size_t alignment = std::alignment_of_v<std::max_align_t>;
    test_allocation(alignment);
  }
  // 64 bytes alignment
  {
    constexpr std::size_t alignment = 64;
    test_allocation(alignment);
  }
  // 256 bytes alignment
  {
    constexpr std::size_t alignment = 256;
    test_allocation(alignment);
  }
  // 1024 bytes alignment
  {
    constexpr std::size_t alignment = 1024;
    test_allocation(alignment);
  }
  // 4096 bytes alignment
  {
    constexpr std::size_t alignment = 4096;
    test_allocation(alignment);
  }
}
