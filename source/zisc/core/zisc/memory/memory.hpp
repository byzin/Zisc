/*!
  \file memory.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
    std::size_t availablePhysicalMemory() const noexcept;

    //! Return the amount of actual physical memory in bytes
    std::size_t totalPhysicalMemory() const noexcept;

    //! Return the amount of swap memory space currently available in bytes
    std::size_t availableVirtualMemory() const noexcept;

    //! Return the amount of swap memory space in bytes
    std::size_t totalVirtualMemory() const noexcept;

    //! Set the amount of actual physical memory currently available in bytes
    void setAvailablePhysicalMemory(const std::size_t s) noexcept;

    //! Set the amount of actual physical memory in bytes
    void setTotalPhysicalMemory(const std::size_t s) noexcept;

    //! Set the amount of swap memory space currently available in bytes
    void setAvailableVirtualMemory(const std::size_t s) noexcept;

    //! Set the amount of swap memory space in bytes
    void setTotalVirtualMemory(const std::size_t s) noexcept;

   private:
    std::size_t total_physical_memory_ = 0;
    std::size_t available_physical_memory_ = 0;
    std::size_t total_virtual_memory_ = 0;
    std::size_t available_virtual_memory_ = 0;
  };

  /*!
    \brief No brief description

    No detailed description.
    */
  class Usage
  {
   public:
    //! Add the given memory size to the usage
    void add(const std::size_t size) noexcept;

    //! Return the peak memory usage
    std::size_t peak() const noexcept;

    //! Release the size of memory from the usage
    void release(const std::size_t size) noexcept;

    //! Set the peak memory usage
    void setPeak(const std::size_t p) noexcept;

    //! Set the total memory usage
    void setTotal(const std::size_t t) noexcept;

    //! Return the total memory usage
    std::size_t total() const noexcept;

   private:
    std::size_t total_ = 0;
    std::size_t peak_ = 0;
  };


  //! Allocate aligned memory
  [[nodiscard]]
  static void* allocate(const std::size_t alignment, const std::size_t size);

  //! Inform the compiler that a pointer is aligned
  template <std::size_t kN, typename Type>
  [[nodiscard]]
  static constexpr Type* assumeAligned(Type* ptr);

  //! Deallocate previously allocated memory
  static void free(void* ptr);

  //! Check if an address of the \a data is aligned at the \a alignment bytes
  static bool isAligned(const void* data, const std::size_t alignment) noexcept;

  //! Retrieve the system memory statistics
  static SystemMemoryStats retrieveSystemStats() noexcept;

 private:
  //! Allocate aligned memory for Windows
  [[nodiscard]]
  static void* allocateForWin(const std::size_t alignment, const std::size_t size);

  //! Deallocate previously allocated memory for Windows
  static void freeForWin(void* ptr);

  //! Retrieve the system memory statistics
  static SystemMemoryStats retrieveSystemStatsImpl() noexcept;
};

// STL style function aliases

//! Allocate aligned memory
[[nodiscard]]
void* aligned_alloc(const std::size_t alignment, const std::size_t size);

//! Inform the compiler that a pointer is aligned
template <std::size_t kN, typename Type>
[[nodiscard]]
constexpr Type* assume_aligned(Type* ptr);

//! Deallocate previously allocated memory
void free(void* ptr);

} // namespace zisc

#include "memory-inl.hpp"

#endif // ZISC_MEMORY_HPP
