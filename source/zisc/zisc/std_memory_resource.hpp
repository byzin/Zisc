/*!
  \file std_memory_resource.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STD_MEMORY_RESOURCE_HPP
#define ZISC_STD_MEMORY_RESOURCE_HPP

// Standard C++ library
#if __has_include(<memory_resource>)
#include <memory_resource>
#elif __has_include(<experimental/memory_resource>)
#include <experimental/memory_resource>
#elif
static_assert(false, "Compiler doesn't have 'memory_resource'.");
#endif
#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
// Zisc
#include "type_traits.hpp"

namespace std {

namespace pmr {

#if __has_include(<memory_resource>)
#elif __has_include(<experimental/memory_resource>)

using memory_resource = std::experimental::pmr::memory_resource;

template <typename Type>
using polymorphic_allocator = std::experimental::pmr::polymorphic_allocator<Type>;

#endif

} // namespace pmr

} // namespace std

namespace zisc {

namespace pmr {

// Sequence containers 

template <typename Type>
using deque = std::deque<Type, std::pmr::polymorphic_allocator<Type>>;

template <typename Type>
using forward_list = std::forward_list<Type,
                                       std::pmr::polymorphic_allocator<Type>>;

template <typename Type>
using list = std::list<Type, std::pmr::polymorphic_allocator<Type>>;

template <typename Type>
using vector = std::vector<Type, std::pmr::polymorphic_allocator<Type>>;

// Associative containers

template <typename Key, typename Compare = std::less<Key>>
using set = std::set<Key,
                     Compare,
                     std::pmr::polymorphic_allocator<Key>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using map = std::map<Key,
                     T,
                     Compare,
                     std::pmr::polymorphic_allocator<std::pair<const Key, T>>>;

template <typename Key, typename Compare = std::less<Key>>
using multiset = std::multiset<Key,
                               Compare,
                               std::pmr::polymorphic_allocator<Key>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using multimap = std::multimap<Key,
                               T,
                               Compare,
                               std::pmr::polymorphic_allocator<std::pair<const Key, T>>>;

// Unordered associative containers

template <typename Key,
          typename Hash = std::hash<Key>,
          typename Pred = std::equal_to<Key>>
using unordered_set = std::unordered_set<Key,
                                         Hash,
                                         Pred,
                                         std::pmr::polymorphic_allocator<Key>>;

template <typename Key,
          typename T,
          typename Hash = std::hash<Key>,
          typename Pred = std::equal_to<Key>>
using unordered_map = std::unordered_map<Key,
                                         T,
                                         Hash,
                                         Pred,
                                         std::pmr::polymorphic_allocator<std::pair<const Key, T>>>;

template <typename Key,
          typename Hash = std::hash<Key>,
          typename Pred = std::equal_to<Key>>
using unordered_multiset = std::unordered_multiset<Key,
                                                   Hash,
                                                   Pred,
                                                   std::pmr::polymorphic_allocator<Key>>;

template <typename Key,
          typename T,
          typename Hash = std::hash<Key>,
          typename Pred = std::equal_to<Key>>
using unordered_multimap = std::unordered_multimap<Key,
                                                   T,
                                                   Hash,
                                                   Pred,
                                                   std::pmr::polymorphic_allocator<std::pair<const Key, T>>>;

// Strings

template <typename CharType, class Traits = std::char_traits<CharType>>
using basic_string = std::basic_string<CharType,
                                       Traits,
                                       std::pmr::polymorphic_allocator<CharType>>;
using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

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


  //! Create an empty
  UniquePtrDeleter() noexcept;

  //! Create a memory deleter
  UniquePtrDeleter(const std::pmr::polymorphic_allocator<Type>& alloc) noexcept;

  //! Move a data
  UniquePtrDeleter(UniquePtrDeleter&& other) noexcept;

  //! Move a data from a super class
  template <typename Super>
  UniquePtrDeleter(UniquePtrDeleter<Super>&& other,
                   EnableIfBaseOf<Type, Super> = kEnabler) noexcept;


  //! Move a data
  UniquePtrDeleter& operator=(UniquePtrDeleter&& other) noexcept;

  //! Delete the given pointer memory
  void operator()(Pointer memory) noexcept;


  //! Return the underlying memory resource
  std::pmr::memory_resource* resource() noexcept;

 private:
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
template <typename Type, typename ...ArgumentTypes>
unique_ptr<Type> allocateUnique(const std::pmr::polymorphic_allocator<Type> alloc,
                                ArgumentTypes&&... arguments);

} // namespace pmr 

} // namespace zisc

#include "std_memory_resource-inl.hpp"

#endif // ZISC_STD_MEMORY_RESOURCE_HPP
