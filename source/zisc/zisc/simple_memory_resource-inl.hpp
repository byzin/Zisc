/*!
  \file simple_memory_resource-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP
#define ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP

#include "simple_memory_resource.hpp"
// Standard C++ library
#include <cstddef>
#include <cstdlib>
#include <memory>
// Zisc
#include "error.hpp"
#include "memory.hpp"
#include "std_memory_resource.hpp"
#include "utility.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
inline
void* SimpleMemoryResource::do_allocate(std::size_t size, std::size_t alignment)
{
  void* data = allocateMemory(size, alignment);
  return data;
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  \param [in] size data No description.
  \param [in] alignment data No description.
  */
inline
void SimpleMemoryResource::do_deallocate(void* data,
                                         std::size_t size,
                                         std::size_t alignment)
{
  deallocateMemory(data);
  static_cast<void>(size);
  static_cast<void>(alignment);
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
bool SimpleMemoryResource::do_is_equal(const std::pmr::memory_resource& other)
    const noexcept
{
  const bool result =
      dynamic_cast<const SimpleMemoryResource*>(std::addressof(other)) != nullptr;
  return result;
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
inline
void* SimpleMemoryResource::allocateMemory(const std::size_t size,
                                           const std::size_t alignment) noexcept
{
  //! \todo Solve code branch
  auto data =
#if defined(Z_APPLE_CLANG)
      aligned_alloc(alignment, size);
#elif defined(Z_VISUAL_STUDIO)
      std::malloc(size);
  static_cast<void>(alignment);
#else
      std::aligned_alloc(alignment, size);
#endif
  ZISC_ASSERT(isAligned(data, alignment), "The data isn't aligned.");
  return data;
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  */
inline
void SimpleMemoryResource::deallocateMemory(void* data) noexcept
{
  std::free(data);
}

} // namespace zisc

#endif // ZISC_SIMPLE_MEMORY_RESOURCE_INL_HPP
