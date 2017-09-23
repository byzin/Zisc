/*!
  \file memory_chunk.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_CHUNK_HPP
#define ZISC_MEMORY_CHUNK_HPP

// Standard C++ library
#include <array>
#include <cstddef>
// Zisc
#include "non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
class MemoryChunk : public NonCopyable<MemoryChunk>
{
 public:
  //! Create an uninitialized chunk
  MemoryChunk() noexcept;

  //! Create an uninitialized chunk
  MemoryChunk(const uint id) noexcept;


  //! Return the boundary value
  static constexpr uint8 boundaryValue() noexcept;

  //! Return the pointer of the aligned data
  template <typename Type>
  Type* data() noexcept;

  //! Return the pointer of the aligned data
  template <typename Type>
  const Type* data() const noexcept;

  //! Return the chunk header alignment
  static constexpr std::size_t headerAlignment() noexcept;

  //! Return the chunk header size
  static constexpr std::size_t headerSize() noexcept;

  //! Return the chunk ID (chunk index)
  uint id() const noexcept;

  //! Check if the allocated memory is freed
  bool isFreed() const noexcept;

  //! Check if the chunk is valid
  bool isValid() const noexcept;

  //! Return the padding value
  static constexpr uint8 paddingValue() noexcept;

  //! Reset the chunk info
  void reset() noexcept;

  //! Set if the memory chunk is freed
  void setFree(const bool is_freed) noexcept;

  //! Set the chunk info
  template <typename Type>
  void setChunkInfo(const uint n, const std::size_t memory_space) noexcept;

  //! Set the chunk ID
  void setId(const uint id) noexcept;

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


  uint64 size_;
  uint32 id_;
  uint8 is_freed_;
  std::array<uint8, 2> offset_;
  uint8 boundary_;
};

} // namespace zisc

#include "memory_chunk-inl.hpp"

#endif // ZISC_MEMORY_CHUNK_HPP
