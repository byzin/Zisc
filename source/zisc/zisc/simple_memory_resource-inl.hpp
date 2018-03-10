/*!
  \file simple_memory_resource-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP
#define ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP

#include "simple_memory_resource.hpp"
// Standard C++ library
#include <cstddef>
#include <cstdlib>
#include <experimental/memory_resource>
// Zisc
#include "error.hpp"
#include "utility.hpp"

namespace zisc {

/*!
  */
inline
void* SimpleMemoryResource::allocateMemory(const std::size_t size,
                                           const std::size_t alignment) noexcept
{
  //! \todo Solve code branch
#if defined(Z_GCC) || defined(Z_APPLE_CLANG)
  static_cast<void>(alignment);
  auto data = std::malloc(size);
#else
  auto data = std::aligned_alloc(alignment, size);
#endif
  ZISC_ASSERT(isAligned(data, alignment), "The data isn't aligned.");
  return data;
}

/*!
  */
inline
void SimpleMemoryResource::deallocateMemory(void* data) noexcept
{
  std::free(data);
}

/*!
  */
inline
void* SimpleMemoryResource::do_allocate(std::size_t size, std::size_t alignment)
{
  return allocateMemory(size, alignment);
}

/*!
  */
inline
void SimpleMemoryResource::do_deallocate(void* data, std::size_t, std::size_t)
{
  return deallocateMemory(data);
}

/*!
  */
inline
bool SimpleMemoryResource::do_is_equal(const MemoryResource& other) const noexcept
{
  const bool result = dynamic_cast<const SimpleMemoryResource*>(&other) != nullptr;
  return result;
}

/*!
  \details
  `alignment` should be power of 2 bytes
  */
inline
bool SimpleMemoryResource::isAligned(const void* data,
                                     const std::size_t alignment) noexcept
{
  const std::ptrdiff_t address = treatAs<std::ptrdiff_t>(data);
  const bool result = (address & (alignment - 1)) == 0;
  return result;
}

/*!
  */
inline
SimpleMemoryResource* SimpleMemoryResource::sharedResource() noexcept
{
  static SimpleMemoryResource shared_resource;
  return &shared_resource;
}

} // namespace zisc

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP
