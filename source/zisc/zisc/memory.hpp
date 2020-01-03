/*!
  \file memory.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_HPP
#define ZISC_MEMORY_HPP

// Standard C++ library
#include <cstddef>

namespace zisc {

//! \todo Implement getter of system memory info

/*!
  \brief No brief description

  No detailed description.
  */
class Memory
{
 public:
  //! Check if an address of the \a data is aligned at the \a alignment bytes
  static bool isAligned(const void* data, const std::size_t alignment) noexcept;
};

} // namespace zisc

#include "memory-inl.hpp"

#endif // ZISC_MEMORY_HPP
