/*!
  \file restricted_lf_queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_RESTRICTED_LF_QUEUE_INL_HPP
#define ZISC_RESTRICTED_LF_QUEUE_INL_HPP

#include "restricted_lf_queue.hpp"
// Standard C++ library
#include <algorithm>
#include <limits>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "algorithm.hpp"
#include "atomic.hpp"
#include "error.hpp"
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <typename T> inline
RestrictedLFQueue<T>::RestrictedLFQueue(
    pmr::memory_resource* mem_resource) noexcept :
        RestrictedLFQueue(1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <typename T> inline
RestrictedLFQueue<T>::RestrictedLFQueue(
    const size_type cap,
    pmr::memory_resource* mem_resource) noexcept :
        BaseQueueType(),
        elements_{typename container_type::allocator_type{mem_resource}},
        indices_{typename pmr::vector<uint64b>::allocator_type{mem_resource}},
        status_{typename pmr::vector<uint8b>::allocator_type{mem_resource}},
        head_{0},
        tail_{0}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename T> inline
RestrictedLFQueue<T>::RestrictedLFQueue(
    RestrictedLFQueue&& other) noexcept :
        BaseQueueType(other),
        elements_{std::move(other.elements_)},
        indices_{std::move(other.indices_)},
        status_{std::move(other.status_)},
        head_{other.head_},
        tail_{other.tail_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename T> inline
auto RestrictedLFQueue<T>::operator=(
    RestrictedLFQueue&& other) noexcept
        -> RestrictedLFQueue&
{
  //! \todo Base type operator=
  elements_ = std::move(other.elements_);
  indices_ = std::move(other.indices_);
  status_ = std::move(other.status_);
  head_ = other.head_;
  tail_ = other.tail_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto RestrictedLFQueue<T>::capacity() const noexcept -> size_type
{
  const size_type cap = elements_.size();
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
constexpr auto RestrictedLFQueue<T>::capacityMax() noexcept -> size_type
{
  const size_type cap = std::numeric_limits<uint64b>::max() >> 1;
  return cap;
}

/*!
  \details No detailed description
  */
template <typename T> inline
void RestrictedLFQueue<T>::clear() noexcept
{
  constexpr uint64b null = getNull(0);
  std::fill(indices_.begin(), indices_.end(), null);
  std::fill(status_.begin(), status_.end(), kFalse);
  head_ = 0;
  tail_ = 0;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto RestrictedLFQueue<T>::data() const noexcept -> const container_type&
{
  return elements_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto RestrictedLFQueue<T>::dequeue() noexcept -> std::tuple<bool, Type>
{
  std::tuple<bool, Type> result;
  const size_type index = dequeueImpl();
  auto& flag = std::get<0>(result);
  flag = index != invalidIndex();
  if (flag) {
    while (!status_[index])
      std::this_thread::yield();
    std::get<1>(result) = std::move(elements_[index]);
    status_[index] = kFalse;
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename T> inline
bool RestrictedLFQueue<T>::enqueue(ConstReference value)
{
  const size_type index = enqueueImpl();
  // Check overflow
  using OverflowError = typename BaseQueueType::OverflowError;
  const bool result = index != invalidIndex();
  if (!result)
    throw OverflowError{"Queue overflow happened.", resource(), value};

  while (status_[index])
    std::this_thread::yield();
  elements_[index] = value;
  status_[index] = kTrue;
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename T> inline
bool RestrictedLFQueue<T>::enqueue(RReference value)
{
  const size_type index = enqueueImpl();
  // Check overflow
  using OverflowError = typename BaseQueueType::OverflowError;
  const bool result = index != invalidIndex();
  if (!result)
    throw OverflowError{"Queue overflow happened.", resource(), std::move(value)};

  while (status_[index])
    std::this_thread::yield();
  elements_[index] = std::move(value);
  status_[index] = kTrue;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
bool RestrictedLFQueue<T>::isEmpty() const noexcept
{
  const size_type s = size();
  const bool result = s == 0;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
pmr::memory_resource* RestrictedLFQueue<T>::resource() const noexcept
{
  pmr::memory_resource* mem_resource = elements_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <typename T> inline
void RestrictedLFQueue<T>::setCapacity(const size_type cap) noexcept
{
  const size_type cap_power2 = Algorithm::roundUpToPowOf2(cap);
  constexpr size_type cap_max = capacityMax();
  if ((capacity() < cap_power2) && (cap_power2 <= cap_max)) {
    elements_.resize(cap_power2);
    indices_.resize(cap_power2);
    status_.resize(cap_power2);
  }
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto RestrictedLFQueue<T>::size() const noexcept -> size_type
{
  const size_type s = (head_ > tail_) ? head_ - tail_ : 0;
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto RestrictedLFQueue<T>::dequeueImpl() noexcept -> size_type
{
  size_type result = invalidIndex();
  while (result == invalidIndex()) {
    const volatile uint64b t = tail_;
    const uint64b h = head_;
    // Double-collect
    if (t != tail_)
      continue;
    // Check if the queue is empty
    if (t == h)
      break;
    // Check if the element is ready taken
    const uint64b cap = cast<uint64b>(capacity());
    const uint64b nextCycle = t / cap + 1;
    const uint64b null = getNull(nextCycle);
    const size_type index = permuteIndex(t % cap);
    const uint64b state = indices_[index];
    if (state != null) {
      // Try to retrieve the element
      const auto old = Atomic::compareAndExchange(&indices_[index], state, null);
      if (old == state)
        result = index;
    }
    // Increment the counter
    Atomic::compareAndExchange(&tail_, t, t + 1);
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto RestrictedLFQueue<T>::enqueueImpl() noexcept -> size_type
{
  size_type result = invalidIndex();
  while (result == invalidIndex()) {
    const volatile uint64b h = head_;
    const uint64b t = tail_;
    // Double-collect
    if (h != head_)
      continue;
    // Check if the queue if full
    const uint64b cap = cast<uint64b>(capacity());
    if (h == t + cap)
      break;
    // Try to perform the offer
    const uint64b cycle = h / cap;
    constexpr uint64b occupied = occupiedFlag();
    const uint64b null = getNull(cycle);
    const size_type index = permuteIndex(h % cap);
    const auto old = Atomic::compareAndExchange(&indices_[index], null, occupied);
    if (old == null)
      result = index;
    // Increment the counter
    Atomic::compareAndExchange(&head_, h, h + 1);
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] cycle No description.
  \return No description
  */
template <typename T> inline
constexpr uint64b RestrictedLFQueue<T>::getNull(const uint64b cycle) noexcept
{
  constexpr size_type s = 8 * sizeof(uint64b);
  constexpr uint64b null_flag = cast<uint64b>(uint64b{0b1u} << (s - 1));
  const uint64b null = null_flag + cycle;
  return null;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
constexpr auto RestrictedLFQueue<T>::invalidIndex() noexcept -> size_type
{
  const size_type index = std::numeric_limits<size_type>::max();
  return index;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename T> inline
auto RestrictedLFQueue<T>::permuteIndex(const uint64b index) const noexcept
    -> size_type
{
  return index;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
constexpr uint64b RestrictedLFQueue<T>::occupiedFlag() noexcept
{
  return 0;
}

} // namespace zisc

#endif // ZISC_RESTRICTED_LF_QUEUE_INL_HPP
