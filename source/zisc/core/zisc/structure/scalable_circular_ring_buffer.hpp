/*!
  \file scalable_circular_ring_buffer.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SCALABLE_CIRCULAR_RING_BUFFER_HPP
#define ZISC_SCALABLE_CIRCULAR_RING_BUFFER_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <memory_resource>
#include <span>
#include <type_traits>
#include <vector>
// Zisc
#include "ring_buffer.hpp"
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief The implementation of Scalable Circular Lock-free Queue (SCQ)

  For more detail, please see the following paper: 
  <a href="https://arxiv.org/abs/1908.04511">A Scalable, Portable, and Memory-Efficient Lock-Free FIFO Queue</a>. <br>
  Assume that the number of threads is less than or equal size().
  */
class ScalableCircularRingBuffer : public RingBuffer<ScalableCircularRingBuffer>
{
 public:
  // Type aliases
  using BaseRingBufferT = RingBuffer<ScalableCircularRingBuffer>;


  //! Create a ring buffer
  explicit ScalableCircularRingBuffer(std::pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  ScalableCircularRingBuffer(ScalableCircularRingBuffer&& other) noexcept = default;

  //! Destroy the ring buffer
  ~ScalableCircularRingBuffer() noexcept;


  //! Move a data
  auto operator=(ScalableCircularRingBuffer&& other) noexcept -> ScalableCircularRingBuffer& = default;


  //! Clear indices
  void clear() noexcept;

  //! Take the first element of the queue
  [[nodiscard]]
  auto dequeue(const bool nonempty) noexcept -> uint64b;

  //! Return the distance between head and tail
  [[nodiscard]]
  auto distance() const noexcept -> std::size_t;

  //! Append the given element value to the end of the queue
  [[nodiscard]]
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
    kThreshold,
    kTail,
    kIndex
  };

  //! Calculate the required memory length
  static auto calcMemChunkSize(const std::size_t s) noexcept -> std::size_t;

  //!
  static auto calcThreshold3(const uint64b half) noexcept -> int64b;

  //!
  static void catchUp(uint64b tailp,
                      uint64b headp,
                      std::atomic<uint64b>& tail_count,
                      std::atomic<uint64b>& head_count) noexcept;

  //!
  template <template<typename> typename Func>
  static auto compare(const uint64b lhs, const uint64b rhs) noexcept -> bool;

  //! Destroy the ring buffer
  void destroy() noexcept;

  //! Return the distance between head and tail
  static auto distance(const std::atomic<uint64b>& tail_count,
                       const std::atomic<uint64b>& head_count) noexcept -> std::size_t;

  //! Return the underlying index counter
  [[nodiscard]]
  auto getIndex(const std::size_t index) noexcept -> std::atomic<uint64b>&;

  //! Return the underlying index counter
  [[nodiscard]]
  auto getIndex(const std::size_t index) const noexcept -> const std::atomic<uint64b>&;

  //! Return the underlying index list
  [[nodiscard]]
  auto getIndexList() noexcept -> std::span<std::atomic<uint64b>>;

  //! Return the underlying index list
  [[nodiscard]]
  auto getIndexList() const noexcept -> std::span<const std::atomic<uint64b>>;

  //! Return the underlying head point
  [[nodiscard]]
  auto head() noexcept -> std::atomic<uint64b>&;

  //! Return the underlying head point
  [[nodiscard]]
  auto head() const noexcept -> const std::atomic<uint64b>&;

  //! Initialize the ring buffer
  void initialize() noexcept;

  //! Remap index in order to avoid false sharing
  [[nodiscard]]
  auto permuteIndex(const uint64b index) const noexcept -> uint64b;

  //! Return the underlying tail point
  [[nodiscard]]
  auto tail() noexcept -> std::atomic<uint64b>&;

  //! Return the underlying tail point
  [[nodiscard]]
  auto tail() const noexcept -> const std::atomic<uint64b>&;

  //! Return the underlying threshold
  [[nodiscard]]
  auto threshold() noexcept -> std::atomic<int64b>&;

  //! Return the underlying threshold
  [[nodiscard]]
  auto threshold() const noexcept -> const std::atomic<int64b>&;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
  using MemChunk = std::aligned_storage_t<kCacheLineSize, kCacheLineSize>;


  std::pmr::vector<MemChunk> memory_;
  std::size_t size_ = 0;
};

} // namespace zisc

#include "scalable_circular_ring_buffer-inl.hpp"

#endif // ZISC_SCALABLE_CIRCULAR_RING_BUFFER_HPP
