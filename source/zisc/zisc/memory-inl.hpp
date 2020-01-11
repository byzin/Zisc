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
#include <limits>
// Zisc
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::SystemMemoryStats::physicalMemoryFree() const noexcept
{
  return physical_memory_free_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::SystemMemoryStats::physicalMemoryTotal() const noexcept
{
  return physical_memory_total_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::SystemMemoryStats::virtualMemoryFree() const noexcept
{
  return virtual_memory_free_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::SystemMemoryStats::virtualMemoryTotal() const noexcept
{
  return virtual_memory_total_;
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Memory::SystemMemoryStats::setPhysicalMemoryFree(const std::size_t s) noexcept
{
  physical_memory_free_ = s;
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Memory::SystemMemoryStats::setPhysicalMemoryTotal(const std::size_t s) noexcept
{
  physical_memory_total_ = s;
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Memory::SystemMemoryStats::setVirtualMemoryFree(const std::size_t s) noexcept
{
  virtual_memory_free_ = s;
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Memory::SystemMemoryStats::setVirtualMemoryTotal(const std::size_t s) noexcept
{
  virtual_memory_total_ = s;
}

/*!
  \details No detailed description

  \return No description
  */
inline
Memory::SystemMemoryStats Memory::retrieveSystemStats() noexcept
{
  SystemMemoryStats stats = retrieveSystemStatsImpl();
  return stats;
}

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
