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
#include <memory_resource>
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
  explicit LockFreeQueue(std::pmr::memory_resource* mem_resource) noexcept;

  //! Create a queue
  LockFreeQueue(const size_type cap, std::pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  LockFreeQueue(LockFreeQueue&& other) noexcept;

  //! Destroy the queue
  ~LockFreeQueue() noexcept;


  //! Move a queue
  auto operator=(LockFreeQueue&& other) noexcept -> LockFreeQueue&;


  //! Return the maximum possible number of elements can be queued
  auto capacity() const noexcept -> size_type;

  //! Return the maximum possible capacity
  static constexpr auto capacityMax() noexcept -> size_type;

  //! Clear the contents
  void clear() noexcept;

  //! Return the direct access to the underlying array
  [[nodiscard]]
  auto data() const noexcept -> std::span<ConstT>;

  //! Take the first element of the queue
  [[nodiscard]]
  auto dequeue() noexcept -> std::optional<ValueT>;

  //! Append the given element value to the end of the queue
  template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...>
  [[nodiscard]]
  auto enqueue(Args&&... args) -> std::optional<size_type>;

  //! Return the value by the given index
  [[nodiscard]]
  auto get(const size_type index) noexcept -> Reference;

  //! Return the value by the given index
  [[nodiscard]]
  auto get(const size_type index) const noexcept -> ConstReference;

  //! Check if the queue is bounded
  static constexpr auto isBounded() noexcept -> bool;

  //! Check if the queue is concurrent
  static constexpr auto isConcurrent() noexcept -> bool;

  //! Return a pointer to the underlying memory resource
  [[nodiscard]]
  auto resource() const noexcept -> std::pmr::memory_resource*;

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(size_type cap);

  //! Return the number of elements
  auto size() const noexcept -> size_type;

 private:
  using StorageT = DataStorage<ValueT>;
  using ConstStorageT = std::add_const_t<StorageT>;
  using StorageRef = std::add_lvalue_reference_t<StorageT>;
  using ConstStorageRef = std::add_lvalue_reference_t<ConstStorageT>;
  using StoragePtr = std::add_pointer_t<StorageT>;
  using ConstStoragePtr = std::add_pointer_t<ConstStorageT>;
  using RingBufferT = RingBufferClass;


  //! Return the ring buffer for allocated elements
  [[nodiscard]]
  auto allocatedElements() noexcept -> BaseRingBufferT&;

  //! Return the ring buffer for allocated elements
  [[nodiscard]]
  auto allocatedElements() const noexcept -> const BaseRingBufferT&;

  //! Return the ring buffer for free elements 
  [[nodiscard]]
  auto freeElements() noexcept -> BaseRingBufferT&;

  //! Return the ring buffer for free elements 
  [[nodiscard]]
  auto freeElements() const noexcept -> const BaseRingBufferT&;

  //! Return the storage by the given index
  [[nodiscard]]
  auto getStorage(const size_type index) noexcept -> StorageRef;

  //! Return the storage by the given index
  [[nodiscard]]
  auto getStorage(const size_type index) const noexcept -> ConstStorageRef;


  RingBufferT free_elements_;
  RingBufferT allocated_elements_;
  std::pmr::vector<StorageT> elements_;
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
