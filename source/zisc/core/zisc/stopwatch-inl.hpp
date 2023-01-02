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
  */
inline
Stopwatch::Stopwatch() noexcept :
    elapsed_time_{Clock::duration::zero()},
    state_{State::Idle}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Stopwatch::Stopwatch(Stopwatch&& other) noexcept :
    start_time_{std::move(other.start_time_)},
    elapsed_time_{std::move(other.elapsed_time_)},
    state_{std::move(other.state_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
Stopwatch& Stopwatch::operator=(Stopwatch&& other) noexcept
{
  start_time_ = std::move(other.start_time_);
  elapsed_time_ = std::move(other.elapsed_time_);
  state_ = std::move(other.state_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::elapsedTime() const noexcept -> Clock::duration
{
  Clock::duration time = elapsed_time_;
  if (isRunning())
    time += elapsedRunningTime();
  return time;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool Stopwatch::isIdle() const noexcept
{
  return state_ == State::Idle;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool Stopwatch::isPausing() const noexcept
{
  return state_ == State::Pause;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool Stopwatch::isRunning() const noexcept
{
  return state_ == State::Run;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::pause() noexcept -> Clock::duration
{
  Clock::duration time = Clock::duration::zero();
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
  start_time_ = Clock::now();
  state_ = State::Run;
}

/*!
  \details No detailed description
  */
inline
void Stopwatch::stop() noexcept
{
  elapsed_time_ = Clock::duration::zero();
  state_ = State::Idle;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Stopwatch::elapsedRunningTime() const noexcept -> Clock::duration
{
  const Clock::time_point current_time = Clock::now();
  const Clock::duration time = current_time - start_time_;
  return time;
}

} // namespace zisc

#endif // ZISC_STOPWATCH_INL_HPP
