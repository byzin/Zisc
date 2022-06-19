/*!
  \file timestamp_simple.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_TIMESTAMP_SIMPLE_HPP
#define ZISC_FLOCK_TIMESTAMP_SIMPLE_HPP

// Standard C++ library
#include <atomic>
#include <type_traits>
// Zisc
#include "timestamp.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \brief Timestamp for snapshots

  Flock time stamp.
  github: <a href="https://github.com/cmuparlay/flock">Flock</a>

  \note No notation.
  \attention No attention.
  */
class TimestampSimple : public Timestamp<TimestampSimple>
{
 public:
  // Type aliases
  using BaseTimestampT = Timestamp<TimestampSimple>;
  using ValueT = typename BaseTimestampT::ValueT;


  //! Create a timestamp
  TimestampSimple() noexcept;


  //!
  ValueT getReadStamp() noexcept;

  //!
  ValueT getWriteStamp() const noexcept;

 private:
  //! Return the delay time
  static constexpr int delayTime() noexcept;


  std::atomic<ValueT> stamp_;
};

} /* namespace zisc::flock */

#include "timestamp_simple-inl.hpp"

#endif /* ZISC_FLOCK_TIMESTAMP_SIMPLE_HPP */
