/*!
  \file unique_pointer.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIQUE_POINTER_HPP
#define ZISC_UNIQUE_POINTER_HPP

// Standard C++ library
#include <type_traits>
// Zisc
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  */
template <typename Type>
class UniquePointer : private NonCopyable<UniquePointer<Type>>
{
 public:
  //! Type aliases for STL
  using value_type = Type;
  using reference = std::add_lvalue_reference_t<value_type>;
  using const_reference = std::add_const_t<reference>;
  using pointer = std::add_pointer_t<value_type>;
  using const_pointer = std::add_const_t<pointer>;


  //! Create an empty unique pointer
  UniquePointer() noexcept;

  //! Create a unique pointer
  UniquePointer(pointer data, std::pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  UniquePointer(UniquePointer&& other) noexcept;

  //! Move a data
  template <typename Super>
  UniquePointer(UniquePointer<Super>&& other,
                EnableIfBaseOf<Type, Super> = kEnabler) noexcept;

  //! Destruct the managed object
  ~UniquePointer() noexcept;


  //! Move a data
  UniquePointer& operator=(UniquePointer&& other) noexcept;

  //! Move a data
  template <typename Super>
  UniquePointer& operator=(UniquePointer&& other) noexcept;

  //! Check whether this owns an object
  explicit operator bool() const noexcept
  {
    const bool result = get() != nullptr;
    return result;
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
  static UniquePointer make(std::pmr::memory_resource* mem_resource,
                                  Types&&... arguments) noexcept;

  //! Return the memory resource
  std::pmr::memory_resource* memoryResource() noexcept;

  //! Return the memory resource
  const std::pmr::memory_resource* memoryResource() const noexcept;

  //! Return the data and release the ownership
  pointer release() noexcept;

  //! Reset the managed object
  void reset() noexcept;

  //! Replace the managed object
  void reset(pointer data, std::pmr::memory_resource* mem_resource) noexcept;

  //! Replace the managed objects with the given object
  template <typename Super>
  void reset(UniquePointer<Super>& other,
             EnableIfBaseOf<Type, Super> = kEnabler) noexcept;

  //! Swap the managed objects
  void swap(UniquePointer& other) noexcept;

 private:
  using Allocator = std::pmr::polymorphic_allocator<value_type>;


  pointer data_ = nullptr;
  std::pmr::memory_resource* mem_resource_ = nullptr;
};

//! Swap the managed objects
template <typename Type>
void swap(UniquePointer<Type>& lhs, UniquePointer<Type>& rhs) noexcept;

} // namespace zisc

#include "unique_pointer-inl.hpp"

#endif // ZISC_UNIQUE_POINTER_HPP
