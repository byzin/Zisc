/*!
  \file future.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUTURE_HPP
#define ZISC_FUTURE_HPP

// Standard C++ library
#include <atomic>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
class PackagedTask;

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_GCC || Z_CLANG

/*!
  \brief Future provides a mechanism to access the result of asynchronous operations

  No detailed description.

  \tparam T No description.
  */
template <NonReference T>
class Future : private NonCopyable<Future<T>>
{
 public:
  // Type aliases
  using Type = std::remove_volatile_t<T>;
  using ConstType = std::add_const_t<Type>;
  using Reference = std::add_lvalue_reference_t<Type>;
  using ConstReference = std::add_lvalue_reference<ConstType>;


  //! Create a future with no shared state
  Future() noexcept;

  //! Create a future
  Future(PackagedTask* t) noexcept;

  //! Move data
  Future(Future&& other) noexcept;

  //! Destroy the future
  ~Future() noexcept;


  //! Move data
  Future& operator=(Future&& other) noexcept;


  //! Return the result
  Reference get();

  //! Return the result
  ConstReference get() const;

  //! Return the underlying operation ID
  int64b id() const noexcept;

  //! Return the invalid ID
  static constexpr int64b invalidId() noexcept;

  //! Check if the future has a shared state
  bool valid() const noexcept;

  //! Wait for the result to become available
  void wait() const;

 private:
  static_assert(std::is_void_v<T> || std::is_move_constructible_v<T>);
  friend PackagedTask;


  // Type aliases
  using ValueT = std::conditional_t<std::is_void_v<Type>, int, Type>;
  using ValueRReference = std::add_rvalue_reference_t<ValueT>;
  using StorageT = std::aligned_storage_t<sizeof(ValueT), std::alignment_of_v<ValueT>>;
  using DataT = std::conditional_t<std::is_default_constructible_v<ValueT> &&
                                   std::is_move_assignable_v<ValueT>,
                                   ValueT,
                                   StorageT>;


  //! Destroy the data in the future
  void destroy() noexcept;

  //! Check if the future is linked with a task
  bool hasTask() const noexcept;

  //! Check if the future is completed
  bool isCompleted() const noexcept;

  //! Check if the result is available
  bool isReady() const noexcept;

  //! Link the future with the given task
  void linkWithTask(PackagedTask* t) noexcept;

  //! Lock the task and future
  bool lock() noexcept;

  //! Return the underlying lock state
  std::atomic_flag& lockState() noexcept;

  //! Return the underlying lock state
  const std::atomic_flag& lockState() const noexcept;

  //! Move data
  void moveData(Future& other) noexcept;

  //! Set the given result value
  void set(ValueRReference result) noexcept;

  //! Return the linked task
  PackagedTask& task() noexcept;

  //! Return the linked task
  const PackagedTask& task() const noexcept;

  //! Unlink the task with the future
  void unlink(PackagedTask* t) noexcept;

  //! Unlock the task and the future
  void unlock(PackagedTask* t) noexcept;


  //! Return the reference to the value
  Reference value() noexcept;

  //! Return the reference to the value
  ConstReference value() const noexcept;


  PackagedTask* task_ = nullptr;
  int64b task_id_ = invalidId();
  DataT data_;
  std::atomic_flag lock_state_;
  std::atomic_flag has_value_;
  std::atomic_flag is_completed_;
  [[maybe_unused]] uint8b padding_;
};

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG

} // namespace zisc

#include "future-inl.hpp"

#endif // ZISC_FUTURE_HPP
