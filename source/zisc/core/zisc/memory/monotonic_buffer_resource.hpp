/*!
  \file monotonic_buffer_resource.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MONOTONIC_BUFFER_RESOURCE_HPP
#define ZISC_MONOTONIC_BUFFER_RESOURCE_HPP

// Standard C++ library
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <memory_resource>
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
    public std::pmr::memory_resource,
    private NonCopyable<MonotonicBufferResource<kSize, kAlignment>>
{
 public:
  // Exception
  using BadAllocT = Memory::BadAllocation;


  //! Create a new resource
  explicit MonotonicBufferResource(std::pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  MonotonicBufferResource(MonotonicBufferResource&& other) noexcept;

  //! Destroy the resource
  ~MonotonicBufferResource() noexcept override = default;


  //! Move a data
  auto operator=(MonotonicBufferResource&& other) noexcept -> MonotonicBufferResource&;


  //! Return the maximum possible available alignment
  static constexpr auto alignment() noexcept -> std::size_t;

  //! Allocate memory
  [[nodiscard]]
  auto allocateMemory(const std::size_t size,
                      const std::size_t alignment) -> void*;

  //! Return the maximum possible available size
  static constexpr auto capacity() noexcept -> std::size_t;

  //! Deallocate memory
  void deallocateMemory(void* data,
                        const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Check if the underlying storage is occupied
  auto isOccupied() const noexcept -> bool;

  //! Release all allocated memory
  void release() noexcept;

  //! Return the used size for allocation
  auto size() const noexcept -> std::size_t;

 private:
  //! Allocate memory
  [[nodiscard]]
  auto do_allocate(std::size_t size,
                   std::size_t alignment) -> void* override;

  //! Deallocate memory
  void do_deallocate(void* data,
                     std::size_t size,
                     std::size_t alignment) override;

  //! Compare for equality with another memory resource
  auto do_is_equal(const std::pmr::memory_resource& other) const noexcept -> bool override;

  //! Initialize the storage
  void initialize(std::pmr::memory_resource* mem_resource) noexcept;

  //! Return the pointer to the underlying storage
  auto storage() noexcept -> std::byte*;

  //! Return the pointer to the underlying storage
  auto storage() const noexcept -> const std::byte*;


  static constexpr std::size_t kBlockSize = kAlignment;
  static constexpr std::size_t kNumOfBlocks = (kSize + (kBlockSize - 1)) / kBlockSize;
  using StorageT = std::aligned_storage_t<kNumOfBlocks * kBlockSize, kAlignment>;


  pmr::unique_ptr<StorageT> storage_;
  std::atomic_size_t use_count_ = 0;
  std::atomic_size_t size_ = 0;
};

} /* namespace zisc */

#include "monotonic_buffer_resource-inl.hpp"

#endif /* ZISC_MONOTONIC_BUFFER_RESOURCE_HPP */
