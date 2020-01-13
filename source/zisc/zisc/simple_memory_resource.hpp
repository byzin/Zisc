/*!
  \file simple_memory_resource.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SIMPLE_MEMORY_RESOURCE_HPP
#define ZISC_SIMPLE_MEMORY_RESOURCE_HPP

// Standard C++ library
#include <array>
#include <cstddef>
#include <type_traits>
// Zisc
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief a saimple memory allocator that uses 'malloc' and 'free'

  No detailed description.
  */
class SimpleMemoryResource : public std::pmr::memory_resource,
                             public NonCopyable<SimpleMemoryResource>
{
  static constexpr std::size_t kPadSize = std::alignment_of_v<std::max_align_t> -
                                          sizeof(std::size_t);

 public:
  /*!
    \brief No brief description

    No detailed description.
    */
  struct alignas(std::alignment_of_v<std::max_align_t>) Header
  {
    std::size_t size_;
    std::array<uint8b, kPadSize> padding_;
  };


  // Create a memory resource
  SimpleMemoryResource() noexcept;

  //! Move a data
  SimpleMemoryResource(SimpleMemoryResource&& other) noexcept;


  //! Move a data
  SimpleMemoryResource& operator=(SimpleMemoryResource&& other) noexcept;


  // STL functions
  //! Allocate memory
  void* do_allocate(std::size_t size,
                    std::size_t alignment) override;

  //! Deallocate memory
  void do_deallocate(void* data,
                     std::size_t size,
                     std::size_t alignment) override;

  //! Compare for equality with another memory resource
  bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;


  //! Allocate memory
  void* allocateMemory(const std::size_t size,
                       const std::size_t alignment) noexcept;

  //! Deallocate memory
  void deallocateMemory(void* data,
                        const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Return the total memory usage
  std::size_t totalMemoryUsage() const noexcept;

  //! Return the peak memory usage
  std::size_t peakMemoryUsage() const noexcept;

 private:
  std::size_t total_memory_usage_ = 0;
  std::size_t peak_memory_usage_ = 0;
};

} // namespace zisc

#include "simple_memory_resource-inl.hpp"

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_HPP
