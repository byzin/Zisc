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
  static constexpr std::size_t capacityMax() noexcept;

  //! Clear indices
  void clear() noexcept;

  //! Return the index represent an overflow
  static constexpr uint64b overflowIndex() noexcept;

  //! Take the first element of the queue
  uint64b dequeue(const bool nonempty) noexcept;

  //! Return the distance between head and tail
  std::size_t distance() const noexcept;

  //! Append the given element value to the end of the queue
  bool enqueue(const uint64b index, const bool nonempty) noexcept;

  //! Full the buffer data
  void full() noexcept;

  //! Return the uint mask for index
  static constexpr uint64b indexMask() noexcept;

  //! Return the invalid index
  static constexpr uint64b invalidIndex() noexcept;

  //! Remap index in order to avoid false sharing
  template <std::size_t kDataSize>
  static uint64b permuteIndex(const uint64b index, const uint64b n) noexcept;

  //! Set the number of elements of indices
  void setSize(const std::size_t s) noexcept;

  //! Return the number of elements of indices
  std::size_t size() const noexcept;

 protected:
  // Type aliases
  using RingBufferT = std::remove_volatile_t<RingBufferClass>;
  using ConstRingBufferT = std::add_const_t<RingBufferT>;
  using RingBufferPtr = std::add_pointer_t<RingBufferT>;
  using ConstRingBufferPtr = std::add_pointer_t<ConstRingBufferT>;
  using RingBufferReference = std::add_lvalue_reference_t<RingBufferT>;
  using ConstRingBufferReference = std::add_lvalue_reference_t<ConstRingBufferT>;


  //! Initialize the ring buffer
  RingBuffer() noexcept;

  //! Copy a ring buffer
  RingBuffer(const RingBuffer& other) noexcept;


  //! Copy a ring buffer
  RingBuffer& operator=(const RingBuffer& other) noexcept;


  //! Return the order of the size
  static uint64b calcOrder(const uint64b s) noexcept;

  //! Return the reference to the ring buffer class
  RingBufferReference ref() noexcept;

  //! Return the reference to the ring buffer class
  ConstRingBufferReference ref() const noexcept;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
};

} // namespace zisc

#include "ring_buffer-inl.hpp"

#endif // ZISC_RING_BUFFER_HPP
