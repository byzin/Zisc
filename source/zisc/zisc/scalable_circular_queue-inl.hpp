/*!
  \file scalable_circular_queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include <tuple>
#include <utility>
#include <vector>
// Zisc
#include "algorithm.hpp"
#include "error.hpp"
#include "std_memory_resource.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <typename T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(pmr::memory_resource* mem_resource)
    noexcept :
        ScalableCircularQueue(1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <typename T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(const size_type cap,
                                                pmr::memory_resource* mem_resource)
    noexcept :
        BaseQueueType(),
        free_elements_{mem_resource},
        allocated_elements_{mem_resource},
        elements_{typename pmr::vector<Type>::allocator_type{mem_resource}}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename T> inline
ScalableCircularQueue<T>::ScalableCircularQueue(ScalableCircularQueue&& other)
    noexcept :
        BaseQueueType(other),
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
template <typename T> inline
auto ScalableCircularQueue<T>::operator=(ScalableCircularQueue&& other)
    noexcept -> ScalableCircularQueue&
{
  //! \todo Base type operator=
  free_elements_ = std::move(other.free_elements_);
  allocated_elements_ = std::move(other.allocated_elements_);
  elements_ = std::move(other.elements_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto ScalableCircularQueue<T>::capacity() const noexcept -> size_type
{
  const size_type cap = elements_.size();
  ZISC_ASSERT(Algorithm::isPowerOf2(cap),
              "The capacity isn't power of 2. capacity = ", cap);
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
constexpr auto ScalableCircularQueue<T>::capacityMax() noexcept -> size_type
{
  const uint64b cap = uint64b{0b1u} << (8 * sizeof(uint64b) - 2);
  return cap;
}

/*!
  \details No detailed description
  */
