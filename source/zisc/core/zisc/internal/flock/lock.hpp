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
#include "tag.hpp"
#include "zisc/boolean.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

// Forward declaration
class Epoch;
template <typename> class EpochPoolImpl;
class Log;
class LogArray;
template <typename, typename> class MemoryPool;
template <typename> class TaggedPoolImpl;
class WriteAnnouncements;

/*!
  \brief No brief description

  No detailed description.
  */
template <bool kIsHelpUsed> 
class Lock : private NonCopyable<Lock<kIsHelpUsed>>
{
 public:
  // Type aliases
  using EntryT = LockEntry;
  template <std::invocable Thank>
  using ResultT = std::invoke_result_t<Thank>;
  template <std::invocable Thank>
  using ResultOptionT = std::optional<ResultT<Thank>>;
  using DescriptorPoolImplT = TaggedPoolImpl<Descriptor>;
  using DescriptorPoolT = MemoryPool<Descriptor, DescriptorPoolImplT>;
  using LogArrayPoolT = EpochPoolImpl<LogArray>;


  //! Create a lock
  Lock() noexcept;

  //! Move a data
  Lock(Lock&& other) noexcept;


  //! Move a data
  Lock& operator=(Lock&& other) noexcept;


  //!
  void clear() noexcept;

  //! Check if the lock uses help
  static constexpr bool isHelpUsed() noexcept;

  //!
  bool isLocked() const noexcept;

  //!
  bool isSelfLocked(const std::size_t current_id) const noexcept;

  //!
  template <std::invocable Thank>
  bool tryLock(Thank&& func,
               Epoch* epoch,
               WriteAnnouncements* write_announcements,
               Log* log,
               std::size_t* current_id,
               Boolean* helping,
               DescriptorPoolT* descriptor_pool) noexcept;

  //!
  template <std::invocable Thank>
  ResultOptionT<Thank> tryLockResult(Thank&& func,
                                     Epoch* epoch,
                                     WriteAnnouncements* write_announcements,
                                     Log* log,
                                     std::size_t* current_id,
                                     Boolean* helping,
                                     DescriptorPoolT* descriptor_pool) noexcept;

 private:
  // Type aliases
  using TaggedT = Tag<const Descriptor*>;


  /*!
    \brief No brief description

    The EntryT for help is organized as
    The lowest 48 bits are a pointer to a descriptor if locked, or null if not
    Highest 16 bits is a tag to avoid ABA problem
    */
  struct Help
  {
    //!
    static bool isLocked(const EntryT le) noexcept;

    //!
    static bool isSelfLocked(const EntryT le, const std::size_t current_id) noexcept;

    //!
    static const Descriptor* removeTag(const EntryT le) noexcept;
  };

  /*!
    \brief No brief description

    The lock_entry for no_help is organized as
    Lowest 32 bits are a count.  If odd then locked, if even then unlocked.
    Next 16 bits are the processor id who has the lock, for checking for reentry
    */
  struct NoHelp
  {
    //!
    static std::size_t getProcid(const EntryT lock) noexcept;

    //!
    static bool isLocked(const EntryT le) noexcept;

    //!
    static bool isSelfLocked(const EntryT le, const std::size_t current_id) noexcept;

    //!
    static std::size_t maskCnt(const EntryT lock) noexcept;

    //!
    static std::size_t releaseLock(const EntryT le) noexcept;

    //!
    static std::size_t takeLock(const EntryT le, const std::size_t current_id) noexcept;
  };


  //! Used to take lock for version with helping
  bool cas(EntryT old_value,
           const Descriptor* d,
           WriteAnnouncements* write_announcements,
           Log* log) noexcept;

  //! Used to take lock for version with no helping
  bool casSimple(EntryT old_value, const std::size_t value) noexcept;

  //!
  void clear(const Descriptor* d,
             WriteAnnouncements* write_announcements,
             Log* log) noexcept;

  //! Runs thunk in appropriate epoch and after it is acquired
  bool helpDescriptor(EntryT le,
                      Epoch* epoch,
                      WriteAnnouncements* write_announcements,
                      Log* log,
                      std::size_t* current_id,
                      Boolean* helping,
                      DescriptorPoolT* descriptor_pool,
                      const bool recursive_help = false) noexcept;

  //!
  EntryT load(Log* log) const noexcept;

  //!
  EntryT read() const noexcept;

  //!
  template <std::invocable Thank>
  ResultOptionT<Thank> tryLockHelp(Thank&& func,
                                   Epoch* epoch,
                                   WriteAnnouncements* write_announcements,
                                   Log* log,
                                   std::size_t* current_id,
                                   Boolean* helping,
                                   DescriptorPoolT* descriptor_pool) noexcept;

  //!
  template <std::invocable Thank>
  ResultOptionT<Thank> tryLockNoHelp(Thank&& func,
                                     Log* log,
                                     const std::size_t current_id) noexcept;


  std::atomic<EntryT> lock_;
};

} /* namespace zisc::flock */

#include "lock-inl.hpp"

#endif /* ZISC_FLOCK_LOCK_HPP */
