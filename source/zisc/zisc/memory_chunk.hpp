/*!
  \file memory_chunk.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_CHUNK_HPP
#define ZISC_MEMORY_CHUNK_HPP

// Standard C++ library
#include <array>
#include <cstddef>
#include <limits>
// Zisc
#include "non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
class MemoryChunk : public NonCopyable<MemoryChunk>
{
 public:
  //! Create a null chunk
  MemoryChunk() noexcept;

  //! Create an uninitialized chunk
  MemoryChunk(const uint32 id) noexcept;


  //! Return the boundary value
  static constexpr uint8 boundaryValue() noexcept;

  //! Return the pointer of the aligned data
  template <typename Type>
  Type* data() noexcept;

  //! Return the pointer of the aligned data
  template <typename Type>
  const Type* data() const noexcept;

  //! Get the chunk header of the memory
  static MemoryChunk* getChunk(void* p) noexcept;

  //! Get the chunk header of the memory
  static const MemoryChunk* getChunk(const void* p) noexcept;

  //! Return the chunk header alignment
  static constexpr std::size_t headerAlignment() noexcept;

  //! Return the chunk header size
  static constexpr std::size_t headerSize() noexcept;

  //! Return the chunk ID (chunk index)
  uint32 id() const noexcept;

  //! Check if "data" is aligned for chunk
  static bool isAligned(const void* data) noexcept;

  //! Check if the allocated memory is freed
  bool isFreed() const noexcept;

  //! Check if the chunk is link chunk
  bool isLinkChunk() const noexcept;

  //! Check if the chunk is null 
  bool isNull() const noexcept;

  //! Return the linnked chunk
  MemoryChunk* linkedChunk() noexcept;

  //! Return the linnked chunk
  const MemoryChunk* linkedChunk() const noexcept;

  //! Return the link chunk ID
  static constexpr uint32 linkId() noexcept;

  //! Return the null chunk ID
  static constexpr uint32 nullId() noexcept;

  //! Return the padding value
  static constexpr uint8 paddingValue() noexcept;

  //! Reset the chunk info
  void reset() noexcept;

  //! Set if the memory chunk is freed
  void setFree(const bool is_freed) noexcept;

  //! Set the chunk info
  void setChunkInfo(const std::size_t size, const std::size_t alignment) noexcept;

  //! Set the chunk info
  template <typename Type>
  void setChunkInfo(const uint n) noexcept;

  //! Set the chunk ID
  void setId(const uint32 id) noexcept;

  //! Set a chunk to be linked
  void setLink(const MemoryChunk* chunk) noexcept;

  //! Return the memory size of the chunk except the chunk header
  std::size_t size() const noexcept;

  //! Return the memory stride of the chunk except the chunk header
  std::size_t stride() const noexcept;

 private:
  //! Return the pointer of the data head
  uint8* dataHead() noexcept;

  //! Return the pointer of the data head
  const uint8* dataHead() const noexcept;

  //! Fill the padding data by the padding value
  void fillPadding() noexcept;

  //! Return the chunk header pointer
  uint8* headerPointer() noexcept;

  //! Return the chunk header pointer
  const uint8* headerPointer() const noexcept;

  //! Initialize the memory chunk
  void initialize() noexcept;

  //! Return the offset of the pointer
  std::size_t offset1() const noexcept;

  //! Return the offset of the pointer
  std::size_t offset2() const noexcept;


  static constexpr uint8 kBoundary = 0xffu;
  static constexpr uint8 kPadding = 0x00u;
  static constexpr uint32 kNullId = std::numeric_limits<uint32>::max();
  static constexpr uint32 kLinkId = kNullId - 1;


  int64 size_ = 0;
  uint32 id_ = kNullId;
  uint8 is_freed_ = kTrue;
  std::array<uint8, 2> offset_ = {{0, 0}};
  uint8 boundary_ = kBoundary;
};

} // namespace zisc

#include "memory_chunk-inl.hpp"

#endif // ZISC_MEMORY_CHUNK_HPP
