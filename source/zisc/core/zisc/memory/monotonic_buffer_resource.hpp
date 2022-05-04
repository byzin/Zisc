/*!
  \file monotonic_buffer_resource.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MONOTONIC_BUFFER_RESOURCE_HPP
#define ZISC_MONOTONIC_BUFFER_RESOURCE_HPP

// Standard C++ library
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <memory>
#include <type_traits>
// Zisc
#include "memory.hpp"
#include "std_memory_resource.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam kSize No description.
  \tparam kAlignment No description.
  */
template <std::size_t kSize, std::size_t kAlignment>
class MonotonicBufferResource :
    public pmr::memory_resource,
    private NonCopyable<MonotonicBufferResource<kSize, kAlignment>>
{
 public:
  // Exception
  using BadAlloc = Memory::BadAlloc;


  //! Create a new resource
  MonotonicBufferResource() noexcept;

  //! Destroy the resource
  ~MonotonicBufferResource() noexcept override = default;


  //! Return the maximum possible available alignment
  static constexpr std::size_t alignment() noexcept;

  //! Allocate memory
  void* allocateMemory(const std::size_t size,
                       const std::size_t alignment);

  //! Return the maximum possible available size
  static constexpr std::size_t capacity() noexcept;

  //! Deallocate memory
  void deallocateMemory(void* data,
                        const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Check if the underlying storage is occupied
  bool isOccupied() const noexcept;

  //! Release all allocated memory
  void release() noexcept;

  //! Return the used size for allocation
  std::size_t size() const noexcept;

 private:
  //! Return the pointer to the underlying storage
  std::byte* storage() noexcept;

  //! Return the pointer to the underlying storage
  const std::byte* storage() const noexcept;

  //! Allocate memory
  void* do_allocate(std::size_t size,
                    std::size_t alignment) override;

  //! Deallocate memory
  void do_deallocate(void* data,
                     std::size_t size,
                     std::size_t alignment) override;

  //! Compare for equality with another memory resource
  bool do_is_equal(const pmr::memory_resource& other) const noexcept override;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
  static_assert(std::alignment_of_v<std::atomic_size_t> <= kCacheLineSize);
  static constexpr std::size_t kAlignmentMax = (std::max)(kAlignment, kCacheLineSize);
  static constexpr std::size_t kBlockSize = kAlignmentMax;
  static constexpr std::size_t kNumOfBlocks = (kSize + (kBlockSize - 1)) / kBlockSize;
  using StorageT = std::aligned_storage_t<kNumOfBlocks * kBlockSize, kAlignmentMax>;


  StorageT storage_;
  std::atomic_size_t use_count_;
  std::atomic_size_t size_;
  [[maybe_unused]] Padding<kAlignmentMax - 2 * sizeof(std::atomic_size_t)> pad_;
};

} /* namespace zisc */

#include "monotonic_buffer_resource-inl.hpp"

#endif /* ZISC_MONOTONIC_BUFFER_RESOURCE_HPP */
