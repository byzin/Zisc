/*!
  \file lock_free_bounded_queue-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_BOUNDED_QUEUE_INL_HPP
#define ZISC_LOCK_FREE_BOUNDED_QUEUE_INL_HPP

#include "lock_free_bounded_queue.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>
// Zisc
#include "algorithm.hpp"
#include "error.hpp"
#include "std_memory_resource.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Type> inline
LockFreeBoundedQueue<Type>::LockFreeBoundedQueue(
    std::pmr::memory_resource* mem_resource) noexcept :
        LockFreeBoundedQueue(1, mem_resource)
{
}

/*!
  */
template <typename Type> inline
LockFreeBoundedQueue<Type>::LockFreeBoundedQueue(
    const std::size_t cap,
    std::pmr::memory_resource* mem_resource) noexcept :
        free_elements_buffer_{mem_resource},
        allocated_elements_buffer_{mem_resource},
        elements_{std::pmr::polymorphic_allocator<Type>{mem_resource}},
        size_{0}
{
  setCapacity(cap);
}

/*!
  */
template <typename Type> inline
LockFreeBoundedQueue<Type>::LockFreeBoundedQueue(
    LockFreeBoundedQueue&& other) noexcept :
        free_elements_buffer_{std::move(other.free_elements_buffer_)},
        allocated_elements_buffer_{std::move(other.allocated_elements_buffer_)},
        elements_{std::move(other.elements_)},
        size_{other.size_.load()}
{
}

/*!
  */
template <typename Type> inline
auto LockFreeBoundedQueue<Type>::operator=(LockFreeBoundedQueue&& other)
    noexcept -> LockFreeBoundedQueue&
{
  free_elements_buffer_ = std::move(other.free_elements_buffer_);
  allocated_elements_buffer_ = std::move(other.allocated_elements_buffer_);
  elements_ = std::move(other.elements_);
  return *this;
}

/*!
  */
template <typename Type> inline
std::size_t LockFreeBoundedQueue<Type>::capacity() const noexcept
{
  const std::size_t cap = elements_.size();
  ZISC_ASSERT(Algorithm::isPowerOf2(cap),
              "The capacity isn't power of 2. capacity = ", cap);
  return cap;
}

/*!
  */
template <typename Type> inline
constexpr std::size_t LockFreeBoundedQueue<Type>::capacityMax() noexcept
{
  using Int = typename RingBuffer::Int;
  const auto cap_max = cast<std::size_t>(std::numeric_limits<Int>::max() >> 1);
  return cap_max;
}

/*!
  */
template <typename Type> inline
void LockFreeBoundedQueue<Type>::clear() noexcept
{
  using UInt = typename RingBuffer::UInt;
  allocated_elements_buffer_.clear();
  const UInt s = 0;
  const UInt e = cast<UInt>(1u << free_elements_buffer_.order());
  free_elements_buffer_.fill(s, e);
  size_ = 0;
}

/*!
  */
template <typename Type> inline
const pmr::vector<Type>& LockFreeBoundedQueue<Type>::data() const noexcept
{
  return elements_;
}

/*!
  */
template <typename Type> inline
std::tuple<bool, Type> LockFreeBoundedQueue<Type>::dequeue() noexcept
{
  using UInt = typename RingBuffer::UInt;
  std::tuple<bool, Type> result;
  const UInt index = allocated_elements_buffer_.dequeue(false); // Get an entry point
  auto& flag = std::get<0>(result);
  flag = index != RingBuffer::invalidIndex();
  if (flag) {
    std::get<1>(result) = std::move(elements_[index]);
    free_elements_buffer_.enqueue(index, true);
    --size_;
  }
  return result;
}

/*!
  */
template <typename Type> inline
bool LockFreeBoundedQueue<Type>::enqueue(const Type& value) noexcept
{
  using UInt = typename RingBuffer::UInt;
  const UInt index = free_elements_buffer_.dequeue(true); // Get an entry index
  const bool result = index != RingBuffer::invalidIndex();
  if (result) {
    elements_[index] = value;
    allocated_elements_buffer_.enqueue(index, false);
    ++size_;
  }
  return result;
}

/*!
  */
template <typename Type> inline
bool LockFreeBoundedQueue<Type>::enqueue(Type&& value) noexcept
{
  using UInt = typename RingBuffer::UInt;
  const UInt index = free_elements_buffer_.dequeue(true); // Get an entry index
  const bool result = index != RingBuffer::invalidIndex();
  if (result) {
    elements_[index] = std::move(value);
    allocated_elements_buffer_.enqueue(index, false);
    ++size_;
  }
  return result;
}

/*!
  */
template <typename Type> inline
bool LockFreeBoundedQueue<Type>::isEmpty() const noexcept
{
  const int s = size();
  const bool result = s == 0;
  return result;
}

/*!
  */
