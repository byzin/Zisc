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
#include <memory>
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
class SimpleMemoryResource : public pmr::memory_resource,
                             private NonCopyable<SimpleMemoryResource>
{
 public:
  /*!
    \brief No brief description

    No detailed description.
    */
  struct Header
  {
    void* pointer_ = nullptr; //!< The header pointer of the allocated memory
    std::size_t size_ = 0; //!< The size of the allocated memory
    std::size_t alignment_; //!< The alignment of the allocated memory
    std::size_t padding_ = 0;
  };


  // Create a memory resource
  SimpleMemoryResource() noexcept;

  //! Move a data
  SimpleMemoryResource(SimpleMemoryResource&& other) noexcept;


  //! Move a data
  SimpleMemoryResource& operator=(SimpleMemoryResource&& other) noexcept;


  //! Allocate memory
  void* allocateMemory(const std::size_t size,
                       const std::size_t alignment) noexcept;

  //! Deallocate memory
  void deallocateMemory(void* data,
                        const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Return the header info of the memory allocation
  const Header* getHeader(const void* data) const noexcept;

  //! Return the memory usage
  const Memory::Usage& memoryUsage() const noexcept;

  //! Return the total memory usage
  std::size_t totalMemoryUsage() const noexcept;

  //! Return the peak memory usage
  std::size_t peakMemoryUsage() const noexcept;

 private:
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
  Header* getHeader(void* data) noexcept;

  //! Return the header info of the memory allocation
  template <Pointer HeaderPtr, Pointer Type>
  static HeaderPtr getHeaderImpl(Type data) noexcept;


  Memory::Usage memory_usage_;
};

} // namespace zisc

#include "simple_memory_resource-inl.hpp"

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_HPP
