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
#include <memory>
#include <tuple>
// Zisc
#include "memory.hpp"
#include "std_memory_resource.hpp"
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief a saimple memory allocator that uses 'malloc' and 'free'

  No detailed description.
  */
class AllocFreeResource : public pmr::memory_resource,
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
    [[maybe_unused]] Padding<sizeof(std::size_t)> pad_;
  };


  // Create a memory resource
  AllocFreeResource() noexcept = default;

  //! Move a data
  AllocFreeResource(AllocFreeResource&& other) noexcept;

  //! Destroy the resource
  ~AllocFreeResource() noexcept override = default;


  //! Move a data
  AllocFreeResource& operator=(AllocFreeResource&& other) noexcept;


  //! Allocate memory
  void* allocateMemory(const std::size_t size,
                       const std::size_t alignment);

  //! Deallocate memory
  void deallocateMemory(void* data,
                        const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Return the header info of the memory allocation
  static const Header* getHeader(const void* data) noexcept;

  //! Return the memory usage
  const Memory::Usage& memoryUsage() const noexcept;

  //! Return the total memory usage
  std::size_t totalMemoryUsage() const noexcept;

  //! Return the peak memory usage
  std::size_t peakMemoryUsage() const noexcept;

 private:
  //! Calculate required alignment for allocation
  static std::size_t calcAllocAlignment(const std::size_t alignment) noexcept;

  //! Calculate required size for allocation
  static std::tuple<std::size_t, std::size_t> calcAllocSize(
      const std::size_t size,
      const std::size_t alignment) noexcept;

  //! Allocate memory
  void* do_allocate(std::size_t size,
                    std::size_t alignment) override;

  //! Deallocate memory
  void do_deallocate(void* data,
                     std::size_t size,
                     std::size_t alignment) override;

  //! Compare for equality with another memory resource
  bool do_is_equal(const pmr::memory_resource& other) const noexcept override;

  //! Return the header info of the memory allocation
  static Header* getHeaderInner(void* data) noexcept;


  Memory::Usage memory_usage_;
};

} // namespace zisc

#include "alloc_free_resource-inl.hpp"

#endif // ZISC_ALLOC_FREE_RESOURCE_HPP
