/*!
  \file portable_ring_buffer-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PORTABLE_RING_BUFFER_INL_HPP
#define ZISC_PORTABLE_RING_BUFFER_INL_HPP

#include "portable_ring_buffer.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <bit>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "ring_buffer.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
PortableRingBuffer::PortableRingBuffer(pmr::memory_resource* mem_resource) noexcept :
    BaseRingBufferT(),
    memory_{typename decltype(memory_)::allocator_type{mem_resource}},
    size_{0}
{
  initialize();
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
PortableRingBuffer::PortableRingBuffer(PortableRingBuffer&& other) noexcept :
    BaseRingBufferT(std::move(other)),
    memory_{std::move(other.memory_)},
    size_{other.size_}
{
}

/*!
  \details No detailed description
  */
inline
PortableRingBuffer::~PortableRingBuffer() noexcept
{
  destroy();
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
auto PortableRingBuffer::operator=(PortableRingBuffer&& other) noexcept -> PortableRingBuffer&
{
  BaseRingBufferT::operator=(std::move(other));
  memory_ = std::move(other.memory_);
  size_ = other.size_;
  return *this;
}

/*!
  \details No detailed description
  */
inline
void PortableRingBuffer::clear() noexcept
{
  for (std::size_t i = 0; i < size(); ++i)
    getIndex(i).store(invalidIndex(), std::memory_order::release);

  head().store(0, std::memory_order::release);
  threshold().store(-1, std::memory_order::release);
  tail().store(0, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] nonempty No description.
  \return No description
  */
inline
uint64b PortableRingBuffer::dequeue(const bool nonempty) noexcept
{
  uint64b index = invalidIndex();
  uint64b headp = 0;
  uint64b tailp = 0;
  uint64b head_cycle = 0;
  uint64b head_index = 0;
  int attempt = 0;
  bool flag = nonempty || (0 <= threshold().load(std::memory_order::acquire));
  bool again = false;

  // Cautious dequeue
  if (nonempty && (distance() == 0)) [[unlikely]] {
    flag = false;
    index = overflowIndex();
  }

  while (flag) {
    const uint64b n = cast<uint64b>(size());
    if (!again) {
      headp = head().fetch_add(1, std::memory_order::acq_rel);
      head_cycle = (headp << 1) | (2 * n - 1);
      head_index = permuteIndex(headp);
      attempt = 0;
    }
    again = false;
    uint64b entry = getIndex(head_index).load(std::memory_order::acquire);
    uint64b entry_cycle = 0;
    uint64b entry_new = 0;
    do {
      entry_cycle = entry | (2 * n - 1);
      flag = entry_cycle != head_cycle;
      if (!flag) {
        getIndex(head_index).fetch_or(n - 1, std::memory_order::acq_rel);
        index = entry & (n - 1);
        break;
      }
      else if ((entry | n) != entry_cycle) {
        entry_new = entry & ~n;
        if (entry == entry_new)
          break;
      }
      else {
        constexpr int amask = (1 << 8) - 1;
        constexpr int amax = 1 << 12;
        tailp = ((attempt & amask) == 0) ? tail().load(std::memory_order::acquire) : tailp;
        again = (++attempt <= amax) && compare<std::greater_equal>(tailp, headp + 1);
        if (again)
          break;
        entry_new = head_cycle ^ ((~entry) & n);
      }
    } while (compare<std::less>(entry_cycle, head_cycle) &&
             !getIndex(head_index).compare_exchange_weak(entry,
                                                         entry_new,
                                                         std::memory_order::acq_rel,
                                                         std::memory_order::acquire));
    if (flag && !again && !nonempty) {
      tailp = tail().load(std::memory_order::acquire);
      flag = compare<std::greater>(tailp, headp + 1);
      if (flag) {
        flag = 0 < threshold().fetch_sub(1, std::memory_order::acq_rel);
        index = flag ? index : invalidIndex();
      }
      else {
        catchUp(tailp, headp + 1);
        threshold().fetch_sub(1, std::memory_order::acq_rel);
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
inline
std::size_t PortableRingBuffer::distance() const noexcept
{
  const uint64b h = head().load(std::memory_order::acquire);
  const uint64b t = tail().load(std::memory_order::acquire);
  const std::size_t d = (h < t) ? cast<std::size_t>(t - h) : 0;
  return d;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] nonempty No description.
  \return No description
  */
inline
bool PortableRingBuffer::enqueue(const uint64b index, const bool nonempty) noexcept
{
  uint64b tailp = 0;
  uint64b tail_cycle = 0;
  uint64b tail_index = 0;
  uint64b entry = 0;
  bool retry = false;
  while (true) {
    const uint64b n = cast<uint64b>(size());
    if (!retry) {
      tailp = tail().fetch_add(1, std::memory_order::acq_rel);
      tail_cycle = (tailp << 1) | (2 * n - 1);
      tail_index = permuteIndex(tailp);
      entry = getIndex(tail_index).load(std::memory_order::acquire);
    }
    retry = false;
    const uint64b entry_cycle = entry | (2 * n - 1);
    if (compare<std::less>(entry_cycle, tail_cycle) &&
        ((entry == entry_cycle) ||
         ((entry == (entry_cycle ^ n)) &&
          compare<std::less_equal>(head().load(std::memory_order::acquire), tailp)))) {
      const uint64b entry_index = index ^ cast<uint64b>(n - 1);
      retry = !getIndex(tail_index).compare_exchange_weak(entry,
                                                          tail_cycle ^ entry_index,
                                                          std::memory_order::acq_rel,
                                                          std::memory_order::acquire);
      if (retry)
        continue;
      const uint64b half = n >> 1;
      const int64b threshold3 = calcThreshold3(half);
      if (!nonempty && (threshold().load(std::memory_order::acquire) != threshold3))
        threshold().store(threshold3, std::memory_order::release);
      break;
    }
  }
  return true;
}

/*!
  \details No detailed description
  */
inline
void PortableRingBuffer::full() noexcept
{
  using AtomicT = std::remove_cvref_t<decltype(getIndex(0))>;

  const uint64b n = cast<uint64b>(size());
  const uint64b half = n >> 1;

  for (uint64b i = 0; i < n; ++i) {
    const uint64b index = permuteIndex(i);
    constexpr std::size_t data_size = sizeof(AtomicT);
    const uint64b v = (i < half)
        ? BaseRingBufferT::permuteIndex<data_size>(n + i, half)
        : invalidIndex();
    getIndex(index).store(v, std::memory_order::release);
  }

  head().store(0, std::memory_order::release);
  threshold().store(calcThreshold3(half), std::memory_order::release);
  tail().store(half, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] s \a s must be a power of 2.
  */
inline
void PortableRingBuffer::setSize(const std::size_t s) noexcept
{
  ZISC_ASSERT((s == 0) || std::has_single_bit(s), "The size isn't 2^n. size = ", s);
  ZISC_ASSERT(s < capacityMax(), "The size exceeds the capacity max. size = ", s);
  destroy();
  {
    ZISC_ASSERT(std::has_single_bit(s), "The s isn't power of 2. s = ", s);
    const std::size_t l = calcMemChunkSize(s);
    memory_.resize(l);
    size_ = s;
  }
  initialize();
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t PortableRingBuffer::size() const noexcept
{
  const std::size_t s = size_;
  ZISC_ASSERT((s == 0) || std::has_single_bit(s), "The size isn't 2^n. size = ", s);
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t PortableRingBuffer::calcMemChunkSize(const std::size_t s) noexcept
{
  std::size_t l = 0;
  using AtomicT = std::remove_cvref_t<decltype(PortableRingBuffer{nullptr}.getIndex(0))>;
  // Offset size
  {
    static_assert(sizeof(AtomicT) <= sizeof(MemChunk));
    static_assert(sizeof(MemChunk) % sizeof(AtomicT) == 0);
    static_assert(alignof(AtomicT) <= alignof(MemChunk));
    l += 3;
  }
  // Indices size
  {
    const std::size_t total = s * sizeof(AtomicT);
    constexpr std::size_t chunk_size = sizeof(MemChunk);
    l += (total + (chunk_size - 1)) / chunk_size;
  }
  return l;
}

/*!
  \details No detailed description

  \param [in] half No description.
  \return No description
  */
inline
int64b PortableRingBuffer::calcThreshold3(const uint64b half) noexcept
{
  const uint64b threshold = 3 * half - 1;
  return cast<int64b>(threshold);
}

/*!
  \details No detailed description

  \param [in] tailp No description.
  \param [in] headp No description.
  */
inline
void PortableRingBuffer::catchUp(uint64b tailp, uint64b headp) noexcept
{
  while (!tail().compare_exchange_weak(tailp,
                                       headp,
                                       std::memory_order::acq_rel,
                                       std::memory_order::acquire)) {
    headp = head().load(std::memory_order::acquire);
    tailp = tail().load(std::memory_order::acquire);
    if (compare<std::greater_equal>(tailp, headp))
      break;
  }
}

/*!
  \details No detailed description
  */
inline
void PortableRingBuffer::destroy() noexcept
{
  if (memory_.empty())
    return;

  // Indices
  if (0 < size()) {
    std::atomic<uint64b>* mem = std::addressof(getIndex(0));
    std::destroy_n(mem, size());
  }
  // Head
  {
    std::atomic<uint64b>* mem = std::addressof(head());
    std::destroy_at(mem);
  }
  // Threshold
  {
    std::atomic<int64b>* mem = std::addressof(threshold());
    std::destroy_at(mem);
  }
  // Tail
  {
    std::atomic<uint64b>* mem = std::addressof(tail());
    std::destroy_at(mem);
  }
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \param [in] func No description.
  \return No description
  */
template <template<typename> typename Func> inline
bool PortableRingBuffer::compare(const uint64b lhs, const uint64b rhs) noexcept
{
  using ParamT = int64b;
  using FuncT = Func<ParamT>;
  static_assert(std::is_same_v<bool, std::invoke_result_t<FuncT, ParamT, ParamT>>);

  const auto d = static_cast<int64b>(lhs - rhs);
  const bool result = FuncT{}(d, static_cast<int64b>(0));

  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<uint64b>& PortableRingBuffer::getIndex(const std::size_t index) noexcept
{
  MemChunk* mem = memory_.data() + 3;
  auto indices = reinterp<std::atomic<uint64b>*>(mem);
  return indices[index];
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<uint64b>& PortableRingBuffer::getIndex(const std::size_t index) const noexcept
{
  const MemChunk* mem = memory_.data() + 3;
  auto indices = reinterp<const std::atomic<uint64b>*>(mem);
  return indices[index];
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<uint64b>& PortableRingBuffer::head() noexcept
{
  MemChunk* mem = memory_.data();
  return *reinterp<std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<uint64b>& PortableRingBuffer::head() const noexcept
{
  const MemChunk* mem = memory_.data();
  return *reinterp<const std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description
  */
inline
void PortableRingBuffer::initialize() noexcept
{
  // Indices
  for (std::size_t i = 0; i < size(); ++i) {
    using AtomicT = std::remove_cvref_t<decltype(getIndex(i))>;
    std::atomic<uint64b>* mem = std::addressof(getIndex(i));
    ::new (mem) AtomicT{};
  }

  if (memory_.size() < 3)
    memory_.resize(3);

  // Head
  {
    using AtomicT = std::remove_cvref_t<decltype(head())>;
    std::atomic<uint64b>* mem = std::addressof(head());
    ::new (mem) AtomicT{};
  }
  // Threshold
  {
    using AtomicT = std::remove_cvref_t<decltype(threshold())>;
    std::atomic<int64b>* mem = std::addressof(threshold());
    ::new (mem) AtomicT{};
  }
  // Tail
  {
    using AtomicT = std::remove_cvref_t<decltype(tail())>;
    std::atomic<uint64b>* mem = std::addressof(tail());
    ::new (mem) AtomicT{};
  }
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
uint64b PortableRingBuffer::permuteIndex(const uint64b index) const noexcept
{
  using AtomicT = std::remove_cvref_t<decltype(getIndex(0))>;
  constexpr std::size_t data_size = sizeof(AtomicT);
  const uint64b i = BaseRingBufferT::permuteIndex<data_size>(index, size());
  return i;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<uint64b>& PortableRingBuffer::tail() noexcept
{
  MemChunk* mem = memory_.data() + 2;
  return *reinterp<std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<uint64b>& PortableRingBuffer::tail() const noexcept
{
  const MemChunk* mem = memory_.data() + 2;
  return *reinterp<const std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<int64b>& PortableRingBuffer::threshold() noexcept
{
  MemChunk* mem = memory_.data() + 1;
  return *reinterp<std::atomic<int64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<int64b>& PortableRingBuffer::threshold() const noexcept
{
  const MemChunk* mem = memory_.data() + 1;
  return *reinterp<const std::atomic<int64b>*>(mem);
}

} // namespace zisc

#endif // ZISC_PORTABLE_RING_BUFFER_INL_HPP
