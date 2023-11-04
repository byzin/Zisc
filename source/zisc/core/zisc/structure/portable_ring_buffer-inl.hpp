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
#include <limits>
#include <memory>
#include <span>
#include <thread>
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
    memory_{typename decltype(memory_)::allocator_type{mem_resource}}
{
  initialize();
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
  */
inline
void PortableRingBuffer::clear() noexcept
{
  head().store(0, std::memory_order::release);
  tail().store(0, std::memory_order::release);

  const std::span indices = getCellList();
  for (std::size_t i = 0; i < indices.size(); ++i) {
    const std::size_t index = permuteIndex(i);
    Cell& cell = indices[index];
    cell.index_.store(static_cast<uint64b>(i), std::memory_order::release);
    cell.value_.store(BaseRingBufferT::invalidIndex(), std::memory_order::release);
  }
}

/*!
  \details No detailed description

  \param [in] nonempty No description.
  \return No description
  */
inline
auto PortableRingBuffer::dequeue(const bool nonempty) noexcept -> uint64b
{
  std::atomic<uint64b>& tail_count = tail();
  std::atomic<uint64b>& head_count = head();
  const std::span indices = getCellList();

  bool flag = true;
  uint64b index = BaseRingBufferT::invalidIndex();

  // Cautious dequeue
  if (nonempty && (distance(tail_count, head_count) == 0)) [[unlikely]] {
    flag = false;
    index = BaseRingBufferT::overflowIndex();
  }

  while (flag) {
    const auto n = cast<uint64b>(size());
    const uint64b head_ticket = head_count.fetch_add(1, std::memory_order::acq_rel);
    const uint64b head_index = permuteIndex(head_ticket % n);
    Cell& cell = indices[head_index];

    int attempt = 0;
    uint64b tt = 0;

    while (true) {
      uint64b cell_index = cell.index_.load(std::memory_order::acquire);
      index = cell.value_.load(std::memory_order::acquire);
      const bool is_unsafe = isUnsafe(cell_index);
      const uint64b node_index = getNodeIndex(cell_index);

      constexpr auto msuccess = std::memory_order::acq_rel;
      constexpr auto mfailure = std::memory_order::acquire;

      if ((head_ticket + n) < node_index)
        break;

      if ((index != BaseRingBufferT::invalidIndex()) && !isBottom(index)) {
        if ((head_ticket + n) == node_index) {
          cell.value_.store(BaseRingBufferT::invalidIndex(), std::memory_order::release);
          flag = false;
          break;
        }
        else {
          if (is_unsafe) {
            if (cell.index_.load(std::memory_order::acquire) == cell_index)
              break;
          }
          else {
            if (cell.index_.compare_exchange_strong(cell_index, getUnsafeFlag(node_index), msuccess, mfailure))
              break;;
          }
        }
      }
      else {
        constexpr int update_interval = 1 << 8;
        constexpr int amax = 4 * 1024;
        if ((attempt % update_interval) == 0)
          tt = tail_count.load(std::memory_order::acquire);
        const uint64b t = getNodeIndex(tt);
        if (is_unsafe || (t < (head_ticket + 1)) || (amax < attempt)) {
          if (isBottom(index) && !cell.value_.compare_exchange_strong(index, BaseRingBufferT::invalidIndex(), msuccess, mfailure))
            continue;
          if (cell.index_.compare_exchange_strong(cell_index, getUnsafeFlag(head_ticket + n), msuccess, mfailure))
            break;
        }
        ++attempt;
      }
    }

    if (flag && (getNodeIndex(tail_count.load(std::memory_order::acquire)) <= (head_ticket + 1))) {
      fixState(tail_count, head_count);
      index = BaseRingBufferT::invalidIndex();
      flag = false;
    }
  }

  return index;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto PortableRingBuffer::distance() const noexcept -> std::size_t
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
auto PortableRingBuffer::enqueue(const uint64b index, [[maybe_unused]] const bool nonempty) noexcept -> bool
{
  thread_local const uint64b thread_local_bottom = getThreadLocalBottom();

  std::atomic<uint64b>& tail_count = tail();
  const std::atomic<uint64b>& head_count = head();
  const std::span indices = getCellList();

  while (true) {
    const auto n = cast<uint64b>(size());
    const uint64b tail_ticket = tail_count.fetch_add(1, std::memory_order::acq_rel);
    const uint64b tail_index = permuteIndex(tail_ticket % n);
    Cell& cell = indices[tail_index];
    uint64b cell_index = cell.index_.load(std::memory_order::acquire);
    uint64b cell_value = cell.value_.load(std::memory_order::acquire);
    if ((cell_value == BaseRingBufferT::invalidIndex()) && 
        (getNodeIndex(cell_index) <= tail_ticket) &&
        (!isUnsafe(cell_index) || head_count.load(std::memory_order::acquire) <= tail_ticket)) {
      constexpr auto msuccess = std::memory_order::acq_rel;
      constexpr auto mfailure = std::memory_order::acquire;
      uint64b bottom = thread_local_bottom;
      if (cell.value_.compare_exchange_strong(cell_value, bottom, msuccess, mfailure)) {
        if (cell.index_.compare_exchange_strong(cell_index, tail_ticket + n, msuccess, mfailure)) {
          if (cell.value_.compare_exchange_strong(bottom, index, msuccess, mfailure))
            break;
        }
        else {
          cell.value_.compare_exchange_strong(bottom, BaseRingBufferT::invalidIndex(), msuccess, mfailure);
        }
      }
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
  const std::span indices = getCellList();
  const auto n = static_cast<uint64b>(indices.size());

  head().store(0, std::memory_order::release);
  tail().store(n, std::memory_order::release);

  for (std::size_t i = 0; i < indices.size(); ++i) {
    const std::size_t index = permuteIndex(i);
    Cell& cell = indices[index];
    const auto v = static_cast<uint64b>(i);
    cell.index_.store(v + n, std::memory_order::release);
    cell.value_.store(v, std::memory_order::release);
  }
}

/*!
  \details No detailed description

  \param [in] s \a s must be a power of 2.
  */
inline
void PortableRingBuffer::setSize(const std::size_t s)
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
auto PortableRingBuffer::size() const noexcept -> std::size_t
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
auto PortableRingBuffer::calcMemChunkSize(const std::size_t s) noexcept -> std::size_t
{
  std::size_t l = 0;
  // Offset size
  {
    using AtomicT = std::remove_cvref_t<decltype(PortableRingBuffer{nullptr}.head())>;
    static_assert(sizeof(AtomicT) <= sizeof(MemChunk));
    static_assert(sizeof(MemChunk) % sizeof(AtomicT) == 0);
    static_assert(alignof(AtomicT) <= alignof(MemChunk));
    l += 2;
  }
  // Indices size
  {
    static_assert(sizeof(Cell) <= sizeof(MemChunk));
    static_assert(sizeof(MemChunk) % sizeof(Cell) == 0);
    static_assert(alignof(Cell) <= alignof(MemChunk));
    const std::size_t total = s * sizeof(Cell);
    constexpr std::size_t chunk_size = sizeof(MemChunk);
    l += (total + (chunk_size - 1)) / chunk_size;
  }
  return l;
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
    const std::span indices = getCellList();
    std::destroy_n(indices.data(), indices.size());
  }
  // Tail
  {
    std::atomic<uint64b>* mem = std::addressof(tail());
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
auto PortableRingBuffer::distance(const std::atomic<uint64b>& tail_count,
                                  const std::atomic<uint64b>& head_count) noexcept -> std::size_t
{
  const uint64b t = tail_count.load(std::memory_order::acquire);
  const uint64b h = head_count.load(std::memory_order::acquire);
  const std::size_t d = (h < t) ? cast<std::size_t>(t - h) : 0;
  return d;
}

/*!
  \details No detailed description

  \param [in] tail_count No description.
  \param [in] head_count No description.
  */
inline
void PortableRingBuffer::fixState(std::atomic<uint64b>& tail_count,
                                  std::atomic<uint64b>& head_count) noexcept
{
  while (true) {
    const uint64b t = tail_count.load(std::memory_order::acquire);
    const uint64b h = head_count.load(std::memory_order::acquire);
    if (tail_count.load(std::memory_order::acquire) != t)
      continue;
    if (t < h) { // h would be less than t if queue is closed
      uint64b tmp = t;
      if (tail_count.compare_exchange_strong(tmp, h, std::memory_order::acq_rel, std::memory_order::acquire))
        break;
      continue;
    }
    break;
  }
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto PortableRingBuffer::getCell(const std::size_t index) noexcept -> Cell&
{
  const std::span indices = getCellList();
  return indices[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto PortableRingBuffer::getCell(const std::size_t index) const noexcept -> const Cell&
{
  const std::span indices = getCellList();
  return indices[index];
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto PortableRingBuffer::getCellList() noexcept -> std::span<Cell>
{
  MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kIndex);
  auto* ptr = reinterp<Cell*>(mem);
  return {ptr, size()};
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto PortableRingBuffer::getCellList() const noexcept -> std::span<const Cell>
{
  const MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kIndex);
  const auto* ptr = reinterp<const Cell*>(mem);
  return {ptr, size()};
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto PortableRingBuffer::getNodeIndex(const uint64b index) noexcept -> uint64b
{
  const uint64b mask = ~unsafeMask();
  return static_cast<uint64b>(index & mask);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto PortableRingBuffer::getThreadLocalBottom() noexcept -> uint64b
{
  const std::thread::id thread_id = std::this_thread::get_id();
  const std::size_t id = std::hash<std::thread::id>{}(thread_id);
  const uint64b bottom = static_cast<uint64b>(id) | unsafeMask();
  return bottom;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto PortableRingBuffer::getUnsafeFlag(const uint64b index) noexcept -> uint64b
{
  const uint64b i = BaseRingBufferT::indexMask() & index;
  const uint64b result = unsafeMask() | i;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto PortableRingBuffer::head() noexcept -> std::atomic<uint64b>&
{
  MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kHead);
  return *reinterp<std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto PortableRingBuffer::head() const noexcept -> const std::atomic<uint64b>&
{
  const MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kHead);
  return *reinterp<const std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description
  */
inline
void PortableRingBuffer::initialize() noexcept
{
  if (memory_.size() < 2) {
    constexpr std::size_t num_of_attempts = 4;
    for (std::size_t i = 0; i < num_of_attempts; ++i) {
      try {
        memory_.resize(2);
        break;
      }
      catch ([[maybe_unused]] const std::exception& error) {
        ZISC_ASSERT(false, "PortableRingBuffer initialization failed.");
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
  // Tail
  {
    using AtomicT = std::remove_cvref_t<decltype(tail())>;
    MemChunk* ptr = mem + static_cast<std::size_t>(MemOffset::kTail);
    ::new (ptr) AtomicT{};
  }

  // Indices
  if (2 < memory_.size()) {
    auto* indices = reinterp<Cell*>(mem + static_cast<std::size_t>(MemOffset::kIndex));
    for (std::size_t i = 0; i < size(); ++i) {
      Cell* ptr = indices + i;
      ::new (ptr) Cell{};
    }
  }
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
inline
auto PortableRingBuffer::isBottom(const uint64b value) noexcept -> bool
{
  const bool result = (value != BaseRingBufferT::invalidIndex()) && isUnsafe(value);
  return result;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto PortableRingBuffer::isUnsafe(const uint64b index) noexcept -> bool
{
  const bool result = (unsafeMask() & index) == unsafeMask();
  return result;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto PortableRingBuffer::permuteIndex(const uint64b index) const noexcept -> uint64b
{
  constexpr std::size_t data_size = sizeof(Cell);
  const uint64b i = BaseRingBufferT::permuteIndex<data_size>(index, size());
  return i;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto PortableRingBuffer::tail() noexcept -> std::atomic<uint64b>&
{
  MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kTail);
  return *reinterp<std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto PortableRingBuffer::tail() const noexcept -> const std::atomic<uint64b>&
{
  const MemChunk* mem = memory_.data() + static_cast<std::size_t>(MemOffset::kTail);
  return *reinterp<const std::atomic<uint64b>*>(mem);
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto PortableRingBuffer::unsafeMask() noexcept -> uint64b
{
  constexpr uint64b m = static_cast<uint64b>(1) << (std::numeric_limits<uint64b>::digits - 1);
  return m;
}

} // namespace zisc

#endif // ZISC_PORTABLE_RING_BUFFER_INL_HPP
