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
  PortableRingBuffer(pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  PortableRingBuffer(PortableRingBuffer&& other) noexcept;

  //! Destroy the ring buffer
  ~PortableRingBuffer() noexcept;


  //! Move a data
  PortableRingBuffer& operator=(PortableRingBuffer&& other) noexcept;


  //! Clear indices
  void clear() noexcept;

  //! Take the first element of the queue
  uint64b dequeue(const bool nonempty) noexcept;

  //! Return the distance between head and tail
  std::size_t distance() const noexcept;

  //! Append the given element value to the end of the queue
  bool enqueue(const uint64b index, const bool nonempty) noexcept;

  //! Full the buffer data
  void full() noexcept;

  //! Set the number of elements of indices
  void setSize(const std::size_t s) noexcept;

  //! Return the number of elements of indices
  std::size_t size() const noexcept;

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
  static std::size_t calcMemChunkSize(const std::size_t s) noexcept;

  //! Destroy the ring buffer
  void destroy() noexcept;

  //! Return the distance between head and tail
  static std::size_t distance(const std::atomic<uint64b>& tail_count,
                              const std::atomic<uint64b>& head_count) noexcept;

  //!
  static void fixState(std::atomic<uint64b>& tail_count,
                       std::atomic<uint64b>& head_count) noexcept;

  //! Return the underlying cell
  Cell& getCell(const std::size_t index) noexcept;

  //! Return the underlying cell
  const Cell& getCell(const std::size_t index) const noexcept;

  //! Return the underlying cell list
  std::span<Cell> getCellList() noexcept;

  //! Return the underlying cell list
  std::span<const Cell> getCellList() const noexcept;

  //! Return the node index
  static uint64b getNodeIndex(const uint64b index) noexcept;

  //! Return a thread local bottom
  static uint64b getThreadLocalBottom() noexcept;

  //! Return the index with unsafe flag
  static uint64b getUnsafeFlag(const uint64b index) noexcept;

  //! Return the underlying head point
  std::atomic<uint64b>& head() noexcept;

  //! Return the underlying head point
  const std::atomic<uint64b>& head() const noexcept;

  //! Initialize the ring buffer
  void initialize() noexcept;

  //! Check if the value is bottom
  static bool isBottom(const uint64b value) noexcept;

  //! CHeck if the given index has unsafe flag
  static bool isUnsafe(const uint64b index) noexcept;

  //! Remap index in order to avoid false sharing
  uint64b permuteIndex(const uint64b index) const noexcept;

  //! Return the underlying tail point
  std::atomic<uint64b>& tail() noexcept;

  //! Return the underlying tail point
  const std::atomic<uint64b>& tail() const noexcept;

  //! Return the unsafe mask
  static constexpr uint64b unsafeMask() noexcept;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
  using MemChunk = std::aligned_storage_t<kCacheLineSize, kCacheLineSize>;


  pmr::vector<MemChunk> memory_;
  std::size_t size_;
};

} // namespace zisc

#include "portable_ring_buffer-inl.hpp"

#endif // ZISC_PORTABLE_RING_BUFFER_HPP
