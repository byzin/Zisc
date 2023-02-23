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
#include <cmath>
#include <limits>
#include <memory>
#include <utility>
// Zisc
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

inline
int64b PackagedTask::encodeInfo(const int64b task_id, const bool wait_for_precedence) noexcept
{
  const int64b info = wait_for_precedence ? -task_id : task_id;
  return info;
}

/*!
  \details No detailed description

  \return No description
  */
inline
int64b PackagedTask::id() const noexcept
{
  return std::abs(info_);
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
bool PackagedTask::isNeededToWaitForPrecedence() const noexcept
{
  const bool flag = std::signbit(info_);
  return flag;
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

  \param [in] task_id No description.
  */
inline
PackagedTask::PackagedTask(const int64b task_info) noexcept :
    info_{task_info}
{
}

/*!
  \details No detailed description

  \param [in] task_id No description.
  */
template <typename Type> inline
PackagedTaskType<Type>::PackagedTaskType(const int64b task_info) noexcept :
    PackagedTask(task_info)
{
}

} // namespace zisc

#endif // ZISC_PACKAGED_TASK_INL_HPP
