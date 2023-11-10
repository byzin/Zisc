/*!
  \file lock_free_queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_QUEUE_INL_HPP
#define ZISC_LOCK_FREE_QUEUE_INL_HPP

#include "lock_free_queue.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <bit>
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory_resource>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "container_overflow_error.hpp"
#include "ring_buffer.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <std::movable T, typename RingBufferClass> inline
LockFreeQueue<T, RingBufferClass>::LockFreeQueue(std::pmr::memory_resource* mem_resource) noexcept
    : LockFreeQueue(1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <std::movable T, typename RingBufferClass> inline
LockFreeQueue<T, RingBufferClass>::LockFreeQueue(const size_type cap,
                                                 std::pmr::memory_resource* mem_resource) noexcept
    : BaseQueueT(),
      free_elements_{mem_resource},
      allocated_elements_{mem_resource},
      elements_{typename decltype(elements_)::allocator_type{mem_resource}}
{
  constexpr std::size_t num_of_attempts = 4;
  for (std::size_t i = 0; i < num_of_attempts; ++i) {
    try {
      setCapacity(cap);
      break;
    }
    catch ([[maybe_unused]] const std::exception& error) {
      ZISC_ASSERT(false, "LockFreeQueue initialization failed.");
    }
  }
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable T, typename RingBufferClass> inline
LockFreeQueue<T, RingBufferClass>::LockFreeQueue(LockFreeQueue&& other) noexcept
    : BaseQueueT(std::move(other)),
      free_elements_{std::move(other.free_elements_)},
      allocated_elements_{std::move(other.allocated_elements_)},
      elements_{std::move(other.elements_)}
{
}

/*!
  \details No detailed description
  */
template <std::movable T, typename RingBufferClass> inline
LockFreeQueue<T, RingBufferClass>::~LockFreeQueue() noexcept
{
  clear();
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::operator=(LockFreeQueue&& other) noexcept
    -> LockFreeQueue&
{
  clear();
  BaseQueueT::operator=(std::move(other));
  free_elements_ = std::move(other.free_elements_);
  allocated_elements_ = std::move(other.allocated_elements_);
  elements_ = std::move(other.elements_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::capacity() const noexcept -> size_type
{
  const size_type cap = elements_.size();
  ZISC_ASSERT((cap == 0) || std::has_single_bit(cap),
              "The capacity isn't power of 2. capacity = ", cap);
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
constexpr auto LockFreeQueue<T, RingBufferClass>::capacityMax() noexcept -> size_type
{
  const size_type cap = BaseRingBufferT::capacityMax() >> 1;
  return cap;
}

/*!
  \details No detailed description
  */
template <std::movable T, typename RingBufferClass> inline
void LockFreeQueue<T, RingBufferClass>::clear() noexcept
{
  // Skip clear operation after moving data to other
  if (elements_.empty())
    return;

  // Destroy all pushed values
  //! \todo Optimize clear performance
  {
    const size_type s = size();
    for (size_type i = 0; i < s; ++i)
      [[maybe_unused]] const std::optional<ValueT> value = dequeue();
  }
  ZISC_ASSERT(BaseQueueT::isEmpty(), "The queue still have value. size=", size());
  allocatedElements().clear();
  freeElements().full();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::data() const noexcept -> std::span<ConstT>
{
  std::span<ConstT> d{getStorage(0).memory(), elements_.size()};
  return d;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::dequeue() noexcept -> std::optional<ValueT> 
{
  std::optional<ValueT> result{};
  const size_type index = allocatedElements().dequeue(false); // Get an entry point
  const bool is_success = index != RingBufferT::invalidIndex();
  if (is_success) {
    StorageRef storage = getStorage(index);
    result = std::move(storage.get());
    storage.destroy();
    [[maybe_unused]] const uint64b r = freeElements().enqueue(index, true);
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] args No description.
  \return No description
  \exception OverflowError No description.
  */
template <std::movable T, typename RingBufferClass>
template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...> inline
auto LockFreeQueue<T, RingBufferClass>::enqueue(Args&&... args) -> std::optional<size_type>
{
  const size_type index = freeElements().dequeue(true); // Get an entry index

  // Check overflow
  using OverflowErr = typename BaseQueueT::OverflowError;
  if (index == RingBufferT::overflowIndex()) {
    const char* message = "Queue overflow happened.";
    throw OverflowErr{message, resource(), ValueT{std::forward<Args>(args)...}};
  }

  const bool is_success = index != RingBufferT::invalidIndex();
  if (is_success) {
    StorageRef storage = getStorage(index);
    storage.set(std::forward<Args>(args)...);
    [[maybe_unused]] const uint64b r = allocatedElements().enqueue(index, false);
  }
  return is_success ? std::make_optional(index) : std::optional<size_type>{};
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::get(const size_type index) noexcept -> Reference
{
  return *getStorage(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::get(const size_type index) const noexcept -> ConstReference
{
  return *getStorage(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
constexpr auto LockFreeQueue<T, RingBufferClass>::isBounded() noexcept -> bool
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
constexpr auto LockFreeQueue<T, RingBufferClass>::isConcurrent() noexcept -> bool
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::resource() const noexcept -> std::pmr::memory_resource* 
{
  std::pmr::memory_resource* mem_resource = elements_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <std::movable T, typename RingBufferClass> inline
void LockFreeQueue<T, RingBufferClass>::setCapacity(size_type cap)
{
  constexpr size_type lowest_size = 1;
  cap = (std::max)(lowest_size, cap);

  const size_type cap_pow2 = std::bit_ceil(cap);
  constexpr size_type cap_max = capacityMax();
  if ((capacity() < cap_pow2) && (cap_pow2 <= cap_max)) {
    clear();
    elements_.resize(cap_pow2);
    constexpr std::size_t cap_offset = std::is_same_v<ScalableCircularRingBuffer, RingBufferClass> ? 1 : 0;
    allocated_elements_.setSize(cap_pow2 << cap_offset);
    free_elements_.setSize(cap_pow2 << cap_offset);
  }
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::size() const noexcept -> size_type
{
  const std::size_t s = allocated_elements_.distance();
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::allocatedElements() noexcept -> BaseRingBufferT&
{
  return allocated_elements_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::allocatedElements() const noexcept -> const BaseRingBufferT&
{
  return allocated_elements_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::freeElements() noexcept -> BaseRingBufferT&
{
  return free_elements_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::freeElements() const noexcept -> const BaseRingBufferT&
{
  return free_elements_;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::getStorage(const size_type index) noexcept
    -> StorageRef
{
  return elements_[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T, typename RingBufferClass> inline
auto LockFreeQueue<T, RingBufferClass>::getStorage(const size_type index) const noexcept
    -> ConstStorageRef 
{
  return elements_[index];
}

} // namespace zisc

#endif // ZISC_LOCK_FREE_QUEUE_INL_HPP
