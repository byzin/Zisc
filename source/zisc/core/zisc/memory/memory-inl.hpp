/*!
  \file memory-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_INL_HPP
#define ZISC_MEMORY_INL_HPP

#include "memory.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <memory>
// Zisc
#include "zisc/bit.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/concurrency/atomic.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
inline
auto Memory::SystemMemoryStats::availablePhysicalMemory() const noexcept -> std::size_t
{
  return available_physical_memory_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Memory::SystemMemoryStats::totalPhysicalMemory() const noexcept -> std::size_t
{
  return total_physical_memory_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Memory::SystemMemoryStats::availableVirtualMemory() const noexcept -> std::size_t
{
  return available_virtual_memory_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Memory::SystemMemoryStats::totalVirtualMemory() const noexcept -> std::size_t
{
  return total_virtual_memory_;
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Memory::SystemMemoryStats::setAvailablePhysicalMemory(const std::size_t s) noexcept
{
  available_physical_memory_ = s;
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Memory::SystemMemoryStats::setTotalPhysicalMemory(const std::size_t s) noexcept
{
  total_physical_memory_ = s;
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Memory::SystemMemoryStats::setAvailableVirtualMemory(const std::size_t s) noexcept
{
  available_virtual_memory_ = s;
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Memory::SystemMemoryStats::setTotalVirtualMemory(const std::size_t s) noexcept
{
  total_virtual_memory_ = s;
}

/*!
  \details No detailed description

  \param [in] size No description.
  */
inline
void Memory::Usage::add(const std::size_t size) noexcept
{
  const std::size_t old = atomic_fetch_add(&total_, size, std::memory_order::acq_rel);
  const std::size_t t = old + size;
  atomic_fetch_max(&peak_, t, std::memory_order::acq_rel);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Memory::Usage::peak() const noexcept -> std::size_t
{
  const std::size_t p = atomic_load(&peak_, std::memory_order::acquire);
  return p;
}

/*!
  \details No detailed description

  \param [in] size No description.
  */
inline
void Memory::Usage::release(const std::size_t size) noexcept
{
  atomic_fetch_sub(&total_, size, std::memory_order::acq_rel);
}

/*!
  \details No detailed description

  \param [in] p No description.
  */
inline
void Memory::Usage::setPeak(const std::size_t p) noexcept
{
  atomic_store(&peak_, p, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] t No description.
  */
inline
void Memory::Usage::setTotal(const std::size_t t) noexcept
{
  atomic_store(&total_, t, std::memory_order::release);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Memory::Usage::total() const noexcept -> std::size_t
{
  const std::size_t t = atomic_load(&total_, std::memory_order::acquire);
  return t;
}

/*!
  \details No detailed description

  \param [in] alignment No description.
  \param [in] size No description.
  \return No description
  */
inline
auto Memory::allocate(const std::size_t alignment, const std::size_t size) noexcept -> void*
{
  void* ptr =
#if defined(Z_WINDOWS)
      alignedAllocWin(alignment, size);
#else // Z_WINDOWS
      std::aligned_alloc(alignment, size);
#endif // Z_WINDOWS
  return ptr;
}

/*!
  \details No detailed description

  \tparam kN No description.
  \tparam Type No description.
  \param [in] ptr No description.
  \return No description
  */
template <std::size_t kN, typename Type> inline
constexpr auto Memory::assumeAligned(Type* ptr) noexcept -> Type*
{
  Type* result =
#if defined(Z_CLANG)
      static_cast<Type*>(__builtin_assume_aligned(ptr, kN));
#else // Z_CLANG
      std::assume_aligned<kN>(ptr);
#endif // Z_CLANG
  return result;
}

/*!
  \details No detailed description

  \param [in,out] ptr No description.
  */
inline
void Memory::free(void* ptr) noexcept
{
#if defined(Z_WINDOWS)
  freeWin(ptr);
#else // Z_WINDOWS
  std::free(ptr);
#endif // Z_WINDOWS
}

/*!
  \details No detailed description

  \param [in] data No description.
  \param [in] alignment \a alignment should be power of 2 bytes
  \return No description
  */
inline
auto Memory::isAligned(const void* data, const std::size_t alignment) noexcept -> bool
{
  const auto address = zisc::bit_cast<std::size_t>(data);
  const bool result = (address & (alignment - 1)) == 0;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto Memory::minAllocAlignment() noexcept -> std::size_t
{
  constexpr std::size_t alignment = alignof(void*);
  return alignment;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Memory::retrieveSystemStats() noexcept -> Memory::SystemMemoryStats
{
  SystemMemoryStats stats = retrieveSystemStatsImpl();
  return stats;
}

/*!
  \details No detailed description

  \param [in] alignment No description.
  \param [in] size No description.
  \return No description
  */
inline
auto aligned_alloc(const std::size_t alignment, const std::size_t size) noexcept -> void*
{
  void* ptr = Memory::allocate(alignment, size);
  return ptr;
}

/*!
  \details No detailed description

  \tparam kN No description.
  \tparam Type No description.
  \param [in] ptr No description.
  \return No description
  */
template <std::size_t kN, typename Type> inline
constexpr auto assume_aligned(Type* ptr) noexcept -> Type*
{
  Type* result = Memory::assumeAligned<kN>(ptr);
  return result;
}

/*!
  \details No detailed description

  \param [in,out] ptr No description.
  */
inline
void free(void* ptr) noexcept
{
  Memory::free(ptr);
}

} // namespace zisc

#endif // ZISC_MEMORY_INL_HPP
