/*!
  \file memory_pool_iterator-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_POOL_ITERATOR_INL_HPP
#define ZISC_MEMORY_POOL_ITERATOR_INL_HPP

#include "memory_pool_iterator.hpp"
// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "error.hpp"
#include "memory_chunk.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename ChunkT> inline
MemoryPoolIterator<ChunkT>::MemoryPoolIterator() noexcept
{
}


/*!
  */
template <typename ChunkT> inline
MemoryPoolIterator<ChunkT>::MemoryPoolIterator(pointer chunk) noexcept
{
  setChunk(chunk);
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::operator*() noexcept -> reference
{
  ZISC_ASSERT(chunk_ != nullptr, "The chunk is null.");
  ZISC_ASSERT(!chunk_->isLinkChunk(), "The chunk is link chunk.");
  return *chunk_;
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::operator*() const noexcept -> const_reference
{
  ZISC_ASSERT(chunk_ != nullptr, "The chunk is null.");
  ZISC_ASSERT(!chunk_->isLinkChunk(), "The chunk is link chunk.");
  return *chunk_;
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::operator->() noexcept -> pointer
{
  ZISC_ASSERT(chunk_ != nullptr, "The chunk is null.");
  ZISC_ASSERT(!chunk_->isLinkChunk(), "The chunk is link chunk.");
  return get();
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::operator->() const noexcept -> const_pointer
{
  ZISC_ASSERT(chunk_ != nullptr, "The chunk is null.");
  ZISC_ASSERT(!chunk_->isLinkChunk(), "The chunk is link chunk.");
  return get();
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::operator++() noexcept -> MemoryPoolIterator&
{
  auto c = getNextChunk(chunk_);
  setChunk(c);
  return *this;
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::operator++(int) noexcept -> MemoryPoolIterator
{
  auto prev_chunk = *this;
  auto c = getNextChunk(chunk_);
  setChunk(c);
  return prev_chunk;
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::end() noexcept -> MemoryPoolIterator
{
  return MemoryPoolIterator{};
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::get() noexcept -> pointer
{
  return chunk_;
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::get() const noexcept -> const_pointer
{
  return chunk_;
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::getNextChunk(pointer chunk) noexcept -> pointer
{
  ZISC_ASSERT(chunk != nullptr, "The chunk is null.");
  using ByteType = std::conditional_t<std::is_const_v<value_type>, const uint8, uint8>;
  auto p = treatAs<ByteType*>(chunk);
  p = p + (chunk->headerSize() + chunk->stride());
  return treatAs<pointer>(p);
}

/*!
  */
template <typename ChunkT> inline
auto MemoryPoolIterator<ChunkT>::getLinkedChunk(pointer chunk) noexcept -> pointer
{
  ZISC_ASSERT(chunk != nullptr, "The chunk is null.");
  ZISC_ASSERT(chunk->isLinkChunk(), "The chunk isn't link chunk.");
  pointer c = chunk->linkedChunk();
  for (; c->isLinkChunk(); c = c->linkedChunk()) {}
  return c;
}

/*!
  */
template <typename ChunkT> inline
void MemoryPoolIterator<ChunkT>::setChunk(pointer chunk) noexcept
{
  if (chunk != nullptr) {
    chunk = (chunk->isLinkChunk()) ? getLinkedChunk(chunk) : chunk;
    chunk = (!chunk->isNull()) ? chunk : nullptr; 
  }
  chunk_ = chunk;
}

/*!
  */
template <typename ChunkT> inline
bool operator==(const MemoryPoolIterator<ChunkT>& lhs,
                const MemoryPoolIterator<ChunkT>& rhs) noexcept
{
  return (lhs.get() == rhs.get());
}

/*!
  */
template <typename ChunkT> inline
bool operator!=(const MemoryPoolIterator<ChunkT>& lhs,
                const MemoryPoolIterator<ChunkT>& rhs) noexcept
{
  return !(lhs == rhs);
}

} // namespace zisc

#endif // ZISC_MEMORY_POOL_ITERATOR_INL_HPP
