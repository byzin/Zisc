/*!
  \file worker_info-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_WORKER_INFO_INL_HPP
#define ZISC_FLOCK_WORKER_INFO_INL_HPP

#include "worker_info.hpp"
// Standard C++ library
#include <algorithm>
#include <iterator>
#include <span>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
WorkerInfo::WorkerInfo(pmr::memory_resource* mem_resource) noexcept :
    thread_id_list_{decltype(thread_id_list_)::allocator_type{mem_resource}}
{
}

/*!
  \details No detailed description

  \param [in] id_list No description.
  \param [in,out] id_list No description.
  */
inline
WorkerInfo::WorkerInfo(const std::span<const std::thread::id> id_list,
                       pmr::memory_resource* mem_resource) noexcept :
    WorkerInfo(mem_resource)
{
  setThreadIdList(id_list);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
WorkerInfo::WorkerInfo(WorkerInfo&& other) noexcept :
    thread_id_list_{std::move(other.thread_id_list_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
WorkerInfo& WorkerInfo::operator=(WorkerInfo&& other) noexcept
{
  thread_id_list_ = std::move(other.thread_id_list_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t WorkerInfo::getCurrentWorkerId() const noexcept
{
  std::size_t index = 0;
  {
    const std::thread::id current_id = std::this_thread::get_id();
    const std::span<const std::thread::id> id_list = threadIdList();
    auto pos = std::lower_bound(id_list.begin(), id_list.end(), current_id);
    if ((pos != id_list.end()) && (*pos == current_id))
      index = std::distance(id_list.begin(), pos);
  }
  return index;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t WorkerInfo::numOfWorkers() const noexcept
{
  return thread_id_list_.size();
}

/*!
  \details No detailed description

  \param [in] id_list No description.
  */
inline
void WorkerInfo::setThreadIdList(const std::span<const std::thread::id> list) noexcept
{
  thread_id_list_.resize(list.size());
  std::copy_n(list.begin(), list.size(), thread_id_list_.begin());
  std::sort(thread_id_list_.begin(), thread_id_list_.end());
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] list No description.
  \return No description
  */
template <typename Type> inline
std::add_lvalue_reference_t<Type> WorkerInfo::takeOut(std::span<Type> list) const noexcept
{
  ZISC_ASSERT(list.size() == numOfWorkers(),
              "The size of the list doesn't match with size of the worker info.");
  const std::size_t index = getCurrentWorkerId();
  return list[index];
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::span<const std::thread::id> WorkerInfo::threadIdList() const noexcept
{
  return thread_id_list_;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_WORKER_INFO_INL_HPP */
