/*!
  \file scalable_circular_queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SCALABLE_CIRCULAR_QUEUE_INL_HPP
#define ZISC_SCALABLE_CIRCULAR_QUEUE_INL_HPP

#include "scalable_circular_queue.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <limits>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "query_result.hpp"
#include "query_value.hpp"
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <Movable T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(pmr::memory_resource* mem_resource) noexcept
    : ScalableCircularQueue(1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <Movable T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(const size_type cap,
                                                pmr::memory_resource* mem_resource) noexcept
    : BaseQueueType(),
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
template <Movable T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(ScalableCircularQueue&& other) noexcept
    : BaseQueueType(other),
      free_elements_{std::move(other.free_elements_)},
      allocated_elements_{std::move(other.allocated_elements_)},
      elements_{std::move(other.elements_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <Movable T> inline
auto ScalableCircularQueue<T>::operator=(ScalableCircularQueue&& other) noexcept
    -> ScalableCircularQueue&
{
  BaseQueueType::operator=(std::move(other));
  free_elements_ = std::move(other.free_elements_);
  allocated_elements_ = std::move(other.allocated_elements_);
  elements_ = std::move(other.elements_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <Movable T> inline
auto ScalableCircularQueue<T>::capacity() const noexcept -> size_type
{
  const size_type cap = elements_.size();
  ZISC_ASSERT((cap == 0) || has_single_bit(cap),
              "The capacity isn't power of 2. capacity = ", cap);
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <Movable T> inline
constexpr auto ScalableCircularQueue<T>::capacityMax() noexcept -> size_type
{
  const size_type cap = size_type{0b1u} << (std::numeric_limits<size_type>::digits - 2);
  return cap;
}

/*!
  \details No detailed description
  */
template <Movable T> inline
void ScalableCircularQueue<T>::clear() noexcept
{
  allocated_elements_.clear();
  free_elements_.full();
  {
    const std::size_t s = capacity();
    elements_.clear();
    elements_.resize(s);
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <Movable T> inline
auto ScalableCircularQueue<T>::data() const noexcept -> const container_type&
{
  return elements_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Movable T> inline
auto ScalableCircularQueue<T>::dequeue() noexcept -> DequeueQueryResultT
{
  DequeueQueryResultT result;
  const uint64b index = allocated_elements_.dequeue(false); // Get an entry point
  const bool is_success = index != RingBuffer::invalidIndex();
  if (is_success) {
    result = std::move(elements_[index]);
    free_elements_.enqueue(index, true);
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <Movable T> inline
auto ScalableCircularQueue<T>::enqueue(ConstReference value) -> EnqueueQueryResultT
{
  return enqueueImpl(value);
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <Movable T> inline
auto ScalableCircularQueue<T>::enqueue(RReference value) -> EnqueueQueryResultT
{
  return enqueueImpl(std::move(value));
}

/*!
  \details No detailed description

  \return No description
  */
template <Movable T> inline
constexpr bool ScalableCircularQueue<T>::isConcurrent() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <Movable T> inline
bool ScalableCircularQueue<T>::isEmpty() const noexcept
{
  const size_type s = size();
  const bool result = s == 0;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Movable T> inline
pmr::memory_resource* ScalableCircularQueue<T>::resource() const noexcept
{
  pmr::memory_resource* mem_resource = elements_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <Movable T> inline
void ScalableCircularQueue<T>::setCapacity(const size_type cap) noexcept
{
  const size_type cap_pow2 = bit_ceil(cap);
  constexpr size_type cap_max = capacityMax();
  if ((capacity() < cap_pow2) && (cap_pow2 <= cap_max)) {
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
template <Movable T> inline
auto ScalableCircularQueue<T>::size() const noexcept -> size_type
{
  const std::size_t s = allocated_elements_.distance();
  return s;
}

/*!
  \details No detailed description

  \tparam ValueT No description.
  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <Movable T> template <typename ValueT> inline
auto ScalableCircularQueue<T>::enqueueImpl(ValueT&& value) -> EnqueueQueryResultT
{
  const uint64b index = free_elements_.dequeue(true); // Get an entry index

  // Check overflow
  using OverflowErr = typename BaseQueueType::OverflowError;
  if (index == RingBuffer::overflowIndex()) {
    throw OverflowErr{"Queue overflow happened.",
                      resource(),
                      std::forward<ValueT>(value)};
  }

  const bool is_success = index != RingBuffer::invalidIndex();
  if (is_success) {
    elements_[index] = std::forward<ValueT>(value);
    allocated_elements_.enqueue(index, false);
  }
  return EnqueueQueryResultT{index};
}

} // namespace zisc

#endif // ZISC_SCALABLE_CIRCULAR_QUEUE_INL_HPP