template <typename T> inline
void ScalableCircularQueue<T>::clear() noexcept
{
  allocated_elements_.clear();
  const uint64b s = 0;
  const uint64b e = cast<uint64b>(uint64b{0b1} << free_elements_.order());
  free_elements_.fill(s, e);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto ScalableCircularQueue<T>::data() const noexcept -> const container_type&
{
  return elements_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto ScalableCircularQueue<T>::dequeue() noexcept -> std::tuple<bool, Type>
{
  std::tuple<bool, Type> result;
  const uint64b index = allocated_elements_.dequeue(false); // Get an entry point
  auto& flag = std::get<0>(result);
  flag = index != RingBuffer::invalidIndex();
  if (flag) {
    std::get<1>(result) = std::move(elements_[index]);
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
template <typename T> inline
bool ScalableCircularQueue<T>::enqueue(ConstReference value)
{
  const uint64b index = free_elements_.dequeue(true); // Get an entry index

  // Check overflow
  using OverflowError = typename BaseQueueType::OverflowError;
  if (index == RingBuffer::overflowIndex())
    throw OverflowError{"Queue overflow happened.", resource(), value};

  const bool result = index != RingBuffer::invalidIndex();
  if (result) {
    elements_[index] = value;
    allocated_elements_.enqueue(index, false);
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
bool ScalableCircularQueue<T>::enqueue(RReference value)
{
  const uint64b index = free_elements_.dequeue(true); // Get an entry index

  // Check overflow
  using OverflowError = typename BaseQueueType::OverflowError;
  if (index == RingBuffer::overflowIndex())
    throw OverflowError{"Queue overflow happened.", resource(), std::move(value)};

  const bool result = index != RingBuffer::invalidIndex();
  if (result) {
    elements_[index] = std::move(value);
    allocated_elements_.enqueue(index, false);
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
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
template <typename T> inline
pmr::memory_resource* ScalableCircularQueue<T>::resource() const noexcept
{
  pmr::memory_resource* mem_resource = elements_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <typename T> inline
void ScalableCircularQueue<T>::setCapacity(const size_type cap) noexcept
{
  const size_type cap_pow2 = zisc::Algorithm::roundUpToPowOf2(cap);
  constexpr size_type cap_max = capacityMax();
  if ((capacity() < cap_pow2) && (cap_pow2 <= cap_max) ) {
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
template <typename T> inline
auto ScalableCircularQueue<T>::size() const noexcept -> size_type
{
  const std::size_t s = allocated_elements_.distance();
  return s;
}

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <typename T> inline
ScalableCircularQueue<T>::RingBuffer::RingBuffer(
    pmr::memory_resource* mem_resource) noexcept :
        indices_{typename pmr::vector<std::atomic<uint64b>>::allocator_type{mem_resource}}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename T> inline
ScalableCircularQueue<T>::RingBuffer::RingBuffer(RingBuffer&& other) noexcept :
    head_{std::move(other.head_.load())},
    threshold_{std::move(other.threshold_.load())},
    tail_{std::move(other.tail_.load())},
    indices_{std::move(other.indices_)},
    order_{other.order_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename T> inline
auto ScalableCircularQueue<T>::RingBuffer::operator=(RingBuffer&& other) noexcept
    -> RingBuffer&
{
  head_ = std::move(other.head_.load());
  threshold_ = std::move(other.threshold_.load());
  tail_ = std::move(other.tail_.load());
  indices_ = std::move(other.indices_);
  order_ = other.order_;
  return *this;
}

/*!
  \details No detailed description
  */
template <typename T> inline
void ScalableCircularQueue<T>::RingBuffer::clear() noexcept
{
  std::fill(indices_.begin(), indices_.end(), invalidIndex());
  head_ = 0;
  threshold_ = -1;
  tail_ = 0;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
constexpr uint64b ScalableCircularQueue<T>::RingBuffer::overflowIndex() noexcept
{
  constexpr uint64b index = std::numeric_limits<uint64b>::max() - 1;
  return index;
}

/*!
  \details No detailed description

  \param [in] nonempty No description.
  \return No description
  */
template <typename T> inline
uint64b ScalableCircularQueue<T>::RingBuffer::dequeue(const bool nonempty) noexcept
{
  uint64b index = invalidIndex();
  uint64b head = 0;
  uint64b head_cycle = 0;
  uint64b head_index = 0;
  int attempt = 0;
  bool flag = nonempty || (0 <= threshold_.load());
  bool again = false;
  while (flag) {
    const uint64b n = cast<uint64b>(size());
    if (!again) {
      head = head_.fetch_add(1, std::memory_order_acq_rel);
      head_cycle = (head << 1) | (2 * n - 1);
      head_index = permuteIndex(head);
      attempt = 0;
    }
    again = false;
    uint64b entry = indices_[head_index].load(std::memory_order_acquire);
    uint64b entry_cycle = 0;
    uint64b entry_new = 0;
    do {
      entry_cycle = entry | (2 * n - 1);
      flag = entry_cycle != head_cycle;
      if (!flag) {
        indices_[head_index].fetch_or(n - 1, std::memory_order_acq_rel);
        index = entry & (n - 1);
        break;
      }
      else if ((entry | n) != entry_cycle) {
        entry_new = entry & ~n;
        if (entry == entry_new)
          break;
      }
      else {
        again = ++attempt <= 10000;
        if (again)
          break;
        if (nonempty) {
          flag = false;
          index = overflowIndex();
          break;
        }
        entry_new = head_cycle ^ ((~entry) & n);
      }
    } while ((diff(entry_cycle, head_cycle) < 0) &&
             !indices_[head_index].compare_exchange_weak(
                  entry,
                  entry_new,
                  std::memory_order_acq_rel,
                  std::memory_order_acquire));
    if (flag && !again && !nonempty) {
      uint64b tail = tail_.load(std::memory_order_acquire);
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
  \details No detailed description

  \return No description
  */
template <typename T> inline
std::size_t ScalableCircularQueue<T>::RingBuffer::distance() const noexcept
{
  const auto head = head_.load(std::memory_order_relaxed);
  const auto tail = tail_.load(std::memory_order_relaxed);
  const std::size_t d = (head < tail) ? cast<std::size_t>(tail - head) : 0;
  return d;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] nonempty No description.
  \return No description
  */
template <typename T> inline
bool ScalableCircularQueue<T>::RingBuffer::enqueue(const uint64b index,
                                                   const bool nonempty) noexcept
{
  uint64b tail = 0;
  uint64b tail_cycle = 0;
  uint64b tail_index = 0;
  uint64b entry = 0;
  bool retry = false;
  while (true) {
    const uint64b n = cast<uint64b>(size());
    if (!retry) {
      tail = tail_.fetch_add(1, std::memory_order_acq_rel);
      tail_cycle = (tail << 1) | (2 * n - 1);
      tail_index = permuteIndex(tail);
      entry = indices_[tail_index].load(std::memory_order_acquire);
    }
    retry = false;
    const uint64b entry_cycle = entry | (2 * n - 1);
    if ((diff(entry_cycle, tail_cycle) < 0) &&
        ((entry == entry_cycle) ||
         ((entry == (entry_cycle ^ n)) &&
          (diff(head_.load(std::memory_order_acquire), tail) <= 0)))) {
      const uint64b entry_index = index ^ cast<uint64b>(n - 1);
      retry = !indices_[tail_index].compare_exchange_weak(
          entry,
          tail_cycle ^ entry_index,
          std::memory_order_acq_rel,
          std::memory_order_acquire);
      if (retry)
        continue;
      const uint64b half = n >> 1;
      const int64b threshold = getThreshold3(half);
      if (!nonempty && (threshold_.load() != threshold))
        threshold_.store(threshold);
      break;
    }
  }
  return true;
}

/*!
  \details No detailed description

  \param [in] s No description.
  \param [in] e No description.
  */
template <typename T> inline
void ScalableCircularQueue<T>::RingBuffer::fill(const uint64b s,
                                                const uint64b e) noexcept
{
  ZISC_ASSERT(e <= size(), "The e is greater than the number of elements.");
  ZISC_ASSERT(s <= e, "The s is greater than the e.");
  const uint64b n = cast<uint64b>(size());
  const uint64b half = n >> 1;

  for (uint64b i = 0; i < n; ++i) {
    const uint64b index = permuteIndex(i);
    const uint64b v = (i < s) ? 2 * n - 1 :
                      (i < e) ? n + i
                              : invalidIndex();
    indices_[index] = v;
  }
  head_ = s;
  threshold_ = getThreshold3(half);
  tail_ = e;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
constexpr uint64b ScalableCircularQueue<T>::RingBuffer::invalidIndex() noexcept
{
  constexpr uint64b index = std::numeric_limits<uint64b>::max();
  return index;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
uint64b ScalableCircularQueue<T>::RingBuffer::order() const noexcept
{
  return order_;
}

/*!
  \details No detailed description

  \param [in] s \a s must be a power of 2.
  */
template <typename T> inline
void ScalableCircularQueue<T>::RingBuffer::setSize(const std::size_t s) noexcept
{
  ZISC_ASSERT(Algorithm::isPowerOf2(s), "The s isn't power of 2. s = ", s);
  decltype(indices_) vec{s, indices_.get_allocator()};
  std::swap(indices_, vec);
  // Set order
  order_ = cast<uint64b>(Algorithm::getExponent(size() >> 1));
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
std::size_t ScalableCircularQueue<T>::RingBuffer::size() const noexcept
{
  const std::size_t s = indices_.size();
  ZISC_ASSERT(Algorithm::isPowerOf2(s), "The s isn't power of 2. s = ", s);
  return s;
}

/*!
  \details No detailed description

  \param [in] tail No description.
  \param [in] head No description.
  */
template <typename T> inline
void ScalableCircularQueue<T>::RingBuffer::catchUp(uint64b tail,
                                                   uint64b head) noexcept
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
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename T> inline
int64b ScalableCircularQueue<T>::RingBuffer::diff(const uint64b lhs,
                                                  const uint64b rhs) const noexcept
{
  auto d = cast<int64b>(lhs) - cast<int64b>(rhs);
  return d;
}

/*!
  \details No detailed description

  \param [in] half No description.
  \return No description
  */
template <typename T> inline
int64b ScalableCircularQueue<T>::RingBuffer::getThreshold3(const uint64b half) const noexcept
{
  const uint64b threshold = 3 * half - 1;
  return cast<int64b>(threshold);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename T> inline
uint64b ScalableCircularQueue<T>::RingBuffer::permuteIndex(const uint64b index) const noexcept
{
  using ValueType = typename decltype(indices_)::value_type;
  constexpr uint64b cache_line_size = Config::l1CacheLineSize();
  constexpr uint64b data_size = sizeof(ValueType);
  constexpr uint64b shift = (data_size < cache_line_size)
      ? Algorithm::getExponent(cache_line_size) - Algorithm::getExponent(data_size)
      : 0;
  const uint64b e = order() + 1;
  uint64b i = index;
  if (shift < e) {
    constexpr uint64b mask = cast<uint64b>((uint64b{0b1} << shift) - 1);
    const uint64b upper = cast<uint64b>(index << shift);
    const uint64b lower = cast<uint64b>((index >> (e - shift)) & mask);
    i = cast<uint64b>(upper | lower);
  }
  const uint64b n = cast<uint64b>(size());
  i = i & (n - 1);
  return i;
}

} // namespace zisc

#endif // ZISC_SCALABLE_CIRCULAR_QUEUE_INL_HPP
