/*!
  \file lock_free_ring_buffer-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_RING_BUFFER_INL_HPP
#define ZISC_LOCK_FREE_RING_BUFFER_INL_HPP

#include "lock_free_ring_buffer.hpp"
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
LockFreeRingBuffer::LockFreeRingBuffer(pmr::memory_resource* mem_resource) noexcept :
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
LockFreeRingBuffer::LockFreeRingBuffer(LockFreeRingBuffer&& other) noexcept :
    memory_{std::move(other.memory_)},
    size_{other.size_}
{
}

/*!
  \details No detailed description
  */
inline
LockFreeRingBuffer::~LockFreeRingBuffer() noexcept
{
  destroy();
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
auto LockFreeRingBuffer::operator=(LockFreeRingBuffer&& other) noexcept -> LockFreeRingBuffer&
{
  memory_ = std::move(other.memory_);
  size_ = other.size_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t LockFreeRingBuffer::capacityMax() noexcept
{
  constexpr std::size_t invalid = (std::numeric_limits<std::size_t>::max)();
  return invalid;
}

/*!
  \details No detailed description
  */
inline
void LockFreeRingBuffer::clear() noexcept
{
  for (std::size_t i = 0; i < size(); ++i)
    getIndex(i).store(invalidIndex(), std::memory_order::release);

  head().store(0, std::memory_order::release);
  threshold().store(-1, std::memory_order::release);
  tail().store(0, std::memory_order::release);

}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr uint64b LockFreeRingBuffer::overflowIndex() noexcept
{
  constexpr auto index = cast<uint64b>(capacityMax() - 1);
  return index;
}

/*!
  \details No detailed description

  \param [in] nonempty No description.
  \return No description
  */
inline
std::size_t LockFreeRingBuffer::dequeue(const bool nonempty) noexcept
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
  if (const uint64b h = head().load(std::memory_order::acquire),
                    t = tail().load(std::memory_order::acquire);
      nonempty && (t <= h)) [[unlikely]] {
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
  return cast<std::size_t>(index);
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t LockFreeRingBuffer::distance() const noexcept
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
bool LockFreeRingBuffer::enqueue(const std::size_t index, const bool nonempty) noexcept
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
      const uint64b entry_index = cast<uint64b>(index) ^ cast<uint64b>(n - 1);
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

  \param [in] s No description.
  \param [in] e No description.
  */
inline
void LockFreeRingBuffer::fill(const uint64b s, const uint64b e) noexcept
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
    getIndex(index).store(v, std::memory_order::release);
  }

  head().store(s, std::memory_order::release);
  threshold().store(calcThreshold3(half), std::memory_order::release);
  tail().store(e, std::memory_order::release);
}

/*!
  \details No detailed description
  */
inline
void LockFreeRingBuffer::full() noexcept
{
  const uint64b n = cast<uint64b>(size());
  const uint64b half = n >> 1;

  for (uint64b i = 0; i < n; ++i) {
    const uint64b index = permuteIndex(i);
    const uint64b v = (i < half) ? permuteIndexImpl(n + i, order(), half)
                                 : invalidIndex();
    getIndex(index).store(v, std::memory_order::release);
  }

  head().store(0, std::memory_order::release);
  threshold().store(calcThreshold3(half), std::memory_order::release);
  tail().store(half, std::memory_order::release);
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr uint64b LockFreeRingBuffer::invalidIndex() noexcept
{
  constexpr auto invalid = cast<uint64b>(capacityMax());
  return invalid;
}

/*!
  \details No detailed description

  \return No description
  */
inline
uint64b LockFreeRingBuffer::order() const noexcept
{
  const uint64b o = cast<uint64b>(std::bit_width(size() >> 1));
  return (0 < o) ? o - 1 : 0;
}

/*!
  \details No detailed description

  \param [in] s \a s must be a power of 2.
  */
inline
void LockFreeRingBuffer::setSize(const std::size_t s) noexcept
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
std::size_t LockFreeRingBuffer::size() const noexcept
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
std::size_t LockFreeRingBuffer::calcMemChunkSize(const std::size_t s) const noexcept
{
  std::size_t l = 0;
  using AtomicT = std::remove_cvref_t<decltype(getIndex(0))>;
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
int64b LockFreeRingBuffer::calcThreshold3(const uint64b half) const noexcept
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
void LockFreeRingBuffer::catchUp(uint64b tailp, uint64b headp) noexcept
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
void LockFreeRingBuffer::destroy() noexcept
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
bool LockFreeRingBuffer::compare(const uint64b lhs, const uint64b rhs) noexcept
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
std::atomic<uint64b>& LockFreeRingBuffer::getIndex(const std::size_t index) noexcept
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
const std::atomic<uint64b>& LockFreeRingBuffer::getIndex(const std::size_t index) const noexcept
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
std::atomic<uint64b>& LockFreeRingBuffer::head() noexcept
{
  MemChunk* mem = memory_.data();
  return *reinterp<std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<uint64b>& LockFreeRingBuffer::head() const noexcept
{
  const MemChunk* mem = memory_.data();
  return *reinterp<const std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description
  */
inline
void LockFreeRingBuffer::initialize() noexcept
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
uint64b LockFreeRingBuffer::permuteIndex(const uint64b index) const noexcept
{
  const uint64b i = permuteIndexImpl(index, order() + 1, size());
  return i;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] o No description.
  \param [in] n No description.
  \return No description
  */
inline
uint64b LockFreeRingBuffer::permuteIndexImpl(const uint64b index,
                                             const uint64b o,
                                             const uint64b n) const noexcept
{
  ZISC_ASSERT(std::has_single_bit(n), "The n isn't power of 2.");
  using AtomicT = std::remove_cvref_t<decltype(getIndex(0))>;
  constexpr uint64b cache_line_size = kCacheLineSize;
  constexpr uint64b data_size = sizeof(AtomicT);
  constexpr uint64b shift = (data_size < cache_line_size)
      ? std::bit_width(cache_line_size) - std::bit_width(data_size)
      : 0;

  uint64b i = index;
  if (shift < o) {
    const uint64b upper = cast<uint64b>(index << shift);
    const uint64b lower = cast<uint64b>((index & (n - 1)) >> (o - shift));
    i = cast<uint64b>(upper | lower);
  }
  i = i & (n - 1);
  return i;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<uint64b>& LockFreeRingBuffer::tail() noexcept
{
  MemChunk* mem = memory_.data() + 2;
  return *reinterp<std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<uint64b>& LockFreeRingBuffer::tail() const noexcept
{
  const MemChunk* mem = memory_.data() + 2;
  return *reinterp<const std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<int64b>& LockFreeRingBuffer::threshold() noexcept
{
  MemChunk* mem = memory_.data() + 1;
  return *reinterp<std::atomic<int64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<int64b>& LockFreeRingBuffer::threshold() const noexcept
{
  const MemChunk* mem = memory_.data() + 1;
  return *reinterp<const std::atomic<int64b>*>(mem);
}

} // namespace zisc

#endif // ZISC_LOCK_FREE_RING_BUFFER_INL_HPP
