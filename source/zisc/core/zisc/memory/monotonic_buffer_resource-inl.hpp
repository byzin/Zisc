/*!
  \file monotonic_buffer_resource-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MONOTONIC_BUFFER_RESOURCE_INL_HPP
#define ZISC_MONOTONIC_BUFFER_RESOURCE_INL_HPP

#include "monotonic_buffer_resource.hpp"
// Standard C++ library
#include <atomic>
#include <bit>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "memory.hpp"
#include "std_memory_resource.hpp"
#include "zisc/algorithm.hpp"
#include "zisc/bit.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
MonotonicBufferResource<kSize, kAlignment>::MonotonicBufferResource(pmr::memory_resource* mem_resource) noexcept
{
  initialize(mem_resource);
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <std::size_t kSize, std::size_t kAlignment> inline
MonotonicBufferResource<kSize, kAlignment>::MonotonicBufferResource(MonotonicBufferResource&& other) noexcept :
    pmr::memory_resource(std::move(other)),
    storage_{std::move(other.storage_)},
    use_count_{other.use_count_.load(std::memory_order::acquire)},
    size_{other.size_.load(std::memory_order::acquire)}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
constexpr std::size_t MonotonicBufferResource<kSize, kAlignment>::alignment() noexcept
{
  constexpr std::size_t a = alignof(StorageT);
  static_assert(std::has_single_bit(a), "The alignment isn't power of 2.");
  return a;
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <std::size_t kSize, std::size_t kAlignment> inline
auto MonotonicBufferResource<kSize, kAlignment>::operator=(MonotonicBufferResource&& other) noexcept -> MonotonicBufferResource&
{
  pmr::memory_resource::operator=(std::move(other));
  storage_ = std::move(other.storage_);
  use_count_ = other.use_count_.load(std::memory_order::acquire);
  size_ = other.size_.load(std::memory_order::acquire);
  return *this;
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  \exception BadAllocT No description.
  */
template <std::size_t kSize, std::size_t kAlignment> inline
void* MonotonicBufferResource<kSize, kAlignment>::allocateMemory(
    const std::size_t size,
    const std::size_t alignment)
{
  std::size_t usage = this->size();
  std::size_t next_usage = 0;
  std::size_t adjustment = 0;
  do {
    const std::size_t address = zisc::bit_cast<std::size_t>(storage() + usage);
    const std::size_t fraction = address & (alignment - 1);
    adjustment = (alignment - fraction) & (alignment - 1);
    const std::size_t alloc_size = adjustment + size;
    next_usage = usage + alloc_size;
    if (capacity() < next_usage) {
      const char* message = "Memory allocation failed.";
      throw BadAllocT{alloc_size, alignment, message};
    }
  } while (!size_.compare_exchange_strong(usage,
                                          next_usage,
                                          std::memory_order::acq_rel,
                                          std::memory_order::acquire));
  std::byte* data = storage() + (usage + adjustment);
  use_count_.fetch_add(1, std::memory_order::relaxed);
  return data;
}


/*!
  \details No detailed description

  \return No description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
constexpr std::size_t MonotonicBufferResource<kSize, kAlignment>::capacity() noexcept
{
  return sizeof(StorageT);
}

/*!
  \details No detailed description

  \param [in] data No description.
  \param [in] size No description.
  \param [in] alignment No description.
  */
template <std::size_t kSize, std::size_t kAlignment> inline
void MonotonicBufferResource<kSize, kAlignment>::deallocateMemory(
    void* data,
    [[maybe_unused]] const std::size_t size,
    [[maybe_unused]] const std::size_t alignment) noexcept
{
  [[maybe_unused]] const auto begin = zisc::bit_cast<std::size_t>(storage());
  [[maybe_unused]] const auto end = zisc::bit_cast<std::size_t>(storage() + capacity());
  [[maybe_unused]] const auto address = zisc::bit_cast<std::size_t>(data);
  ZISC_ASSERT(isInBounds(address, begin, end),
              "The data wasn't allocated by this allocator.");
  use_count_.fetch_sub(1, std::memory_order::relaxed);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
bool MonotonicBufferResource<kSize, kAlignment>::isOccupied() const noexcept
{
  const bool result = 0 < use_count_.load(std::memory_order::acquire);
  return result;
}

/*!
  \details No detailed description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
void MonotonicBufferResource<kSize, kAlignment>::release() noexcept
{
  use_count_.store(0, std::memory_order::release);
  size_.store(0, std::memory_order::release);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
std::size_t MonotonicBufferResource<kSize, kAlignment>::size() const noexcept
{
  const std::size_t s = size_.load(std::memory_order::acquire);
  return s;
}

/*!
  \details No detailed description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
void MonotonicBufferResource<kSize, kAlignment>::initialize(
    pmr::memory_resource* mem_resource) noexcept
{
  pmr::polymorphic_allocator<StorageT> alloc{mem_resource};
  storage_ = pmr::allocateUnique<StorageT>(alloc);
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  \exception BadAllocT No description.
  */
template <std::size_t kSize, std::size_t kAlignment> inline
void* MonotonicBufferResource<kSize, kAlignment>::do_allocate(
    std::size_t size,
    std::size_t alignment)
{
  return allocateMemory(size, alignment);
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  */
template <std::size_t kSize, std::size_t kAlignment> inline
void MonotonicBufferResource<kSize, kAlignment>::do_deallocate(
    void* data,
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
template <std::size_t kSize, std::size_t kAlignment> inline
bool MonotonicBufferResource<kSize, kAlignment>::do_is_equal(
    const pmr::memory_resource& other) const noexcept
{
  const auto* other_mem = dynamic_cast<const MonotonicBufferResource*>(&other);
  const bool result = (other_mem != nullptr) && (other_mem->storage() == storage());
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
std::byte* MonotonicBufferResource<kSize, kAlignment>::storage() noexcept
{
  static_assert(sizeof(std::byte) == 1, "The size of std::byte isn't 1.");
  return reinterp<std::byte*>(storage_.get());
}

/*!
  \details No detailed description

  \return No description
  */
template <std::size_t kSize, std::size_t kAlignment> inline
const std::byte* MonotonicBufferResource<kSize, kAlignment>::storage() const noexcept
{
  static_assert(sizeof(std::byte) == 1, "The size of std::byte isn't 1.");
  return reinterp<const std::byte*>(storage_.get());
}

} /* namespace zisc */

#endif /* ZISC_MONOTONIC_BUFFER_RESOURCE_INL_HPP */
