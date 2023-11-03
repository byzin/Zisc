/*!
  \file packaged_task.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PACKAGED_TASK_HPP
#define ZISC_PACKAGED_TASK_HPP

// Standard C++ library
#include <future>
#include <memory>
// Zisc
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


  //! Create a package with no task
  PackagedTask() noexcept = default;

  //! Destroy a task data
  virtual ~PackagedTask() noexcept = default;


  //! Run the underlying task
  void operator()(const int64b thread_id, const DiffT offset);

  //! Return the encoded task info
  static auto encodeInfo(const int64b task_id, const bool wait_for_precedence) noexcept -> int64b;

  //! Return the underlying task ID
  [[nodiscard]]
  auto id() const noexcept -> int64b;

  //! Return the invalid task ID
  static constexpr auto invalidId() noexcept -> int64b;

  //! Check if all precedence tasks need to be completed before the task running
  [[nodiscard]]
  auto isNeededToWaitForPrecedence() const noexcept -> bool;

  //! Check if the pacakge has a valid task
  [[nodiscard]]
  auto isValid() const noexcept -> bool;

  //! Run the underlying task
  virtual void run(const int64b thread_id, const DiffT offset) = 0;

 protected:
  //! Create a package with the given task id
  explicit PackagedTask(const int64b task_info) noexcept;


 private:
  int64b info_ = invalidId();
};

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  */
template <typename Type>
class PackagedTaskType : public PackagedTask
{
 public:
  // Type aliases
  using ReturnT = Type;


  //! Return the future of the underlying task
  virtual auto getFuture() noexcept -> std::future<ReturnT> = 0;

 protected:
  //! Create a package with the given task id
  explicit PackagedTaskType(const int64b task_info) noexcept;
};

} // namespace zisc

#include "packaged_task-inl.hpp"

#endif // ZISC_PACKAGED_TASK_HPP
