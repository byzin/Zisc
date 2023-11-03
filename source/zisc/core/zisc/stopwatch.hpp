/*!
  \file stopwatch.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STOPWATCH_HPP
#define ZISC_STOPWATCH_HPP

// Standard C++ library
#include <chrono>
#include <cstdint>
// Zisc
#include "non_copyable.hpp"

namespace zisc {

/*!
  \brief Measure elapsed processor time

  No detailed description.

  \todo Add tests
  */
class Stopwatch : private NonCopyable<Stopwatch>
{
 public:
  //! Clock type
  using ClockT = std::chrono::steady_clock;


  //! Return the total elapsed time
  [[nodiscard]]
  auto elapsedTime() const noexcept -> ClockT::duration;

  //! Check if stopwatch is idle state
  [[nodiscard]]
  auto isIdle() const noexcept -> bool;

  //! Check if stopwatch is pause state
  [[nodiscard]]
  auto isPausing() const noexcept -> bool;

  //! Check if stopwatch is run state
  [[nodiscard]]
  auto isRunning() const noexcept -> bool;

  //! Pause time measurement and return the elapsed time of this running
  auto pause() noexcept -> ClockT::duration;

  //! Start to measure the time
  void start() noexcept;

  //! Stop time measurement
  void stop() noexcept;

 private:
  /*!
    \brief No brief description

    No detailed description.
    */
  enum class State : ClockT::rep
  {
    Idle,
    Run,
    Pause
  };


  //! Return the elapsed time of this running
  [[nodiscard]]
  auto elapsedRunningTime() const noexcept -> ClockT::duration;


  ClockT::time_point start_time_{};
  ClockT::duration elapsed_time_ = ClockT::duration::zero();
  State state_ = State::Idle;
};

} // namespace zisc

/*!
  \example stopwatch_example.cpp

  This is an example of how to use zisc::Stopwatch.
  */

#include "stopwatch-inl.hpp"

#endif // ZISC_STOPWATCH_HPP
