/*!
  \file memory_chunk_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <iostream>
#include <memory>
#include <type_traits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/memory_chunk.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

using MemoryPool = std::aligned_storage_t<zisc::MemoryChunk::headerSize() * 16,
                                          zisc::MemoryChunk::headerAlignment()>;

using TestData1 = std::aligned_storage_t<32, 32>;
using TestData2 = std::aligned_storage_t<4, 4>;

zisc::MemoryChunk* getChunkHeade(MemoryPool* memory_pool, std::size_t& memory_space)
{
  constexpr std::size_t a = 32;
  constexpr std::size_t s = sizeof(zisc::MemoryChunk);
  void* p = zisc::cast<void*>(memory_pool);

  auto result = std::align(a, s, p, memory_space);
  return zisc::cast<zisc::MemoryChunk*>(result);
}

}

TEST(MemoryChunkTest, InitTest)
{
  ::MemoryPool memory_pool;
  // Test1
  {
    constexpr std::size_t a = alignof(::TestData1);
    constexpr std::size_t s = sizeof(::TestData1);
    std::size_t memory_space = sizeof(::MemoryPool);

    // Create an uninitialized memory chunk
    auto chunk = ::getChunkHeade(&memory_pool, memory_space);
    ASSERT_TRUE(chunk != nullptr) << "The alignment of the chunk is failed.";

    // Initialize the chunk
    chunk->reset();
    chunk->setId(0);
    ASSERT_EQ(0, chunk->id());
    ASSERT_FALSE(chunk->isValid()) << "The chunk is valid.";
    ASSERT_TRUE(chunk->isFreed()) << "The chunk isn't free.";

    chunk->setChunkInfo<::TestData1>(1, memory_space);
    chunk->setFree(false);
    ASSERT_TRUE(chunk->isValid()) << "The chunk is invalid.";
    ASSERT_FALSE(chunk->isFreed()) << "The chunk is free.";
    ASSERT_EQ(s, chunk->size()) << "The size of the chunk is wrong.";
    ASSERT_EQ(s + 16, chunk->stride()) << "The stride of the chunk is wrong.";

    // Get data pointer
    const auto p1 = zisc::treatAs<const zisc::uint8*>(chunk);
    const auto p2 = chunk->data<const zisc::uint8>();
    ASSERT_EQ(a, p2 - p1) << "The data address is wrong.";
  }
  // Test2
  {
    constexpr zisc::uint n = 3;
    constexpr std::size_t s = zisc::cast<std::size_t>(n) * sizeof(::TestData2);
    std::size_t memory_space = sizeof(::MemoryPool);

    // Create an uninitialized memory chunk
    auto chunk = ::getChunkHeade(&memory_pool, memory_space);
    ASSERT_TRUE(chunk != nullptr) << "The alignment of the chunk is failed.";

    // Initialize the chunk
    chunk->reset();
    chunk->setId(1);
    ASSERT_EQ(1, chunk->id());
    ASSERT_FALSE(chunk->isValid()) << "The chunk is valid.";
    ASSERT_TRUE(chunk->isFreed()) << "The chunk isn't free.";

    chunk->setChunkInfo<::TestData2>(n, memory_space);
    chunk->setFree(false);
    ASSERT_TRUE(chunk->isValid()) << "The chunk is invalid.";
    ASSERT_FALSE(chunk->isFreed()) << "The chunk is free.";
    ASSERT_EQ(s, chunk->size()) << "The size of the chunk is wrong.";
    ASSERT_EQ(s + 4, chunk->stride()) << "The stride of the chunk is wrong.";

    // Get data pointer
    const auto p1 = zisc::treatAs<const zisc::uint8*>(chunk);
    const auto p2 = chunk->data<const zisc::uint8>();
    ASSERT_EQ(16, p2 - p1) << "The data address is wrong.";
  }
}
