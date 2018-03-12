/*!
  \file memory_manager-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_MANAGER_INL_HPP
#define ZISC_MEMORY_MANAGER_INL_HPP

#include "memory_manager.hpp"
// Standard C++ library
#include <cstddef>
#include <type_traits>
#include <utility>
// Zisc
#include "error.hpp"
#include "memory_arena.hpp"
#include "memory_chunk.hpp"
#include "memory_manager_iterator.hpp"
#include "memory_resource.hpp"
#include "non_copyable.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
MemoryManager<kArenaType, kArenaSize>::MemoryManager() noexcept
{
}


/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
MemoryManager<kArenaType, kArenaSize>::MemoryManager(MemoryManager&& other) noexcept
    : arena_{std::move(other.arena_)}
{
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
auto MemoryManager<kArenaType, kArenaSize>::operator=(MemoryManager&& other) noexcept
    -> MemoryManager&
{
  arena_ = std::move(other.arena_);
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
auto MemoryManager<kArenaType, kArenaSize>::begin() noexcept -> iterator
{
  return iterator{arena_.getFirstChunk()};
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
auto MemoryManager<kArenaType, kArenaSize>::begin() const noexcept -> const_iterator
{
  return const_iterator{arena_.getFirstChunk()};
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
auto MemoryManager<kArenaType, kArenaSize>::cbegin() const noexcept -> const_iterator
{
  return const_iterator{arena_.getFirstChunk()};
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
auto MemoryManager<kArenaType, kArenaSize>::end() noexcept -> iterator
{
  return iterator{};
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
auto MemoryManager<kArenaType, kArenaSize>::end() const noexcept -> const_iterator
{
  return const_iterator{};
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
auto MemoryManager<kArenaType, kArenaSize>::cend() const noexcept -> const_iterator
{
  return const_iterator{};
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
std::size_t MemoryManager<kArenaType, kArenaSize>::capacity() const noexcept
{
  return arena_.capacity();
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
void MemoryManager<kArenaType, kArenaSize>::reset() noexcept
{
  arena_.reset();
}

/*!
  */
template <MemoryArenaType kArenaType, std::size_t kArenaSize> inline
std::size_t MemoryManager<kArenaType, kArenaSize>::usedMemory() const noexcept
{
  return arena_.usedMemory();
}

} // namespace zisc

#endif // ZISC_MEMORY_MANAGER_INL_HPP
