/*!
  \file lock_free_bounded_queue.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_BOUNDED_QUEUE_HPP
#define ZISC_LOCK_FREE_BOUNDED_QUEUE_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <tuple>
#include <vector>
// Zisc
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Scalable Circular Queue (SCQ)
  \details
  For more detail, please see the following paper: 
  <a href="https://arxiv.org/abs/1908.04511">A Scalable, Portable, and Memory-Efficient Lock-Free FIFO Queue</a>. \n
  Assume that the number of threads is less than or equal size().
  */
template <typename Type>
class LockFreeBoundedQueue : public NonCopyable<LockFreeBoundedQueue<Type>>
{
 public:
  //! Create a queue
  LockFreeBoundedQueue(std::pmr::memory_resource* mem_resource) noexcept;

  //! Create a queue
  LockFreeBoundedQueue(const std::size_t cap,
                       std::pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  LockFreeBoundedQueue(LockFreeBoundedQueue&& other) noexcept;


  //! Move a queue
  LockFreeBoundedQueue& operator=(LockFreeBoundedQueue&& other) noexcept;


  //! Return the maximum possible number of elements
  std::size_t capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr std::size_t capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Return the direct access to the underlying array
  const pmr::vector<Type>& data() const noexcept;

  //! Take the first element of the queue
  std::tuple<bool, Type> dequeue() noexcept;

  //! Append the given element value to the end of the queue
  bool enqueue(const Type& value) noexcept;

  //! Append the given element value to the end of the queue
  bool enqueue(Type&& value) noexcept;

  //! Check whether the queue is empty
  bool isEmpty() const noexcept;

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(const std::size_t cap) noexcept;

  //! Return the number of elements
  int size() const noexcept;

 private:
  //! Represent a ring buffer of a queue
  class RingBuffer
  {
   public:
    using Int = int32b;
    using UInt = uint32b;


    //! Create a ring buffer
    RingBuffer(std::pmr::memory_resource* mem_resource) noexcept;

    //! Move a data
    RingBuffer(RingBuffer&& other) noexcept;


    //! Move a data
    RingBuffer& operator=(RingBuffer&& other) noexcept;


    //! Clear indices
    void clear() noexcept;

    //! Take the first element of the queue
    UInt dequeue(const bool nonempty) noexcept;

    //! Append the given element value to the end of the queue
    bool enqueue(const UInt index, const bool nonempty) noexcept;

    //! Fill indices
    void fill(const UInt s, const UInt e) noexcept;

    //! Return the invalid index
    static constexpr UInt invalidIndex() noexcept;

    //! Return the order of a ring buffer size
    std::size_t order() const noexcept;

    //! Set the number of elements of indices
    void setSize(const std::size_t s) noexcept;

    //! Return the number of elements of indices
    std::size_t size() const noexcept;

   private:
    //! Return the cache bytes
    static constexpr std::size_t cacheBytes() noexcept;

    //! Return the cache shift size
    static constexpr std::size_t cacheShift() noexcept;

    //!
    void catchUp(UInt tail, UInt head) noexcept;

    //
    Int diff(const UInt lhs, const UInt rhs) const noexcept;

    //!
    Int getThreshold3(const UInt half) const noexcept;

    //!
    UInt mapIndex(const UInt index) const noexcept;

    //!
    static constexpr UInt ringMin() noexcept;


    pmr::vector<std::atomic<UInt>> indices_;
    std::atomic<UInt> head_{0};
    std::atomic<Int> threshold_{0};
    std::atomic<UInt> tail_{0};
    uint32b padding_ = 0;
  };


  RingBuffer free_elements_buffer_;
  RingBuffer allocated_elements_buffer_;
  pmr::vector<Type> elements_;
  std::atomic_int size_;
  uint32b padding_ = 0;
};

} // namespace zisc

/*!
  \example lock_free_bounded_queue_example.cpp
  This is an example of how to use LockFreeBoundedQueue class.
  */

#include "lock_free_bounded_queue-inl.hpp"

#endif // ZISC_LOCK_FREE_BOUNDED_QUEUE_HPP
