/*!
  \file alloc_free_resource-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALLOC_FREE_RESOURCE_INL_HPP
#define ZISC_ALLOC_FREE_RESOURCE_INL_HPP

#include "alloc_free_resource.hpp"
// Standard C++ library
#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdlib>
#include <memory_resource>
#include <tuple>
#include <type_traits>
#include <utility>
// Zisc
#include "memory.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
AllocFreeResource::AllocFreeResource(AllocFreeResource&& other) noexcept :
    std::pmr::memory_resource(other),
    memory_usage_{std::move(other).memory_usage_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
auto AllocFreeResource::operator=(AllocFreeResource&& other) noexcept -> AllocFreeResource&
{
  std::pmr::memory_resource::operator=(other);
  memory_usage_ = std::move(other).memory_usage_;
  return *this;
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  \exception BadAllocT No description.
  */
inline
auto AllocFreeResource::allocateMemory(const std::size_t size,
                                       const std::size_t alignment) -> void*
{
  // Allocate memory
  const std::size_t alloc_alignment = calcAllocAlignment(alignment);
  const auto [alloc_size, header_size] = calcAllocSize(size, alloc_alignment);
  void* ptr = aligned_alloc(alloc_alignment, alloc_size);
  if (ptr == nullptr) [[unlikely]] {
    const char* message = "Memory allocation failed.";
    throw BadAllocT{alloc_size, alloc_alignment, message};
  }

  // Get the pointer to the data
  auto* p = reinterp<std::byte*>(ptr);
  std::byte* data = p + header_size;
  ZISC_ASSERT(Memory::isAligned(data, alignment), "The data isn't aligned.");
  // Initialize header
  {
    Header* header = getHeaderInner(data);
    header->pointer_ = p;
    header->size_ = alloc_size;
    header->alignment_ = alloc_alignment;
    memory_usage_.add(alloc_size);
  }
  return data;
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  \param [in] size No description.
  \param [in] alignment No description.
  */
inline
void AllocFreeResource::deallocateMemory(
    void* data,
    [[maybe_unused]] const std::size_t size,
    [[maybe_unused]] const std::size_t alignment) noexcept
{
  Header* header = getHeaderInner(reinterp<std::byte*>(data));
  const std::size_t alloc_size = header->size_;
  zisc::free(header->pointer_);
  memory_usage_.release(alloc_size);
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
inline
auto AllocFreeResource::getHeader(const void* data) noexcept -> const Header*
{
  constexpr std::size_t header_size = sizeof(Header);
  static_assert(std::has_single_bit(header_size), "The header size isn't power of 2.");

  const auto* data_ptr = reinterp<const std::byte*>(data);
  const auto* header = reinterp<const Header*>(data_ptr - header_size);
  [[maybe_unused]] constexpr std::size_t header_alignment = std::alignment_of_v<Header>;
  ZISC_ASSERT(Memory::isAligned(header, header_alignment), "The header isn't aligned.");
  return header;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto AllocFreeResource::memoryUsage() const noexcept -> const Memory::Usage&
{
  return memory_usage_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto AllocFreeResource::totalMemoryUsage() const noexcept -> std::size_t
{
  return memoryUsage().total();
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto AllocFreeResource::peakMemoryUsage() const noexcept -> std::size_t
{
  return memoryUsage().peak();
}

/*!
  \details No detailed description

  \param [in] alignment No description.
  \return No description
  */
inline
auto AllocFreeResource::calcAllocAlignment(const std::size_t alignment) noexcept -> std::size_t
{
  constexpr std::size_t min_alignment = (std::max)(std::alignment_of_v<Header>,
                                                   Memory::minAllocAlignment());
  const std::size_t alloc_alignment = (std::max)(alignment, min_alignment);
  return alloc_alignment;
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
inline
auto AllocFreeResource::calcAllocSize(const std::size_t size,
                                      const std::size_t alignment) noexcept
    -> std::tuple<std::size_t, std::size_t>
{
  const std::size_t data_block = (size + alignment - 1) / alignment;
  const std::size_t header_block = (sizeof(Header) + alignment - 1) / alignment;
  const std::size_t alloc_size = (header_block + data_block) * alignment;
  const std::size_t header_size = header_block * alignment;
  return std::make_tuple(alloc_size, header_size);
}

/*!
  \details No detailed description

  \param [in] data No description.
  \return No description
  */
inline
auto AllocFreeResource::getHeaderInner(void* data) noexcept -> Header*
{
  const Header* header = getHeader(data);
  return const_cast<Header*>(header);
}

} // namespace zisc

#endif // ZISC_ALLOC_FREE_RESOURCE_INL_HPP
