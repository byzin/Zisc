/*!
  \file memory_arena_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <type_traits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/memory_arena.hpp"
#include "zisc/memory_manager_iterator.hpp"
#include "zisc/zisc_config.hpp"

using Storage1 = std::aligned_storage_t<32, 8>;
using Storage2 = std::aligned_storage_t<4, 4>;
using Storage3 = std::aligned_storage_t<256, 8>;
using Storage4 = Storage2;
using Storage5 = std::aligned_storage_t<1024, 8>;

using ChunkIterator = zisc::MemoryManagerIterator<zisc::MemoryChunk>;

TEST(MemoryArenaTest, StaticMemoryArenaTest)
{
  constexpr std::size_t arena_size = 1024;
  zisc::StaticMemoryArena<arena_size> arena;

  ASSERT_EQ(arena_size, arena.capacity());
  ASSERT_EQ(zisc::MemoryChunk::headerSize(), arena.usedMemory());

  {
    const auto chunk = arena.getFirstChunk();
    ASSERT_TRUE(chunk->isNull());
  }

  for (int i = 0; i < 2; ++i) {
    {
      auto chunk = arena.allocate(sizeof(Storage1), std::alignment_of_v<Storage1>);
      ASSERT_EQ(sizeof(Storage1), chunk->stride());
      ASSERT_EQ(64, arena.usedMemory());
    }
    {
      auto chunk = arena.allocate(sizeof(Storage2), std::alignment_of_v<Storage2>);
      ASSERT_EQ(sizeof(Storage2) + 4, chunk->stride());
      ASSERT_EQ(88, arena.usedMemory());
    }
    {
      auto chunk = arena.allocate(sizeof(Storage3), std::alignment_of_v<Storage3>);
      ASSERT_EQ(sizeof(Storage3), chunk->stride());
      ASSERT_EQ(360, arena.usedMemory());
    }
    {
      auto chunk = arena.allocate(sizeof(Storage4), std::alignment_of_v<Storage4>);
      ASSERT_EQ(sizeof(Storage4) + 4, chunk->stride());
      ASSERT_EQ(384, arena.usedMemory());
    }
    {
      auto chunk = arena.allocate(sizeof(Storage5), std::alignment_of_v<Storage5>);
      ASSERT_EQ(nullptr, chunk);
    }

    zisc::uint32b id = 0;
    for (auto ite = ChunkIterator{arena.getFirstChunk()};
         ite != ChunkIterator{};
         ++ite) {
      ASSERT_EQ(id, ite->id());
      ++id;
    }
    ASSERT_EQ(4, id);

    arena.reset();
  }

  {
    const auto chunk = arena.getFirstChunk();
    ASSERT_TRUE(chunk->isNull());
    ASSERT_EQ(zisc::MemoryChunk::headerSize(), arena.usedMemory());
  }
}

TEST(MemoryArenaTest, DynamicMemoryArenaTest)
{
  constexpr std::size_t arena_size = 32;
  zisc::DynamicMemoryArena<arena_size> arena;

  ASSERT_EQ(arena_size, arena.capacity());
  ASSERT_EQ(zisc::MemoryChunk::headerSize(), arena.usedMemory());

  {
    const auto chunk = arena.getFirstChunk();
    ASSERT_TRUE(chunk->isNull());
  }

  {
    auto chunk = arena.allocate(sizeof(Storage1), std::alignment_of_v<Storage1>);
    ASSERT_EQ(sizeof(Storage1), chunk->stride());
    ASSERT_EQ(80, arena.usedMemory());
    ASSERT_EQ(arena_size * 3, arena.capacity());
  }
  {
    auto chunk = arena.allocate(sizeof(Storage2), std::alignment_of_v<Storage2>);
    ASSERT_EQ(sizeof(Storage2) + 4, chunk->stride());
    ASSERT_EQ(120, arena.usedMemory());
    ASSERT_EQ(arena_size * 7, arena.capacity());
  }
  {
    auto chunk = arena.allocate(sizeof(Storage3), std::alignment_of_v<Storage3>);
    ASSERT_EQ(sizeof(Storage3), chunk->stride());
    ASSERT_EQ(424, arena.usedMemory());
    ASSERT_EQ(arena_size * 31, arena.capacity());
  }
  {
    auto chunk = arena.allocate(sizeof(Storage4), std::alignment_of_v<Storage4>);
    ASSERT_EQ(sizeof(Storage4) + 4, chunk->stride());
    ASSERT_EQ(448, arena.usedMemory());
    ASSERT_EQ(arena_size * 31, arena.capacity());
  }

  {
    auto ite = ChunkIterator{arena.getFirstChunk()};
    ASSERT_EQ(0, ite->id());
    ++ite;
    ASSERT_EQ(1, ite->id());
    ++ite;
    ASSERT_EQ(3, ite->id());
    ++ite;
    ASSERT_EQ(2, ite->id());
    ++ite;
    ASSERT_EQ(ChunkIterator{}, ite);
  }

  arena.reset();

  ASSERT_EQ(80, arena.usedMemory());
  {
    auto chunk = arena.allocate(sizeof(Storage1), std::alignment_of_v<Storage1>);
    ASSERT_EQ(sizeof(Storage1), chunk->stride());
    ASSERT_EQ(128, arena.usedMemory());
    ASSERT_EQ(arena_size * 31, arena.capacity());
  }
  {
    auto chunk = arena.allocate(sizeof(Storage5), std::alignment_of_v<Storage5>);
    ASSERT_EQ(sizeof(Storage5), chunk->stride());
    ASSERT_EQ(1200, arena.usedMemory());
    ASSERT_EQ(arena_size * 127, arena.capacity());
  }
  {
    auto chunk = arena.allocate(sizeof(Storage2), std::alignment_of_v<Storage2>);
    ASSERT_EQ(sizeof(Storage2) + 4, chunk->stride());
    ASSERT_EQ(1224, arena.usedMemory());
    ASSERT_EQ(arena_size * 127, arena.capacity());
  }
  {
    auto chunk = arena.allocate(sizeof(Storage3), std::alignment_of_v<Storage3>);
    ASSERT_EQ(sizeof(Storage3), chunk->stride());
    ASSERT_EQ(1496, arena.usedMemory());
    ASSERT_EQ(arena_size * 127, arena.capacity());
  }
  {
    auto chunk = arena.allocate(sizeof(Storage4), std::alignment_of_v<Storage4>);
    ASSERT_EQ(sizeof(Storage4) + 4, chunk->stride());
    ASSERT_EQ(1520, arena.usedMemory());
    ASSERT_EQ(arena_size * 127, arena.capacity());
  }

  {
    auto ite = ChunkIterator{arena.getFirstChunk()};
    ASSERT_EQ(0, ite->id());
    ++ite;
    ASSERT_EQ(2, ite->id());
    ++ite;
    ASSERT_EQ(4, ite->id());
    ++ite;
    ASSERT_EQ(3, ite->id());
    ++ite;
    ASSERT_EQ(1, ite->id());
    ++ite;
    ASSERT_EQ(ChunkIterator{}, ite);
  }

  arena.reset();

  {
    const auto chunk = arena.getFirstChunk();
    ASSERT_TRUE(chunk->isLinkChunk());
    ASSERT_EQ(zisc::MemoryChunk::headerSize() * 7, arena.usedMemory());
  }
}
