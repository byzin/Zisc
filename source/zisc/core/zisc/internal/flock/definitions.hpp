/*!
  \file definitions.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_DEFINITIONS_HPP
#define ZISC_FLOCK_DEFINITIONS_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>

namespace zisc::flock {

//!
using LogEntry = std::atomic<void*>;

//!
using LockEntry = std::size_t;

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_DEFINITIONS_HPP */
