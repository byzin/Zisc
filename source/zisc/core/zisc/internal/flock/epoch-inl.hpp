/*!
  \file epoch-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_EPOCH_INL_HPP
#define ZISC_FLOCK_EPOCH_INL_HPP

#include "epoch.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <span>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "timestamp.hpp"
#include "worker_info.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

/*!
  \details No detailed description
  */
inline
Epoch::AnnounceSlot::AnnounceSlot() noexcept :
    last_{-1}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Epoch::AnnounceSlot::AnnounceSlot(AnnounceSlot&& other) noexcept :
    last_{other.last_.load(std::memory_order::acquire)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
auto Epoch::AnnounceSlot::operator=(AnnounceSlot&& other) noexcept -> AnnounceSlot&
{
  const ValueT v = other.last_.load(std::memory_order::acquire);
  last_.store(v, std::memory_order::release);
  return *this;
}

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
Epoch::Epoch(const std::span<const std::thread::id> id_list,
             TimestampPtr timestamp,
             pmr::memory_resource* mem_resource) noexcept :
    announcement_list_{decltype(announcement_list_)::allocator_type{mem_resource}},
    worker_info_{id_list, mem_resource},
    timestamp_{timestamp},
    current_epoch_{0},
    done_stamp_{-1},
    prev_stamp_{-1}
{
  initialize();
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
inline
Epoch::Epoch(Epoch&& other) noexcept :
    announcement_list_{std::move(other.announcement_list_)},
    worker_info_{std::move(other.worker_info_)},
    timestamp_{std::move(other.timestamp_)},
    current_epoch_{other.current_epoch_.load(std::memory_order::acquire)},
    done_stamp_{other.done_stamp_},
    prev_stamp_{other.prev_stamp_}
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
inline
Epoch& Epoch::operator=(Epoch&& other) noexcept
{
  announcement_list_ = std::move(other.announcement_list_);
  worker_info_ = std::move(other.worker_info_);
  timestamp_ = std::move(other.timestamp_);
  {
    const ValueT v = other.current_epoch_.load(std::memory_order::acquire);
    current_epoch_.store(v, std::memory_order::release);
  }
  done_stamp_ = other.done_stamp_;
  prev_stamp_ = other.prev_stamp_;
  return *this;
}

/*!
  \details No detailed description
  */
inline
void Epoch::announce() noexcept
{
  AnnounceSlot& slot = workerInfo().takeOut(announcementList());
  const ValueT e = getCurrent();
  slot.last_.exchange(e, std::memory_order::acquire);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Epoch::getCurrent() const noexcept -> ValueT
{
  const ValueT c = current_epoch_.load(std::memory_order::acquire);
  return c;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Epoch::getMyEpoch() const noexcept -> ValueT
{
  const AnnounceSlot& slot = workerInfo().takeOut(announcementList());
  const ValueT e = slot.last_.load(std::memory_order::acquire);
  return e;
}

/*!
  \details No detailed description

  \return No description
  */
inline
void Epoch::setMyEpoch(const ValueT e) noexcept
{
  AnnounceSlot& slot = workerInfo().takeOut(announcementList());
  slot.last_.store(e, std::memory_order::release);
}

/*!
  \details No detailed description
  */
inline
void Epoch::unannounce() noexcept
{
  AnnounceSlot& slot = workerInfo().takeOut(announcementList());
  slot.last_.store(-1, std::memory_order::release);
}

/*!
  \details No detailed description
  */
inline
void Epoch::updateEpoch() noexcept
{
  ValueT e = getCurrent();
  bool all_there = true;
  for (std::size_t j = 0; all_there && (j < 2); ++j) { // do twice
    for (std::size_t i = 0; i < announcementList().size(); ++i) {
      const AnnounceSlot& slot = announcementList()[i];
      const ValueT v = slot.last_.load(std::memory_order::acquire);
      if ((v != -1) && (v < e)) {
        all_there = false;
        break;
      }
    }
  }
  if (all_there) {
    // timestamps are for multiversioning (snapshots)
    // we set done_stamp to the stamp from the previous epoch update
    const ValueT current_stamp = timestamp().getReadStamp();
    if (current_epoch_.compare_exchange_strong(e, e + 1, std::memory_order::acq_rel, std::memory_order::acquire)) {
      done_stamp_ = prev_stamp_;
      prev_stamp_ = current_stamp;
    }
  }
}

/*!
  \details No detailed description

  \return No description
  */
inline
const WorkerInfo& Epoch::workerInfo() const noexcept
{
  return worker_info_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Epoch::announcementList() noexcept -> std::span<AnnounceSlot>
{
  return announcement_list_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Epoch::announcementList() const noexcept -> std::span<const AnnounceSlot>
{
  return announcement_list_;
}

/*!
  \details No detailed description

  \tparam Thank No description.
  \param [in] func No description.
  \return No description
  */
template <std::invocable Thank> inline
std::invoke_result_t<Thank> Epoch::with(Thank&& func) noexcept
{
  announce();
  std::invoke_result_t<Thank> result = func();
  unannounce();
  return result;
}

/*!
  \details No detailed description
  */
inline
void Epoch::initialize() noexcept
{
  announcement_list_.clear();
  announcement_list_.resize(workerInfo().numOfWorkers());
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Epoch::timestamp() noexcept -> TimestampReference
{
  return *timestamp_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Epoch::timestamp() const noexcept -> ConstTimestampReference
{
  return *timestamp_;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_EPOCH_INL_HPP */
