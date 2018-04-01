/*!
  \file unique_memory_pointer.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIQUE_MEMORY_POINTER_HPP
#define ZISC_UNIQUE_MEMORY_POINTER_HPP

// Standard C++ library
#include <type_traits>
// Zisc
#include "memory_resource.hpp"
#include "non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Type>
class UniqueMemoryPointer : public NonCopyable<UniqueMemoryPointer<Type>>
{
 public:
  using value_type = Type;
  using reference = std::add_lvalue_reference_t<value_type>;
  using const_reference = std::add_const_t<reference>;
  using pointer = std::add_pointer_t<value_type>;
  using const_pointer = std::add_const_t<pointer>;


  //! Create an empty unique pointer
  UniqueMemoryPointer() noexcept;

  //! Create a unique pointer
  UniqueMemoryPointer(pointer data, pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  UniqueMemoryPointer(UniqueMemoryPointer&& other) noexcept;

  //! Destruct the managed object
  ~UniqueMemoryPointer() noexcept;


  //! Move a data
  UniqueMemoryPointer& operator=(UniqueMemoryPointer&& other) noexcept;

  //! Check whether this owns an object
  explicit operator bool() const noexcept
  {
    return get() != nullptr;
  }

  //! Dereferences pointer to the managed object
  reference operator*() noexcept;

  //! Dereferences pointer to the managed object
  const_reference operator*() const noexcept;

  //! Dereferences pointer to the managed object
  pointer operator->() noexcept;

  //! Dereferences pointer to the managed object
  const_pointer operator->() const noexcept;


  //! Return a pointer to the managed object
  pointer get() noexcept;

  //! Return a pointer to the managed object
  const_pointer get() const noexcept;

  //! Create a unique pointer
  template <typename ...Types>
  static UniqueMemoryPointer make(pmr::memory_resource* mem_resource,
                                  Types&&... arguments) noexcept;

  //! Reset the managed object
  void reset() noexcept;

  //! Replace the managed object
  void reset(pointer data, pmr::memory_resource* mem_resource) noexcept;

  //! Swap the managed objects
  void swap(UniqueMemoryPointer& other) noexcept;

 private:
  using Allocator = pmr::polymorphic_allocator<value_type>;


  pointer data_ = nullptr;
  pmr::memory_resource* mem_resource_ = nullptr;
};

//! Swap the managed objects
template <typename Type>
void swap(UniqueMemoryPointer<Type>& lhs, UniqueMemoryPointer<Type>& rhs) noexcept;

} // namespace zisc

#include "unique_memory_pointer-inl.hpp"

#endif // ZISC_UNIQUE_MEMORY_POINTER_HPP
