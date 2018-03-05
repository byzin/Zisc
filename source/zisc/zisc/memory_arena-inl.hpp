/*!
  \file memory_arena-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_ARENA_INL_HPP
#define ZISC_MEMORY_ARENA_INL_HPP

#include "memory_arena.hpp"
// Standard C++ library
#include <cstddef>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "error.hpp"
#include "memory_chunk.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Static memory arena

/*!
  */
template <std::size_t kArenaSize> inline
StaticMemoryArena<kArenaSize>::MemoryArena() noexcept
{
  initialize();
}

/*!
  */
template <std::size_t kArenaSize> inline
MemoryChunk* StaticMemoryArena<kArenaSize>::allocate(
    const std::size_t size,
    const std::size_t alignment) noexcept
{
  std::size_t used_memory = usedMemory();
  constexpr std::size_t chunk_size = MemoryChunk::headerSize();

  auto chunk = getChunk(used_memory - chunk_size);
  ZISC_ASSERT(chunk->isNull(), "The chunk isn't null.");
  chunk->setChunkInfo(size, alignment);

  used_memory += chunk_size + chunk->stride();
  if (used_memory <= capacity()) {
    // Set chunk ID
    chunk->setId(num_of_chunk_);
    ++num_of_chunk_;

    auto tail = getChunk(used_memory - chunk_size);
    tail->reset();

    used_memory_ = used_memory;
  }
  else {
    // Allocation failed
    chunk->reset();
    chunk = nullptr;
  }
  return chunk;
}

/*!
  */
template <std::size_t kArenaSize> inline
std::size_t StaticMemoryArena<kArenaSize>::capacity() const noexcept
{
  return kArenaSize;
}

/*!
  */
template <std::size_t kArenaSize> inline
MemoryChunk* StaticMemoryArena<kArenaSize>::getFirstChunk() noexcept
{
  auto chunk = treatAs<MemoryChunk*>(&data_);
  ZISC_ASSERT(MemoryChunk::isAligned(chunk), "The chunk address isn't aligned.");
  return chunk;
}

/*!
  */
template <std::size_t kArenaSize> inline
const MemoryChunk* StaticMemoryArena<kArenaSize>::getFirstChunk() const noexcept
{
  const auto chunk = treatAs<const MemoryChunk*>(&data_);
  ZISC_ASSERT(MemoryChunk::isAligned(chunk), "The chunk address isn't aligned.");
  return chunk;
}

/*!
  */
template <std::size_t kArenaSize> inline
void StaticMemoryArena<kArenaSize>::reset() noexcept
{
  auto chunk = getFirstChunk();
  chunk->reset();
  used_memory_ = MemoryChunk::headerSize();
  num_of_chunk_ = 0;
}

/*!
  */
template <std::size_t kArenaSize> inline
std::size_t StaticMemoryArena<kArenaSize>::usedMemory() const noexcept
{
  return used_memory_;
}

/*!
  */
template <std::size_t kArenaSize> inline
MemoryChunk* StaticMemoryArena<kArenaSize>::getChunk(const std::size_t offset)
    noexcept
{
  auto chunk = treatAs<MemoryChunk*>(treatAs<uint8*>(&data_) + offset);
  ZISC_ASSERT(MemoryChunk::isAligned(chunk), "The chunk address isn't aligned.");
  return chunk;
}

/*!
  */
template <std::size_t kArenaSize> inline
void StaticMemoryArena<kArenaSize>::initialize() noexcept
{
  // Reset memory usage
  reset();
}

// Dynamic memory arena

/*!
  */
template <std::size_t kArenaSize> inline
DynamicMemoryArena<kArenaSize>::MemoryArena() noexcept
{
  initialize();
}

/*!
  */
template <std::size_t kArenaSize> inline
DynamicMemoryArena<kArenaSize>::MemoryArena(MemoryArena&& other) noexcept :
    arena_{std::move(other.arena_)}
{
}

/*!
  */
template <std::size_t kArenaSize> inline
DynamicMemoryArena<kArenaSize>::~MemoryArena() noexcept
{
  for (auto& memory : arena_)
    std::free(memory.data_);
}

/*!
  */
