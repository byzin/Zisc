/*!
  \file aligned_memory_pool_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <type_traits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/aligned_memory_pool.hpp"

TEST(AlignedMemoryPoolTest, AllocateTest)
{
  constexpr std::size_t memory_size = 1024;
  constexpr std::size_t alignment = std::alignment_of<double>::value;
  zisc::AlignedMemoryPool<memory_size, alignment> memory_pool;

  ASSERT_EQ(memory_size, memory_pool.size());

  std::size_t used_memory = 0;

  auto value1 = memory_pool.allocate<std::int64_t>();
  *value1 = 10;
  ASSERT_EQ(10, *value1);
  used_memory += sizeof(*value1);
  ASSERT_EQ(used_memory, memory_pool.usedMemory()) 
      << "Memory allocation test failed.";

  auto value2 = memory_pool.allocate<double>(3.14);
  ASSERT_DOUBLE_EQ(3.14, *value2);
  used_memory += sizeof(*value2);
  ASSERT_EQ(used_memory, memory_pool.usedMemory())
      << "Memory allocation test failed.";
}
