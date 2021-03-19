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
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/thread/thread_manager.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <NonReference T> inline
Future<T>::Future() noexcept
{
  static_assert(sizeof(has_value_) == 1, "std::atomic_flag isn't 1 byte.");
}

/*!
  \details No detailed description

  \param [in] operation_id No description.
  \param [in] manager No description.
  */
template <NonReference T> inline
Future<T>::Future(const int64b operation_id, ThreadManager* manager) noexcept :
    id_{operation_id},
    manager_{manager}
{
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
  return id_;
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
  const bool flag = manager_ != nullptr;
  return flag;
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
void Future<T>::wait() const
{
  auto th_manager = manager();
  const int64b thread_id = th_manager->getCurrentThreadId();
  while (!isReady()) {
    bool has_task = false;
    if (thread_id != ThreadManager::unmanagedThreadId()) {
      // Run another task while waiting for the complesion
      has_task = runAnotherTask(thread_id);
    }
    if (!has_task)
      std::this_thread::yield();
  }
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
void Future<T>::destroy() noexcept
{
  if constexpr (std::is_destructible_v<ValueT> && !std::is_same_v<DataT, ValueT>) {
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
bool Future<T>::isReady() const noexcept
{
  const bool flag = has_value_.test(std::memory_order::acquire);
  return flag;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
ThreadManager* Future<T>::manager() const noexcept
{
  return manager_;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
bool Future<T>::runAnotherTask(const int64b thread_id) const
{
  ZISC_ASSERT(thread_id != ThreadManager::unmanagedThreadId(), 
              "Unmanaged thread tries to run a work in the thread manager.");
  auto th_manager = manager();
  auto task = th_manager->fetchTask();
  const bool has_task = task.hasTask();
  if (has_task)
    task.run(thread_id);
  return has_task;
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
  const bool old = has_value_.test_and_set(std::memory_order::release);
  ZISC_ASSERT(!old, "The result already has a value.");
  // Notify the thread manager that the task was completed
  auto th_manager = manager();
  th_manager->finishTask(id());
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
