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
#include <array>
#include <cstddef>
#include <limits>
// Zisc
#include "error.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"
// Platform
#if defined(Z_WINDOWS)
#define NOMINMAX
#include <windows.h>
#elif defined(Z_LINUX)
#include <sys/sysinfo.h>
#elif defined(Z_MAC)
#include <mach/mach.h>
#include <sys/sysctl.h>
#include <sys/types.h>
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
  {
    mach_port_t host_port = mach_host_self();
    vm_size_t pagesize = 0;
    host_page_size(host_port, &pagesize);

    // Total size of physical memory
    {
      std::array<int, 2> mib{{CTL_HW, HW_MEMSIZE}};
      std::size_t total_physical_mem = 0;
      std::size_t size = sizeof(total_physical_mem);
      const int result = sysctl(mib.data(),
                                mib.size(),
                                &total_physical_mem,
                                &size,
                                nullptr,
                                0);
      ZISC_ASSERT(0 == result, "Retrieving physical memory info failed.");
      static_cast<void>(result);
      stats.setPhysicalMemoryTotal(total_physical_mem);
    }
    // Free physical memory size
    {
      mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
      vm_statistics_data_t vmstats;
      const auto result = host_statistics(host_port,
                                          HOST_VM_INFO,
                                          treatAs<host_info_t>(&vmstats),
                                          &count);
      ZISC_ASSERT(result == KERN_SUCCESS,
                  "Retrieving physical memory info failed.");
      static_cast<void>(result);
      const std::size_t free_physical_mem = vmstats.free_count * pagesize;
      stats.setPhysicalMemoryFree(free_physical_mem);
    }
    // Virtual memory
    {
      xsw_usage vmusage;
      std::size_t size = sizeof(vmusage);
      const auto result = sysctlbyname("vm.swapusage",
                                       &vmusage,
                                       &size,
                                       nullptr,
                                       0);
      ZISC_ASSERT(0 == result, "Retrieving virtual memory info failed.");
      static_cast<void>(result);
      stats.setVirtualMemoryTotal(vmusage.xsu_total);
      stats.setVirtualMemoryFree(vmusage.xsu_avail);
    }
  }
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
