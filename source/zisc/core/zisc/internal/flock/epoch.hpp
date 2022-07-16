/*!
  \file epoch.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_EPOCH_HPP
#define ZISC_FLOCK_EPOCH_HPP

// Standard C++ library
#include <atomic>
#include <concepts>
#include <cstddef>
#include <memory>
#include <span>
#include <thread>
#include <type_traits>
#include <vector>
// Zisc
#include "timestamp.hpp"
#include "timestamp_simple.hpp"
#include "worker_info.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

/*!
  \brief

  Flock epoch class.
  github: <a href="https://github.com/cmuparlay/flock">Flock</a>
  Timestamps are distinct from epochs and are for multiversioning (snapshots)
  any time stamps less or equal to done_stamp indicate it is safe to collect them
  (noone will travel through them)

  \note No notation.
  \attention No attention.
  */
class Epoch : private NonCopyable<Epoch>
{
 public:
  // Type aliases
  using TimestampImplT = TimestampSimple;
  using TimestampT = Timestamp<TimestampImplT>;
  using ConstTimestampT = std::add_const_t<TimestampT>;
  using TimestampPtr = std::add_pointer_t<TimestampT>;
  using ConstTimestampPtr = std::add_pointer_t<ConstTimestampT>;
  using TimestampReference = std::add_lvalue_reference_t<TimestampT>;
  using ConstTimestampReference = std::add_lvalue_reference_t<ConstTimestampT>;
  using ValueT = typename TimestampT::ValueT;

  /*!
    \brief No brief description

    No detailed description.
    */
  struct alignas(64) AnnounceSlot : private NonCopyable<AnnounceSlot>
  {
    //! Create a slot
    AnnounceSlot() noexcept;

    //! Move data
    AnnounceSlot(AnnounceSlot&& other) noexcept;

    //! Move data
    AnnounceSlot& operator=(AnnounceSlot&& other) noexcept;

    std::atomic<ValueT> last_{-1};
    [[maybe_unused]] Padding<64 - std::alignment_of_v<decltype(last_)>> pad_;
  };


  //! Create a epoch
  Epoch(const std::span<const std::thread::id> id_list,
        TimestampPtr timestamp,
        pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  Epoch(Epoch&& other) noexcept;


  //! Move a data
  Epoch& operator=(Epoch&& other) noexcept;


  //!
  void announce() noexcept;

  //!
  ValueT getCurrent() const noexcept;

  //!
  ValueT getMyEpoch() const noexcept;

  //!
  void setMyEpoch(const ValueT e) noexcept;

  //!
  void unannounce() noexcept;

  //!
  void updateEpoch() noexcept;

  //!
  template <std::invocable Thank>
  std::invoke_result_t<Thank> with(Thank&& func) noexcept;

  //! Return the underlying worker info
  const WorkerInfo& workerInfo() const noexcept;

 private:
  //! Return the underlying announcement list
  std::span<AnnounceSlot> announcementList() noexcept;

  //! Return the underlying announcement list
  std::span<const AnnounceSlot> announcementList() const noexcept;

  //! Initialize the epoch
  void initialize() noexcept;

  //! Return the underlying timestamp
  TimestampReference timestamp() noexcept;

  //! Return the underlying timestamp
  ConstTimestampReference timestamp() const noexcept;


  pmr::vector<AnnounceSlot> announcement_list_;
  WorkerInfo worker_info_;
  TimestampPtr timestamp_;
  std::atomic<ValueT> current_epoch_;
  ValueT done_stamp_;
  ValueT prev_stamp_;
};

} /* namespace zisc::flock */

#include "epoch-inl.hpp"

#endif /* ZISC_FLOCK_EPOCH_HPP */
