/*!
  \file alloc_free_resource.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALLOC_FREE_RESOURCE_HPP
#define ZISC_ALLOC_FREE_RESOURCE_HPP

// Standard C++ library
#include <cstddef>
#include <memory_resource>
#include <tuple>
// Zisc
#include "memory.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief a saimple memory allocator that uses 'malloc' and 'free'

  No detailed description.
  */
class AllocFreeResource : public std::pmr::memory_resource,
                          private NonCopyable<AllocFreeResource>
{
 public:
  // Exception
  using BadAllocT = Memory::BadAllocation;

  /*!
    \brief No brief description

    No detailed description.
    */
  struct Header
  {
    std::byte* pointer_ = nullptr; //!< The header pointer of the allocated memory
    std::size_t size_ = 0; //!< The size of the allocated memory
    std::size_t alignment_ = 1; //!< The alignment of the allocated memory
    [[maybe_unused]] Padding<sizeof(std::size_t)> pad_{};
  };


  // Create a memory resource
  AllocFreeResource() noexcept = default;

  //! Move a data
  AllocFreeResource(AllocFreeResource&& other) noexcept;

  //! Destroy the resource
  ~AllocFreeResource() noexcept override = default;


  //! Move a data
  auto operator=(AllocFreeResource&& other) noexcept -> AllocFreeResource&;


  //! Allocate memory
  [[nodiscard]]
  auto allocateMemory(const std::size_t size,
                      const std::size_t alignment) -> void*;

  //! Deallocate memory
  void deallocateMemory(void* data,
                        const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Return the header info of the memory allocation
  static auto getHeader(const void* data) noexcept -> const Header*;

  //! Return the memory usage
  [[nodiscard]]
  auto memoryUsage() const noexcept -> const Memory::Usage&;

  //! Return the total memory usage
  [[nodiscard]]
  auto totalMemoryUsage() const noexcept -> std::size_t;

  //! Return the peak memory usage
  [[nodiscard]]
  auto peakMemoryUsage() const noexcept -> std::size_t;

 private:
  //! Calculate required alignment for allocation
  static auto calcAllocAlignment(const std::size_t alignment) noexcept -> std::size_t;

  //! Calculate required size for allocation
  static auto calcAllocSize(const std::size_t size,
                            const std::size_t alignment) noexcept
      -> std::tuple<std::size_t, std::size_t>;

  //! Allocate memory
  [[nodiscard]]
  auto do_allocate(std::size_t size,
                   std::size_t alignment) -> void* override;

  //! Deallocate memory
  void do_deallocate(void* data,
                     std::size_t size,
                     std::size_t alignment) override;

  //! Compare for equality with another memory resource
  [[nodiscard]]
  auto do_is_equal(const std::pmr::memory_resource& other) const noexcept -> bool override;

  //! Return the header info of the memory allocation
  static auto getHeaderInner(void* data) noexcept -> Header*;


  Memory::Usage memory_usage_;
};

} // namespace zisc

#include "alloc_free_resource-inl.hpp"

#endif // ZISC_ALLOC_FREE_RESOURCE_HPP