template <typename Type> inline
void LockFreeBoundedQueue<Type>::setCapacity(const std::size_t cap) noexcept
{
  const std::size_t cap_power2 = zisc::Algorithm::roundUpToPowerOf2(cap);
  constexpr std::size_t cap_max = capacityMax();
  if ((capacity() < cap_power2) && (cap_power2 <= cap_max) ) {
    elements_.resize(cap_power2);
    allocated_elements_buffer_.setSize(cap_power2 << 1);
    free_elements_buffer_.setSize(cap_power2 << 1);
  }
  clear();
}

/*!
  */
template <typename Type> inline
int LockFreeBoundedQueue<Type>::size() const noexcept
{
  const int s = size_.load();
  return s;
}

/*!
  */
template <typename Type> inline
LockFreeBoundedQueue<Type>::RingBuffer::RingBuffer(
    std::pmr::memory_resource* mem_resource) noexcept :
        indices_{std::pmr::polymorphic_allocator<std::atomic<UInt>>{mem_resource}}
{
}

/*!
  */
template <typename Type> inline
LockFreeBoundedQueue<Type>::RingBuffer::RingBuffer(RingBuffer&& other) noexcept :
    indices_{std::move(other.indices_)},
    head_{std::move(other.head_.load())},
    threshold_{std::move(other.threshold_.load())},
    tail_{std::move(other.tail_.load())}
{
}

/*!
  */
template <typename Type> inline
auto LockFreeBoundedQueue<Type>::RingBuffer::operator=(RingBuffer&& other)
    noexcept -> RingBuffer&
{
  indices_ = std::move(other.indices_);
  head_ = std::move(other.head_.load());
  threshold_ = std::move(other.threshold_.load());
  tail_ = std::move(other.tail_.load());
  return *this;
}

/*!
  */
template <typename Type> inline
void LockFreeBoundedQueue<Type>::RingBuffer::clear() noexcept
{
  std::fill(indices_.begin(), indices_.end(), invalidIndex());
  head_ = 0;
  threshold_ = -1;
  tail_ = 0;
}

/*!
  */
template <typename Type> inline
auto LockFreeBoundedQueue<Type>::RingBuffer::dequeue(const bool nonempty) noexcept
    -> UInt
{
  UInt index = invalidIndex();
  UInt head = 0;
  UInt head_cycle = 0;
  UInt head_index = 0;
  int attempt = 0;
  bool flag = nonempty || (0 <= threshold_.load());
  bool again = false;
  while (flag) {
    const UInt n = cast<UInt>(size());
    if (!again) {
      head = head_.fetch_add(1, std::memory_order_acq_rel);
      head_cycle = (head << 1) | (2 * n - 1);
      head_index = mapIndex(head);
      attempt = 0;
    }
    again = false;
    UInt entry = indices_[head_index].load(std::memory_order_acquire);
    UInt entry_cycle = 0;
    UInt entry_new = 0;
    do {
      entry_cycle = entry | (2 * n - 1);
      flag = entry_cycle != head_cycle;
      if (!flag) {
        indices_[head_index].fetch_or(n - 1, std::memory_order_acq_rel);
        index = entry & (n - 1);
        break;
      }
      else if ((entry | n) != entry_cycle) {
        entry_new = entry & ~cast<UInt>(n);
        if (entry == entry_new)
          break;
      }
      else {
        again = ++attempt <= 10000;
        if (again)
          break;
        entry_new = head_cycle ^ ((~entry) & cast<UInt>(n));
      }
    } while ((diff(entry_cycle, head_cycle) < 0) &&
             !indices_[head_index].compare_exchange_weak(
                  entry,
                  entry_new,
                  std::memory_order_acq_rel,
                  std::memory_order_acquire));
    if (flag && !again && !nonempty) {
      UInt tail = tail_.load(std::memory_order_acquire);
      flag = 0 < diff(tail, head + 1);
      if (flag) {
        flag = 0 < threshold_.fetch_sub(1, std::memory_order_acq_rel);
        index = flag ? index : invalidIndex();
      }
      else {
        catchUp(tail, head + 1);
        threshold_.fetch_sub(1, std::memory_order_acq_rel);
        index = invalidIndex();
      }
    }
  }
  return index;
}

/*!
  */
template <typename Type> inline
bool LockFreeBoundedQueue<Type>::RingBuffer::enqueue(
    const UInt index,
    const bool nonempty) noexcept
{
  UInt tail = 0;
  UInt tail_cycle = 0;
  UInt tail_index = 0;
  UInt entry = 0;
  bool retry = false;
  while (true) {
    const UInt n = cast<UInt>(size());
    if (!retry) {
      tail = tail_.fetch_add(1, std::memory_order_acq_rel);
      tail_cycle = (tail << 1) | (2 * n - 1);
      tail_index = mapIndex(tail);
      entry = indices_[tail_index].load(std::memory_order_acquire);
    }
    retry = false;
    const UInt entry_cycle = entry | (2 * n - 1);
    if ((diff(entry_cycle, tail_cycle) < 0) &&
        ((entry == entry_cycle) ||
         ((entry == (entry_cycle ^ n)) &&
          (diff(head_.load(std::memory_order_acquire), tail) <= 0)))) {
      const UInt entry_index = index ^ cast<UInt>(n - 1);
      retry = !indices_[tail_index].compare_exchange_weak(
          entry,
          tail_cycle ^ entry_index,
          std::memory_order_acq_rel,
          std::memory_order_acquire);
      if (retry)
        continue;
      const UInt half = n >> 1;
      const Int threshold = getThreshold3(half);
      if (!nonempty && (threshold_.load() != threshold))
        threshold_.store(threshold);
      break;
    }
  }
  return true;
}

