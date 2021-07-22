/*!
  \file future-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUTURE_INL_HPP
#define ZISC_FUTURE_INL_HPP

#include "future.hpp"
// Standard C++ library
#include <atomic>
#include <limits>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
// Zisc
#include "packaged_task.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <NonReference T> inline
Future<T>::Future() noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] t No description.
  */
template <NonReference T> inline
Future<T>::Future(PackagedTask* t) noexcept
{
  linkWithTask(t);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <NonReference T> inline
Future<T>::Future(Future&& other) noexcept
{
  moveData(other);
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
Future<T>::~Future() noexcept
{
  destroy();
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <NonReference T> inline
auto Future<T>::operator=(Future&& other) noexcept -> Future&
{
  moveData(other);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
auto Future<T>::get() -> Reference
{
  wait();
  return value();
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
auto Future<T>::get() const -> ConstReference
{
  wait();
  return value();
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
int64b Future<T>::id() const noexcept
{
  return task_id_;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
constexpr int64b Future<T>::invalidId() noexcept
{
  return (std::numeric_limits<int64b>::min)();
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
bool Future<T>::valid() const noexcept
{
  const bool result = id() != invalidId();
  return result;
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
void Future<T>::wait() const
{
  while (!isReady())
    std::this_thread::yield();
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
void Future<T>::destroy() noexcept
{
  bool is_ready = !isCompleted() && lock();
  if (is_ready) {
    PackagedTask& t = task();
    unlink(&t);
    unlock(&t);
  }

  if constexpr (std::is_destructible_v<ValueT> && std::is_same_v<DataT, StorageT>) {
    if (isReady()) {
      Reference v = value();
      std::destroy_at(std::addressof(v));
    }
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
bool Future<T>::hasTask() const noexcept
{
  const bool result = task_ != nullptr;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
bool Future<T>::isCompleted() const noexcept
{
  const bool result = is_completed_.test(std::memory_order::acquire);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
bool Future<T>::isReady() const noexcept
{
  const bool flag = has_value_.test(std::memory_order::acquire);
  return flag;
}

/*!
  \details No detailed description

  \param [in,out] t No description.
  */
template <NonReference T> inline
void Future<T>::linkWithTask(PackagedTask* t) noexcept
{
  task_ = t;
  if (t) {
    task_id_ = t->id();
    t->setFuture(this);
  }
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
bool Future<T>::lock() noexcept
{
  bool is_ready = false;
  // Lock the task and future
  bool retry = false;
  while (!is_ready) {
    if (retry)
      std::this_thread::yield();

    // Check if the future is locked
    std::atomic_flag& future_lock = lockState();
    const bool is_future_locked = future_lock.test_and_set(std::memory_order::acq_rel);
    if (is_future_locked) {
      retry = true;
      continue;
    }

    // Terminate the set if a task is unlinked
    if (!hasTask()) {
      future_lock.clear();
      break;
    }

    // Check if the task is locked
    std::atomic_flag& task_lock = task().lockState();
    const bool is_task_locked = task_lock.test_and_set(std::memory_order::acq_rel);
    if (is_task_locked) {
      retry = true;
      future_lock.clear();
      continue;
    }

    is_ready = true;
  }
  return is_ready;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
std::atomic_flag& Future<T>::lockState() noexcept
{
  return lock_state_;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
const std::atomic_flag& Future<T>::lockState() const noexcept
{
  return lock_state_;
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <NonReference T> inline
void Future<T>::moveData(Future& other) noexcept
{
  bool is_ready = !other.isCompleted() && other.lock();
  if (is_ready)
    lockState().test_and_set(std::memory_order::release);

  // Move data
  task_ = nullptr;
  task_id_ = other.id();
  if (other.isReady()) {
    ZISC_ASSERT(!is_ready, "The other future is still locked.");
    data_ = std::move(other.data_);
    has_value_.test_and_set(std::memory_order::release);
    is_completed_.test_and_set(std::memory_order::release);
    other.has_value_.clear();
  }
  else if (other.hasTask()) {
    ZISC_ASSERT(is_ready, "The other future isn't locked.");
    linkWithTask(std::addressof(other.task()));
    other.is_completed_.test_and_set(std::memory_order::release);
  }

  if (is_ready)
    unlock(std::addressof(task()));
}

/*!
  \details No detailed description

  \param [in] result No description.
  */
template <NonReference T> inline
void Future<T>::set(ValueRReference result) noexcept
{
  // Set value
  if constexpr (!std::is_void_v<Type>) {
    if constexpr (std::is_same_v<DataT, ValueT>)
      data_ = std::move(result);
    else 
      new (std::addressof(data_)) ValueT{std::move(result)};
  }
  has_value_.test_and_set(std::memory_order::release);
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
PackagedTask& Future<T>::task() noexcept
{
  ZISC_ASSERT(hasTask(), "A task isn't set.");
  return *task_;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
const PackagedTask& Future<T>::task() const noexcept
{
  ZISC_ASSERT(hasTask(), "A task isn't set.");
  return *task_;
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
void Future<T>::unlink(PackagedTask* t) noexcept
{
  t->is_completed_.test_and_set(std::memory_order::release);
  is_completed_.test_and_set(std::memory_order::release);

  t->setFuture(nullptr);
  linkWithTask(nullptr);
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
void Future<T>::unlock(PackagedTask* t) noexcept
{
  std::atomic_flag& future_lock = lockState();
  future_lock.clear(std::memory_order::release);
  std::atomic_flag& task_lock = t->lockState();
  task_lock.clear(std::memory_order::release);
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
auto Future<T>::value() noexcept -> Reference
{
  if constexpr (!std::is_void_v<Type>) {
    if constexpr (std::is_same_v<DataT, ValueT>)
      return data_;
    else
      return *reinterp<ValueT*>(std::addressof(data_));
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
auto Future<T>::value() const noexcept -> ConstReference
{
  if constexpr (!std::is_void_v<Type>) {
    if constexpr (std::is_same_v<DataT, ValueT>)
      return data_;
    else
      return *reinterp<ConstType*>(std::addressof(data_));
  }
}

} // namespace zisc

#endif // ZISC_FUTURE_INL_HPP
