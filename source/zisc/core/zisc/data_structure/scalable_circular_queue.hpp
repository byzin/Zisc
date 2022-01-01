/*!
  \file scalable_circular_queue.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
#include <type_traits>
#include <vector>
// Zisc
#include "query_result.hpp"
#include "query_value.hpp"
#include "queue.hpp"
#include "ring_buffer.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief Scalable Circular Lock-free Queue (SCQ)

  For more detail, please see the following paper: 
  <a href="https://arxiv.org/abs/1908.04511">A Scalable, Portable, and Memory-Efficient Lock-Free FIFO Queue</a>. <br>
  Assume that the number of threads is less than or equal size().

  \tparam T No description.
  */
template <Movable T>
class ScalableCircularQueue : public Queue<ScalableCircularQueue<T>, T>
{
 public:
  // Type aliases
  using BaseQueueType = Queue<ScalableCircularQueue<T>, T>;
  using Type = typename BaseQueueType::Type;
  using ConstType = typename BaseQueueType::ConstType;
  using Reference = typename BaseQueueType::Reference;
  using RReference = typename BaseQueueType::RReference;
  using ConstReference = typename BaseQueueType::ConstReference;
  using Pointer = typename BaseQueueType::Pointer;
  using ConstPointer = typename BaseQueueType::ConstPointer;
  using EnqueueQueryResultT = typename BaseQueueType::EnqueueQueryResultT;
  using DequeueQueryResultT = typename BaseQueueType::DequeueQueryResultT;

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
  [[nodiscard]] DequeueQueryResultT dequeue() noexcept;

  //! Append the given element value to the end of the queue
  [[nodiscard]] EnqueueQueryResultT enqueue(ConstReference value);

  //! Append the given element value to the end of the queue
  [[nodiscard]] EnqueueQueryResultT enqueue(RReference value);

  //! Check if the queue is concurrent
  static constexpr bool isConcurrent() noexcept;

  //! Check whether the queue is empty
  bool isEmpty() const noexcept;

  //! Return a pointer to the underlying memory resource
  pmr::memory_resource* resource() const noexcept;

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements
  size_type size() const noexcept;

 private:
  //! Append the given element value to the end of the queue
  template <typename ValueT>
  [[nodiscard]] EnqueueQueryResultT enqueueImpl(ValueT&& value);


  RingBuffer free_elements_;
  RingBuffer allocated_elements_;
  container_type elements_;
};

} // namespace zisc

/*!
  \example lock_free_bounded_queue_example.cpp

  This is an example of how to use zisc::LockFreeQueue.
  */

#include "scalable_circular_queue-inl.hpp"

#endif // ZISC_SCALABLE_CIRCULAR_QUEUE_HPP
