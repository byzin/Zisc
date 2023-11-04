/*!
  \file portable_ring_buffer.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PORTABLE_RING_BUFFER_HPP
#define ZISC_PORTABLE_RING_BUFFER_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <memory>
#include <span>
#include <type_traits>
#include <vector>
// Zisc
#include "ring_buffer.hpp"
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief The implementation of Lock-free Portable Ring Queue (LPRQ)

  For more detail, please see the following paper: 
  <a href="https://dl.acm.org/doi/10.1145/3572848.3577485">The State-of-the-Art LCRQ Concurrent Queue Algorithm Does NOT Require CAS2</a>. <br>
  Assume that the number of threads is less than or equal size().
  */
class PortableRingBuffer : public RingBuffer<PortableRingBuffer>
{
 public:
  // Type aliases
  using BaseRingBufferT = RingBuffer<PortableRingBuffer>;


  //! Create a ring buffer
  explicit PortableRingBuffer(pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  PortableRingBuffer(PortableRingBuffer&& other) noexcept = default;

  //! Destroy the ring buffer
  ~PortableRingBuffer() noexcept;


  //! Move a data
  auto operator=(PortableRingBuffer&& other) noexcept -> PortableRingBuffer& = default;


  //! Clear indices
  void clear() noexcept;

  //! Take the first element of the queue
  auto dequeue(const bool nonempty) noexcept -> uint64b;

  //! Return the distance between head and tail
  [[nodiscard]]
  auto distance() const noexcept -> std::size_t;

  //! Append the given element value to the end of the queue
  auto enqueue(const uint64b index, const bool nonempty) noexcept -> bool;

  //! Full the buffer data
  void full() noexcept;

  //! Set the number of elements of indices
  void setSize(const std::size_t s);

  //! Return the number of elements of indices
  [[nodiscard]]
  auto size() const noexcept -> std::size_t;

 private:
  //! Represent the memory offset
  enum class MemOffset : std::size_t
  {
    kHead = 0,
    kTail,
    kIndex
  };

  //!
  struct Cell
  {
    std::atomic<uint64b> value_;
    std::atomic<uint64b> index_;
  };


  //! Calculate the required memory length
  static auto calcMemChunkSize(const std::size_t s) noexcept -> std::size_t;

  //! Destroy the ring buffer
  void destroy() noexcept;

  //! Return the distance between head and tail
  static auto distance(const std::atomic<uint64b>& tail_count,
                       const std::atomic<uint64b>& head_count) noexcept -> std::size_t;

  //!
  static void fixState(std::atomic<uint64b>& tail_count,
                       std::atomic<uint64b>& head_count) noexcept;

  //! Return the underlying cell
  [[nodiscard]]
  auto getCell(const std::size_t index) noexcept -> Cell&;

  //! Return the underlying cell
  [[nodiscard]]
  auto getCell(const std::size_t index) const noexcept -> const Cell&;

  //! Return the underlying cell list
  [[nodiscard]]
  auto getCellList() noexcept -> std::span<Cell>;

  //! Return the underlying cell list
  [[nodiscard]]
  auto getCellList() const noexcept -> std::span<const Cell>;

  //! Return the node index
  static auto getNodeIndex(const uint64b index) noexcept -> uint64b;

  //! Return a thread local bottom
  static auto getThreadLocalBottom() noexcept -> uint64b;

  //! Return the index with unsafe flag
  static auto getUnsafeFlag(const uint64b index) noexcept -> uint64b;

  //! Return the underlying head point
  [[nodiscard]]
  auto head() noexcept -> std::atomic<uint64b>&;

  //! Return the underlying head point
  [[nodiscard]]
  auto head() const noexcept -> const std::atomic<uint64b>&;

  //! Initialize the ring buffer
  void initialize() noexcept;

  //! Check if the value is bottom
  static auto isBottom(const uint64b value) noexcept -> bool;

  //! CHeck if the given index has unsafe flag
  static auto isUnsafe(const uint64b index) noexcept -> bool;

  //! Remap index in order to avoid false sharing
  [[nodiscard]]
  auto permuteIndex(const uint64b index) const noexcept -> uint64b;

  //! Return the underlying tail point
  [[nodiscard]]
  auto tail() noexcept -> std::atomic<uint64b>&;

  //! Return the underlying tail point
  [[nodiscard]]
  auto tail() const noexcept -> const std::atomic<uint64b>&;

  //! Return the unsafe mask
  static constexpr auto unsafeMask() noexcept -> uint64b;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
  using MemChunk = std::aligned_storage_t<kCacheLineSize, kCacheLineSize>;


  pmr::vector<MemChunk> memory_;
  std::size_t size_ = 0;
};

} // namespace zisc

#include "portable_ring_buffer-inl.hpp"

#endif // ZISC_PORTABLE_RING_BUFFER_HPP
