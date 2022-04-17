/*!
  \file queue.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_QUEUE_HPP
#define ZISC_QUEUE_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <type_traits>
// Zisc
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam QueueClass No description.
  \tparam T No description.
  */
template <typename QueueClass, std::movable T>
class Queue : private NonCopyable<Queue<QueueClass, T>>
{
 public:
  // Type aliases
  using ValueT = std::remove_volatile_t<T>;
  using ConstT = std::add_const_t<ValueT>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_lvalue_reference_t<ConstT>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_pointer_t<ConstT>;

  // Type aliases for STL
  using value_type = ValueT;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = Reference;
  using const_reference = ConstReference;
  using pointer = Pointer;
  using const_pointer = ConstPointer;

  /*!
    \brief No brief description

    No detailed description.
    */
  class OverflowError : public SystemError
  {
   public:
    //! Construct the lock free queue error
    OverflowError(const std::string_view what_arg,
                  pmr::memory_resource* mem_resource,
                  ConstReference value);

    //! Construct the lock free queue error
    OverflowError(const std::string_view what_arg,
                  pmr::memory_resource* mem_resource,
                  RReference value);

    //! Move data
    OverflowError(OverflowError&& other);

    //! Finalize the lock free queue error
    ~OverflowError() noexcept override;


    //! Move data
    OverflowError& operator=(OverflowError&& other);


    //! Return the overflowing value
    Reference get() noexcept;

    //! Return the overflowing value
    ConstReference get() const noexcept;

   private:
    std::shared_ptr<ValueT> value_;
  };


  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity if the queue is bounded
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

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

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements
  size_type size() const noexcept;

 protected:
  // Type aliases
  using QueueT = std::remove_volatile_t<QueueClass>;
  using ConstQueueT = std::add_const_t<QueueT>;
  using QueuePtr = std::add_pointer_t<QueueT>;
  using ConstQueuePtr = std::add_pointer_t<ConstQueueT>;
  using QueueReference = std::add_lvalue_reference_t<QueueT>;
  using ConstQueueReference = std::add_lvalue_reference_t<ConstQueueT>;


  //! Create a queue
  Queue() noexcept;

  //! Move a data
  Queue(const Queue& other) noexcept;


  //! Move a queue
  Queue& operator=(const Queue& other) noexcept;


  //! Return the reference to the queue class
  QueueReference ref() noexcept;

  //! Return the reference to the queue class
  ConstQueueReference ref() const noexcept;
};

} // namespace zisc

/*!
  \example lock_free_bounded_queue_example.cpp

  This is an example of how to use zisc::Queue.
  */

#include "queue-inl.hpp"

#endif // ZISC_QUEUE_HPP
