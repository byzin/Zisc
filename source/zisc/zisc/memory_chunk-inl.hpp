/*!
  \file memory_chunk-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
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
#include <type_traits>
// Zisc
#include "math_const.hpp"
#include "error.hpp"
#include "simple_memory_resource.hpp"
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
MemoryChunk::MemoryChunk(const uint32b id) noexcept :
    id_{id}
{
  initialize();
}

/*!
  */
inline
constexpr uint8b MemoryChunk::boundaryValue() noexcept
{
  return kBoundary;
}

/*!
  */
template <typename Type> inline
Type* MemoryChunk::data() noexcept
{
  auto p = headerPointer() + (headerSize() + offset1());
  return treatAs<Type*>(p);
}

/*!
  */
template <typename Type> inline
const Type* MemoryChunk::data() const noexcept
{
  auto p = headerPointer() + (headerSize() + offset1());
  return treatAs<const Type*>(p);
}

/*!
  */
inline
MemoryChunk* MemoryChunk::getChunk(void* p) noexcept
{
  uint8b* chunk = cast<uint8b*>(p) - 1;
  for (; *chunk == paddingValue(); --chunk) {}
  return (*chunk == boundaryValue())
      ? reinterpret_cast<MemoryChunk*>(chunk - (headerSize() - 1))
      : nullptr;
}

/*!
  */
inline
const MemoryChunk* MemoryChunk::getChunk(const void* p) noexcept
{
  const uint8b* chunk = cast<const uint8b*>(p) - 1;
  for (; *chunk == paddingValue(); --chunk) {}
  return (*chunk == boundaryValue())
      ? reinterpret_cast<const MemoryChunk*>(chunk - (headerSize() - 1))
      : nullptr;
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
uint32b MemoryChunk::id() const noexcept
{
  return id_;
}

/*!
  */
inline
bool MemoryChunk::isAligned(const void* data) noexcept
{
  return SimpleMemoryResource::isAligned(data, headerAlignment());
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
bool MemoryChunk::isLinkChunk() const noexcept
{
  const bool is_link_chunk = (id() == linkId());
  return is_link_chunk;
}

/*!
  */
inline
bool MemoryChunk::isNull() const noexcept
{
  const bool is_null = (id() == nullId());
  return is_null;
}

/*!
  */
inline
MemoryChunk* MemoryChunk::linkedChunk() noexcept
{
  using ChunkPointer = MemoryChunk* const*;
  std::size_t address = size();
  ChunkPointer linked_chunk = treatAs<ChunkPointer>(&address);
  return *linked_chunk;
}

/*!
  */
inline
const MemoryChunk* MemoryChunk::linkedChunk() const noexcept
{
  using ChunkPointer = const MemoryChunk* const*;
  const std::size_t address = size();
  ChunkPointer linked_chunk = treatAs<ChunkPointer>(&address);
  return *linked_chunk;
}

/*!
  */
inline
constexpr uint32b MemoryChunk::linkId() noexcept
{
  return kLinkId;
}

/*!
  */
inline
constexpr uint32b MemoryChunk::nullId() noexcept
{
  return kNullId;
}

/*!
  */
inline
constexpr uint8b MemoryChunk::paddingValue() noexcept
{
  return kPadding;
}

/*!
  */
inline
void MemoryChunk::reset() noexcept
{
  size_ = 0;
  id_ = nullId();
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
inline
void MemoryChunk::setChunkInfo(const std::size_t size,
                               const std::size_t alignment) noexcept
{
  ZISC_ASSERT(0 < size, "The size is zero.");
  ZISC_ASSERT(0 < alignment, "The alignment is zero.");

  void* p = cast<void*>(dataHead());
  constexpr std::size_t memory_space = std::numeric_limits<std::size_t>::max();
  std::size_t space = memory_space;

  void* result = std::align(alignment, size, p, space);
  if (result != nullptr) {
    {
      const std::size_t o1 = memory_space - space;
      ZISC_ASSERT(o1 < alignment, "The size of the offset1 exceeded the limit.");
      offset_[0] = cast<uint8b>(o1);
    }
    {
      constexpr std::size_t chunk_align = headerAlignment();
      static_assert(Algorithm::isPowerOf2(chunk_align),
                    "The chunk align isn't power of 2.");
      std::size_t o2 = (chunk_align - constant::mod<chunk_align>(size));
      o2 = (o2 == chunk_align) ? 0 : o2;
      ZISC_ASSERT(o2 < chunk_align, "The size of the offset2 exceeded the limit.");
      offset_[1] = cast<uint8b>(o2);
    }
    {
      size_ = cast<uint64b>(size);
    }
    fillPadding();
  }
}

/*!
  */
template <typename Type> inline
void MemoryChunk::setChunkInfo(const uint n) noexcept
{
  ZISC_ASSERT(0 < n, "The n is zero.");

  constexpr std::size_t alignment = alignof(Type);
  const std::size_t size = cast<std::size_t>(n) * sizeof(Type);
  setChunkInfo(size, alignment);
}

/*!
  */
inline
void MemoryChunk::setId(const uint32b id) noexcept
{
  id_ = id;
}

/*!
  */
inline
void MemoryChunk::setLink(const MemoryChunk* chunk) noexcept
{
  setId(linkId());
  const auto address = treatAs<std::size_t>(chunk);
  size_ = cast<uint64b>(address);
}

/*!
  */
inline
std::size_t MemoryChunk::size() const noexcept
{
  return cast<std::size_t>(size_);
}

/*!
  */
inline
std::size_t MemoryChunk::stride() const noexcept
{
  const std::size_t s = size() + offset1() + offset2();
  return s;
}

/*!
  */
inline
uint8b* MemoryChunk::dataHead() noexcept
{
  auto p = headerPointer() + headerSize();
  return p;
}

/*!
  */
inline
const uint8b* MemoryChunk::dataHead() const noexcept
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
uint8b* MemoryChunk::headerPointer() noexcept
{
  auto p = treatAs<uint8b*>(this);
  return p;
}

/*!
  */
inline
const uint8b* MemoryChunk::headerPointer() const noexcept
{
  auto p = treatAs<const uint8b*>(this);
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
  static_assert(alignof(uint8b) == 1, "The align of uint8b isn't 1.");
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
