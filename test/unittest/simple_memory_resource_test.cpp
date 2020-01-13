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
#include "zisc/simple_memory_resource.hpp"
#include "zisc/std_memory_resource.hpp"
#include "zisc/zisc_config.hpp"

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

  auto p1 = mem_resource.do_allocate(sizeof(uint8b),
                                     std::alignment_of_v<uint8b>);
  total += sizeof(uint8b) + header_s;
  peak = total;
  ASSERT_EQ(mem_resource.totalMemoryUsage(), total);
  ASSERT_EQ(mem_resource.peakMemoryUsage(), peak);

  auto p2 = mem_resource.do_allocate(sizeof(uint16b),
                                     std::alignment_of_v<uint16b>);
  total += sizeof(uint16b) + header_s;
  peak = total;
  ASSERT_EQ(mem_resource.totalMemoryUsage(), total);
  ASSERT_EQ(mem_resource.peakMemoryUsage(), peak);

  auto p3 = mem_resource.do_allocate(sizeof(::Data1),
                                     std::alignment_of_v<::Data1>);
  total += sizeof(::Data1) + header_s;
  peak = total;
  ASSERT_EQ(mem_resource.totalMemoryUsage(), total);
  ASSERT_EQ(mem_resource.peakMemoryUsage(), peak);

  mem_resource.do_deallocate(p3, sizeof(::Data1), std::alignment_of_v<::Data1>);
  total -= sizeof(::Data1) + header_s;
  ASSERT_EQ(mem_resource.totalMemoryUsage(), total);
  ASSERT_EQ(mem_resource.peakMemoryUsage(), peak);

  auto p4 = mem_resource.do_allocate(sizeof(uint32b),
                                     std::alignment_of_v<uint32b>);
  total += sizeof(uint32b) + header_s;
  ASSERT_EQ(mem_resource.totalMemoryUsage(), total);
  ASSERT_EQ(mem_resource.peakMemoryUsage(), peak);

  auto p5 = mem_resource.do_allocate(sizeof(uint64b),
                                     std::alignment_of_v<uint64b>);
  total += sizeof(uint64b) + header_s;
  ASSERT_EQ(mem_resource.totalMemoryUsage(), total);
  ASSERT_EQ(mem_resource.peakMemoryUsage(), peak);

  p3 = mem_resource.do_allocate(sizeof(::Data1),
                                     std::alignment_of_v<::Data1>);
  total += sizeof(::Data1) + header_s;
  peak = total;
  ASSERT_EQ(mem_resource.totalMemoryUsage(), total);
  ASSERT_EQ(mem_resource.peakMemoryUsage(), peak);

  auto p6 = mem_resource.do_allocate(sizeof(long double),
                                     std::alignment_of_v<long double>);
  total += sizeof(long double) + header_s;
  peak = total;
  ASSERT_EQ(mem_resource.totalMemoryUsage(), total);
  ASSERT_EQ(mem_resource.peakMemoryUsage(), peak);

  mem_resource.do_deallocate(p1, sizeof(uint8b), std::alignment_of_v<uint8b>);
  mem_resource.do_deallocate(p2, sizeof(uint16b), std::alignment_of_v<uint16b>);
  mem_resource.do_deallocate(p3, sizeof(::Data1), std::alignment_of_v<::Data1>);
  mem_resource.do_deallocate(p4, sizeof(uint32b), std::alignment_of_v<uint32b>);
  mem_resource.do_deallocate(p5, sizeof(uint64b), std::alignment_of_v<uint64b>);
  mem_resource.do_deallocate(p6, sizeof(long double), std::alignment_of_v<long double>);

  {
    zisc::SimpleMemoryResource mem_resource1{std::move(mem_resource)};
    zisc::SimpleMemoryResource mem_resource2;
    mem_resource2 = std::move(mem_resource1);
    ASSERT_FALSE(mem_resource2.totalMemoryUsage());
    ASSERT_EQ(mem_resource2.peakMemoryUsage(), peak);
  }
}

TEST(SimpleMemoryResourceTest, ResourceComparisonTest)
{
  zisc::SimpleMemoryResource mem_resource1;
  zisc::SimpleMemoryResource mem_resource2;

  ASSERT_TRUE(mem_resource1.do_is_equal(mem_resource1));
  ASSERT_FALSE(mem_resource1.do_is_equal(mem_resource2));
}
