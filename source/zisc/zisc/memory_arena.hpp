/*!
  \file memory_arena.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_ARENA_HPP
#define ZISC_MEMORY_ARENA_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
#include <vector>
// Zisc
#include "memory_chunk.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Memory allocation type of an arena
  */
enum class MemoryArenaType
{
  kStatic,
  kDynamic
};

/*!
  \brief A body of a memory pool
  */
template <MemoryArenaType, std::size_t> class MemoryArena;

/*!
  \details
  Static memory arena
  */
template <std::size_t kArenaSize>
class MemoryArena<MemoryArenaType::kStatic, kArenaSize>
{
 public:
  //! Initialize the arena
  MemoryArena() noexcept;


  //! Allocate memory
  MemoryChunk* allocate(const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Return the allocated space
  std::size_t capacity() const noexcept;

  //! Return the first memory chunk
  MemoryChunk* getFirstChunk() noexcept;

  //! Return the first memory chunk
  const MemoryChunk* getFirstChunk() const noexcept;

  //! Reset the used memory
  void reset() noexcept;

  //! Return the used memory
  std::size_t usedMemory() const noexcept;

 private:
  //! Return the memory chunk
  MemoryChunk* getChunk(const std::size_t offset) noexcept;

  //! Initialize the arena
  void initialize() noexcept;


  std::aligned_storage_t<kArenaSize, MemoryChunk::headerAlignment()> data_;
  std::size_t used_memory_ = 0;
  uint32 num_of_chunk_ = 0;
  uint32 padding_ = 0;
};

/*!
  \details
  Dynamic memory arena
  */
template <std::size_t kArenaSize>
class MemoryArena<MemoryArenaType::kDynamic, kArenaSize>
{
 public:
  //! Initialize the arena
  MemoryArena() noexcept;

  //! Move arena data
  MemoryArena(MemoryArena&& other) noexcept;

  //! Deallocate the allocated arena memory
  ~MemoryArena() noexcept;


  //! Allocate memory
  MemoryChunk* allocate(const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Return the allocated space
  std::size_t capacity() const noexcept;

  //! Return the first memory chunk
  MemoryChunk* getFirstChunk() noexcept;

  //! Return the first memory chunk
  const MemoryChunk* getFirstChunk() const noexcept;

  //! Reset the used memory
  void reset() noexcept;

  //! Return the used memory
  std::size_t usedMemory() const noexcept;

 private:
  struct Memory
  {
    uint8* data_;
    std::size_t used_memory_ = 0;
  };


  //! Allocate memory
  static void* allocateMemory(const std::size_t size) noexcept;

  //! Expand this memory arena
  bool expandArena() noexcept;

  //! Return the memory chunk
  MemoryChunk* getChunk(const std::size_t memory_num,
                        const std::size_t offset) noexcept;

  //! Return the size of specified memory
  static std::size_t getMemoryCapacity(const std::size_t memory_num) noexcept;

  //! Initialize the arena
  void initialize() noexcept;


  std::vector<Memory> arena_;
  uint32 num_of_chunk_ = 0;
  uint32 padding_ = 0;
};

// Type alias
template <std::size_t kArenaSize>
using StaticMemoryArena = MemoryArena<MemoryArenaType::kStatic, kArenaSize>;
template <std::size_t kArenaSize>
using DynamicMemoryArena = MemoryArena<MemoryArenaType::kDynamic, kArenaSize>;

} // namespace zisc

#include "memory_arena-inl.hpp"

#endif // ZISC_MEMORY_ARENA_HPP
