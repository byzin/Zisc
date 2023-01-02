/*!
  \file packaged_task-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PACKAGED_TASK_INL_HPP
#define ZISC_PACKAGED_TASK_INL_HPP

#include "packaged_task.hpp"
// Standard C++ library
#include <limits>
#include <memory>
#include <utility>
// Zisc
#include "future.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] thread_id No description.
  \param [in] offset No description.
  */
inline
void PackagedTask::operator()(const int64b thread_id, const DiffT offset)
{
  run(thread_id, offset);
}

/*!
  \details No detailed description

  \tparam T No description.
  \return No description
  */
template <typename T> inline
auto PackagedTask::getFuture() noexcept -> UniqueFutureT<T>
{
  using Pointer = typename UniqueFutureT<T>::pointer;
  auto* data = reinterp<Pointer>(getFutureImpl());
  return UniqueFutureT<T>{data};
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
bool PackagedTask::isValid() const noexcept
{
  const bool result = id() != invalidId();
  return result;
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
