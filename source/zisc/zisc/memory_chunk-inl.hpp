/*!
  \file memory_chunk-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_CHUNK_INL_HPP
#define ZISC_MEMORY_CHUNK_INL_HPP

#include "memory_chunk.hpp"
// Standard C++ library
#include <cstddef>
#include <limits>
#include <memory>
// Zisc
#include "error.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
inline
MemoryChunk::MemoryChunk() noexcept
{
}

/*!
  */
inline
MemoryChunk::MemoryChunk(const uint id) noexcept :
    size_{0},
    id_{cast<uint32>(id)},
    is_freed_{kTrue},
    offset_{{0, 0}},
    boundary_{boundaryValue()}
{
  initialize();
}

/*!
  */
inline
constexpr uint8 MemoryChunk::boundaryValue() noexcept
{
  return kBoundary;
}

/*!
  */
template <typename Type> inline
Type* MemoryChunk::data() noexcept
{
  auto p = headerPointer() + (headerSize() + offset1());
  return cast<Type*>(p);
}

/*!
  */
template <typename Type> inline
const Type* MemoryChunk::data() const noexcept
{
  auto p = headerPointer() + (headerSize() + offset1());
  return cast<const Type*>(p);
}

/*!
  */
inline
constexpr std::size_t MemoryChunk::headerAlignment() noexcept
{
  constexpr std::size_t header_alignment = alignof(MemoryChunk);
  return header_alignment;
}

/*!
  */
inline
constexpr std::size_t MemoryChunk::headerSize() noexcept
{
  constexpr std::size_t header_size = sizeof(MemoryChunk);
  return header_size;
}

/*!
  */
inline
uint MemoryChunk::id() const noexcept
{
  return cast<uint>(id_);
}

/*!
  */
inline
bool MemoryChunk::isFreed() const noexcept
{
  const bool is_freed = (is_freed_ == kTrue);
  return is_freed;
}

/*!
  */
inline
bool MemoryChunk::isValid() const noexcept
{
  const bool is_valid = (0 < size());
  return is_valid;
}

/*!
  */
inline
constexpr uint8 MemoryChunk::paddingValue() noexcept
{
  return kPadding;
}

/*!
  */
inline
void MemoryChunk::reset() noexcept
{
  size_ = 0;
  id_ = 0;
  is_freed_ = kTrue;
  offset_[0] = 0;
  offset_[1] = 0;
  boundary_ = boundaryValue();
}

/*!
  */
inline
void MemoryChunk::setFree(const bool is_freed) noexcept
{
  is_freed_ = (is_freed) ? kTrue : kFalse;
}

/*!
  */
template <typename Type> inline
void MemoryChunk::setChunkInfo(const uint n,
                               const std::size_t memory_space) noexcept
{
  ZISC_ASSERT(0 < n, "The n is zero.");
  ZISC_ASSERT(0 < memory_space, "The memory space is zero.");

  constexpr std::size_t a = alignof(Type);
  const std::size_t s = cast<std::size_t>(n) * sizeof(Type);
  void* p = cast<void*>(dataHead());
  std::size_t space = memory_space;

  void* result = std::align(a, s, p, space);
  if (result != nullptr) {
    {
      const std::size_t o1 = memory_space - space;
      ZISC_ASSERT(o1 < cast<std::size_t>(std::numeric_limits<uint8>::max()),
                  "The size the offset1 exceeded the limit.");
      offset_[0] = cast<uint8>(o1);
    }
    {
      constexpr std::size_t chunk_align = headerAlignment();
      std::size_t o2 = (chunk_align - (s % chunk_align));
      o2 = (o2 == chunk_align) ? 0 : o2;
      ZISC_ASSERT(o2 < cast<std::size_t>(std::numeric_limits<uint8>::max()),
                  "The size the offset2 exceeded the limit.");
      offset_[1] = cast<uint8>(o2);
    }
    {
      size_ = cast<uint64>(s);
    }
    fillPadding();
  }
}

/*!
  */
inline
void MemoryChunk::setId(const uint id) noexcept
{
  id_ = cast<uint32>(id);
}

/*!
  */
inline
std::size_t MemoryChunk::size() const noexcept
{
  return size_;
}

/*!
  */
std::size_t MemoryChunk::stride() const noexcept
{
  const std::size_t s = size() + offset1() + offset2();
  return s;
}

/*!
  */
inline
uint8* MemoryChunk::dataHead() noexcept
{
  auto p = headerPointer() + headerSize();
  return p;
}

/*!
  */
inline
const uint8* MemoryChunk::dataHead() const noexcept
{
  auto p = headerPointer() + headerSize();
  return p;
}

/*!
  */
inline
void MemoryChunk::fillPadding() noexcept
{
  auto p = dataHead();
  for (uint i = 0; i < cast<uint>(offset1()); ++i)
    p[i] = paddingValue();
}

/*!
  */
inline
uint8* MemoryChunk::headerPointer() noexcept
{
  auto p = treatAs<uint8*>(this);
  return p;
}

/*!
  */
inline
const uint8* MemoryChunk::headerPointer() const noexcept
{
  auto p = treatAs<const uint8*>(this);
  return p;
}

/*!
  */
inline
void MemoryChunk::initialize() noexcept
{
  // Check size
  static_assert(headerAlignment() == 8, "The align of the chunk isn't 8.");
  static_assert(headerSize() == 16, "The size of the chunk isn't 16.");
  static_assert(alignof(uint8) == 1, "The align of uint8 isn't 1.");
}

/*!
  */
inline
std::size_t MemoryChunk::offset1() const noexcept
{
  return cast<std::size_t>(offset_[0]);
}

/*!
  */
inline
std::size_t MemoryChunk::offset2() const noexcept
{
  return cast<std::size_t>(offset_[1]);
}

} // namespace zisc

#endif // ZISC_MEMORY_CHUNK_INL_HPP
