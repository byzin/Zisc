/*!
  \file simple_memory_resource.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SIMPLE_MEMORY_RESOURCE_HPP
#define ZISC_SIMPLE_MEMORY_RESOURCE_HPP

// Standard C++ library
#include <cstddef>
#include <mutex>
// Zisc
#include "std_memory_resource.hpp"

namespace zisc {

/*!
  \brief a saimple memory allocator that uses 'malloc' and 'free'
  */
class SimpleMemoryResource : public std::pmr::memory_resource
{
 public:
  //! Allocate memory
  static void* allocateMemory(const std::size_t size,
                              const std::size_t alignment) noexcept;

  //! For api compatibility with MemoryManager
  std::size_t capacity() const noexcept;

  //! Deallocate memory
  static void deallocateMemory(void* data) noexcept;

  //! Allocate memory
  void* do_allocate(std::size_t size, std::size_t alignment) override;

  //! Deallocate memory
  void do_deallocate(void* data, std::size_t size, std::size_t alignment) override;

  //! Compare for equality with another memory resource
  bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

  //! Check if an address of 'data' is aligned at 'alignment' bytes
  static bool isAligned(const void* data, const std::size_t alignment) noexcept;

  //! For api compatibility with MemoryManager
  void reset() noexcept;

  //! For api compatibility with MemoryManager
  void setMutex(std::mutex* mutex) noexcept;

  //! Return the shared memory resource
  static SimpleMemoryResource* sharedResource() noexcept;

  //! For api compatibility with MemoryManager
  std::size_t usedMemory() const noexcept;
};

} // namespace zisc

#include "simple_memory_resource-inl.hpp"

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_HPP