template <std::size_t kArenaSize> inline
MemoryChunk* DynamicMemoryArena<kArenaSize>::allocate(
    const std::size_t size,
    const std::size_t alignment) noexcept
{
  constexpr std::size_t chunk_size = MemoryChunk::headerSize();
  // Find the best allocatable memory space
  MemoryChunk* chunk = nullptr;
  for (std::size_t memory_num = 0; true; ++memory_num) {
    if (memory_num == arena_.size()) {
      // Expand memory arena
      const bool result = expandArena();
      if (!result)
        break;
    }

    auto& memory = arena_[memory_num];
    std::size_t used_memory = memory.used_memory_;

    chunk = getChunk(memory_num, used_memory - chunk_size);
    ZISC_ASSERT(chunk->isNull() || chunk->isLinkChunk(),
                "The chunk isn't null or link chunk: arena[",
                memory_num, "] + ", used_memory);
    auto link = chunk->isLinkChunk() ? chunk->linkedChunk() : nullptr;

    chunk->setChunkInfo(size, alignment);
    used_memory += chunk_size + chunk->stride();
    if (used_memory <= getMemoryCapacity(memory_num)) {
      // Set ID
      chunk->setId(num_of_chunk_);
      ++num_of_chunk_;

      auto tail = getChunk(memory_num, used_memory - chunk_size);
      tail->reset();
      if (link != nullptr)
        tail->setLink(link);

      memory.used_memory_ = used_memory;
      break;
    }
    else {
      // Allocation failed
      chunk->reset();
      if (link != nullptr)
        chunk->setLink(link);
      chunk = nullptr;
    }
  }
  return chunk;
}

/*!
  */
template <std::size_t kArenaSize> inline
std::size_t DynamicMemoryArena<kArenaSize>::capacity() const noexcept
{
  const std::size_t c = getMemoryCapacity(arena_.size()) - kArenaSize;
  return c;
}

/*!
  */
template <std::size_t kArenaSize> inline
MemoryChunk* DynamicMemoryArena<kArenaSize>::getFirstChunk() noexcept
{
  auto chunk = treatAs<MemoryChunk*>(arena_[0].data_);
  ZISC_ASSERT(MemoryChunk::isAligned(chunk), "The chunk address isn't aligned.");
  return chunk;
}

/*!
  */
template <std::size_t kArenaSize> inline
const MemoryChunk* DynamicMemoryArena<kArenaSize>::getFirstChunk() const noexcept
{
  const auto chunk = treatAs<const MemoryChunk*>(arena_[0].data_);
  ZISC_ASSERT(MemoryChunk::isAligned(chunk), "The chunk address isn't aligned.");
  return chunk;
}

/*!
  */
template <std::size_t kArenaSize> inline
void DynamicMemoryArena<kArenaSize>::reset() noexcept
{
  for (std::size_t i = 0; i < arena_.size(); ++i) {
    auto& memory = arena_[i];
    memory.used_memory_ = MemoryChunk::headerSize();
    auto chunk = getChunk(i, 0);
    chunk->reset();
    if (i != (arena_.size() - 1))
      chunk->setLink(getChunk(i + 1, 0));
  }
  num_of_chunk_ = 0;
}

/*!
  */
template <std::size_t kArenaSize> inline
std::size_t DynamicMemoryArena<kArenaSize>::usedMemory() const noexcept
{
  std::size_t used_memory = 0;
  for (const auto& memory : arena_)
    used_memory += memory.used_memory_;
  return used_memory;
}

/*!
  */
template <std::size_t kArenaSize> inline
bool DynamicMemoryArena<kArenaSize>::expandArena() noexcept
{
  constexpr std::size_t chunk_alignment = MemoryChunk::headerAlignment();
  constexpr std::size_t chunk_size = MemoryChunk::headerSize();

  const std::size_t level = arena_.size();
  auto data = std::aligned_alloc(chunk_alignment, getMemoryCapacity(level));

  const bool result = data != nullptr;
  if (result) {
    // Initialize the allocated memory
    auto chunk = cast<MemoryChunk*>(data);
    chunk->reset();
    // Connect the allocated memory into the chunk chain
    if (0 < level) {
      auto tail = getChunk(level - 1, arena_[level - 1].used_memory_ - chunk_size);
      ZISC_ASSERT(tail->isNull(), "The tail chunk isn't null.");
      tail->setLink(chunk);
    }
    arena_.emplace_back(Memory{cast<uint8*>(data), chunk_size});
  }

  return result;
}

/*!
  */
template <std::size_t kArenaSize> inline
MemoryChunk* DynamicMemoryArena<kArenaSize>::getChunk(
    const std::size_t memory_num,
    const std::size_t offset) noexcept
{
  auto chunk = treatAs<MemoryChunk*>(arena_[memory_num].data_ + offset);
  ZISC_ASSERT(MemoryChunk::isAligned(chunk), "The chunk address isn't aligned.");
  return chunk;
}

/*!
  */
template <std::size_t kArenaSize> inline
std::size_t DynamicMemoryArena<kArenaSize>::getMemoryCapacity(
    const std::size_t memory_num) noexcept
{
  const std::size_t c = kArenaSize << memory_num;
  return c;
}

/*!
  */
template <std::size_t kArenaSize> inline
void DynamicMemoryArena<kArenaSize>::initialize() noexcept
{
  // Reserve memory pointers
  arena_.reserve(8);
  // Allocate first memory
  const bool result = expandArena();
  ZISC_ASSERT(result, "The first memory allocation failed.");
  // Reset memory usage
  reset();
}

} // namespace zisc

#endif // ZISC_MEMORY_ARENA_INL_HPP
