/*!
  \file scalable_circular_queue.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SCALABLE_CIRCULAR_QUEUE_HPP
#define ZISC_SCALABLE_CIRCULAR_QUEUE_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>
// Zisc
#include "bounded_queue.hpp"
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_GCC || Z_CLANG

/*!
  \brief Scalable Circular Queue (SCQ)

  For more detail, please see the following paper: 
  <a href="https://arxiv.org/abs/1908.04511">A Scalable, Portable, and Memory-Efficient Lock-Free FIFO Queue</a>. <br>
  Assume that the number of threads is less than or equal size().

  \tparam T No description.
  */
template <Queueable T>
class ScalableCircularQueue : public BoundedQueue<ScalableCircularQueue<T>, T>
{
 public:
  // Types
  using BaseQueueType = BoundedQueue<ScalableCircularQueue<T>, T>;
  using Type = typename BaseQueueType::Type;
  using ConstType = typename BaseQueueType::ConstType;
  using Reference = typename BaseQueueType::Reference;
  using RReference = typename BaseQueueType::RReference;
  using ConstReference = typename BaseQueueType::ConstReference;
  using Pointer = typename BaseQueueType::Pointer;
  using ConstPointer = typename BaseQueueType::ConstPointer;

  // Type aliases for STL
  using container_type = pmr::vector<Type>;
  using value_type = typename BaseQueueType::value_type;
  using size_type = typename BaseQueueType::size_type;
  using reference = typename BaseQueueType::reference;
  using const_reference = typename BaseQueueType::const_reference;


  //! Create a queue
  ScalableCircularQueue(pmr::memory_resource* mem_resource) noexcept;

  //! Create a queue
  ScalableCircularQueue(const size_type cap,
                        pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  ScalableCircularQueue(ScalableCircularQueue&& other) noexcept;


  //! Move a queue
  ScalableCircularQueue& operator=(ScalableCircularQueue&& other) noexcept;


  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Return the direct access to the underlying array
  const container_type& data() const noexcept;

  //! Take the first element of the queue
  std::tuple<bool, Type> dequeue() noexcept;

  //! Append the given element value to the end of the queue
  bool enqueue(ConstReference value);

  //! Append the given element value to the end of the queue
  bool enqueue(RReference value);

  //! Check whether the queue is empty
  bool isEmpty() const noexcept;

  //! Return a pointer to the underlying memory resource
  pmr::memory_resource* resource() const noexcept;

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements
  size_type size() const noexcept;

 private:
  //! Represent a ring buffer of a queue
  class RingBuffer
  {
   public:
    //! Create a ring buffer
    RingBuffer(pmr::memory_resource* mem_resource) noexcept;

    //! Move a data
    RingBuffer(RingBuffer&& other) noexcept;


    //! Move a data
    RingBuffer& operator=(RingBuffer&& other) noexcept;


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

    //! Fill indices
    void fill(const uint64b s, const uint64b e) noexcept;

    //! Return the invalid index
    static constexpr uint64b invalidIndex() noexcept;

    //! Return the order of a ring buffer size
    uint64b order() const noexcept;

    //! Set the number of elements of indices
    void setSize(const std::size_t s) noexcept;

    //! Return the number of elements of indices
    std::size_t size() const noexcept;

   private:
    //!
    void catchUp(uint64b tail, uint64b head) noexcept;

    //
    int64b diff(const uint64b lhs, const uint64b rhs) const noexcept;

    //!
    int64b getThreshold3(const uint64b half) const noexcept;

    //! Remap index in order to avoid false sharing
    uint64b permuteIndex(const uint64b index) const noexcept;


    static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();


    alignas(kCacheLineSize) std::atomic<uint64b> head_{0};
    alignas(kCacheLineSize) std::atomic<int64b> threshold_{0};
    alignas(kCacheLineSize) std::atomic<uint64b> tail_{0};
    pmr::vector<std::atomic<uint64b>> indices_;
    uint64b order_;
  };


  RingBuffer free_elements_;
  RingBuffer allocated_elements_;
  container_type elements_;
};

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG

} // namespace zisc

/*!
  \example lock_free_bounded_queue_example.cpp

  This is an example of how to use zisc::LockFreeBoundedQueue.
  */

#include "scalable_circular_queue-inl.hpp"

#endif // ZISC_SCALABLE_CIRCULAR_QUEUE_HPP
