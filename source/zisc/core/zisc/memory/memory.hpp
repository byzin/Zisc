/*!
  \file memory.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_HPP
#define ZISC_MEMORY_HPP

// Standard C++ library
#include <cstddef>
#include <new>
#include <string>
#include <string_view>

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
    \brief No brief description

    No detailed description.
    */
  class BadAllocation : public std::bad_alloc
  {
   public:
    //! Create a new exception
    BadAllocation(const std::size_t size,
                  const std::size_t alignment,
                  const std::string_view explanation);

    //! Return the alignment that is failed to be allocated
    [[nodiscard]]
    auto alignment() const noexcept -> std::size_t;

    //! Return the size that is failed to be allocated
    [[nodiscard]]
    auto size() const noexcept -> std::size_t;

    //! Return the explanatory string
    [[nodiscard]]
    auto what() const noexcept -> const char* override;

   private:
    //! Return the explanatory string
    [[nodiscard]]
    static auto explain(const std::size_t size,
                        const std::size_t alignment,
                        const std::string_view explanation) -> std::string;


    std::string explanation_;
    std::size_t size_;
    std::size_t alignment_;
  };

  /*!
    \brief The statistics of system memory

    No detailed description.
    */
  class SystemMemoryStats
  {
   public:
    //! Return the amount of actual physical memory currently available in bytes
    [[nodiscard]]
    auto availablePhysicalMemory() const noexcept -> std::size_t;

    //! Return the amount of actual physical memory in bytes
    [[nodiscard]]
    auto totalPhysicalMemory() const noexcept -> std::size_t;

    //! Return the amount of swap memory space currently available in bytes
    [[nodiscard]]
    auto availableVirtualMemory() const noexcept -> std::size_t;

    //! Return the amount of swap memory space in bytes
    [[nodiscard]]
    auto totalVirtualMemory() const noexcept -> std::size_t;

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
    [[nodiscard]]
    auto peak() const noexcept -> std::size_t;

    //! Release the size of memory from the usage
    void release(const std::size_t size) noexcept;

    //! Set the peak memory usage
    void setPeak(const std::size_t p) noexcept;

    //! Set the total memory usage
    void setTotal(const std::size_t t) noexcept;

    //! Return the total memory usage
    [[nodiscard]]
    auto total() const noexcept -> std::size_t;

   private:
    std::size_t total_ = 0;
    std::size_t peak_ = 0;
  };


  //! Allocate aligned memory
  [[nodiscard]]
  static auto allocate(const std::size_t alignment, const std::size_t size) noexcept -> void*;

  //! Inform the compiler that a pointer is aligned
  template <std::size_t kN, typename Type>
  [[nodiscard]]
  static constexpr auto assumeAligned(Type* ptr) noexcept -> Type*;

  //! Deallocate previously allocated memory
  static void free(void* ptr) noexcept;

  //! Check if an address of the \a data is aligned at the \a alignment bytes
  static auto isAligned(const void* data, const std::size_t alignment) noexcept -> bool;

  //! Return the minimum alignment guaranteed to be accepted by \a allocate method
  static constexpr auto minAllocAlignment() noexcept -> std::size_t;

  //! Retrieve the system memory statistics
  static auto retrieveSystemStats() noexcept -> SystemMemoryStats;

 private:
  //! Allocate aligned memory for Windows
  [[nodiscard]]
  static auto alignedAllocWin(const std::size_t alignment, const std::size_t size) noexcept
      -> void*;

  //! Deallocate previously allocated memory for Windows
  static void freeWin(void* ptr) noexcept;

  //! Retrieve the system memory statistics
  static auto retrieveSystemStatsImpl() noexcept -> SystemMemoryStats;
};

// STL style function aliases

//! Allocate aligned memory
[[nodiscard]]
auto aligned_alloc(const std::size_t alignment, const std::size_t size) noexcept -> void*;

//! Inform the compiler that a pointer is aligned
template <std::size_t kN, typename Type>
[[nodiscard]]
constexpr auto assume_aligned(Type* ptr) noexcept -> Type*;

//! Deallocate previously allocated memory
void free(void* ptr) noexcept;

} // namespace zisc

#include "memory-inl.hpp"

#endif // ZISC_MEMORY_HPP
