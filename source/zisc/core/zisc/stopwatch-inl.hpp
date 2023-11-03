/*!
  \file stopwatch-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STOPWATCH_INL_HPP
#define ZISC_STOPWATCH_INL_HPP

#include "stopwatch.hpp"
// Standard C++ library
#include <chrono>
#include <cstdint>
#include <utility>

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::elapsedTime() const noexcept -> ClockT::duration
{
  ClockT::duration time = elapsed_time_;
  if (isRunning())
    time += elapsedRunningTime();
  return time;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::isIdle() const noexcept -> bool
{
  return state_ == State::Idle;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::isPausing() const noexcept -> bool
{
  return state_ == State::Pause;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::isRunning() const noexcept -> bool
{
  return state_ == State::Run;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::pause() noexcept -> ClockT::duration
{
  ClockT::duration time = ClockT::duration::zero();
  if (isRunning()) {
    time = elapsedRunningTime();
    elapsed_time_ += time;
    state_ = State::Pause;
  }
  return time;
}

/*!
  \details No detailed description
  */
inline
void Stopwatch::start() noexcept
{
  start_time_ = ClockT::now();
  state_ = State::Run;
}

/*!
  \details No detailed description
  */
inline
void Stopwatch::stop() noexcept
{
  elapsed_time_ = ClockT::duration::zero();
  state_ = State::Idle;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::elapsedRunningTime() const noexcept -> ClockT::duration
{
  const ClockT::time_point current_time = ClockT::now();
  const ClockT::duration time = current_time - start_time_;
  return time;
}

} // namespace zisc

#endif // ZISC_STOPWATCH_INL_HPP
