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
#include <cstdlib>
#include <limits>
#include <memory>
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/thread/atomic.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::SystemMemoryStats::availablePhysicalMemory() const noexcept
{
  return available_physical_memory_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::SystemMemoryStats::totalPhysicalMemory() const noexcept
{
  return total_physical_memory_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::SystemMemoryStats::availableVirtualMemory() const noexcept
{
  return available_virtual_memory_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::SystemMemoryStats::totalVirtualMemory() const noexcept
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
  Atomic::add(std::addressof(total_), size);
  Atomic::max(std::addressof(peak_), total());
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::Usage::peak() const noexcept
{
  return peak_;
}

/*!
  \details No detailed description

  \param [in] size No description.
  */
inline
void Memory::Usage::release(const std::size_t size) noexcept
{
  Atomic::sub(std::addressof(total_), size);
}

/*!
  \details No detailed description

  \param [in] p No description.
  */
inline
void Memory::Usage::setPeak(const std::size_t p) noexcept
{
  peak_ = p;
}

/*!
  \details No detailed description

  \param [in] t No description.
  */
inline
void Memory::Usage::setTotal(const std::size_t t) noexcept
{
  total_ = t;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Memory::Usage::total() const noexcept
{
  return total_;
}

/*!
  \details No detailed description

  \param [in] alignment No description.
  \param [in] size No description.
  \return No description
  */
inline
void* Memory::allocate(const std::size_t alignment, const std::size_t size)
{
  void* ptr =
#if defined(Z_WINDOWS)
      _aligned_malloc(size, alignment);
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
constexpr Type* Memory::assumeAligned(Type* ptr)
{
  Type* result =
#if defined(Z_MSVC)
      ptr; //!< \todo Fix me
#elif defined(Z_CLANG)
      cast<Type*>(__builtin_assume_aligned(ptr, kN));
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
void Memory::free(void* ptr)
{
#if defined(Z_WINDOWS)
  _aligned_free(ptr);
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
bool Memory::isAligned(const void* data, const std::size_t alignment) noexcept
{
  const std::size_t address = reinterp<std::size_t>(data);
  const bool result = (address & (alignment - 1)) == 0;
  return result;
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

  \param [in] alignment No description.
  \param [in] size No description.
  \return No description
  */
inline
void* aligned_alloc(const std::size_t alignment, const std::size_t size)
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
constexpr Type* assume_aligned(Type* ptr)
{
  Type* result = Memory::assumeAligned<kN>(ptr);
  return result;
}

/*!
  \details No detailed description

  \param [in,out] ptr No description.
  */
inline
void free(void* ptr)
{
  Memory::free(ptr);
}

} // namespace zisc

#endif // ZISC_MEMORY_INL_HPP
