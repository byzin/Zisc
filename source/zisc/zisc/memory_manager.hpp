/*!
  \file memory_manager.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_MANAGER_HPP
#define ZISC_MEMORY_MANAGER_HPP

// Standard C++ library
#include <cstddef>
#include <mutex>
#include <type_traits>
// Zisc
#include "memory_arena.hpp"
#include "memory_chunk.hpp"
#include "memory_manager_iterator.hpp"
#include "memory_resource.hpp"
#include "non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize>
class MemoryManager : public pmr::memory_resource,
                      private NonCopyable<MemoryManager<kArenaType, kArenaSize>>
{
 public:
  using iterator = MemoryManagerIterator<MemoryChunk>;
  using const_iterator = MemoryManagerIterator<const MemoryChunk>;


  //! Create a memory pool
  MemoryManager() noexcept;

  //! Move a memory pool
  MemoryManager(MemoryManager&& other) noexcept;


  //! Move a memory pool
  MemoryManager& operator=(MemoryManager&& other) noexcept;


  //! Return an iterator to the first chunk of the pool
  iterator begin() noexcept;

  //! Return an iterator to the first chunk of the pool
  const_iterator begin() const noexcept;

  //! Return an iterator to the first chunk of the pool
  const_iterator cbegin() const noexcept;

  //! Return an iterator to the element following the last chunk of the pool
  iterator end() noexcept;

  //! Return an iterator to the element following the last chunk of the pool
  const_iterator end() const noexcept;

  //! Return an iterator to the element following the last chunk of the pool
  const_iterator cend() const noexcept;

  //! Return the allocated memory space
  std::size_t capacity() const noexcept;

  //! Allocate storage
  void* do_allocate(std::size_t size, std::size_t alignment) override;

  //! Deallocate the storage
  void do_deallocate(void* data, std::size_t size, std::size_t alignment) override;

  //! Compare for equality
  bool do_is_equal(const pmr::memory_resource& other) const noexcept override;

  //! Reset the used memory
  void reset() noexcept;

  //! Set a mutex for synchronized memory allocation
  void setMutex(std::mutex* mutex) noexcept;

  //! Return the used memory
  std::size_t usedMemory() const noexcept;

 private:
  static_assert(0 < kArenaSize, "The arena size isn't positive.");
  static constexpr std::size_t kSize = MemoryChunk::headerSize() *
      (kArenaSize / MemoryChunk::headerSize() + 
       ((kArenaSize % MemoryChunk::headerSize() != 0) ? 1 : 0));
  static_assert(kSize % MemoryChunk::headerAlignment() == 0,
                "The size isn't multiple of the chunk alignment");
  static_assert(kSize % MemoryChunk::headerSize() == 0,
                "The size isn't multiple of the chunk size");


  MemoryArena<kArenaType, kSize> arena_;
  std::mutex* mutex_ = nullptr;
};

// Type alias
template <std::size_t kArenaSize>
using StaticMemoryManager = MemoryManager<MemoryArenaType::kStatic, kArenaSize>;
template <std::size_t kArenaSize>
using DynamicMemoryManager = MemoryManager<MemoryArenaType::kDynamic, kArenaSize>;

} // namespace zisc

#include "memory_manager-inl.hpp"

#endif // ZISC_MEMORY_MANAGER_HPP
