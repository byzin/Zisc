/*!
  \file log.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_LOG_HPP
#define ZISC_FLOCK_LOG_HPP

// Standard C++ library
#include <concepts>
#include <type_traits>
#include <utility>
// Zisc
#include "definitions.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

// Forward declaration
class LogArray;
template <typename> class EpochPoolImpl;

/*!
  \brief No brief description

  the log is maintained per thread and keeps track of all committed values for a
  thunk the log_array pointed to by vals is shared among all thread processing the
  same thunk the count is separate for each thread, indicating how many commits it
  has done whichever thread commits first at a given position in the log_array
  sticks such that all future commits will see it (i.e., whoever arrives first
  wins). commit_value commits a value to the log at the current position.  The value
  cannot be zero (nullptr). commit_value_safe allows the value to be zero but uses
  bit 48 as a marked flag so it only supports up to 6 bytes It supports pointers
  assuming they fit within 6 bytes (i.e. upper 2 bytes are empty).

  \note No notation.
  \attention No attention.
  */
class Log : private NonCopyable<Log>
{
 public:
  // Type aliases
  using EntryT = LogEntry;
  using ConstEntryT = std::add_const_t<EntryT>;
  using EntryPtr = std::add_pointer_t<EntryT>;
  using ConstEntryPtr = std::add_pointer_t<ConstEntryT>;
  using EntryReference = std::add_lvalue_reference_t<EntryT>;
  using ConstEntryReference = std::add_lvalue_reference_t<ConstEntryT>;
  template <typename Type>
  using CommitResultT = std::pair<std::remove_cvref_t<Type>, bool>;
  using MemoryPoolT = EpochPoolImpl<LogArray>;


  //! Create a empty log
  Log() noexcept;

  //! Move data
  Log(Log&& other) noexcept;

  //! Destroyt the log
  ~Log() noexcept;


  //! Move data
  Log& operator=(Log&& other) noexcept;


  //!
  template <typename Type>
  CommitResultT<Type> commitValue(Type&& value) noexcept;

  //!
  template <typename Type>
  CommitResultT<Type> commitValueSafe(Type&& value) noexcept;

  //! Return the count
  std::size_t count() const noexcept;

  //!
  EntryPtr currentEntry() noexcept;

  //!
  ConstEntryPtr currentEntry() const noexcept;

  //!
  template <std::invocable Function>
  std::invoke_result_t<Function> doWith(Function&& func,
                                        LogArray* pa,
                                        const std::size_t c) noexcept;

  //! Check if the log is empty
  bool isEmpty() const noexcept;

  //!
  EntryPtr nextEntry() noexcept;

  //! Set a memory pool for log array
  void setLogArrayPool(MemoryPoolT* log_array_pool) noexcept;

  //! Skip a chunk of code if finished by another helper on the log
  template <std::invocable Function>
  bool skipIfDone(Function&& func) noexcept;

 private:
  //!
  void set(LogArray* pa, const std::size_t c) noexcept;


  MemoryPoolT* log_array_pool_;
  LogArray* values_;
  std::size_t count_;
};

} /* namespace zisc::flock */

#include "log-inl.hpp"

#endif /* ZISC_FLOCK_LOG_HPP */
