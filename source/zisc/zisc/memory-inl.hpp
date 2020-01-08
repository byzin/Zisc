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
#include "zisc/zisc_config.hpp"
// Platform
#if defined(Z_WINDOWS)
#define NOMINMAX
#include <windows.h>
#elif defined(Z_LINUX)
#include <sys/sysinfo.h>
#elif defined(Z_MAC)
#endif

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

/*!
  \details No detailed description

  \return No description
  */
inline
Memory::SystemMemoryStats Memory::retrieveSystemStatsImpl() noexcept
{
  SystemMemoryStats stats;
#if defined(Z_WINDOWS)
  {
    MEMORYSTATUSEX info{};
    info.dwLength = sizeof(info);
//    GlobalMemoryStatusEx(&info);
    stats.setPhysicalMemoryTotal(info.ullTotalPhys);
    stats.setPhysicalMemoryFree(info.ullAvailPhys);
    stats.setVirtualMemoryTotal(info.ullTotalVirtual);
    stats.setVirtualMemoryFree(info.ullAvailVirtual);
  }
#elif defined(Z_LINUX)
  {
    struct sysinfo info{};
    sysinfo(&info);
    const std::size_t total_physical_mem = info.totalram * info.mem_unit;
    stats.setPhysicalMemoryTotal(total_physical_mem);
    const std::size_t free_physical_mem = info.freeram * info.mem_unit;
    stats.setPhysicalMemoryFree(free_physical_mem);
    const std::size_t total_virtual_mem = info.totalswap * info.mem_unit;
    stats.setVirtualMemoryTotal(total_virtual_mem);
    const std::size_t free_virtual_mem = info.freeswap * info.mem_unit;
    stats.setVirtualMemoryFree(free_virtual_mem);
  }
#elif defined(Z_MAC)
#else
  {
    constexpr std::size_t m = std::numeric_limits<std::size_t>::max();
    stats.setPhysicalMemoryTotal(m);
    stats.setPhysicalMemoryFree(m);
    stats.setVirtualMemoryTotal(m);
    stats.setVirtualMemoryFree(m);
  }
#endif
  return stats;
}

} // namespace zisc

#endif // ZISC_MEMORY_INL_HPP
