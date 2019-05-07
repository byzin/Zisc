/*!
  \file memory_manager_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/error.hpp"
#include "zisc/memory_chunk.hpp"
#include "zisc/memory_manager.hpp"
#include "zisc/memory_manager_iterator.hpp"
#include "zisc/unique_memory_pointer.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

TEST(MemoryManagerTest, AllocationTest)
{
  constexpr std::size_t memory_size = 1024;

  std::mutex m;
  zisc::DynamicMemoryManager<memory_size> memory_manager;

  memory_manager.setMutex(&m);

  ASSERT_EQ(sizeof(zisc::MemoryChunk), memory_manager.usedMemory());
  ASSERT_EQ(memory_size, memory_manager.capacity());

  constexpr int expected1 = 24;
  constexpr double expected2 = 3.14;
  constexpr bool expected3 = true;

  auto p1 = zisc::UniqueMemoryPointer<int>::make(&memory_manager, expected1);
  auto p2 = zisc::UniqueMemoryPointer<double>::make(&memory_manager, expected2);
  auto p3 = zisc::UniqueMemoryPointer<bool>::make(&memory_manager, expected3);

  ASSERT_EQ(expected1, *p1);
  ASSERT_EQ(expected2, *p2);
  ASSERT_EQ(expected3, *p3);

  {
    auto i = memory_manager.begin();
    ASSERT_EQ(expected1, *(i->data<int>()));

    ++i;
    ASSERT_EQ(expected2, *(i->data<double>()));

    ++i;
    ASSERT_EQ(expected3, *(i->data<bool>()));
  }

  p1.reset();
  p2.reset();
  p3.reset();
}
