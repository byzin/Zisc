/*!
  \file timestamp-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_TIMESTAMP_INL_HPP
#define ZISC_FLOCK_TIMESTAMP_INL_HPP

#include "timestamp.hpp"
// Standard C++ library
#include <type_traits>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \return No description
  */
template <typename TimestampClass> inline
auto Timestamp<TimestampClass>::getReadStamp() noexcept -> ValueT
{
  auto& timestamp = ref();
  return timestamp.getReadStamp();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename TimestampClass> inline
auto Timestamp<TimestampClass>::getWriteStamp() noexcept -> ValueT
{
  auto& timestamp = ref();
  return timestamp.getWriteStamp();
}

/*!
  \details No detailed description
  */
template <typename TimestampClass> inline
Timestamp<TimestampClass>::Timestamp() noexcept
{
}

/*!
  \details No detailed description
  */
template <typename TimestampClass> inline
auto Timestamp<TimestampClass>::ref() noexcept -> TimestampReference
{
  auto* t = static_cast<TimestampPtr>(this);
  return *t;
}

/*!
  \details No detailed description
  */
template <typename TimestampClass> inline
auto Timestamp<TimestampClass>::ref() const noexcept -> ConstTimestampReference
{
  const auto* t = static_cast<ConstTimestampPtr>(this);
  return *t;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_TIMESTAMP_INL_HPP */
