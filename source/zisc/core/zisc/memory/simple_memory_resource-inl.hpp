/*!
  \file simple_memory_resource-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
#include <utility>
// Zisc
#include "memory.hpp"
#include "std_memory_resource.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

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
  std::swap(memory_usage_, other.memory_usage_);
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
  std::swap(memory_usage_, other.memory_usage_);
  return *this;
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
  constexpr std::size_t halignment = std::alignment_of_v<Header>;
  const std::size_t alloc_alignment = (std::max)(alignment, halignment);
  const std::size_t dsize = ((size + alloc_alignment - 1) / alloc_alignment) * alloc_alignment;
  ZISC_ASSERT(dsize % alloc_alignment == 0,
              "The data size isn't an integral multiple of the alignment.");
  const std::size_t hsize = (std::max)(sizeof(Header), alloc_alignment);
  ZISC_ASSERT(hsize % alloc_alignment == 0,
              "The header size isn't an integral multiple of the alignment.");

  const std::size_t alloc_size = hsize + dsize;
  void* ptr = aligned_alloc(alloc_alignment, alloc_size);

  // Get the pointer to the data
  uint8b* data = zisc::cast<uint8b*>(ptr) + hsize;
  ZISC_ASSERT(Memory::isAligned(data, alignment), "The data isn't aligned.");
  // Initialize header
  {
    Header* header = getHeaderImpl<Header*>(data);
    header->pointer_ = ptr;
    header->size_ = alloc_size;
    header->alignment_ = alloc_alignment;
    memory_usage_.add(alloc_size);
  }
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
  Header* header = getHeaderImpl<Header*>(cast<uint8b*>(data));
  const std::size_t alloc_size = header->size_;
  free(header->pointer_);
  memory_usage_.release(alloc_size);
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
inline
auto SimpleMemoryResource::getHeader(const void* data) const noexcept -> const Header*
{
  const Header* header = getHeaderImpl<const Header*>(cast<const uint8b*>(data));
  return header;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const Memory::Usage& SimpleMemoryResource::memoryUsage() const noexcept
{
  return memory_usage_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t SimpleMemoryResource::totalMemoryUsage() const noexcept
{
  return memoryUsage().total();
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t SimpleMemoryResource::peakMemoryUsage() const noexcept
{
  return memoryUsage().peak();
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
template <Pointer HeaderPtr, Pointer Type> inline
HeaderPtr SimpleMemoryResource::getHeaderImpl(Type data) noexcept
{
  constexpr std::size_t size = sizeof(Header);
  constexpr bool is_power_of_2 = (size & (size - 1)) == 0;
  static_assert(is_power_of_2, "The size of Header isn't power of 2.");

  HeaderPtr header = reinterp<HeaderPtr>(data - sizeof(Header));
  constexpr std::size_t alignment = std::alignment_of_v<Header>;
  ZISC_ASSERT(Memory::isAligned(header, alignment), "The header isn't aligned.");
  return header;
}

} // namespace zisc

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP
