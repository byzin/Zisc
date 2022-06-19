/*!
  \file write_announcements-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_WRITE_ANNOUNCEMENT_INL_HPP
#define ZISC_FLOCK_WRITE_ANNOUNCEMENT_INL_HPP

#include "write_announcements.hpp"
// Standard C++ library
#include <atomic>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
// Zisc
#include "worker_info.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \param [in] mem_resource No description.
  */
inline
WriteAnnouncements::WriteAnnouncements(pmr::memory_resource* mem_resource) noexcept :
    announcement_{decltype(announcement_)::allocator_type{mem_resource}}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
WriteAnnouncements::WriteAnnouncements(WriteAnnouncements&& other) noexcept :
    announcement_{std::move(other.announcement_)},
    worker_info_{std::move(other.worker_info_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
WriteAnnouncements& WriteAnnouncements::operator=(WriteAnnouncements&& other) noexcept
{
  announcement_ = std::move(other.announcement_);
  worker_info_ = std::move(other.worker_info_);
  return *this;
}

/*!
  \details No detailed description
  */
inline
void WriteAnnouncements::clear() noexcept
{
  const std::size_t index = workerInfo().getCurrentWorkerId();
  announcement_[index * stride()].v_.store(0, std::memory_order::release);
}

/*!
  \details No detailed description

  \return No description
  */
inline
pmr::vector<std::size_t> WriteAnnouncements::scan() const noexcept
{
  pmr::memory_resource* mem_resource = announcement_.get_allocator().resource();
  pmr::vector<std::size_t>::allocator_type tag_alloc{mem_resource};
  pmr::vector<std::size_t> announced_tags{tag_alloc};
  announced_tags.reserve(workerInfo().numOfWorkers());
  for (std::size_t i = 0; i < workerInfo().numOfWorkers() * stride(); i += stride())
    announced_tags.emplace_back(announcement_[i].v_.load(std::memory_order::acquire));
  return announced_tags;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
inline
void WriteAnnouncements::set(const std::size_t value) noexcept
{
  const std::size_t index = workerInfo().getCurrentWorkerId();
  announcement_[index * stride()].v_.store(value, std::memory_order::release);
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t WriteAnnouncements::stride() noexcept
{
  return 16;
}

/*!
  \details No detailed description

  \param [in] info No description.
  */
inline
void WriteAnnouncements::setWorkerInfo(const WorkerInfo& info) noexcept
{
  worker_info_ = &info;
  announcement_.resize(WorkerInfo().numOfWorkers() * stride());
}

/*!
  \details No detailed description

  \return No description
  */
inline
const WorkerInfo& WriteAnnouncements::workerInfo() const noexcept
{
  return *worker_info_;
}

/*!
  \details No detailed description
  */
inline
WriteAnnouncements::AtomicT::AtomicT() noexcept :
    v_{0}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
WriteAnnouncements::AtomicT::AtomicT(AtomicT&& other) noexcept :
    v_{other.v_.load(std::memory_order::acquire)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
auto WriteAnnouncements::AtomicT::operator=(AtomicT&& other) noexcept -> AtomicT&
{
  const std::size_t v = other.v_.load(std::memory_order::acquire);
  v_.store(v, std::memory_order::release);
  return *this;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_WRITE_ANNOUNCEMENT_INL_HPP */
