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
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "descriptor.hpp"
#include "epoch.hpp"
#include "log.hpp"
#include "memory_pool.hpp"
#include "tag.hpp"
#include "tagged_pool_impl.hpp"
#include "worker_info.hpp"
#include "zisc/boolean.hpp"
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {


/*!
  \details No detailed description
  */
template <bool kIsHelpUsed> inline
Lock<kIsHelpUsed>::Lock() noexcept :
    lock_{TaggedT::init(nullptr)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <bool kIsHelpUsed> inline
Lock<kIsHelpUsed>::Lock(Lock&& other) noexcept :
    lock_{other.lock_.load(std::memory_order::acquire)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <bool kIsHelpUsed> inline
auto Lock<kIsHelpUsed>::operator=(Lock&& other) noexcept -> Lock&
{
  lock_.store(other.lock_.load(std::memory_order::acquire), std::memory_order::release);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <bool kIsHelpUsed> inline
constexpr bool Lock<kIsHelpUsed>::isHelpUsed() noexcept
{
  return kIsHelpUsed;
}

/*!
  \details No detailed description

  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::isLocked() const noexcept
{
  using HelpT = std::conditional_t<isHelpUsed(), Help, NoHelp>;
  const EntryT le = lock_.load(std::memory_order::acquire);
  return HelpT::isLocked(le);
}

/*!
  \details No detailed description

  \param [in] current_id No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::isSelfLocked(const std::size_t current_id) const noexcept
{
  using HelpT = std::conditional_t<isHelpUsed(), Help, NoHelp>;
  const EntryT le = lock_.load(std::memory_order::acquire);
  return HelpT::isSelfLocked(le, current_id);
}

/*!
  \details No detailed description

  \tparam Thank No description.
  \param [in] func No description.
  \param [in,out] epoch No description.
  \param [in,out] write_announcements No description.
  \param [in,out] log No description.
  \param [in,out] current_id No description.
  \param [in,out] helping No description.
  \param [in,out] descriptor_pool No description.
  \return No description
  */
template <bool kIsHelpUsed> template <std::invocable Thank> inline
auto Lock<kIsHelpUsed>::tryLock(Thank&& func,
                                Epoch* epoch,
                                WriteAnnouncements* write_announcements,
                                Log* log,
                                std::size_t* current_id,
                                Boolean* helping,
                                DescriptorPoolT* descriptor_pool) noexcept
    -> ResultOptionalT<Thank>
{
  ResultOptionalT<Thank> result = isHelpUsed()
      ? tryLockHelp(std::forward<Thank>(func), epoch, write_announcements, log, current_id, helping, descriptor_pool)
      : tryLockNoHelp(std::forward<Thank>(func), *current_id);
  return result;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::Help::isLocked(const EntryT le) noexcept
{
  const bool result = TaggedT::value(le) != nullptr;
  return result;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \param [in] current_id No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::Help::isSelfLocked(const EntryT le,
                                           const std::size_t current_id) noexcept
{
  const bool result = current_id == removeTag(le)->threadId();
  return result;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
const Descriptor* Lock<kIsHelpUsed>::Help::removeTag(const EntryT le) noexcept
{
  const Descriptor* d = TaggedT::value(le);
  return d;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
std::size_t Lock<kIsHelpUsed>::NoHelp::getProcid(const EntryT lock) noexcept
{
  const std::size_t result = (lock >> 32) & ((1ull << 16) - 1);
  return result;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::NoHelp::isLocked(const EntryT le) noexcept
{
  const bool result = (le % 2) == 1;
  return result;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \param [in] current_id No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::NoHelp::isSelfLocked(const EntryT le,
                                             const std::size_t current_id) noexcept
{
  const bool result = (current_id + 1) == getProcid(le);
  return result;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
std::size_t Lock<kIsHelpUsed>::NoHelp::maskCnt(const EntryT lock) noexcept
{
  const std::size_t result = lock & ((1ull << 32) - 1);
  return result;
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
std::size_t Lock<kIsHelpUsed>::NoHelp::releaseLock(const EntryT le) noexcept
{
  return maskCnt(le + 1);
}

/*!
  \details No detailed description

  \param [in] le No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
std::size_t Lock<kIsHelpUsed>::NoHelp::takeLock(const EntryT le,
                                                const std::size_t current_id) noexcept
{
  const std::size_t result = ((current_id + 1) << 32) | maskCnt(le + 1);
  return result;
}

/*!
  \details No detailed description

  \param [in] old_value No description.
  \param [in] d No description.
  \param [in,out] write_announcements No description.
  \param [in,out] log No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::cas(EntryT old_value,
                            const Descriptor* d,
                            WriteAnnouncements* write_announcements,
                            Log* log) noexcept
{
  const EntryT current = read();
  const bool result = (current == old_value) && TaggedT::cas(lock_,
                                                             old_value,
                                                             d,
                                                             write_announcements,
                                                             log);
  return result;
}

/*!
  \details No detailed description

  \param [in] old_value No description.
  \param [in] d No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::casSimple(EntryT old_value, const std::size_t value) noexcept
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
template <bool kIsHelpUsed> inline
void Lock<kIsHelpUsed>::clear(const Descriptor* d,
                              WriteAnnouncements* write_announcements,
                              Log* log) noexcept
{
  EntryT current = lock_.load(std::memory_order::acquire);
  if (TaggedT::value(current) == d) {
    // true indicates this is ABA free since current cannot
    // be reused until all helpers are done with it.
    TaggedT::cas(lock_, current, nullptr, write_announcements, log, true);
  }
}

/*!
  \details No detailed description

  \param [in] le No description.
  \param [in,out] epoch No description.
  \param [in,out] write_announcements No description.
  \param [in,out] log No description.
  \param [in,out] current_id No description.
  \param [in,out] helping No description.
  \param [in,out] descriptor_pool No description.
  \return No description
  */
template <bool kIsHelpUsed> inline
bool Lock<kIsHelpUsed>::helpDescriptor(EntryT le,
                                       Epoch* epoch,
                                       WriteAnnouncements* write_announcements,
                                       Log* log,
                                       std::size_t* current_id,
                                       Boolean* helping,
                                       DescriptorPoolT* descriptor_pool,
                                       const bool recursive_help) noexcept
{
  if (!recursive_help && *helping)
    return false;

  Descriptor* desc = const_cast<Descriptor*>(Help::removeTag(le));
  bool still_locked = read() == le;
  if (!still_locked)
    return false;

  Epoch::ValueT my_epoch = epoch->getMyEpoch();
  Epoch::ValueT other_epoch = desc->epochNum();
  if (other_epoch < my_epoch)
    epoch->setMyEpoch(other_epoch); // inherit epoch of helpee

  const std::size_t my_id = *current_id;
  *current_id = desc->threadId(); // inherit thread id of helpee
  descriptor_pool->acquire(desc);
  still_locked = read() == le;
  if (still_locked) {
    const bool hold_h = *helping;
    *helping = true;
    (*desc)(log);
    clear(desc, write_announcements, log);
    *helping = hold_h;
  }
  *current_id = my_id; // reset thread id
  epoch->setMyEpoch(my_epoch); // reset to my epoch
  return still_locked;
}

/*!
  \details No detailed description

  \param [in] log No description.
  */
template <bool kIsHelpUsed> inline
auto Lock<kIsHelpUsed>::load(Log* log) const noexcept -> EntryT
{
  const EntryT v = lock_.load(std::memory_order::acquire);
  const EntryT result = log->commitValue(v).first;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <bool kIsHelpUsed> inline
auto Lock<kIsHelpUsed>::read() const noexcept -> EntryT
{
  const EntryT result = lock_.load(std::memory_order::acquire);
  return result;
}

/*!
  \details No detailed description

  \tparam Thank No description.
  \param [in] func No description.
  \param [in,out] epoch No description.
  \param [in,out] write_announcements No description.
  \param [in,out] log No description.
  \param [in,out] current_id No description.
  \param [in,out] helping No description.
  \param [in,out] descriptor_pool No description.
  \return No description
  */
template <bool kIsHelpUsed> template <std::invocable Thank> inline
auto Lock<kIsHelpUsed>::tryLockHelp(Thank&& func,
                                    Epoch* epoch,
                                    WriteAnnouncements* write_announcements,
                                    Log* log,
                                    std::size_t* current_id,
                                    Boolean* helping,
                                    DescriptorPoolT* descriptor_pool) noexcept
    -> ResultOptionalT<Thank>
{
  EntryT current = load(log);
  // check if reentrant lock (already locked by self)
  if (Help::isLocked(current) && Help::isSelfLocked(current, *current_id)) {
    // If so, run without acquiring
    return std::make_optional(func());
  }

  // idempotent allocation of descriptor
  // storing current into descriptor saves one logging event since they are committed
  // together
  auto [my_desc, i_own] = descriptor_pool->newObjAcquired(func, current, *epoch, *current_id);

  // If descriptor is already retired, then done and return value
  using ResultT = typename ResultOptionalT<Thank>::value_type;
  if (descriptor_pool->isDone(my_desc))
    return descriptor_pool->doneValueResult<ResultT>(my_desc);

  ResultOptionalT<Thank> result{};
  // Retrieve agreed upon current for idempotence
  // current = my_desc->current
  if (!Help::isLocked(current)) {
    // Use a CAS to try to acquire the lock
    cas(current, my_desc, write_announcements, log);

    // Could be a load() without the my_desc->done test
    // Using read() is an optimization to avoid a logging event
    current = read();
    if (my_desc->isDone() || (Help::removeTag(current) == my_desc)) {
      // Run func with log from my_desc
      result = log->doWith(&my_desc->logArray(), 0, [&func]()
      {
        return func();
      });

      // Mark as done and clear the lock
      my_desc->setDone(true);
      clear(my_desc, write_announcements, log);
    }
  }
  else {
    helpDescriptor(current, epoch, write_announcements, log, current_id, helping, descriptor_pool);
  }

  // Retire the thunk
  descriptor_pool->retireAcquiredResult(my_desc, i_own, result);
  return result;
}

/*!
  \details No detailed description

  \param [in] func No description.
  \param [in] epoch No description.
  \return No description
  */
template <bool kIsHelpUsed> template <std::invocable Thank> inline
auto Lock<kIsHelpUsed>::tryLockNoHelp(Thank&& func,
                                      const std::size_t current_id) noexcept
    -> ResultOptionalT<Thank>
{
  ResultOptionalT<Thank> result{};
  EntryT current = read();
  if (!NoHelp::isLocked(current)) { // unlocked
    std::size_t new_l = NoHelp::takeLock(current, current_id);
    if (casSimple(current, new_l)) { // try lock
      auto r = func();
      lock_.store(NoHelp::releaseLock(new_l), std::memory_order::release); // release
      result = std::move(r);
    }
  }
  else if (NoHelp::isSelfLocked(current, current_id)) { // reentry
    result = func();
  }
  return result;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_LOCK_INL_HPP */
