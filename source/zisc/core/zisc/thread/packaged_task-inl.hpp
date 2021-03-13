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
#include <memory>
#include <limits>
// Zisc
#include "atomic.hpp"
#include "zisc/concepts.hpp"
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
SharedFuture<T> PackagedTask::getFuture() noexcept
{
  SharedFuture<T> f;
  getFuture(std::addressof(f));
  return f;
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
  const bool flag = id() != invalidId();
  return flag;
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

} // namespace zisc

#endif // ZISC_PACKAGED_TASK_INL_HPP
