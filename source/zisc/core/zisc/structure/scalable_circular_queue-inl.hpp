/*!
  \file scalable_circular_queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SCALABLE_CIRCULAR_QUEUE_INL_HPP
#define ZISC_SCALABLE_CIRCULAR_QUEUE_INL_HPP

#include "scalable_circular_queue.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <bit>
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "container_overflow_error.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <std::movable T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(pmr::memory_resource* mem_resource) noexcept
    : ScalableCircularQueue(1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <std::movable T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(const size_type cap,
                                                pmr::memory_resource* mem_resource) noexcept
    : BaseQueueT(),
      free_elements_{mem_resource},
      allocated_elements_{mem_resource},
      elements_{typename decltype(elements_)::allocator_type{mem_resource}}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(ScalableCircularQueue&& other) noexcept
    : BaseQueueT(other),
      free_elements_{std::move(other.free_elements_)},
      allocated_elements_{std::move(other.allocated_elements_)},
      elements_{std::move(other.elements_)}
{
}

/*!
  \details No detailed description
  */
template <std::movable T> inline
ScalableCircularQueue<T>::~ScalableCircularQueue() noexcept
{
  clear();
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable T> inline
auto ScalableCircularQueue<T>::operator=(ScalableCircularQueue&& other) noexcept
    -> ScalableCircularQueue&
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
template <std::movable T> inline
auto ScalableCircularQueue<T>::capacity() const noexcept -> size_type
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
template <std::movable T> inline
constexpr auto ScalableCircularQueue<T>::capacityMax() noexcept -> size_type
{
  const size_type cap = (std::numeric_limits<size_type>::max)() >> 2;
  return cap;
}

/*!
  \details No detailed description
  */
template <std::movable T> inline
void ScalableCircularQueue<T>::clear() noexcept
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
  allocated_elements_.clear();
  free_elements_.full();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto ScalableCircularQueue<T>::data() const noexcept -> std::span<ConstT>
{
  std::span<ConstT> d{getStorage(0).memory(), elements_.size()};
  return d;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto ScalableCircularQueue<T>::dequeue() noexcept -> std::optional<ValueT> 
{
  std::optional<ValueT> result{};
  const size_type index = allocated_elements_.dequeue(false); // Get an entry point
  const bool is_success = index != RingBuffer::invalidIndex();
  if (is_success) {
    StorageRef storage = getStorage(index);
    result = std::move(storage.get());
    storage.destroy();
    free_elements_.enqueue(index, true);
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] args No description.
  \return No description
  \exception OverflowError No description.
  */
template <std::movable T>
template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...> inline
auto ScalableCircularQueue<T>::enqueue(Args&&... args) -> std::optional<size_type>
{
  const size_type index = free_elements_.dequeue(true); // Get an entry index

  // Check overflow
  using OverflowErr = typename BaseQueueT::OverflowError;
  if (index == RingBuffer::overflowIndex()) {
    const char* message = "Queue overflow happened.";
    throw OverflowErr{message, resource(), ValueT{std::forward<Args>(args)...}};
  }

  const bool is_success = index != RingBuffer::invalidIndex();
  if (is_success) {
    StorageRef storage = getStorage(index);
    storage.set(std::forward<Args>(args)...);
    allocated_elements_.enqueue(index, false);
  }
  return is_success ? std::make_optional(index) : std::optional<size_type>{};
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T> inline
auto ScalableCircularQueue<T>::get(const size_type index) noexcept -> Reference
{
  return *getStorage(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T> inline
auto ScalableCircularQueue<T>::get(const size_type index) const noexcept -> ConstReference
{
  return *getStorage(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
constexpr bool ScalableCircularQueue<T>::isBounded() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
constexpr bool ScalableCircularQueue<T>::isConcurrent() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
pmr::memory_resource* ScalableCircularQueue<T>::resource() const noexcept
{
  pmr::memory_resource* mem_resource = elements_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <std::movable T> inline
void ScalableCircularQueue<T>::setCapacity(size_type cap) noexcept
{
  constexpr size_type lowest_size = 1;
  cap = (std::max)(lowest_size, cap);

  const size_type cap_pow2 = std::bit_ceil(cap);
  constexpr size_type cap_max = capacityMax();
  if ((capacity() < cap_pow2) && (cap_pow2 <= cap_max)) {
    clear();
    elements_.resize(cap_pow2);
    allocated_elements_.setSize(cap_pow2 << 1);
    free_elements_.setSize(cap_pow2 << 1);
  }
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto ScalableCircularQueue<T>::size() const noexcept -> size_type
{
  const std::size_t s = allocated_elements_.distance();
  return s;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T> inline
auto ScalableCircularQueue<T>::getStorage(const size_type index) noexcept
    -> StorageRef
{
  return elements_[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T> inline
auto ScalableCircularQueue<T>::getStorage(const size_type index) const noexcept
    -> ConstStorageRef 
{
  return elements_[index];
}

} // namespace zisc

#endif // ZISC_SCALABLE_CIRCULAR_QUEUE_INL_HPP
