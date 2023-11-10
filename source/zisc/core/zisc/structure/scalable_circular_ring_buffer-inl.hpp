/*!
  \file scalable_circular_ring_buffer-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SCALABLE_CIRCULAR_RING_BUFFER_INL_HPP
#define ZISC_SCALABLE_CIRCULAR_RING_BUFFER_INL_HPP

#include "scalable_circular_ring_buffer.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <bit>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory_resource>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "ring_buffer.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
ScalableCircularRingBuffer::ScalableCircularRingBuffer(std::pmr::memory_resource* mem_resource) noexcept :
    memory_{typename decltype(memory_)::allocator_type{mem_resource}}
{
  initialize();
}

/*!
  \details No detailed description
  */
inline
ScalableCircularRingBuffer::~ScalableCircularRingBuffer() noexcept
{
  destroy();
}

/*!
  \details No detailed description
  */
inline
void ScalableCircularRingBuffer::clear() noexcept
{
  head().store(0, std::memory_order::release);
  threshold().store(-1, std::memory_order::release);
  tail().store(0, std::memory_order::release);

  const std::span indices = getIndexList();
  std::for_each_n(indices.begin(), indices.size(), [](std::atomic<uint64b>& index) noexcept
  {
    index.store(BaseRingBufferT::invalidIndex(), std::memory_order::release);
  });
}

/*!
  \details No detailed description

  \param [in] nonempty No description.
  \return No description
  */
