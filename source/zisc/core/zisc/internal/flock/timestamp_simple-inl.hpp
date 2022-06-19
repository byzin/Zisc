/*!
  \file timestamp_simple-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_TIMESTAMP_SIMPLE_INL_HPP
#define ZISC_FLOCK_TIMESTAMP_SIMPLE_INL_HPP

#include "timestamp_simple.hpp"
// Standard C++ library
#include <atomic>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \details No detailed description
  */
inline
TimestampSimple::TimestampSimple() noexcept : stamp_{0}
{
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto TimestampSimple::getReadStamp() noexcept -> ValueT
{
  ValueT ts = stamp_.load(std::memory_order::acquire);

  // Delay to reduce contention
  //for (volatile int i = 0; i < delayTime(); ++i)
  //  ;

  // Only update timestamp if has not changed
  if (stamp_.load(std::memory_order::acquire) == ts)
    stamp_.compare_exchange_strong(ts, ts + 1, std::memory_order::acq_rel, std::memory_order::acquire);

  return ts;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto TimestampSimple::getWriteStamp() const noexcept -> ValueT
{
  const ValueT ts = stamp_.load(std::memory_order::acquire);
  return ts;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr int TimestampSimple::delayTime() noexcept
{
  return 800;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_TIMESTAMP_SIMPLE_INL_HPP */
