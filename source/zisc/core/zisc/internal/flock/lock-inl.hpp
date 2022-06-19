/*!
  \file lock-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_LOCK_INL_HPP
#define ZISC_FLOCK_LOCK_INL_HPP

#include "lock.hpp"
// Standard C++ library
#include <atomic>
#include <concepts>
#include <memory>
#include <numeric>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "descriptor.hpp"
#include "epoch.hpp"
#include "log.hpp"
#include "tag.hpp"
#include "worker_info.hpp"
#include "write_announcements.hpp"
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {


/*!
  \details No detailed description

  \param [in] worker_info No description.
  \param [in,out] mem_resource No description.
  */
inline
Lock::Lock(Epoch* epoch, pmr::memory_resource* mem_resource) noexcept :
    log_list_{decltype(log_list_)::allocator_type{mem_resource}},
    helping_list_{decltype(helping_list_)::allocator_type{mem_resource}},
    current_id_list_{decltype(current_id_list_)::allocator_type{mem_resource}},
    write_announcements_{mem_resource},
    tag_{&write_announcements_},
    lock_{0},
    descriptor_pool_{epoch, mem_resource}
{
  initialize();
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Lock::Lock(Lock&& other) noexcept :
    log_list_{std::move(other.log_list_)},
    helping_list_{std::move(other.helping_list_)},
    current_id_list_{std::move(other.current_id_list_)},
    write_announcements_{std::move(other.write_announcements_)},
    tag_{std::move(other.tag_)},
    lock_{other.lock_.load(std::memory_order::acquire)},
    descriptor_pool_{std::move(other.descriptor_pool_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Lock& Lock::operator=(Lock&& other) noexcept
{
  log_list_ = std::move(other.log_list_);
  helping_list_ = std::move(other.helping_list_);
  current_id_list_ = std::move(other.current_id_list_);
  write_announcements_ = std::move(other.write_announcements_);
  tag_ = std::move(other.tag_);
  lock_.store(other.lock_.load(std::memory_order::acquire), std::memory_order::release);
  descriptor_pool_ = std::move(other.descriptor_pool_);
  return *this;
}

/*!
  \details No detailed description

  \tparam Thank No description.
  \param [in] func No description.
  \param [in] try_only No description.
  \return No description
  */
template <std::invocable Thank> inline
bool Lock::tryLock(Thank&& func, const bool try_only) noexcept
{
  bool result = false;
  if (try_only) {
    const ResultOptionT<Thank> r = isHelpUsed()
        ? tryLockHelp(std::forward<Thank>(func))
        : tryLockNoHelp(std::forward<Thank>(func));
    result = r.has_value() && cast<bool>(r.value());
  }
  else {
    const ResultT<Thank> r = isHelpUsed()
        ? withLockHelp(std::forward<Thank>(func))
        : withLockNoHelp(std::forward<Thank>(func));
    result = cast<bool>(r);
  }
  return result;
}

/*!
  \details No detailed description

  \tparam Thank No description.
  \param [in] func No description.
  \param [in] try_only No description.
  \return No description
  */
template <std::invocable Thank> inline
auto Lock::tryLockResult(Thank&& func, const bool try_only) noexcept
    -> ResultOptionT<Thank>
{
  ResultOptionT<Thank> result{};
  if (try_only) {
    result = isHelpUsed()
        ? tryLockHelp(std::forward<Thank>(func))
        : tryLockNoHelp(std::forward<Thank>(func));
  }
  else {
    ResultT<Thank> r = isHelpUsed()
        ? withLockHelp(std::forward<Thank>(func))
        : withLockNoHelp(std::forward<Thank>(func));
    result = std::make_optional(std::move(r));
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] info No description.
  */
inline
void Lock::setWorkerInfo(const WorkerInfo& info) noexcept
{
  log_list_.resize(info.numOfWorkers());
  helping_list_.resize(info.numOfWorkers(), false);
  current_id_list_.resize(info.numOfWorkers());
  std::iota(current_id_list_.begin(), current_id_list_.end(), 0);
  tag_.setLogList(log_list_, info);
  write_announcements_.setWorkerInfo(info);
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
inline
bool Lock::Help::isLocked(const EntryT le) noexcept
{
  const bool result = TagT::value(le) != nullptr;
  return result;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
inline
const Descriptor* Lock::Help::removeTag(const EntryT le) noexcept
{
  const Descriptor* d = TagT::value(le);
  return d;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \param [in] info No description.
  \return No description
  */
inline
bool Lock::Help::isLockSelf(const EntryT le, const WorkerInfo& info) noexcept
{
  const std::size_t thread_id = info.getCurrentWorkerId();
  const bool result = thread_id == removeTag(le)->threadId();
  return result;
}
 
/*!
  \details No detailed description

  \param [in] old_value No description.
  \param [in] d No description.
  \return No description
  */
inline
bool Lock::cas(EntryT old_value, const Descriptor* d) noexcept
{
  const EntryT current = read();
  const bool result = (current == old_value) && tag_.cas(lock_, old_value, d);
  return result;
}

/*!
  \details No detailed description

  \param [in] old_value No description.
  \param [in] d No description.
  \return No description
  */
inline
bool Lock::casSimple(EntryT old_value, const std::size_t value) noexcept
{
  const bool result = lock_.compare_exchange_strong(old_value,
                                                    bit_cast<EntryT>(value),
                                                    std::memory_order::acq_rel,
                                                    std::memory_order::acquire);
  return result;
}

/*!
  \details No detailed description

  \param [in] d No description.
  */
inline
void Lock::clear(const Descriptor* d) noexcept
{
  EntryT current = lock_.load(std::memory_order::acquire);
  if (tag_.value(current) == d) {
    // true indicates this is ABA free since current cannot
    // be reused until all helpers are done with it.
    tag_.cas(lock_, current, nullptr, true);
  }
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::span<std::size_t> Lock::currentIdList() noexcept
{
  return current_id_list_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::span<const std::size_t> Lock::currentIdList() const noexcept
{
  return current_id_list_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
Epoch& Lock::epoch() noexcept
{
  return *epoch_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const Epoch& Lock::epoch() const noexcept
{
  return *epoch_;
}

inline
bool Lock::helpDescriptor(EntryT le, const bool recursive_help) noexcept
{
  bool& helping = workerInfo().takeOut(helpingList());
  if (!recursive_help && helping)
    return false;

  const Descriptor* desc = Help::removeTag(le);
  bool still_locked = read() == le;
  if (!still_locked)
    return false;

  Epoch::ValueT my_epoch = epoch().getMyEpoch();
  Epoch::ValueT other_epoch = desc->epochNum();
  if (other_epoch < my_epoch)
    epoch().setMyEpoch(other_epoch); // inherit epoch of helpee

  std::size_t& current_id = workerInfo().takeOut(currentIdList());
  const std::size_t my_id = current_id;
  current_id = desc->threadId();
  Descriptor* d = const_cast<Descriptor*>(desc);
  descriptor_pool_.acquire(d);
  still_locked = read() == le;
  if (still_locked) {
    const bool hold_h = helping;
    helping = true;
    (*d)();
    clear(d);
    helping = hold_h;
  }
  current_id = my_id; // reset thread id
  epoch().setMyEpoch(my_epoch);
  return still_locked;
}

/*!
  \details No detailed description
  */
inline
void Lock::initialize() noexcept
{
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Lock::load() noexcept -> EntryT
{
  Log& log = workerInfo().takeOut(logList());
  const EntryT v = lock_.load(std::memory_order::acquire);
  const EntryT result = log.commitValue(v).first;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::span<Log> Lock::logList() noexcept
{
  return log_list_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::span<const Log> Lock::logList() const noexcept
{
  return log_list_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Lock::read() const noexcept -> EntryT
{
  const EntryT result = lock_.load(std::memory_order::acquire);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const WorkerInfo& Lock::workerInfo() const noexcept
{
  return write_announcements_.workerInfo();
}

/*!
  \details No detailed description

  \tparam Thunk No description.
  \param [in] func No description.
  \return No description
  */
template <std::invocable Thank> inline
auto Lock::withLockHelp(Thank&& func) noexcept -> ResultT<Thank>
{
  using ReturnT = ResultT<Thank>;
  static_assert(sizeof(ReturnT) <= 4 || std::is_pointer_v<ReturnT>,
                "Result of tryLockResult must be a pointer or at most 4 bytes.");
  EntryT current = read();

  auto [my_descriptor, i_own] = descriptor_pool_.newObjAcquired(this, func, current);
  if (descriptor_pool_.isDone(my_descriptor)) { // if already retired, then done
    ResultOptionT<Thank> r = descriptor_pool_.doneValueResult<ReturnT>(my_descriptor);
    ZISC_ASSERT(r.has_value(), ""); // with_lock is guaranteed to succeed
    return r.value();
  }

  bool locked = Help::isLocked(current);
  Log& log = WorkerInfo().takeOut(logList());
  while (true) {
    if (my_descriptor->isDone() || // already done
        (Help::removeTag(current) == my_descriptor) || // already acquired
        (!locked && cas(current, my_descriptor))) { // successfully acquired
      // Run the body func with the log from my_descriptor
      ReturnT result = log.doWith(&my_descriptor->logArray(), 0, [&func]()
      {
        return func();
      });

      // Mark as done and clear the lock
      my_descriptor->setDone(true);
      clear(my_descriptor);

      // Retire the descriptor saving the result in the enclosing descriptor is any
      descriptor_pool_.retireAcquiredResult(my_descriptor,
                                            i_own,
                                            std::optional<ReturnT>(result));
      return result;
    }
    else if (locked) {
      helpDescriptor(current);
    }
    current = read();
    locked = Help::isLocked(current);
  }
}

/*!
  \details No detailed description

  \tparam Thank No description.
  \param [in] func No description.
  \return No description
  */
template <std::invocable Thank> inline
auto Lock::tryLockHelp(Thank&& func) noexcept -> ResultOptionT<Thank>
{
  ResultOptionT<Thank> result{};
  EntryT current = load();

  // check if reentrant lock (already locked by self)
  if (Help::isLocked(current) && Help::isLockSelf(current, workerInfo())) {
    // If so, run without acquiring
    return std::make_optional(func());
  }

  // idempotent allocation of descriptor
  // storing current into descriptor saves one logging event since they are committed
  // together
  auto [my_descriptor, i_own] = descriptor_pool_.newObjAcquired(this, func, current);

  // If descriptor is already retired, then done and return value
  if (descriptor_pool_.isDone(my_descriptor))
    return descriptor_pool_.doneValueResult<ResultT<Thank>>(my_descriptor);

  // Retrieve agreed upon current for idempotence
  // current = my_descriptor->current
  if (!Help::isLocked(current)) {
    // Use a CAS to try to acquire the lock
    cas(current, my_descriptor);

    // Could be a load() without the my_descriptor->done test
    // Using read() is an optimization to avoid a logging event
    current = read();
    if (my_descriptor->isDone() || Help::removeTag(current) == my_descriptor) {
      // Run func with log from my_descriptor
      Log& log = WorkerInfo().takeOut(logList());
      result = log.doWith(&my_descriptor->logArray(), 0, [&func]()
      {
        return func();
      });

      // Mark as done and clear the lock
      my_descriptor->setDone(true);
      clear(my_descriptor);
    }
  }
  else {
    helpDescriptor(current);
  }

  // Retire the thunk
  descriptor_pool_.retireAcquiredResult(my_descriptor, i_own, result);
  return result;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_LOCK_INL_HPP */
