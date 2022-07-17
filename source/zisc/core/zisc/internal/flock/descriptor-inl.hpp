/*!
  \file descriptor-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_DESCRIPTOR_INL_HPP
#define ZISC_FLOCK_DESCRIPTOR_INL_HPP

#include "descriptor.hpp"
// Standard C++ library
#include <atomic>
#include <concepts>
#include <cstddef>
#include <limits>
#include <optional>
#include <span>
// Zisc
#include "definitions.hpp"
#include "epoch.hpp"
#include "log.hpp"
#include "log_array.hpp"
#include "worker_info.hpp"
#include "zisc/function_reference.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \param [in] func No description.
  \param [in] current No description.
  \param [in] epoch No description.
  \param [in] current_id No description.
  */
inline
Descriptor::Descriptor(ThunkT func,
                       const EntryT current,
                       const Epoch& epoch,
                       const std::size_t current_id) noexcept :
    func_{func},
    current_{current},
    epoch_num_{epoch.getMyEpoch()},
    thread_id_{current_id},
    done_{false},
    freed_{false}
{
  initialize(false);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Descriptor::Descriptor(Descriptor&& other) noexcept :
    func_{std::move(other.func_)},
    current_{std::move(other.current_)},
    log_array_{std::move(other.log_array_)},
    epoch_num_{std::move(other.epoch_num_)},
    thread_id_{std::move(other.thread_id_)},
    done_{other.done_},
    freed_{other.freed_}
{
  initialize(other.acquired().test(std::memory_order::acquire));
}

/*!
  \details No detailed description
  */
inline
Descriptor::~Descriptor() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
Descriptor& Descriptor::operator=(Descriptor&& other) noexcept
{
  func_ = std::move(other.func_);
  current_ = std::move(other.current_);
  log_array_ = std::move(other.log_array_);
  epoch_num_ = std::move(other.epoch_num_);
  thread_id_ = std::move(other.thread_id_);
  done_ = other.done_;
  freed_ = other.freed_;
  initialize(other.acquired().test(std::memory_order::acquire));
  return *this;
}

/*!
  \details No detailed description

  \param [in] log No description.
  */
inline
std::optional<std::size_t> Descriptor::operator()(Log* log) noexcept
{
  return run(log);
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic_flag& Descriptor::acquired() noexcept
{
  return acquired_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic_flag& Descriptor::acquired() const noexcept
{
  return acquired_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
Epoch::ValueT Descriptor::epochNum() const noexcept
{
  return epoch_num_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool Descriptor::isDone() const noexcept
{
  return done_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
LogArray& Descriptor::logArray() noexcept
{
  return log_array_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const LogArray& Descriptor::logArray() const noexcept
{
  return log_array_;
}

/*!
  \details No detailed description
  */
inline
std::optional<std::size_t> Descriptor::run(Log* log) noexcept
{
  // run f using log based on lg_array
  std::optional<std::size_t> result = log->doWith(&log_array_, 0, func_);
  done_ = true;
  return result;
}

/*!
  \details No detailed description

  \param [in] done No description.
  */
inline
void Descriptor::setDone(const bool done) noexcept
{
  done_ = done;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Descriptor::threadId() const noexcept
{
  return thread_id_;
}

/*!
  \details No detailed description

  \param [in] acquired_flag No description.
  */
inline
void Descriptor::initialize(const bool acquired_flag) noexcept
{
  if (acquired_flag)
    acquired().test_and_set(std::memory_order::release);
  else
    acquired().clear(std::memory_order::release);
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_DESCRIPTOR_INL_HPP */
