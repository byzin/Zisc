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
#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>
// Zisc
#include "queue.hpp"
#include "ring_buffer.hpp"
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
template <std::movable T>
class ScalableCircularQueue : public Queue<ScalableCircularQueue<T>, T>
{
 public:
  // Type aliases
  using BaseQueueT = Queue<ScalableCircularQueue<T>, T>;
  using ValueT = typename BaseQueueT::ValueT;
  using ConstT = typename BaseQueueT::ConstT;
  using Reference = typename BaseQueueT::Reference;
  using RReference = typename BaseQueueT::RReference;
  using ConstReference = typename BaseQueueT::ConstReference;
  using Pointer = typename BaseQueueT::Pointer;
  using ConstPointer = typename BaseQueueT::ConstPointer;

  // Type aliases for STL
  using container_type = pmr::vector<ValueT>;
  using value_type = typename BaseQueueT::value_type;
  using size_type = typename BaseQueueT::size_type;
  using reference = typename BaseQueueT::reference;
  using const_reference = typename BaseQueueT::const_reference;


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
  [[nodiscard]] std::optional<ValueT> dequeue() noexcept;

  //! Append the given element value to the end of the queue
  [[nodiscard]] std::optional<size_type> enqueue(ConstReference value);

  //! Append the given element value to the end of the queue
  [[nodiscard]] std::optional<size_type> enqueue(RReference value);

  //! Return the value by the given index
  Reference get(const size_type index) noexcept;

  //! Return the value by the given index
  ConstReference get(const size_type index) const noexcept;

  //! Check if the queue is bounded
  static constexpr bool isBounded() noexcept;

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
  [[nodiscard]] std::optional<size_type> enqueueImpl(ValueT&& value);


  RingBuffer free_elements_;
  RingBuffer allocated_elements_;
  container_type elements_;
};

} // namespace zisc

/*!
  \example concurrent_bounded_queue_example.cpp

  This is an example of how to use zisc::Queue.
  */

#include "scalable_circular_queue-inl.hpp"

#endif // ZISC_SCALABLE_CIRCULAR_QUEUE_HPP
