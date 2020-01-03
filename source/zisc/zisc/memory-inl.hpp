/*!
  \file memory-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_INL_HPP
#define ZISC_MEMORY_INL_HPP

#include "memory.hpp"
// Standard C++ library
#include <cstddef>
// Zisc
#include "utility.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] data No description.
  \param [in] alignment \a alignment should be power of 2 bytes
  \return No description
  */
inline
bool Memory::isAligned(const void* data, const std::size_t alignment) noexcept
{
  const std::size_t address = treatAs<std::size_t>(data);
  const bool result = (address & (alignment - 1)) == 0;
  return result;
}

} // namespace zisc

#endif // ZISC_MEMORY_INL_HPP
