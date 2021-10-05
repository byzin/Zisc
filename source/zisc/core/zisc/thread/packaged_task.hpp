/*!
  \file packaged_task.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PACKAGED_TASK_HPP
#define ZISC_PACKAGED_TASK_HPP

// Standard C++ library
#include <atomic>
#include <memory>
// Zisc
#include "atomic.hpp"
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
template <NonReference> class Future;

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_GCC || Z_CLANG
#if defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wweak-vtables"
#endif // Z_CLANG

/*!
  \brief No brief description

  No detailed description.
  */
class PackagedTask : private NonCopyable<PackagedTask>
{
 public:
  // Type aliases
  using DiffType = Atomic::WordValueType;


  //! Create a package with no task
  PackagedTask() noexcept;

  //! Destroy a task data
  virtual ~PackagedTask() noexcept;


  //! Run a underlying task
  void operator()(const int64b thread_id, const DiffType offset);

  //! Return the future of the underlying task
  template <NonReference T>
  Future<T>& getFuture() noexcept;

  //! Check if the packaged task has a future
  bool hasFuture() const noexcept;

  //! Return the underlying task ID
  int64b id() const noexcept;

  //! Return the invalid task ID
  static constexpr int64b invalidId() noexcept;

  //! Return the parent task ID
  int64b parentId() const noexcept;

  //! Run a underlying task
  virtual void run(const int64b thread_id, const DiffType offset) = 0;

  //! Check if the pacakge has a valid task
  bool valid() const noexcept;

 protected:
  //! Create a package with the given task id
  PackagedTask(const int64b task_id, const int64b parent_task_id) noexcept;


  //! Destroy the task data
  template <NonReference T>
  void destroy() noexcept;

  //! Set the result of the task execution
  template <NonReference T, NonReference ValueT>
  void setResult(ValueT&& result) noexcept;

 private:
  template <NonReference>
  friend class Future;


  //! Check if the task is completed
  bool isCompleted() const noexcept;

  //! Return the lock state
  std::atomic_flag& lockState() noexcept;

  //! Return the lock state
  const std::atomic_flag& lockState() const noexcept;

  //! Lock the task and future
  template <NonReference T>
  bool lock() noexcept;

  //! Set a future for the underlying task
  void setFuture(void* f) noexcept;

  //! Unlink the task with the given future
  template <NonReference T>
  void unlink(Future<T>* f) noexcept;

  //! Unlock the task and the given future
  template <NonReference T>
  void unlock(Future<T>* f) noexcept;


  int64b id_ = invalidId();
  int64b parent_id_ = invalidId();
  void* future_ = nullptr;
  std::atomic_flag lock_state_ = ATOMIC_FLAG_INIT;
  std::atomic_flag is_completed_ = ATOMIC_FLAG_INIT;
};

#if defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_CLANG
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG

// Type aliases
using SharedTask = std::shared_ptr<PackagedTask>;

} // namespace zisc

#include "packaged_task-inl.hpp"

#endif // ZISC_PACKAGED_TASK_HPP
