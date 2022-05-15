/*!
  \file packaged_task.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PACKAGED_TASK_HPP
#define ZISC_PACKAGED_TASK_HPP

// Standard C++ library
#include <memory>
// Zisc
#include "future.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class PackagedTask : private NonCopyable<PackagedTask>
{
 public:
  // Type aliases
  using DiffT = int64b;
  template <typename T>
  using UniqueFutureT = std::unique_ptr<Future<T>>;


  //! Create a package with no task
  PackagedTask() noexcept = default;

  //! Destroy a task data
  virtual ~PackagedTask() noexcept = default;


  //! Run the underlying task
  void operator()(const int64b thread_id, const DiffT offset);

  //! Return the future of the underlying task
  template <typename T>
  UniqueFutureT<T> getFuture() noexcept;

  //! Return the underlying task ID
  int64b id() const noexcept;

  //! Return the invalid task ID
  static constexpr int64b invalidId() noexcept;

  //! Return the parent task ID
  int64b parentId() const noexcept;

  //! Run the underlying task
  virtual void run(const int64b thread_id, const DiffT offset) = 0;

  //! Check if the pacakge has a valid task
  bool valid() const noexcept;

 protected:
  //! Create a package with the given task id
  PackagedTask(const int64b task_id, const int64b parent_task_id) noexcept;


  //! Return the future of the underlying task
  virtual void* getFutureImpl() noexcept = 0;

 private:
  int64b id_ = invalidId();
  int64b parent_id_ = invalidId();
};

// Type aliases
using SharedTask = std::shared_ptr<PackagedTask>;

} // namespace zisc

#include "packaged_task-inl.hpp"

#endif // ZISC_PACKAGED_TASK_HPP
