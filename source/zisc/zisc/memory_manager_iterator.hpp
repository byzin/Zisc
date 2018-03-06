/*!
  \file memory_manager_iterator.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_MANAGER_ITERATOR_HPP
#define ZISC_MEMORY_MANAGER_ITERATOR_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Iterator class for memory pool
  */
template <typename ChunkT>
class MemoryManagerIterator
{
 public:
  using value_type = ChunkT;
  using difference_type = std::size_t;
  using reference = value_type&;
  using const_reference = const std::remove_const_t<value_type>&;
  using pointer = value_type*;
  using const_pointer = const std::remove_const_t<value_type>*;


  //! Create an end iterator
  MemoryManagerIterator() noexcept;

  //! Initialize the iterator
  MemoryManagerIterator(pointer chunk) noexcept;


  //! Return the reference of the value
  reference operator*() noexcept;

  //! Return the reference of the value
  const_reference operator*() const noexcept;

  //! Return the pointer to the value
  pointer operator->() noexcept;

  //! Return the pointer to the value
  const_pointer operator->() const noexcept;

  //! Pre-increment the iterator
  MemoryManagerIterator& operator++() noexcept;

  //! Post-increment the iterator
  MemoryManagerIterator operator++(int) noexcept;


  //! Return an end iterator
  static MemoryManagerIterator end() noexcept;

  //! Return the pointer to the value
  pointer get() noexcept;

  //! Return the pointer to the value
  const_pointer get() const noexcept;

 private:
  //! Return the next chunk
  static pointer getNextChunk(pointer chunk) noexcept;

  //! Return the linked chunk
  static pointer getLinkedChunk(pointer chunk) noexcept;

  //! Set a chunk
  void setChunk(pointer chunk) noexcept;


  pointer chunk_ = nullptr;
};

//! Check if two iterator refer same chunk
template <typename ChunkT>
bool operator==(const MemoryManagerIterator<ChunkT>& lhs,
                const MemoryManagerIterator<ChunkT>& rhs) noexcept;

//! Check if two iterator refer same chunk
template <typename ChunkT>
bool operator!=(const MemoryManagerIterator<ChunkT>& lhs,
                const MemoryManagerIterator<ChunkT>& rhs) noexcept;

} // namespace zisc

#include "memory_manager_iterator-inl.hpp"

#endif // ZISC_MEMORY_MANAGER_ITERATOR_HPP
