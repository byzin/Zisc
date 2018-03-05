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
#include <memory_resource>
#include <type_traits>
// Zisc
#include "memory_chunk.hpp"
#include "memory_pool_iterator.hpp"
#include "non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief The memory type of a pool
  */
enum class MemoryPoolType
{
  kStatic,
  kDynamic
};

//! The body of a memory pool
template <MemoryPoolType, std::size_t> class MemoryArena;

/*!
  */
template <typename PoolType, std::size_t kArenaSize>
class MemoryPool : public std::pmr::memory_resource,
                   public NonCopyable<MemoryPool<PoolType, kArenaSize>>
{
 public:
  using iterator = MemoryPoolIterator<MemoryChunk>;
  using const_iterator = MemoryPoolIterator<const MemoryChunk>;


  //! Create a memory pool
  MemoryPool() noexcept;

  //! Move a memory pool
  MemoryPool(MemoryPool&& other) noexcept;


  //! Move a memory pool
  MemoryPool& operator=(MemoryPool&& other) noexcept;


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

  //! Allocate storage
  void* do_allocate(std::size_t bytes, std::size_t alignment) override;

  //! Deallocate the storage
  void do_deallocate(std::size_t bytes, std::size_t alignment) override;

  //! Compare for equality
  bool do_is_equal(const std::pmr::memory_resource& other) const override;

 private:
  static_assert(0 < kArenaSize, "The arena size isn't positive.");
  static constexpr std::size_t kSize = MemoryChunk::headerSize() *
      (kArenaSize / MemoryChunk::headerSize() + 
       (0 < kArenaSize % MemoryChunk::headerSize()) ? 1 : 0);
  static_assert(kSize % MemoryChunk::headerAlignment() == 0,
                "The size isn't multiple of the chunk alignment");
  static_assert(kSize % MemoryChunk::headerSize() == 0,
                "The size isn't multiple of the chunk size");


  MemoryArena<PoolType, kSize> arena_;
};

} // namespace zisc

#include "memory_pool-inl.hpp"

#endif // ZISC_MEMORY_POOL_HPP
