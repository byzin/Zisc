/*!
  \file lock_free_queue.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_QUEUE_HPP
#define ZISC_LOCK_FREE_QUEUE_HPP

// Standard C++ library
#include <atomic>
#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>
// Zisc
#include "ring_buffer.hpp"
#include "portable_ring_buffer.hpp"
#include "queue.hpp"
#include "scalable_circular_ring_buffer.hpp"
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  \tparam RingBufferClass No description.
  */
template <std::movable T, typename RingBufferClass>
class LockFreeQueue : public Queue<LockFreeQueue<T, RingBufferClass>, T>
{
 public:
  // Type aliases
  using BaseRingBufferT = RingBuffer<RingBufferClass>;
  using BaseQueueT = Queue<LockFreeQueue<T, RingBufferClass>, T>;
  using ValueT = typename BaseQueueT::ValueT;
  using ConstT = typename BaseQueueT::ConstT;
  using Reference = typename BaseQueueT::Reference;
  using RReference = typename BaseQueueT::RReference;
  using ConstReference = typename BaseQueueT::ConstReference;
  using Pointer = typename BaseQueueT::Pointer;
  using ConstPointer = typename BaseQueueT::ConstPointer;

  // Type aliases for STL
  using value_type = typename BaseQueueT::value_type;
  using size_type = typename BaseQueueT::size_type;
  using reference = typename BaseQueueT::reference;
  using const_reference = typename BaseQueueT::const_reference;


  //! Create a queue
  LockFreeQueue(pmr::memory_resource* mem_resource) noexcept;

  //! Create a queue
  LockFreeQueue(const size_type cap, pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  LockFreeQueue(LockFreeQueue&& other) noexcept;

  //! Destroy the queue
  ~LockFreeQueue() noexcept;


  //! Move a queue
  LockFreeQueue& operator=(LockFreeQueue&& other) noexcept;


  //! Return the maximum possible number of elements can be queued
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Return the direct access to the underlying array
  std::span<ConstT> data() const noexcept;

  //! Take the first element of the queue
  [[nodiscard]]
  std::optional<ValueT> dequeue() noexcept;

  //! Append the given element value to the end of the queue
  template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...>
  [[nodiscard]]
  std::optional<size_type> enqueue(Args&&... args);

  //! Return the value by the given index
  Reference get(const size_type index) noexcept;

  //! Return the value by the given index
  ConstReference get(const size_type index) const noexcept;

  //! Check if the queue is bounded
  static constexpr bool isBounded() noexcept;

  //! Check if the queue is concurrent
  static constexpr bool isConcurrent() noexcept;

  //! Return a pointer to the underlying memory resource
  pmr::memory_resource* resource() const noexcept;

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(size_type cap) noexcept;

  //! Return the number of elements
  size_type size() const noexcept;

 private:
  using StorageT = DataStorage<ValueT>;
  using ConstStorageT = std::add_const_t<StorageT>;
  using StorageRef = std::add_lvalue_reference_t<StorageT>;
  using ConstStorageRef = std::add_lvalue_reference_t<ConstStorageT>;
  using StoragePtr = std::add_pointer_t<StorageT>;
  using ConstStoragePtr = std::add_pointer_t<ConstStorageT>;
  using RingBufferT = RingBufferClass;


  //! Return the ring buffer for allocated elements
  BaseRingBufferT& allocatedElements() noexcept;

  //! Return the ring buffer for allocated elements
  const BaseRingBufferT& allocatedElements() const noexcept;

  //! Return the ring buffer for free elements 
  BaseRingBufferT& freeElements() noexcept;

  //! Return the ring buffer for free elements 
  const BaseRingBufferT& freeElements() const noexcept;

  //! Return the storage by the given index
  StorageRef getStorage(const size_type index) noexcept;

  //! Return the storage by the given index
  ConstStorageRef getStorage(const size_type index) const noexcept;


  RingBufferT free_elements_;
  RingBufferT allocated_elements_;
  pmr::vector<StorageT> elements_;
};

// Type aliases
template <std::movable T>
using ScalableCircularQueue = LockFreeQueue<T, ScalableCircularRingBuffer>;
template <std::movable T>
using PortableRingQueue = LockFreeQueue<T, PortableRingBuffer>;

} // namespace zisc

/*!
  \example concurrent_bounded_queue_example.cpp

  This is an example of how to use zisc::Queue.
  */

#include "lock_free_queue-inl.hpp"

#endif // ZISC_LOCK_FREE_QUEUE_HPP
