/*!
  \file packaged_task-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PACKAGED_TASK_INL_HPP
#define ZISC_PACKAGED_TASK_INL_HPP

#include "packaged_task.hpp"
// Standard C++ library
#include <atomic>
#include <memory>
#include <limits>
#include <thread>
#include <utility>
// Zisc
#include "atomic.hpp"
#include "future.hpp"
#include "spin_lock_mutex.hpp"
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
inline
PackagedTask::PackagedTask() noexcept
{
}

/*!
  \details No detailed description
  */
inline
PackagedTask::~PackagedTask() noexcept
{
}

/*!
  \details No detailed description

  \param [in] thread_id No description.
  \param [in] offset No description.
  */
inline
void PackagedTask::operator()(const int64b thread_id, const DiffType offset)
{
  run(thread_id, offset);
}

/*!
  \details No detailed description

  \tparam T No description.
  \return No description
  */
template <NonReference T> inline
Future<T>& PackagedTask::getFuture() noexcept
{
  ZISC_ASSERT(hasFuture(), "A future isn't set.");
  auto f = cast<Future<T>*>(future_);
  return *f;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool PackagedTask::hasFuture() const noexcept
{
  const bool result = future_ != nullptr;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
int64b PackagedTask::id() const noexcept
{
  return id_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr int64b PackagedTask::invalidId() noexcept
{
  return (std::numeric_limits<int64b>::min)();
}

/*!
  \details No detailed description

  \return No description
  */
inline
int64b PackagedTask::parentId() const noexcept
{
  return parent_id_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool PackagedTask::valid() const noexcept
{
  const bool result = id() != invalidId();
  return result;
}

/*!
  \details No detailed description

  \param [in] task_id No description.
  \param [in] parent_task_id No description.
  */
inline
PackagedTask::PackagedTask(const int64b task_id, const int64b parent_task_id) noexcept :
    id_{task_id},
    parent_id_{parent_task_id}
{
}

/*!
  \details No detailed description

  \tparam T No description.
  */
template <NonReference T> inline
void PackagedTask::destroy() noexcept
{
  auto is_ready = !isCompleted() && lock<T>();
  if (is_ready) {
    Future<T>& f = getFuture<T>();
    unlink(&f);
    unlock(&f);
  }
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in] result No description.
  */
template <NonReference T, NonReference ValueT> inline
void PackagedTask::setResult(ValueT&& result) noexcept
{
  auto is_ready = !isCompleted() && lock<T>();
  if (is_ready) {
    Future<T>& f = getFuture<T>();
    f.set(std::forward<ValueT>(result));
    unlink(&f);
    unlock(&f);
  }
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool PackagedTask::isCompleted() const noexcept
{
  const bool result = is_completed_.test(std::memory_order::acquire);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic_flag& PackagedTask::lockState() noexcept
{
  return lock_state_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic_flag& PackagedTask::lockState() const noexcept
{
  return lock_state_;
}

/*!
  \details No detailed description

  \tparam T No description.
  */
template <NonReference T> inline
bool PackagedTask::lock() noexcept
{
  bool is_ready = false;
  // Lock the task and future
  bool retry = false;
  while (!is_ready) {
    if (retry)
      std::this_thread::yield();

    // Check if the task is locked
    std::atomic_flag& task_lock = lockState();
    const bool is_task_locked = task_lock.test_and_set(std::memory_order::acq_rel);
    if (is_task_locked) {
      retry = true;
      continue;
    }

    // Terminate the set if a future is unlinked
    if (!hasFuture()) {
      task_lock.clear();
      break;
    }

    // Check if the future is locked
    std::atomic_flag& future_lock = getFuture<T>().lockState();
    const bool is_future_locked = future_lock.test_and_set(std::memory_order::acq_rel);
    if (is_future_locked) {
      retry = true;
      task_lock.clear();
      continue;
    }

    is_ready = true;
  }
  return is_ready;
}

/*!
  \details No detailed description

  \param [in,out] f No description.
  */
inline
void PackagedTask::setFuture(void* f) noexcept
{
  future_ = f;
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in,out] f No description.
  */
template <NonReference T> inline
void PackagedTask::unlink(Future<T>* f) noexcept
{
  f->is_completed_.test_and_set(std::memory_order::release);
  is_completed_.test_and_set(std::memory_order::release);

  f->linkWithTask(nullptr);
  setFuture(nullptr);
}

/*!
  \details No detailed description

  \tparam T No description.
  \param [in,out] f No description.
  */
template <NonReference T> inline
void PackagedTask::unlock(Future<T>* f) noexcept
{
  std::atomic_flag& future_lock = f->lockState();
  future_lock.clear(std::memory_order::release);
  std::atomic_flag& task_lock = lockState();
  task_lock.clear(std::memory_order::release);
}

} // namespace zisc

#endif // ZISC_PACKAGED_TASK_INL_HPP
