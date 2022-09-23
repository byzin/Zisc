/*!
  \file alloc_free_resource.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "alloc_free_resource.hpp"
// Standard C++ library
#include <cstddef>
#include <memory>
#include <new>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  \exception BadAllocT No description.
  */
void* AllocFreeResource::do_allocate(std::size_t size, std::size_t alignment)
{
  return allocateMemory(size, alignment);
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  \param [in] size data No description.
  \param [in] alignment data No description.
  */
void AllocFreeResource::do_deallocate(void* data,
                                      std::size_t size,
                                      std::size_t alignment)
{
  deallocateMemory(data, size, alignment);
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
bool AllocFreeResource::do_is_equal(const zisc::pmr::memory_resource& other) const noexcept
{
  const bool result = this == std::addressof(other);
  return result;
}

} // namespace zisc
