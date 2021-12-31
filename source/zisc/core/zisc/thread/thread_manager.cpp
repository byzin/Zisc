/*!
  \file thread_manager.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "thread_manager.hpp"
// Standard C++ library
#include <cstddef>
#include <memory>
// Zisc
#include "zisc/bit.hpp"
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
ThreadManager::OverflowError::~OverflowError() noexcept
{
}

/*!
  \details No detailed description
  */
ThreadManager::TaskResource::~TaskResource() noexcept
{
}

/*!
  \details No detailed description

  \param [in] size No description.
  \param [in] alignment No description.
  \return No description
  */
void* ThreadManager::TaskResource::do_allocate(std::size_t size,
                                               std::size_t alignment)
{
#if !defined(Z_MSVC)
  [[maybe_unused]] constexpr std::size_t s = storageSize();
  ZISC_ASSERT(size <= s,
              "The required size exceeds the size of the storage.");
  [[maybe_unused]] constexpr std::size_t a = storageAlignment();
  ZISC_ASSERT(has_single_bit(alignment) && (alignment <= a),
              "The storage alignment doesn't fit for the required alignment.");
#endif // Z_MSVC
  void* ptr = std::addressof(storage_);
  return ptr;
}

/*!
  \details No detailed description

  \param [in,out] data No description.
  \param [in] size No description.
  \param [in] alignment No description.
  */
void ThreadManager::TaskResource::do_deallocate([[maybe_unused]] void* data,
                                                [[maybe_unused]] std::size_t size,
                                                [[maybe_unused]] std::size_t alignment)
{
  ZISC_ASSERT(std::addressof(storage_) == data,
              "The given data doesn't point to the underlying storage.");
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
bool ThreadManager::TaskResource::do_is_equal(const pmr::memory_resource& other)
    const noexcept
{
  const bool result = this == std::addressof(other);
  return result;
}

} // namespace zisc
