/*!
  \file ring_buffer.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_RING_BUFFER_HPP
#define ZISC_RING_BUFFER_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Represent base class of ring buffer in a queue

  No detailed description.
  */
template <typename RingBufferClass>
class RingBuffer
{
 public:
  //! Return the maximum possible capacity
  static constexpr auto capacityMax() noexcept -> std::size_t;

  //! Clear indices
  void clear() noexcept;

  //! Return the index represent an overflow
  static constexpr auto overflowIndex() noexcept -> uint64b;

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

  //! Return the uint mask for index
  static constexpr auto indexMask() noexcept -> uint64b;

  //! Return the invalid index
  static constexpr auto invalidIndex() noexcept -> uint64b;

  //! Remap index in order to avoid false sharing
  template <std::size_t kDataSize>
  static auto permuteIndex(const uint64b index, const uint64b n) noexcept -> uint64b;

  //! Set the number of elements of indices
  void setSize(const std::size_t s);

  //! Return the number of elements of indices
  [[nodiscard]]
  auto size() const noexcept -> std::size_t;

 protected:
  // Type aliases
  using RingBufferT = std::remove_volatile_t<RingBufferClass>;
  using ConstRingBufferT = std::add_const_t<RingBufferT>;
  using RingBufferPtr = std::add_pointer_t<RingBufferT>;
  using ConstRingBufferPtr = std::add_pointer_t<ConstRingBufferT>;
  using RingBufferReference = std::add_lvalue_reference_t<RingBufferT>;
  using ConstRingBufferReference = std::add_lvalue_reference_t<ConstRingBufferT>;


  //! Initialize the ring buffer
  RingBuffer() noexcept = default;

  //! Copy a ring buffer
  RingBuffer(const RingBuffer& other) noexcept;


  //! Copy a ring buffer
  auto operator=(const RingBuffer& other) noexcept -> RingBuffer&;


  //! Return the order of the size
  static auto calcOrder(const uint64b s) noexcept -> uint64b;

  //! Return the reference to the ring buffer class
  [[nodiscard]]
  auto ref() noexcept -> RingBufferReference;

  //! Return the reference to the ring buffer class
  [[nodiscard]]
  auto ref() const noexcept -> ConstRingBufferReference;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
};

} // namespace zisc

#include "ring_buffer-inl.hpp"

#endif // ZISC_RING_BUFFER_HPP
