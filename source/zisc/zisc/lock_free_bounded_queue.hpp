/*!
  \file lock_free_bounded_queue.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_BOUNDED_QUEUE_HPP
#define ZISC_LOCK_FREE_BOUNDED_QUEUE_HPP

// Standard C++ library
#include <cstddef>
#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>
// Zisc
#include "error.hpp"
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam QueueClass No description.
  \tparam T No description.
  */
template <typename QueueClass, typename T>
class LockFreeBoundedQueue : public NonCopyable<LockFreeBoundedQueue<QueueClass, T>>
{
 public:
  // Types
  using Type = std::remove_cv_t<T>;
  using ConstType = std::add_const_t<Type>;
  using Reference = std::add_lvalue_reference_t<Type>;
  using RReference = std::add_rvalue_reference_t<Type>;
  using ConstReference = std::add_lvalue_reference_t<ConstType>;
  using Pointer = std::add_pointer_t<Type>;
  using ConstPointer = std::add_pointer_t<ConstType>;

  // Type aliases for STL
  using value_type = Type;
  using size_type = std::size_t;
  using reference = Reference;
  using const_reference = ConstReference;


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

    //!
    ~OverflowError() override;


    //! Return the overflowing value
    Reference get() noexcept;

    //! Return the overflowing value
    ConstReference get() const noexcept;

   private:
    std::shared_ptr<Type> value_;
  };


  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Take the first element of the queue
  std::tuple<bool, Type> dequeue() noexcept;

  //! Append the given element value to the end of the queue
  bool enqueue(ConstReference value);

  //! Append the given element value to the end of the queue
  bool enqueue(RReference value);

  //! Check whether the queue is empty
  bool isEmpty() const noexcept;

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements
  size_type size() const noexcept;

 protected:
  // Types
  using QueuePtr = std::add_pointer_t<QueueClass>;
  using ConstQueuePtr = std::add_pointer_t<std::add_const_t<QueueClass>>;


  //! Create a queue
  LockFreeBoundedQueue() noexcept;

  //! Move a data
  LockFreeBoundedQueue(const LockFreeBoundedQueue& other) noexcept;


  //! Move a queue
  LockFreeBoundedQueue& operator=(const LockFreeBoundedQueue& other) noexcept;
};

} // namespace zisc

/*!
  \example lock_free_bounded_queue_example.cpp

  This is an example of how to use zisc::LockFreeBoundedQueue.
  */

#include "lock_free_bounded_queue-inl.hpp"

#endif // ZISC_LOCK_FREE_BOUNDED_QUEUE_HPP
