/*!
  \file queue.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
template <std::movable> class ContainerOverflowError;

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
  using ConstReference = std::add_const_t<std::add_lvalue_reference_t<ConstT>>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_const_t<std::add_pointer_t<ConstT>>;

  // Type aliases for STL
  using value_type = ValueT;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = Reference;
  using const_reference = ConstReference;
  using pointer = Pointer;
  using const_pointer = ConstPointer;

  // Exception
  using OverflowError = ContainerOverflowError<T>;


  //! Return the maximum possible number of elements can be queued
  [[nodiscard]]
  auto capacity() const noexcept -> size_type;

  //! Return the maximum possible capacity if the queue is bounded
  static constexpr auto capacityMax() noexcept -> size_type;

  //! Clear the contents
  void clear() noexcept;

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

  //! Check whether the queue is empty
  [[nodiscard]]
  auto isEmpty() const noexcept -> bool;

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(const size_type cap);

  //! Return the number of elements
  [[nodiscard]]
  auto size() const noexcept -> size_type;

 protected:
  // Type aliases
  using QueueT = std::remove_volatile_t<QueueClass>;
  using ConstQueueT = std::add_const_t<QueueT>;
  using QueuePtr = std::add_pointer_t<QueueT>;
  using ConstQueuePtr = std::add_pointer_t<ConstQueueT>;
  using QueueReference = std::add_lvalue_reference_t<QueueT>;
  using ConstQueueReference = std::add_lvalue_reference_t<ConstQueueT>;


  //! Create a queue
  Queue() noexcept = default;

  //! Move a data
  Queue(const Queue& other) noexcept;


  //! Move a queue
  auto operator=(const Queue& other) noexcept -> Queue&;


  //! Return the reference to the queue class
  auto ref() noexcept -> QueueReference;

  //! Return the reference to the queue class
  auto ref() const noexcept -> ConstQueueReference;
};

} // namespace zisc

/*!
  \example concurrent_bounded_queue_example.cpp

  This is an example of how to use zisc::Queue.
  */

#include "queue-inl.hpp"

#endif // ZISC_QUEUE_HPP