/*!
  */
template <typename Type> inline
void LockFreeBoundedQueue<Type>::RingBuffer::fill(const UInt s,
                                                  const UInt e) noexcept
{
  ZISC_ASSERT(e <= size(), "The e is greater than the number of elements.");
  ZISC_ASSERT(s <= e, "The s is greater than the e.");
  const UInt n = cast<UInt>(size());
  const UInt half = n >> 1;

  for (UInt i = 0; i < n; ++i) {
    const UInt index = mapIndex(i);
    const UInt v = (i < s) ? 2 * n - 1 :
                   (i < e) ? n + i
                           : invalidIndex();
    indices_[index] = v;
  }
  head_ = s;
  threshold_ = getThreshold3(half);
  tail_ = e;
}

/*!
  */
template <typename Type> inline
constexpr auto LockFreeBoundedQueue<Type>::RingBuffer::invalidIndex() noexcept
    -> UInt
{
  constexpr UInt index = std::numeric_limits<UInt>::max();
  return index;
}

/*!
  */
template <typename Type> inline
std::size_t LockFreeBoundedQueue<Type>::RingBuffer::order() const noexcept
{
  std::size_t o = Algorithm::getExponent(size() >> 1);
  return o;
}

/*!
  \param[in] s \a s must be a power of 2.
  */
template <typename Type> inline
void LockFreeBoundedQueue<Type>::RingBuffer::setSize(const std::size_t s) noexcept
{
  ZISC_ASSERT(Algorithm::isPowerOf2(s), "The s isn't power of 2. s = ", s);
  decltype(indices_) vec{s, indices_.get_allocator()};
  std::swap(indices_, vec);
}

/*!
  */
template <typename Type> inline
std::size_t LockFreeBoundedQueue<Type>::RingBuffer::size() const noexcept
{
  const std::size_t s = indices_.size();
  ZISC_ASSERT(Algorithm::isPowerOf2(s), "The s isn't power of 2. s = ", s);
  return s;
}

/*!
  */
template <typename Type> inline
constexpr std::size_t LockFreeBoundedQueue<Type>::RingBuffer::cacheBytes() noexcept
{
  constexpr std::size_t bytes = 0b1u << cacheShift();
  return bytes;
}

/*!
  */
template <typename Type> inline
constexpr std::size_t LockFreeBoundedQueue<Type>::RingBuffer::cacheShift() noexcept
{
  const std::size_t bytes = sizeof(void*);
  constexpr std::size_t shift = (bytes == 4) ? 4 :
                                (bytes == 8) ? 7 : //!< It's true for x86/x86-64 but needs to be defined for other CPUs
                                               12;
  return shift;
}

/*!
  */
template <typename Type> inline
void LockFreeBoundedQueue<Type>::RingBuffer::catchUp(UInt tail,
                                                     UInt head) noexcept
{
  while (!tail_.compare_exchange_weak(tail,
                                      head,
                                      std::memory_order_acq_rel,
                                      std::memory_order_acquire)) {
    head = head_.load();
    tail = tail_.load();
    if (diff(tail, head) >= 0)
      break;
  }
}

/*!
  */
template <typename Type> inline
auto LockFreeBoundedQueue<Type>::RingBuffer::diff(const UInt lhs, const UInt rhs)
    const noexcept -> Int
{
  auto d = cast<Int>(lhs) - cast<Int>(rhs);
  return d;
}

/*!
  */
template <typename Type> inline
auto LockFreeBoundedQueue<Type>::RingBuffer::getThreshold3(const UInt half)
    const noexcept -> Int
{
  const UInt threshold = 3 * half - 1;
  return cast<Int>(threshold);
}

/*!
  */
template <typename Type> inline
auto LockFreeBoundedQueue<Type>::RingBuffer::mapIndex(const UInt index)
    const noexcept -> UInt
{
  const UInt n = cast<UInt>(size());
  UInt dst_index = index & (n - 1);
  if constexpr (0 < ringMin()) {
    const UInt o = cast<UInt>(order());
    dst_index = (dst_index >> (o + 1 - ringMin())) |
                ((index << ringMin()) & (n - 1));
  }
  return dst_index;
}

/*!
  */
template <typename Type> inline
constexpr auto LockFreeBoundedQueue<Type>::RingBuffer::ringMin() noexcept -> UInt
{
  const std::size_t e = Algorithm::getExponent(sizeof(void*));
  const std::size_t m = cacheShift() - e;
  return cast<UInt>(m);
}

} // namespace zisc

#endif // ZISC_LOCK_FREE_BOUNDED_QUEUE_INL_HPP
