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
  zisc::swap(memory_usage_, other.memory_usage_);
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
  zisc::swap(memory_usage_, other.memory_usage_);
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
  static_assert(sizeof(uint8b) == 1);
  constexpr std::size_t halignment = std::alignment_of_v<Header>;
  const std::size_t alloc_alignment = std::max(alignment, halignment);

  const std::size_t dsize = (size % alloc_alignment == 0)
      ? size
      : (size / alloc_alignment + 1) * alloc_alignment;
  ZISC_ASSERT(dsize % alloc_alignment == 0,
              "The data size isn't an integral multiple of the alignment.");
  const std::size_t hsize = std::max(sizeof(Header), alloc_alignment);
  ZISC_ASSERT(hsize % alloc_alignment == 0,
              "The header size isn't an integral multiple of the alignment.");

  const std::size_t alloc_size = hsize + dsize;
  void* ptr = alignedAlloc(alloc_size, alloc_alignment);

  // Get the pointer to the data
  uint8b* data = zisc::cast<uint8b*>(ptr) + hsize;
  ZISC_ASSERT(Memory::isAligned(data, alignment), "The data isn't aligned.");
  // Initialize header
  {
    Header* header = getHeader(data);
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
  static_assert(sizeof(uint8b) == 1);
  Header* header = getHeader(data);
  const std::size_t alloc_size = header->size_;
  alignedFree(header->pointer_);
  memory_usage_.release(alloc_size);
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
inline
auto SimpleMemoryResource::getHeader(void* data) noexcept
    -> Header*
{
  constexpr std::size_t size = sizeof(Header);
  constexpr bool is_power_of_2 = (size & (size - 1)) == 0;
  static_assert(is_power_of_2, "The size of Header isn't power of 2.");

  uint8b* p = zisc::cast<uint8b*>(data);
  Header* header = treatAs<Header*>(p - sizeof(Header));
  constexpr std::size_t alignment = std::alignment_of_v<Header>;
  ZISC_ASSERT(Memory::isAligned(header, alignment), "The header isn't aligned.");
  return header;
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
inline
auto SimpleMemoryResource::getHeader(const void* data) const noexcept
    -> const Header*
{
  constexpr std::size_t size = sizeof(Header);
  constexpr bool is_power_of_2 = (size & (size - 1)) == 0;
  static_assert(is_power_of_2, "The size of Header isn't power of 2.");

  const uint8b* p = zisc::cast<const uint8b*>(data);
  const Header* header = treatAs<const Header*>(p - sizeof(Header));
  constexpr std::size_t alignment = std::alignment_of_v<Header>;
  ZISC_ASSERT(Memory::isAligned(header, alignment), "The header isn't aligned.");
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

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
inline
void* SimpleMemoryResource::alignedAlloc(const std::size_t size,
                                         const std::size_t alignment) noexcept
{
  void* data =
#if defined(Z_WINDOWS)
      _aligned_malloc(size, alignment);
#elif defined(Z_MAC) && defined(Z_CLANG)
      aligned_alloc(alignment, size);
#else
      std::aligned_alloc(alignment, size);
#endif
  return data;
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  */
inline
void SimpleMemoryResource::alignedFree(void* data) noexcept
{
#if defined(Z_WINDOWS)
  _aligned_free(data);
#elif defined(Z_MAC) && defined(Z_CLANG)
  free(data);
#else
  std::free(data);
#endif
}

} // namespace zisc

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP
