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
  //! Calculate the required memory length
  static std::size_t calcMemChunkSize(const std::size_t s) noexcept;

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

#include "portable_ring_buffer-inl.hpp"

#endif // ZISC_PORTABLE_RING_BUFFER_HPP
