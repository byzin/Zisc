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
#include <memory>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
class ThreadManager;

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

  //! Create a future of the 
  Future(const int64b operation_id, ThreadManager* manager) noexcept;

  //! Destroy the future
  ~Future() noexcept;


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


  // Type aliases
  friend ThreadManager;
  using ValueT = std::conditional_t<std::is_void_v<Type>, int, Type>;
  using ConstValueT = std::add_const_t<ValueT>;
  using ValueReference = std::add_lvalue_reference_t<ValueT>;
  using ValueRReference = std::add_rvalue_reference_t<ValueT>;
  using ConstValueReference = std::add_lvalue_reference_t<ConstValueT>;
  using DataT = std::conditional_t<std::is_default_constructible_v<ValueT>,
      ValueT,
      std::aligned_storage_t<sizeof(ValueT), alignof(ValueT)>>;


  //! Destroy the result data
  void destroy() noexcept;

  //! Check if the result is available
  bool isReady() const noexcept;

  //! Return the underlying thread manager
  ThreadManager* manager() const noexcept;

  //! Run another task if current thread is managed by the thread manager
  bool runAnotherTask(const int64b thread_id) const;

  //! Set the given result value
  void set(ValueRReference result) noexcept;

  //! Return the reference to the value
  ValueReference value() noexcept;

  //! Return the reference to the value
  ConstValueReference value() const noexcept;


  int64b id_ = invalidId();
  ThreadManager* manager_ = nullptr;
  DataT data_;
  std::atomic_flag has_value_;
};

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG

// Type aliases
template <NonReference T>
using SharedFuture = std::shared_ptr<Future<T>>;

} // namespace zisc

#include "future-inl.hpp"

#endif // ZISC_FUTURE_HPP
