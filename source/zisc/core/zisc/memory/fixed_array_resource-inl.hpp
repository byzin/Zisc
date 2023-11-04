/*!
  \file fixed_array_resource-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FIXED_ARRAY_RESOURCE_INL_HPP
#define ZISC_FIXED_ARRAY_RESOURCE_INL_HPP

#include "fixed_array_resource.hpp"
// Standard C++ library
#include <atomic>
#include <bit>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "std_memory_resource.hpp"
#include "zisc/algorithm.hpp"
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/concurrency/bitset.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <typename Type> inline
FixedArrayResource<Type>::FixedArrayResource(pmr::memory_resource* mem_resource) noexcept :
    count_{0},
    storage_list_{typename decltype(storage_list_)::allocator_type{mem_resource}},
    used_list_{mem_resource}
{
  static_assert(alignof(decltype(count_)) % alignof(decltype(storage_list_)) == 0);
  static_assert(alignof(decltype(storage_list_)) == alignof(decltype(used_list_)));
  static_assert(sizeof(count_) + sizeof(pad1_) == kCacheLineSize);
  static_assert(sizeof(storage_list_) + sizeof(used_list_) + sizeof(pad2_) == kCacheLineSize);
  try {
    setCountMax(1);
  }
  catch ([[maybe_unused]] const std::exception& error) {
    ZISC_ASSERT(false, "FixedArrayResource initialization failed.");
  }
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type> inline
FixedArrayResource<Type>::FixedArrayResource(FixedArrayResource&& other) noexcept :
    count_{other.count()},
    storage_list_{std::move(other.storage_list_)},
    used_list_{std::move(other.used_list_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type> inline
auto FixedArrayResource<Type>::operator=(FixedArrayResource&& other) noexcept
    -> FixedArrayResource&
{
  count_.store(other.count(), std::memory_order::release);
  storage_list_ = std::move(other.storage_list_);
  used_list_ = std::move(other.used_list_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr auto FixedArrayResource<Type>::alignmentMax() noexcept -> std::size_t
{
  static_assert(alignof(StorageT) == alignof(Type));
  const std::size_t s = alignof(StorageT);
  return s;
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  \exception BadAllocT No description.
  */
