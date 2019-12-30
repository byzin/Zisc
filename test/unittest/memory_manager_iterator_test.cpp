/*!
  \file memory_manager_iterator_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <type_traits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/memory_chunk.hpp"
#include "zisc/memory_manager_iterator.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

constexpr std::size_t kStorageSize = 1024;
using ChunkStorage = std::aligned_storage_t<kStorageSize,
                                            zisc::MemoryChunk::headerAlignment()>;

template <typename ChunkT>
zisc::MemoryManagerIterator<ChunkT> makeIterator(
  zisc::uint8b* pool,
  zisc::uint32b id,
  std::size_t& storage_size)
{
  // Chunk1
  auto chunk1 = zisc::treatAs<zisc::MemoryChunk*>(pool);
  {
    storage_size -= chunk1->headerSize();
    chunk1->reset();
    chunk1->setId(id++);
    chunk1->setChunkInfo<int>(1);
    storage_size -= chunk1->stride();
  }
  // Chunk2
  {
    const std::size_t chunk2_offset =  kStorageSize - storage_size;
    auto chunk2 = zisc::treatAs<zisc::MemoryChunk*>(pool + chunk2_offset);
    storage_size -= chunk2->headerSize();
    chunk2->reset();
    chunk2->setId(id++);
    chunk2->setChunkInfo<double>(2);
    storage_size -= chunk2->stride();
  }
  {
    // Chunk3
    const std::size_t chunk3_offset =  kStorageSize - storage_size;
    auto chunk3 = zisc::treatAs<zisc::MemoryChunk*>(pool + chunk3_offset);
    storage_size -= chunk3->headerSize();
    chunk3->reset();
    // Chunk3
    const std::size_t chunk3_2_offset =  kStorageSize - storage_size;
    auto chunk3_2 = zisc::treatAs<zisc::MemoryChunk*>(pool + chunk3_2_offset);
    storage_size -= chunk3_2->headerSize();
    chunk3_2->reset();
    chunk3->setLink(chunk3_2);
    // Chunk4
    const std::size_t chunk4_offset =  kStorageSize - storage_size;
    auto chunk4 = zisc::treatAs<zisc::MemoryChunk*>(pool + chunk4_offset);
    storage_size -= chunk4->headerSize();
    chunk4->reset();
    chunk4->setId(id++);
    chunk4->setChunkInfo<char>(1);
    storage_size -= chunk4->stride();
    chunk3_2->setLink(chunk4);
  }
  {
    // Chunk5
    const std::size_t chunk5_offset =  kStorageSize - storage_size;
    auto chunk5 = zisc::treatAs<zisc::MemoryChunk*>(pool + chunk5_offset);
    storage_size -= chunk5->headerSize();
    chunk5->reset();
    // Chunk6
    const std::size_t chunk6_offset =  kStorageSize - storage_size;
    auto chunk6 = zisc::treatAs<zisc::MemoryChunk*>(pool + chunk6_offset);
    storage_size -= chunk6->headerSize();
    chunk6->reset();
    chunk5->setLink(chunk6);
  }
  return zisc::MemoryManagerIterator<ChunkT>{chunk1};
}

} // namespace 

TEST(MemoryManagerIteratorTest, IterationTest)
{
  ::ChunkStorage storage;
  std::size_t storage_size = ::kStorageSize;
  auto pool = zisc::treatAs<zisc::uint8b*>(&storage);
  zisc::uint32b id = 1;

  auto i = ::makeIterator<zisc::MemoryChunk>(pool, id, storage_size);
  const auto end = zisc::MemoryManagerIterator<zisc::MemoryChunk>::end();
  ASSERT_LT(0, storage_size);
  // Chunk1
  {
    ASSERT_EQ(1, (*i).id());
    ASSERT_NE(end, i);

    i->setId(100);
    ASSERT_EQ(100, i->id());

    ++i;
  }
  // Chunk2
  {
    auto chunk_iterator = i++;
    ASSERT_EQ(2, chunk_iterator->id());
    ASSERT_NE(end, chunk_iterator);
  }
  // Chunk4
  {
    ASSERT_EQ(3, i->id());
    ASSERT_NE(end, i);
    ++i;
  }
  // End
  {
    ASSERT_EQ(end, i);
  }
}

TEST(MemoryManagerIteratorTest, ConstIterationTest)
{
  ::ChunkStorage storage;
  std::size_t storage_size = ::kStorageSize;
  auto pool = zisc::treatAs<zisc::uint8b*>(&storage);
  zisc::uint32b id = 1;

  auto i = ::makeIterator<const zisc::MemoryChunk>(pool, id, storage_size);
  const auto end = zisc::MemoryManagerIterator<const zisc::MemoryChunk>::end();
  ASSERT_LT(0, storage_size);
  // Chunk1
  {
    ASSERT_EQ(1, (*i).id());
    ASSERT_NE(end, i);
    ASSERT_EQ(1, i->id());
    ++i;
  }
  // Chunk2
  {
    auto chunk_iterator = i++;
    ASSERT_EQ(2, chunk_iterator->id());
    ASSERT_NE(end, chunk_iterator);
  }
  // Chunk4
  {
    ASSERT_EQ(3, i->id());
    ASSERT_NE(end, i);
    ++i;
  }
  // End
  {
    ASSERT_EQ(end, i);
  }
}
