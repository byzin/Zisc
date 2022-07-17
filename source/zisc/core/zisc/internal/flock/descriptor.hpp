/*!
  \file descriptor.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_DESCRIPTOR_HPP
#define ZISC_FLOCK_DESCRIPTOR_HPP

// Standard C++ library
#include <atomic>
#include <concepts>
#include <cstddef>
#include <optional>
#include <span>
// Zisc
#include "definitions.hpp"
#include "epoch.hpp"
#include "log_array.hpp"
#include "zisc/function_reference.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

// Forward declaration
class Log;
class WorkerInfo;

/*!
  \brief Store the thunk along with the log

  No detailed description.
  */
class Descriptor : private NonCopyable<Descriptor>
{
 public:
  // Type aliases
  using EntryT = LockEntry;
  using ThunkT = FunctionReference<std::optional<std::size_t> ()>;

  //! Create a descriptor
  Descriptor(ThunkT func,
             const EntryT current,
             const Epoch& epoch,
             const std::size_t current_id) noexcept;

  //! Move a data
  Descriptor(Descriptor&& other) noexcept;

  //! Destroy the descriptor
  ~Descriptor() noexcept;


  //! Move a data
  Descriptor& operator=(Descriptor&& other) noexcept;

  //!
  std::optional<std::size_t> operator()(Log* log) noexcept;


  //! Return the underlying acquired flag
  std::atomic_flag& acquired() noexcept;

  //! Return the underlying acquired flag
  const std::atomic_flag& acquired() const noexcept;

  //! Return the epoch num
  Epoch::ValueT epochNum() const noexcept;

  //! Check if the descriptor is done
  bool isDone() const noexcept;

  //! Return the log array
  LogArray& logArray() noexcept;

  //! Return the log array
  const LogArray& logArray() const noexcept;

  //!
  std::optional<std::size_t> run(Log* log) noexcept;

  //!
  void setDone(const bool done) noexcept;

  //! Return the underlying thread id
  std::size_t threadId() const noexcept;

 private:
  //! Initialize the descriptor
  void initialize(const bool acquired_flag) noexcept;


  ThunkT func_;
  EntryT current_;
  LogArray log_array_;
  Epoch::ValueT epoch_num_;
  std::size_t thread_id_;
  std::atomic_flag acquired_; //!< indicates thunk is being helped, lives beyond the 'lifetime' of the descriptor
  bool done_;
  bool freed_; //!< Just for debugging
};

} /* namespace zisc::flock */

#include "descriptor-inl.hpp"

#endif /* ZISC_FLOCK_DESCRIPTOR_HPP */
