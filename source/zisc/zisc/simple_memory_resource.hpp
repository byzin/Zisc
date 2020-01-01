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
#include <cstddef>
// Zisc
#include "std_memory_resource.hpp"

namespace zisc {

/*!
  \brief a saimple memory allocator that uses 'malloc' and 'free'

  No detailed description.
  */
class SimpleMemoryResource : public std::pmr::memory_resource
{
 public:
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
  static void* allocateMemory(const std::size_t size,
                              const std::size_t alignment) noexcept;

  //! Deallocate memory
  static void deallocateMemory(void* data) noexcept;
};

} // namespace zisc

#include "simple_memory_resource-inl.hpp"

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_HPP
