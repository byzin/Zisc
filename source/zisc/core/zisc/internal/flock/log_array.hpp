/*!
  \file log_array.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_LOG_ARRAY_HPP
#define ZISC_FLOCK_LOG_ARRAY_HPP

// Standard C++ library
#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>
// Zisc
#include "definitions.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

// Forward declaration
template <typename> class EpochPoolImpl;

/*!
  \brief No brief description

  No detailed description.
  */
class LogArray : private NonCopyable<LogArray>
{
 public:
  // Type aliases
  using EntryT = LogEntry;
  using ConstEntryT = std::add_const_t<EntryT>;
  using EntryPtr = std::add_pointer_t<EntryT>;
  using ConstEntryPtr = std::add_pointer<ConstEntryT>;
  using EntryReference = std::add_lvalue_reference_t<EntryT>;
  using ConstEntryReference = std::add_lvalue_reference_t<ConstEntryT>;
  using MemoryPoolT = EpochPoolImpl<LogArray>;


  //! Create a log array
  LogArray() noexcept;

  //! Move data
  LogArray(LogArray&& other) noexcept;

  //! Destroy the log array
  ~LogArray() noexcept;


  //! Move data
  LogArray& operator=(LogArray&& other) noexcept;


  //!
  EntryReference operator[](const std::size_t index) noexcept;

  //!
  ConstEntryReference operator[](const std::size_t index) const noexcept;


  //! Destroy the log array
  void destroy(MemoryPoolT* pool) noexcept;

  //!
  EntryReference get(const std::size_t index) noexcept;

  //!
  ConstEntryReference get(const std::size_t index) const noexcept;

  //! Return the length of the underlying log array
  static constexpr std::size_t length() noexcept;

  //! Return the next array atomically
  std::atomic<LogArray*>& next() noexcept;

  //! Return the next array atomically
  const std::atomic<LogArray*>& next() const noexcept;

 private:
  //! Copy data
  void copy(LogArray& other) noexcept;

  //! Initialize the log array
  void initialize() noexcept;


  static constexpr std::size_t kLogLength = 8; //!< Default log length. Will grow if needed


  std::array<EntryT, kLogLength> entries_;
  std::atomic<LogArray*> next_;
};

} /* namespace zisc::flock */

#include "log_array-inl.hpp"

#endif /* ZISC_FLOCK_LOG_ARRAY_HPP */
