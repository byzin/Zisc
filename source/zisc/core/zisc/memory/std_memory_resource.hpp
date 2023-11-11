/*!
  \file std_memory_resource.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STD_MEMORY_RESOURCE_HPP
#define ZISC_STD_MEMORY_RESOURCE_HPP

// Standard C++ library
#include <concepts>
#include <memory>
#include <memory_resource>
#include <sstream>
#include <type_traits>

namespace zisc::pmr {

// StringStream

template <typename CharType, typename Traits = std::char_traits<CharType>>
using basic_stringbuf = std::basic_stringbuf<CharType,
                                             Traits,
                                             std::pmr::polymorphic_allocator<CharType>>;
using stringbuf = basic_stringbuf<char>;
using wstringbuf = basic_stringbuf<wchar_t>;

template <typename CharType, typename Traits = std::char_traits<CharType>>
using basic_istringstream = std::basic_istringstream<CharType,
                                                     Traits,
                                                     std::pmr::polymorphic_allocator<CharType>>;
using istringstream = basic_istringstream<char>;
using wistringstream = basic_istringstream<wchar_t>;

template <typename CharType, typename Traits = std::char_traits<CharType>>
using basic_ostringstream = std::basic_ostringstream<CharType,
                                                     Traits,
                                                     std::pmr::polymorphic_allocator<CharType>>;
using ostringstream = basic_ostringstream<char>;
using wostringstream = basic_ostringstream<wchar_t>;

template <typename CharType, typename Traits = std::char_traits<CharType>>
using basic_stringstream = std::basic_stringstream<CharType,
                                                   Traits,
                                                   std::pmr::polymorphic_allocator<CharType>>;
using stringstream = basic_stringstream<char>;
using wstringstream = basic_stringstream<wchar_t>;

// Memory

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  */
template <typename Type>
class UniquePtrDeleter
{
 public:
  //! Pointer type of the Type
  using Pointer = std::add_pointer_t<Type>;

  // std compatible
  using pointer = Pointer;


  //! Create an empty deleter
  constexpr UniquePtrDeleter() noexcept = default;

  //! Create a memory deleter
  constexpr UniquePtrDeleter(const std::pmr::polymorphic_allocator<Type>& alloc) noexcept;

  //! Move a data
  constexpr UniquePtrDeleter(UniquePtrDeleter&& other) noexcept = default;

  //! Move a data from a super class
  template <std::derived_from<Type> Derived>
  constexpr UniquePtrDeleter(UniquePtrDeleter<Derived>&& other) noexcept;


  //! Move a data
  constexpr auto operator=(UniquePtrDeleter&& other) noexcept -> UniquePtrDeleter& = default;

  //! Move a data from a super class
  template <std::derived_from<Type> Derived>
  constexpr auto operator=(UniquePtrDeleter<Derived>&& other) noexcept -> UniquePtrDeleter&;

  //! Delete the given pointer memory
  constexpr void operator()(Pointer memory) noexcept;

  //! Delete the given pointer memory
  template <std::derived_from<Type> Derived>
  constexpr void operator()(std::add_pointer_t<Derived> memory) noexcept;


  //! Return the underlying memory resource
  constexpr auto resource() noexcept -> std::pmr::memory_resource*;

 private:
  //! Delete the given pointer memory
  template <typename T>
  constexpr void deleteMemory(std::add_pointer_t<T> memory) noexcept;


  std::pmr::memory_resource* resource_ = nullptr;
};

/*!
  \brief Smart pointer with unique object with polymorphic_allocator

  No detailed description.

  \tparam Type No description.
  */
template <typename Type>
using unique_ptr = std::unique_ptr<Type, UniquePtrDeleter<Type>>;

//! Create a unique pointer that manages a new object allocated using polymorphic_allocator
template <typename Type, typename ...ArgTypes>
auto allocateUnique(const std::pmr::polymorphic_allocator<Type> alloc,
                    ArgTypes&&... arguments) -> unique_ptr<Type>;

} // namespace zisc::pmr

#include "std_memory_resource-inl.hpp"

#endif // ZISC_STD_MEMORY_RESOURCE_HPP
