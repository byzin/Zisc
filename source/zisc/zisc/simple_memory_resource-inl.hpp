/*!
  \file simple_memory_resource-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP
#define ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP

#include "simple_memory_resource.hpp"
// Standard C++ library
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <type_traits>
// Zisc
#include "atomic.hpp"
#include "error.hpp"
#include "memory.hpp"
#include "std_memory_resource.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
inline
SimpleMemoryResource::SimpleMemoryResource() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
SimpleMemoryResource::SimpleMemoryResource(SimpleMemoryResource&& other) noexcept
{
  zisc::swap(total_memory_usage_, other.total_memory_usage_);
  zisc::swap(peak_memory_usage_, other.peak_memory_usage_);
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
SimpleMemoryResource& SimpleMemoryResource::operator=(
    SimpleMemoryResource&& other) noexcept
{
  zisc::swap(total_memory_usage_, other.total_memory_usage_);
  zisc::swap(peak_memory_usage_, other.peak_memory_usage_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
inline
void* SimpleMemoryResource::do_allocate(std::size_t size, std::size_t alignment)
{
  void* data = allocateMemory(size, alignment);
  return data;
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  \param [in] size data No description.
  \param [in] alignment data No description.
  */
inline
void SimpleMemoryResource::do_deallocate(void* data,
                                         std::size_t size,
                                         std::size_t alignment)
{
  deallocateMemory(data, size, alignment);
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
bool SimpleMemoryResource::do_is_equal(const std::pmr::memory_resource& other)
    const noexcept
{
  const bool result = this == std::addressof(other);
  return result;
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
inline
void* SimpleMemoryResource::allocateMemory(const std::size_t size,
                                           const std::size_t alignment) noexcept
{
  static_assert(sizeof(uint8b) == 1);
  std::size_t alloc_size = sizeof(Header) + size;
  uint8b* p = zisc::cast<uint8b*>(std::malloc(alloc_size));
  // Initialize header
  {
    Header* header = treatAs<Header*>(p);
    header->size_ = alloc_size;
    Atomic::add(&total_memory_usage_, alloc_size);
    Atomic::max(&peak_memory_usage_, total_memory_usage_);
  }
  // Get the pointer to the data
  void* data = p + sizeof(Header);
  ZISC_ASSERT(Memory::isAligned(data, alignment), "The data isn't aligned.");
  return data;
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  */
inline
void SimpleMemoryResource::deallocateMemory(void* data,
                                            const std::size_t /* size */,
                                            const std::size_t /* alignment */) noexcept
{
  static_assert(sizeof(uint8b) == 1);
  uint8b* p = zisc::cast<uint8b*>(data);
  // Get the pointer to the header
  p = p - sizeof(Header);
  {
    Header* header = treatAs<Header*>(p);
    Atomic::sub(&total_memory_usage_, header->size_);
  }
  std::free(p);
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t SimpleMemoryResource::totalMemoryUsage() const noexcept
{
  return total_memory_usage_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t SimpleMemoryResource::peakMemoryUsage() const noexcept
{
  return peak_memory_usage_;
}

} // namespace zisc

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP
