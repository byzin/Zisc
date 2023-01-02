/*!
  \file memory.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "memory.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <limits>
#include <new>
#include <string>
#include <string_view>
// Zisc
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
// Platform
#if defined(Z_WINDOWS)
#define NOMINMAX
#include <malloc.h>
#include <Windows.h>
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

  \param [in] alignment No description.
  \param [in] size No description.
  \param [in] explanation No description.
  */
Memory::BadAllocation::BadAllocation(const std::size_t size,
                                     const std::size_t alignment,
                                     const std::string_view explanation) :
    explanation_{explain(size, alignment, explanation)},
    size_{size},
    alignment_{alignment}
{
}

/*!
  \details No detailed description

  \return No description
  */
std::size_t Memory::BadAllocation::alignment() const noexcept
{
  return alignment_;
}

/*!
  \details No detailed description

  \return No description
  */
std::size_t Memory::BadAllocation::size() const noexcept
{
  return size_;
}

/*!
  \details No detailed description

  \return No description
  */
const char* Memory::BadAllocation::what() const noexcept
{
  return explanation_.data();
}

/*!
  \details No detailed description

  \param [in] alignment No description.
  \param [in] size No description.
  \param [in] explanation No description.
  \return No description
  */
std::string Memory::BadAllocation::explain(const std::size_t size,
                                           const std::size_t alignment,
                                           const std::string_view explanation)
{
  constexpr std::size_t max_length = 2048;
  std::array<char, max_length> e;
  std::snprintf(e.data(), max_length, "%s size=%zu, alignment=%zu.\n", explanation.data(),
                                                                       size,
                                                                       alignment);
  return std::string{e.data()};
}

/*!
  \details No detailed description

  \param [in] alignment No description.
  \param [in] size No description.
  \return No description
  */
[[nodiscard]]
void* Memory::alignedAllocWin([[maybe_unused]] const std::size_t alignment,
                              [[maybe_unused]] const std::size_t size) noexcept
{
  void* ptr = nullptr;
#if defined(Z_WINDOWS)
  ptr = _aligned_malloc(size, alignment);
#endif // Z_WINDOWS
  return ptr;
}

/*!
  \details No detailed description

  \param [out] ptr No description.
  */
void Memory::freeWin([[maybe_unused]] void* ptr) noexcept
{
#if defined(Z_WINDOWS)
  _aligned_free(ptr);
#endif // Z_WINDOWS
}

/*!
  \details No detailed description

  \return No description
  */
Memory::SystemMemoryStats Memory::retrieveSystemStatsImpl() noexcept
{
  SystemMemoryStats stats;
  bool success = false;
#if defined(Z_WINDOWS)
  {
    MEMORYSTATUSEX info{};
    info.dwLength = sizeof(info);
    success = GlobalMemoryStatusEx(&info) != 0;
    stats.setTotalPhysicalMemory(info.ullTotalPhys);
    stats.setAvailablePhysicalMemory(info.ullAvailPhys);
    stats.setTotalVirtualMemory(info.ullTotalVirtual);
    stats.setAvailableVirtualMemory(info.ullAvailVirtual);
    ZISC_ASSERT(success, "Retrieving memory info failed.");
  }
#elif defined(Z_LINUX)
  {
    struct sysinfo info{};
    success = sysinfo(&info) == 0;
    ZISC_ASSERT(success, "Retrieving memory info failed.");
    const std::size_t total_physical_mem = info.totalram * info.mem_unit;
    stats.setTotalPhysicalMemory(total_physical_mem);
    const std::size_t available_ram = info.freeram + info.bufferram;
    const std::size_t available_physical_mem = available_ram * info.mem_unit;
    stats.setAvailablePhysicalMemory(available_physical_mem);
    const std::size_t total_virtual_mem = info.totalswap * info.mem_unit;
    stats.setTotalVirtualMemory(total_virtual_mem);
    const std::size_t available_virtual_mem = info.freeswap * info.mem_unit;
    stats.setAvailableVirtualMemory(available_virtual_mem);
  }
#elif defined(Z_MAC)
  {
    // Total size of physical memory
    {
      std::array<int, 2> mib{{CTL_HW, HW_MEMSIZE}};
      std::size_t mem_size = 0;
      std::size_t size = sizeof(mem_size);
      const int result = sysctl(mib.data(), mib.size(), &mem_size, &size, nullptr, 0);
      success = result == 0;
      ZISC_ASSERT(success, "Retrieving physical memory info failed.");

      stats.setTotalPhysicalMemory(mem_size);
    }
    // Free physical memory size
    if (success) {
      // Retrieve pagesize
      mach_port_t host_port = mach_host_self();
      vm_size_t pagesize = 0;
      int result = host_page_size(host_port, &pagesize);
      success = result == KERN_SUCCESS;
      ZISC_ASSERT(success, "Retrieving pagesize failed.");
      // Retrieve free memory size
      mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
      vm_statistics_data_t vm_stats;
      result = host_statistics(host_port,
                               HOST_VM_INFO,
                               reinterp<host_info_t>(&vm_stats),
                               &count);
      success = result == KERN_SUCCESS;
      ZISC_ASSERT(success, "Retrieving physical memory info failed.");

      const std::size_t mem_size = vm_stats.free_count * pagesize;
      stats.setAvailablePhysicalMemory(mem_size);
    }
    // Virtual memory
    if (success) {
      std::array<int, 2> mib{{CTL_VM, VM_SWAPUSAGE}};
      xsw_usage vmusage;
      std::size_t size = sizeof(vmusage);
      const auto result = sysctl(mib.data(), mib.size(), &vmusage, &size, nullptr, 0);
      success = result == 0;
      ZISC_ASSERT(success, "Retrieving virtual memory info failed.");
      stats.setTotalVirtualMemory(vmusage.xsu_total);
      stats.setAvailableVirtualMemory(vmusage.xsu_avail);
    }
  }
#endif
  if (!success) {
    constexpr std::size_t m = (std::numeric_limits<std::size_t>::max)();
    stats.setTotalPhysicalMemory(m);
    stats.setAvailablePhysicalMemory(m);
    stats.setTotalVirtualMemory(m);
    stats.setAvailableVirtualMemory(m);
  }
  return stats;
}

} // namespace zisc
