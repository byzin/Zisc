/*!
  \file lock.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_LOCK_HPP
#define ZISC_FLOCK_LOCK_HPP

// Standard C++ library
#include <atomic>
#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <vector>
// Zisc
#include "definitions.hpp"
#include "descriptor.hpp"
#include "log.hpp"
#include "memory_pool.hpp"
#include "tag.hpp"
#include "tagged_pool_impl.hpp"
#include "write_announcements.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

// Forward declaration
class Epoch;
class WorkerInfo;

/*!
  \brief No brief description

  No detailed description.
  */
class Lock : private NonCopyable<Lock>
{
 public:
  // Type aliases
  using EntryT = LockEntry;
  template <std::invocable Thank>
  using ResultT = std::invoke_result_t<Thank>;
  template <std::invocable Thank>
  using ResultOptionT = std::optional<ResultT<Thank>>;


  static constexpr bool kDefaultTryOnly = true;


  //! Create a lock
  Lock(Epoch* epoch, pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  Lock(Lock&& other) noexcept;


  //! Move a data
  Lock& operator=(Lock&& other) noexcept;


  //!
  void clear() noexcept;

  //!
  bool isLocked() const noexcept;

  //!
  template <std::invocable Thank>
  bool tryLock(Thank&& func, const bool try_only = kDefaultTryOnly) noexcept;

  //!
  template <std::invocable Thank>
  ResultOptionT<Thank> tryLockResult(Thank&& func,
                                     const bool try_only = kDefaultTryOnly) noexcept;

  //! Check if the lock uses help
  static constexpr bool isHelpUsed() noexcept;

  //! Set worker info
  void setWorkerInfo(const WorkerInfo& info) noexcept;

 private:
  // Type aliases
  using TagT = Tag<const Descriptor*>;
  using MemoryPoolImplT = TaggedPoolImpl<Descriptor>;


  // The EntryT for help is organized as
  // The lowest 48 bits are a pointer to a descriptor if locked, or null if not
  // Highest 16 bits is a tag to avoid ABA problem
  struct Help
  {
    //!
    static bool isLocked(const EntryT le) noexcept;

    //!
    static const Descriptor* removeTag(const EntryT le) noexcept;

    //!
    static bool isLockSelf(const EntryT le, const WorkerInfo& info) noexcept;
  };


  //!
  bool cas(EntryT old_value, const Descriptor* d) noexcept;

  //!
  bool casSimple(EntryT old_value, const std::size_t value) noexcept;

  //!
  void clear(const Descriptor* d) noexcept;

  //! Return the current id list
  std::span<std::size_t> currentIdList() noexcept;

  //! Return the current id list
  std::span<const std::size_t> currentIdList() const noexcept;

  //! Return the underlying epoch
  Epoch& epoch() noexcept;

  //! Return the underlying epoch
  const Epoch& epoch() const noexcept;

  //!
  bool helpDescriptor(EntryT le, const bool recursive_help = false) noexcept;

  //! Return the underlying helping list
  std::span<bool> helpingList() noexcept;

  //! Return the underlying helping list
  std::span<const bool> helpingList() const noexcept;

  //! Initialize the lock
  void initialize() noexcept;

  //!
  EntryT load() noexcept;

  //! Return the underlying log list for workers
  std::span<Log> logList() noexcept;

  //! Return the underlying log list for workers
  std::span<const Log> logList() const noexcept;

  //!
  EntryT read() const noexcept;

  //!
  template <std::invocable Thank>
  ResultT<Thank> withLockHelp(Thank&& func) noexcept;

  //!
  template <std::invocable Thank>
  ResultT<Thank> withLockNoHelp(Thank&& func) noexcept;

  //! Return the underlying worker info
  const WorkerInfo& workerInfo() const noexcept;

  //!
  template <std::invocable Thank>
  ResultOptionT<Thank> tryLockHelp(Thank&& func) noexcept;

  //!
  template <std::invocable Thank>
  ResultOptionT<Thank> tryLockNoHelp(Thank&& func) noexcept;


  pmr::vector<Log> log_list_;
  pmr::vector<bool> helping_list_;
  pmr::vector<std::size_t> current_id_list_;
  WriteAnnouncements write_announcements_;
  TagT tag_;
  std::atomic<EntryT> lock_;
  MemoryPool<Descriptor, MemoryPoolImplT> descriptor_pool_;
  Epoch* epoch_;
};

} /* namespace zisc::flock */

#include "lock-inl.hpp"

#endif /* ZISC_FLOCK_LOCK_HPP */
