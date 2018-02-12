/*!
  \file memory_pool.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_POOL_HPP
#define ZISC_MEMORY_POOL_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
#include <vector>
// Zisc
#include "memory_chunk.hpp"
#include "non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
class MemoryPool : public NonCopyable<MemoryPool>
{
 public:
  //! Create a memory pool
  MemoryPool(const std::size_t memory_size) noexcept;

  //! Move a memory pool
  MemoryPool(MemoryPool&& other) noexcept;


  //! Move a memory pool
  MemoryPool& operator=(MemoryPool&& other) noexcept;


  //! Allocate the memory space for Type instances
  template <typename Type>
  MemoryChunk* allocate(const uint n = 1) noexcept;

  //! Return the data pointer of the memory pool
  uint8* data() noexcept;

  //! Return the data pointer of the memory pool
  const uint8* data() const noexcept;

  //! Return the memory chunk by the index
  MemoryChunk* getChunk(const uint index) noexcept;

  //! Return the memory chunk by the index
  const MemoryChunk* getChunk(const uint index) const noexcept;

  //! Return the memory chunk by the data pointer
  MemoryChunk* getChunk(void* ptr) noexcept;

  //! Return the memory chunk by the data pointer 
  const MemoryChunk* getChunk(const void* ptr) const noexcept;

  //! Check if the memory pool contains the data
  bool hasContained(const void* ptr) const noexcept;

  //! Return the num of memory chunks
  uint numOfChunks() const noexcept;

  //! Reset the memory pool state
  void reset() noexcept;

  //! Reset and resize the memory pool.
  std::size_t setSize(const std::size_t memory_size) noexcept;

  //! Return the size of the memory pool in bytes
  std::size_t size() const noexcept;

  //! Return the used memory size in bytes
  std::size_t usedMemory() const noexcept;

 private:
  //! Return the chunk position of the memory pool
  std::size_t getChunkPosition(const uint index) const noexcept;

  //! Return the chunk position of the memory pool
  std::size_t getChunkPosition(const void* ptr) const noexcept;

  //! Initialize
  void initialize(const std::size_t memory_size) noexcept;


  using AlignedBlock = std::aligned_storage_t<MemoryChunk::headerAlignment(),
                                              MemoryChunk::headerAlignment()>;


  std::vector<AlignedBlock> memory_pool_;
  std::size_t used_bytes_;
  uint32 num_of_chunks_;
  uint32 padding_;
};

} // namespace zisc

#include "memory_pool-inl.hpp"

#endif // ZISC_MEMORY_POOL_HPP
