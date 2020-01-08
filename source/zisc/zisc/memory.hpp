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
  /*!
    \brief The statistics of system memory

    No detailed description.
    */
  class SystemMemoryStats
  {
   public:
    //! Return the amount of actual physical memory currently available in bytes
    std::size_t physicalMemoryFree() const noexcept;

    //! Return the amount of actual physical memory in bytes
    std::size_t physicalMemoryTotal() const noexcept;

    //! Return the amount of swap memory space currently available in bytes
    std::size_t virtualMemoryFree() const noexcept;

    //! Return the amount of swap memory space in bytes
    std::size_t virtualMemoryTotal() const noexcept;

    //! Set the amount of actual physical memory currently available in bytes
    void setPhysicalMemoryFree(const std::size_t s) noexcept;

    //! Set the amount of actual physical memory in bytes
    void setPhysicalMemoryTotal(const std::size_t s) noexcept;

    //! Set the amount of swap memory space currently available in bytes
    void setVirtualMemoryFree(const std::size_t s) noexcept;

    //! Set the amount of swap memory space in bytes
    void setVirtualMemoryTotal(const std::size_t s) noexcept;

   private:
    std::size_t physical_memory_total_ = 0;
    std::size_t physical_memory_free_ = 0;
    std::size_t virtual_memory_total_ = 0;
    std::size_t virtual_memory_free_ = 0;
  };

  //! Retrieve the system memory statistics
  static SystemMemoryStats retrieveSystemStats() noexcept;

  //! Check if an address of the \a data is aligned at the \a alignment bytes
  static bool isAligned(const void* data, const std::size_t alignment) noexcept;

 private:
  //! Retrieve the system memory statistics
  static SystemMemoryStats retrieveSystemStatsImpl() noexcept;
};

} // namespace zisc

#include "memory-inl.hpp"

#endif // ZISC_MEMORY_HPP