inline
auto ScalableCircularRingBuffer::dequeue(const bool nonempty) noexcept -> uint64b
{
  std::atomic<uint64b>& tail_count = tail();
  std::atomic<uint64b>& head_count = head();
  std::atomic<int64b>& th = threshold();
  const std::span indices = getIndexList();

  uint64b index = BaseRingBufferT::invalidIndex();
  uint64b headp = 0;
  uint64b tailp = 0;
  uint64b head_cycle = 0;
  uint64b head_index = 0;
  int attempt = 0;
  bool flag = nonempty || (0 <= th.load(std::memory_order::acquire));
  bool again = false;

  // Cautious dequeue
  if (nonempty && (distance(tail_count, head_count) == 0)) [[unlikely]] {
    flag = false;
    index = BaseRingBufferT::overflowIndex();
  }

  while (flag) {
    const auto n = cast<uint64b>(size());
    if (!again) {
      headp = head_count.fetch_add(1, std::memory_order::acq_rel);
      head_cycle = (headp << 1) | (2 * n - 1);
      head_index = permuteIndex(headp);
      attempt = 0;
    }
    again = false;
    uint64b entry = indices[head_index].load(std::memory_order::acquire);
    uint64b entry_cycle = 0;
    uint64b entry_new = 0;
    do {
      entry_cycle = entry | (2 * n - 1);
      flag = entry_cycle != head_cycle;
      if (!flag) {
        indices[head_index].fetch_or(n - 1, std::memory_order::acq_rel);
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
        tailp = ((attempt & amask) == 0) ? tail_count.load(std::memory_order::acquire) : tailp;
        again = (++attempt <= amax) && compare<std::greater_equal>(tailp, headp + 1);
        if (again)
          break;
        entry_new = head_cycle ^ ((~entry) & n);
      }
    } while (compare<std::less>(entry_cycle, head_cycle) &&
             !indices[head_index].compare_exchange_weak(entry,
                                                        entry_new,
                                                        std::memory_order::acq_rel,
                                                        std::memory_order::acquire));
    if (flag && !again && !nonempty) {
      tailp = tail_count.load(std::memory_order::acquire);
      flag = compare<std::greater>(tailp, headp + 1);
      if (flag) {
        flag = 0 < th.fetch_sub(1, std::memory_order::acq_rel);
        index = flag ? index : BaseRingBufferT::invalidIndex();
      }
      else {
        catchUp(tailp, headp + 1, tail_count, head_count);
        th.fetch_sub(1, std::memory_order::acq_rel);
        index = BaseRingBufferT::invalidIndex();
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
auto ScalableCircularRingBuffer::distance() const noexcept -> std::size_t
{
  return distance(tail(), head());
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] nonempty No description.
  \return No description
  */
inline
auto ScalableCircularRingBuffer::enqueue(const uint64b index, const bool nonempty) noexcept -> bool
{
  std::atomic<uint64b>& tail_count = tail();
  const std::atomic<uint64b>& head_count = head();
  std::atomic<int64b>& th = threshold();
  const std::span indices = getIndexList();

  uint64b tailp = 0;
  uint64b tail_cycle = 0;
  uint64b tail_index = 0;
  uint64b entry = 0;
  bool retry = false;
  while (true) {
    const auto n = cast<uint64b>(size());
    if (!retry) {
      tailp = tail_count.fetch_add(1, std::memory_order::acq_rel);
      tail_cycle = (tailp << 1) | (2 * n - 1);
      tail_index = permuteIndex(tailp);
      entry = indices[tail_index].load(std::memory_order::acquire);
    }
    retry = false;
    const uint64b entry_cycle = entry | (2 * n - 1);
    if (compare<std::less>(entry_cycle, tail_cycle) &&
        ((entry == entry_cycle) ||
         ((entry == (entry_cycle ^ n)) &&
          compare<std::less_equal>(head_count.load(std::memory_order::acquire), tailp)))) {
      const uint64b entry_index = index ^ cast<uint64b>(n - 1);
      retry = !indices[tail_index].compare_exchange_weak(entry,
                                                         tail_cycle ^ entry_index,
                                                         std::memory_order::acq_rel,
                                                         std::memory_order::acquire);
      if (retry)
        continue;
      const uint64b half = n >> 1;
      const int64b threshold3 = calcThreshold3(half);
      if (!nonempty && (th.load(std::memory_order::acquire) != threshold3))
        th.store(threshold3, std::memory_order::release);
      break;
    }
  }
  return true;
}

/*!
  \details No detailed description
  */
inline
void ScalableCircularRingBuffer::full() noexcept
{
  using AtomicT = std::remove_cvref_t<decltype(getIndex(0))>;

  const auto n = cast<uint64b>(size());
  const uint64b half = n >> 1;

  head().store(0, std::memory_order::release);
  threshold().store(calcThreshold3(half), std::memory_order::release);
  tail().store(half, std::memory_order::release);

  const std::span indices = getIndexList();
  for (uint64b i = 0; i < n; ++i) {
    const uint64b index = permuteIndex(i);
    constexpr std::size_t data_size = sizeof(AtomicT);
    const uint64b v = (i < half)
        ? BaseRingBufferT::permuteIndex<data_size>(n + i, half)
        : BaseRingBufferT::invalidIndex();
    indices[index].store(v, std::memory_order::release);
  }
}

/*!
  \details No detailed description

  \param [in] s \a s must be a power of 2.
  */
inline
void ScalableCircularRingBuffer::setSize(const std::size_t s)
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
auto ScalableCircularRingBuffer::size() const noexcept -> std::size_t
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
auto ScalableCircularRingBuffer::calcMemChunkSize(const std::size_t s) noexcept -> std::size_t
{
  std::size_t l = 0;
  using AtomicT = std::remove_cvref_t<decltype(ScalableCircularRingBuffer{nullptr}.getIndex(0))>;
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
auto ScalableCircularRingBuffer::calcThreshold3(const uint64b half) noexcept -> int64b
{
  const uint64b threshold = 3 * half - 1;
  return cast<int64b>(threshold);
}

/*!
  \details No detailed description

  \param [in] tailp No description.
  \param [in] headp No description.
  \param [in,out] tail_count No description.
  \param [in,out] head_count No description.
  */
inline
void ScalableCircularRingBuffer::catchUp(uint64b tailp,
                                         uint64b headp,
                                         std::atomic<uint64b>& tail_count,
                                         std::atomic<uint64b>& head_count) noexcept
{
  while (!tail_count.compare_exchange_weak(tailp,
                                           headp,
                                           std::memory_order::acq_rel,
                                           std::memory_order::acquire)) {
    tailp = tail_count.load(std::memory_order::acquire);
    headp = head_count.load(std::memory_order::acquire);
    if (compare<std::greater_equal>(tailp, headp))
      break;
  }
}

/*!
  \details No detailed description
  */
inline
void ScalableCircularRingBuffer::destroy() noexcept
{
  if (memory_.empty())
    return;

  // Indices
  if (0 < size()) {
    const std::span indices = getIndexList();
    std::destroy_n(indices.data(), indices.size());
  }
  // Tail
  {
    std::atomic<uint64b>* mem = std::addressof(tail());
    std::destroy_at(mem);
  }
  // Threshold
  {
    std::atomic<int64b>* mem = std::addressof(threshold());
    std::destroy_at(mem);
  }
  // Head
  {
    std::atomic<uint64b>* mem = std::addressof(head());
    std::destroy_at(mem);
  }
}

/*!
  \details No detailed description

  \param [in] tail_count No description.
  \param [in] head_count No description.
  \return No description
  */
inline
auto ScalableCircularRingBuffer::distance(const std::atomic<uint64b>& tail_count,
                                          const std::atomic<uint64b>& head_count) noexcept -> std::size_t
{
  const uint64b t = tail_count.load(std::memory_order::acquire);
  const uint64b h = head_count.load(std::memory_order::acquire);
  const std::size_t d = (h < t) ? cast<std::size_t>(t - h) : 0;
  return d;
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
auto ScalableCircularRingBuffer::compare(const uint64b lhs, const uint64b rhs) noexcept -> bool
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
auto ScalableCircularRingBuffer::getIndex(const std::size_t index) noexcept
    -> std::atomic<uint64b>&
{
  const std::span indices = getIndexList();
  return indices[index];
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ScalableCircularRingBuffer::getIndex(const std::size_t index) const noexcept
    -> const std::atomic<uint64b>&
{
  const std::span indices = getIndexList();
  return indices[index];
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ScalableCircularRingBuffer::getIndexList() noexcept -> std::span<std::atomic<uint64b>>
{
  MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kIndex);
  auto* ptr = reinterp<std::atomic<uint64b>*>(mem);
  return {ptr, size()};
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ScalableCircularRingBuffer::getIndexList() const noexcept -> std::span<const std::atomic<uint64b>>
{
  const MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kIndex);
  const auto* ptr = reinterp<const std::atomic<uint64b>*>(mem);
  return {ptr, size()};
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ScalableCircularRingBuffer::head() noexcept -> std::atomic<uint64b>&
{
  MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kHead);
  return *reinterp<std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ScalableCircularRingBuffer::head() const noexcept -> const std::atomic<uint64b>&
{
  const MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kHead);
  return *reinterp<const std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description
  */
inline
void ScalableCircularRingBuffer::initialize() noexcept
{
  if (memory_.size() < 3) {
    constexpr std::size_t num_of_attempts = 4;
    for (std::size_t i = 0; i < num_of_attempts; ++i) {
      try {
        memory_.resize(3);
        break;
      }
      catch ([[maybe_unused]] const std::exception& error) {
        ZISC_ASSERT(false, "ScalableCircularRingBuffer initialization failed.");
      }
    }
  }

  MemChunk* mem = memory_.data();

  // Head
  {
    using AtomicT = std::remove_cvref_t<decltype(head())>;
    MemChunk* ptr = mem + static_cast<std::size_t>(MemOffset::kHead);
    ::new (ptr) AtomicT{};
  }
  // Threshold
  {
    using AtomicT = std::remove_cvref_t<decltype(threshold())>;
    MemChunk* ptr = mem + static_cast<std::size_t>(MemOffset::kThreshold);
    ::new (ptr) AtomicT{};
  }
  // Tail
  {
    using AtomicT = std::remove_cvref_t<decltype(tail())>;
    MemChunk* ptr = mem + static_cast<std::size_t>(MemOffset::kTail);
    ::new (ptr) AtomicT{};
  }
  // Indices
  if (3 < memory_.size()) {
    using AtomicT = std::remove_cvref_t<decltype(getIndex(0))>;
    auto* indices = reinterp<AtomicT*>(mem + static_cast<std::size_t>(MemOffset::kIndex));
    for (std::size_t i = 0; i < size(); ++i) {
      AtomicT* ptr = indices + i;
      ::new (ptr) AtomicT{};
    }
  }
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto ScalableCircularRingBuffer::permuteIndex(const uint64b index) const noexcept -> uint64b
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
auto ScalableCircularRingBuffer::tail() noexcept -> std::atomic<uint64b>&
{
  MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kTail);
  return *reinterp<std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ScalableCircularRingBuffer::tail() const noexcept -> const std::atomic<uint64b>&
{
  const MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kTail);
  return *reinterp<const std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ScalableCircularRingBuffer::threshold() noexcept -> std::atomic<int64b>&
{
  MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kThreshold);
  return *reinterp<std::atomic<int64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ScalableCircularRingBuffer::threshold() const noexcept -> const std::atomic<int64b>&
{
  const MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kThreshold);
  return *reinterp<const std::atomic<int64b>*>(mem);
}

} // namespace zisc

#endif // ZISC_SCALABLE_CIRCULAR_RING_BUFFER_INL_HPP