template <typename Type> inline
auto FixedArrayResource<Type>::allocateMemory(const std::size_t size,
                                              const std::size_t alignment) -> void*
{
  // Check if the resource can proceed the request
  if ((sizeMax() < size) && (alignmentMax() < alignment)) {
    const char* message = (sizeMax() < size)
        ? "The required size exceeds the maximum available size in the resource."
        : "The required alignment exceeds the maximum available alignment";
    throw BadAllocT{size, alignment, message};
  }

  // Issue an index hint for finding a free storage
  std::size_t index = count_.fetch_add(1, std::memory_order::acq_rel);
  if (countMax() <= index) {
    count_.fetch_sub(1, std::memory_order::acq_rel);
    const char* message = "The number of allocation count exceeded the limit.";
    throw BadAllocT{size, alignment, message};
  }
  index = permuteIndex(index);

  // Get a ownership of a free storage
  index = findAndGetOwnership(index);
  if (index == invalidIndex()) {
    const char* message = "Fatal error. Appropriate storage not found.";
    throw BadAllocT{size, alignment, message};
  }

  return std::addressof(storage_list_[index]);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto FixedArrayResource<Type>::count() const noexcept -> std::size_t
{
  const std::size_t c = count_.load(std::memory_order::acquire);
  return c;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto FixedArrayResource<Type>::countMax() const noexcept -> std::size_t
{
  const std::size_t c = storage_list_.size();
  return c;
}

/*!
  \details No detailed description
  */
template <typename Type> inline
void FixedArrayResource<Type>::clear() noexcept
{
  count_.store(0, std::memory_order::release);
  used_list_.reset();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto FixedArrayResource<Type>::data() noexcept -> Pointer
{
  auto* p = reinterp<Pointer>(storage_list_.data());
  return p;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto FixedArrayResource<Type>::data() const noexcept -> ConstPointer
{
  const auto* p = reinterp<ConstPointer>(storage_list_.data());
  return p;
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  \param [in] size No description.
  \param [in] alignment No description.
  */
template <typename Type> inline
void FixedArrayResource<Type>::deallocateMemory(void* data,
                                                [[maybe_unused]] const std::size_t size,
                                                [[maybe_unused]] const std::size_t alignment) noexcept
{
  auto* d = static_cast<StorageT*>(data);
  const auto index = cast<std::size_t>(std::distance(storage_list_.data(), d));
  [[maybe_unused]] constexpr std::size_t begin = 0;
  ZISC_ASSERT(isInBounds(index, begin, countMax()), "The data is unmanaged data.");
  [[maybe_unused]] const bool had_ownership = used_list_.testAndSet(index, false);
  ZISC_ASSERT(had_ownership, "The ownership of the data was broken.");
  count_.fetch_sub(1, std::memory_order::acq_rel);
}

/*!
  \details No detailed description

  \param [in] c No description.
  */
template <typename Type> inline
void FixedArrayResource<Type>::setCountMax(const std::size_t c)
{
  storage_list_.resize(c);
  used_list_.setSize(c);
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr auto FixedArrayResource<Type>::sizeMax() noexcept -> std::size_t
{
  static_assert(sizeof(StorageT) == sizeof(Type));
  const std::size_t s = sizeof(StorageT);
  return s;
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
template <typename Type> inline
auto FixedArrayResource<Type>::do_allocate(std::size_t size,
                                           std::size_t alignment) -> void*
{
  return allocateMemory(size, alignment);
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
template <typename Type> inline
void FixedArrayResource<Type>::do_deallocate(void* data,
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
template <typename Type> inline
auto FixedArrayResource<Type>::do_is_equal(const pmr::memory_resource& other) const noexcept -> bool
{
  const bool result = this == std::addressof(other);
  return result;
}

/*!
  \details No detailed description

  \param [in] start No description.
  \return No description
  */
template <typename Type> inline
auto FixedArrayResource<Type>::findAndGetOwnership(const std::size_t start) noexcept -> std::size_t
{
  using BitT = Bitset::BitT;
  constexpr std::size_t block_bits = Bitset::blockBitSize();

  std::size_t index = start - (start % block_bits);
  BitT mask = cast<BitT>(1 << (start % block_bits)) - 1;

  const std::size_t n = countMax();
  const std::size_t n_blocks = (n + (block_bits - 1)) / block_bits;
  const std::size_t end = 2 * n_blocks;
  for (std::size_t block_count = 0; block_count < end;) {
    ZISC_ASSERT((index % block_bits) == 0, "The index isn't block aligned: ", index);
    // Find a candidate position
    const BitT block = used_list_.getBlockBits(index) | mask;
    const std::size_t n_used = std::countr_one(block);
    const std::size_t candidate = index + n_used;
    if ((n_used != block_bits) && (candidate < n)) {
      // Try to get the ownership of the candidate
      const bool has_ownership = !used_list_.testAndSet(candidate, true);
      if (has_ownership) {
        index = candidate;
        block_count = end;
      }
    }
    else {
      ++block_count;
      // Set next position
      index = (block_count == end) ? invalidIndex() :
              (n <= candidate)     ? 0
                                   : index + block_bits;
      mask = 0;
    }
  }
  return index;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr auto FixedArrayResource<Type>::invalidIndex() noexcept -> std::size_t
{
  const std::size_t invalid = (std::numeric_limits<std::size_t>::max)();
  return invalid;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename Type> inline
auto FixedArrayResource<Type>::permuteIndex(const std::size_t index) const noexcept -> std::size_t
{
  constexpr std::size_t shift = std::bit_width(kCacheLineSize * 8);
  const std::size_t n = countMax();
  const std::size_t bits = std::bit_width(n);

  std::size_t new_index = index;
  if (shift < bits) {
    const std::size_t upper = index << shift;
    const std::size_t lower = index >> (bits - shift);
    const std::size_t mask = cast<std::size_t>(1 << bits) - 1;
    const std::size_t i = (upper | lower) & mask;
    new_index = (i < n) ? i : index;
  }
  return new_index;
}

} /* namespace zisc */

#endif /* ZISC_FIXED_ARRAY_RESOURCE_INL_HPP */
