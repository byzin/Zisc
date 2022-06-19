/*!
  \file timestamp.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_TIMESTAMP_HPP
#define ZISC_FLOCK_TIMESTAMP_HPP

// Standard C++ library
#include <type_traits>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \brief Timestamp for snapshots

  Flock time stamp.
  github: <a href="https://github.com/cmuparlay/flock">Flock</a>

  \note No notation.
  \attention No attention.
  */
template <typename TimestampClass>
class Timestamp : NonCopyable<Timestamp<TimestampClass>>
{
 public:
  using ValueT = int64b;


  //!
  ValueT getReadStamp() noexcept;

  //!
  ValueT getWriteStamp() noexcept;

 protected:
  // Type aliases
  using TimestampT = std::remove_volatile_t<TimestampClass>;
  using ConstTimestampT = std::add_const_t<TimestampT>;
  using TimestampPtr = std::add_pointer_t<TimestampT>;
  using ConstTimestampPtr = std::add_pointer_t<ConstTimestampT>;
  using TimestampReference = std::add_lvalue_reference_t<TimestampT>;
  using ConstTimestampReference = std::add_lvalue_reference_t<ConstTimestampT>;


  //! Create a timestamp
  Timestamp() noexcept;


  //! Return the reference to the timestamp class
  TimestampReference ref() noexcept;

  //! Return the reference to the timestamp class
  ConstTimestampReference ref() const noexcept;
};

} /* namespace zisc::flock */

#include "timestamp-inl.hpp"

#endif /* ZISC_FLOCK_TIMESTAMP_HPP */
