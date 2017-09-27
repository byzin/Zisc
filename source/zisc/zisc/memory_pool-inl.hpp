/*!
  \file memory_pool-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_POOL_INL_HPP
#define ZISC_MEMORY_POOL_INL_HPP

#include "memory_pool.hpp"
// Standard C++ library
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "memory_chunk.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
inline
MemoryPool::MemoryPool(const std::size_t memory_size) noexcept :
    used_bytes_{0},
    num_of_chunks_{0}
{
  initialize(memory_size);
}

/*!
  */
inline
MemoryPool::MemoryPool(MemoryPool&& other) noexcept :
    memory_pool_{std::move(other.memory_pool_)},
    used_bytes_{other.used_bytes_},
    num_of_chunks_{other.num_of_chunks_}
{
}

/*!
  */
inline
MemoryPool& MemoryPool::operator=(MemoryPool&& other) noexcept
{
  memory_pool_ = std::move(other.memory_pool_);
  used_bytes_ = other.used_bytes_;
  num_of_chunks_ = other.num_of_chunks_;
  return *this;
}

/*!
  */
template <typename Type> inline
MemoryChunk* MemoryPool::allocate(const uint n) noexcept
{
  MemoryChunk* chunk = nullptr;
  std::size_t memory_space = size() - usedMemory();
  if ((MemoryChunk::headerSize() + sizeof(Type) * n) <= memory_space) {
    // Create a memory chunk
    chunk = treatAs<MemoryChunk*>(data() + usedMemory());
    memory_space = memory_space - MemoryChunk::headerSize();

    // Initialize the memory chunk
    chunk->reset();
    chunk->setId(numOfChunks());
    chunk->setChunkInfo<Type>(n, memory_space);

    // Update the memory pool info
    if (chunk->stride() <= memory_space) {
      memory_space = memory_space - chunk->stride();
      used_bytes_ = size() - memory_space;
      ++num_of_chunks_;
      ZISC_ASSERT((MemoryChunk::headerSize() + chunk->stride()) % MemoryChunk::headerAlignment() == 0,
                  "The alignment of memory chunk failed.");
    }
    else {
      chunk = nullptr;
    }
  }
  return chunk;
}

/*!
  */
inline
uint8* MemoryPool::data() noexcept
{
  return treatAs<uint8*>(memory_pool_.data());
}

/*!
  */
inline
const uint8* MemoryPool::data() const noexcept
{
  return treatAs<const uint8*>(memory_pool_.data());
}

/*!
  */
inline
MemoryChunk* MemoryPool::getChunk(const uint index) noexcept
{
  MemoryChunk* chunk = (index < numOfChunks())
      ?  treatAs<MemoryChunk*>(data() + getChunkPosition(index))
      : nullptr;
  return chunk;
}

/*!
  */
inline
const MemoryChunk* MemoryPool::getChunk(const uint index) const noexcept
{
  const MemoryChunk* chunk = (index < numOfChunks())
      ? treatAs<const MemoryChunk*>(data() + getChunkPosition(index))
      : nullptr;
  return chunk;
}

/*!
  */
inline
MemoryChunk* MemoryPool::getChunk(void* ptr) noexcept
{
  MemoryChunk* chunk = (hasContained(ptr))
      ? treatAs<MemoryChunk*>(data() + getChunkPosition(ptr))
      : nullptr;
  return chunk;
}

/*!
  */
inline
const MemoryChunk* MemoryPool::getChunk(const void* ptr) const noexcept
{
  const MemoryChunk* chunk = (hasContained(ptr))
      ? treatAs<const MemoryChunk*>(data() + getChunkPosition(ptr))
      : nullptr;
  return chunk;
}

/*!
  */
inline
bool MemoryPool::hasContained(const void* ptr) const noexcept
{
  auto d = cast<const uint8*>(ptr);
  auto lower = data();
  auto upper = lower + size();
  return isInBounds(d, lower, upper);
}

/*!
  */
inline
uint MemoryPool::numOfChunks() const noexcept
{
  return cast<uint>(num_of_chunks_);
}

/*!
  */
inline
void MemoryPool::reset() noexcept
{
  used_bytes_ = 0;
  num_of_chunks_ = 0;
}

/*!
  */
inline
std::size_t MemoryPool::size() const noexcept
{
  const std::size_t memory_size = sizeof(AlignedBlock) * memory_pool_.size();
  return memory_size;
}

/*!
  */
inline
std::size_t MemoryPool::setSize(const std::size_t memory_size) noexcept
{
  reset();

  constexpr auto alignment = MemoryChunk::headerAlignment();
  // Resize the memory pool
  {
    const std::size_t block_length = (memory_size % alignment == 0)
        ? (memory_size / alignment)
        : (memory_size / alignment + 1);
    memory_pool_.resize(block_length);
  }

  // Align the memory pool
  {
    auto memory_space = size();
    auto p = cast<void*>(memory_pool_.data());
    auto result = std::align(alignment, size(), p, memory_space);
    if (result == nullptr) {
      ZISC_ASSERT(false, "The alignment of the memory pool failed.");
    }
    const auto l = size() - memory_space;
    ZISC_ASSERT(l == 0, "The address of the pool head isn't aligned.");
    used_bytes_ += l;
  }

  return size();
}

/*!
  */
inline
std::size_t MemoryPool::usedMemory() const noexcept
{
  return used_bytes_;
}

/*!
  */
inline
std::size_t MemoryPool::getChunkPosition(const uint index) const noexcept
{
  const auto p = data();
  std::size_t position = 0;
  for (uint i = 0; i < index; ++i) {
    auto chunk = treatAs<const MemoryChunk*>(p + position);
    position += chunk->headerSize() + chunk->stride();
  }
  return position;
}

/*!
  */
inline
std::size_t MemoryPool::getChunkPosition(const void* ptr) const noexcept
{
  auto p = cast<const uint8*>(ptr);
  for (; *(p - 1) != MemoryChunk::boundaryValue(); --p) {
    ZISC_ASSERT(*(p - 1) == MemoryChunk::paddingValue(),
                "The padding value is invalid.");
  }
  p = p - MemoryChunk::headerSize();
  const std::size_t position = p - data();
  return position;
}

/*!
  */
inline
void MemoryPool::initialize(const std::size_t memory_size) noexcept
{
  setSize(memory_size);

  // Check type
  static_assert(sizeof(std::size_t) <= sizeof(std::vector<AlignedBlock>), "");
  static_assert(alignof(std::size_t) <= alignof(std::vector<AlignedBlock>), "");
  static_assert(sizeof(uint32) <= sizeof(std::size_t), "");
  static_assert(alignof(uint32) <= alignof(std::size_t), "");

  // Avoid warnings
  padding_ = 0;
}

} // namespace zisc

#endif // ZISC_MEMORY_POOL_INL_HPP
