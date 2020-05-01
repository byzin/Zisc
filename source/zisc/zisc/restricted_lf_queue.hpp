/*!
  \file restricted_lf_queue.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_RESTRICTED_LF_QUEUE_HPP
#define ZISC_RESTRICTED_LF_QUEUE_HPP

// Standard C++ library
#include <atomic>
#include <tuple>
#include <type_traits>
#include <vector>
// Zisc
#include "error.hpp"
#include "lock_free_bounded_queue.hpp"
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief Lock-free bounded queue

  For more detail, please see the following paper:
  <a href="https://dl.acm.org/doi/10.1145/3332466.3374508">POSTER: Restricted Memory-Friendly Lock-Free Bounded Queues</a>. <br>

  \tparam T No description.
  */
template <typename T>
class RestrictedLFQueue : public LockFreeBoundedQueue<RestrictedLFQueue<T>, T>
{
 public:
  // Types
  using BaseQueueType = LockFreeBoundedQueue<RestrictedLFQueue<T>, T>;
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
  RestrictedLFQueue(pmr::memory_resource* mem_resource) noexcept;

  //! Create a queue
  RestrictedLFQueue(const size_type cap,
                    pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  RestrictedLFQueue(RestrictedLFQueue&& other) noexcept;


  //! Move a queue
  RestrictedLFQueue& operator=(RestrictedLFQueue&& other) noexcept;


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

  //! Append the given element to the end of the queue
  bool enqueue(ConstReference value);

  //! Append the given element to the end of the queue
  bool enqueue(RReference value);

  //! Check whether the queue is empty
  bool isEmpty() const noexcept;

  //! Return a pointer to the underlying memory resource
  pmr::memory_resource* resource() const noexcept;

  //! Change the maximum possible number of elements. The queue data is cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements
  size_type size() const noexcept;

 private:
  //! Take the first element of the queue
  size_type dequeueImpl() noexcept;

  //! Append the given element to the end of the queue
  size_type enqueueImpl() noexcept;

  //! Return the versioned null corresponding to the given round
  static constexpr uint64b getNull(const uint64b cycle) noexcept;

  //! Return the invalid index
  static constexpr size_type invalidIndex() noexcept;

  //! Remap index
  size_type permuteIndex(const uint64b index) const noexcept;

  //! Return the occupied flag
  static constexpr uint64b occupiedFlag() noexcept;

  container_type elements_;
  pmr::vector<uint64b> indices_;
  pmr::vector<uint8b> status_;
  uint64b head_;
  uint64b tail_;
};

} // namespace zisc

#include "restricted_lf_queue-inl.hpp"

#endif // ZISC_RESTRICTED_LF_QUEUE_HPP
