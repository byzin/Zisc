/*!
  \file lock_free_ring_buffer.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_RING_BUFFER_HPP
#define ZISC_LOCK_FREE_RING_BUFFER_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>
// Zisc
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief Represent a ring buffer in a queue

  No detailed description.
  */
class LockFreeRingBuffer
{
 public:
  //! Create a ring buffer
  LockFreeRingBuffer(pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  LockFreeRingBuffer(LockFreeRingBuffer&& other) noexcept;

  //! Destroy the ring buffer
  ~LockFreeRingBuffer() noexcept;


  //! Move a data
  LockFreeRingBuffer& operator=(LockFreeRingBuffer&& other) noexcept;


  //! Return the maximum possible capacity
  static constexpr std::size_t capacityMax() noexcept;

  //! Clear indices
  void clear() noexcept;

  //! Return the index represent an overflow
  static constexpr uint64b overflowIndex() noexcept;

  //! Take the first element of the queue
  std::size_t dequeue(const bool nonempty) noexcept;

  //! Return the distance between head and tail
  std::size_t distance() const noexcept;

  //! Append the given element value to the end of the queue
  bool enqueue(const std::size_t index, const bool nonempty) noexcept;

  //! Fill the buffer data
  void fill(const uint64b s, const uint64b e) noexcept;

  //! Full the buffer data
  void full() noexcept;

  //! Return the invalid index
  static constexpr uint64b invalidIndex() noexcept;

  //! Remap index in order to avoid false sharing
  static uint64b permuteIndex(const uint64b index, const uint64b n) noexcept;

  //! Set the number of elements of indices
  void setSize(const std::size_t s) noexcept;

  //! Return the number of elements of indices
  std::size_t size() const noexcept;

 private:
  //! Calculate the required memory length
  static std::size_t calcMemChunkSize(const std::size_t s) noexcept;

  //! Return the order of the size
  static uint64b calcOrder(const uint64b s) noexcept;

  //!
  static int64b calcThreshold3(const uint64b half) noexcept;

  //!
  void catchUp(uint64b tailp, uint64b headp) noexcept;

  //!
  template <template<typename> typename Func>
  static bool compare(const uint64b lhs, const uint64b rhs) noexcept;

  //! Destroy the ring buffer
  void destroy() noexcept;

  //! Return the underlying index counter
  std::atomic<uint64b>& getIndex(const std::size_t index) noexcept;

  //! Return the underlying index counter
  const std::atomic<uint64b>& getIndex(const std::size_t index) const noexcept;

  //! Return the underlying head point
  std::atomic<uint64b>& head() noexcept;

  //! Return the underlying head point
  const std::atomic<uint64b>& head() const noexcept;

  //! Initialize the ring buffer
  void initialize() noexcept;

  //! Remap index in order to avoid false sharing
  uint64b permuteIndex(const uint64b index) const noexcept;

  //! Return the underlying tail point
  std::atomic<uint64b>& tail() noexcept;

  //! Return the underlying tail point
  const std::atomic<uint64b>& tail() const noexcept;

  //! Return the underlying threshold
  std::atomic<int64b>& threshold() noexcept;

  //! Return the underlying threshold
  const std::atomic<int64b>& threshold() const noexcept;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
  using MemChunk = std::aligned_storage_t<kCacheLineSize, kCacheLineSize>;


  pmr::vector<MemChunk> memory_;
  std::size_t size_;
};

} // namespace zisc

#include "lock_free_ring_buffer-inl.hpp"

#endif // ZISC_LOCK_FREE_RING_BUFFER_HPP
