/*!
  \file memory.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "memory.hpp"
// Standard C++ library
#include <cstddef>
#include <limits>
// Zisc
#include "utility.hpp"
#include "zisc_config.hpp"
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
Memory::SystemMemoryStats Memory::retrieveSystemStatsImpl() noexcept
{
  SystemMemoryStats stats;
#if defined(Z_WINDOWS)
  {
    MEMORYSTATUSEX info{};
    info.dwLength = sizeof(info);
    GlobalMemoryStatusEx(&info);
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
