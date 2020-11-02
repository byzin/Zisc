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
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
// Zisc
#include "zisc/concepts.hpp"

namespace zisc {

namespace pmr {

// Memory resource

using memory_resource =
#if __has_include(<memory_resource>)
    std::pmr::memory_resource;
#elif __has_include(<experimental/memory_resource>)
    std::experimental::pmr::memory_resource;
#else
    void*;
    static_assert(false, "'std::pmr::memory_resource' not found.");
#endif

template <typename Type>
using polymorphic_allocator =
#if __has_include(<memory_resource>)
    std::pmr::polymorphic_allocator<Type>;
#elif __has_include(<experimental/memory_resource>)
    std::experimental::pmr::polymorphic_allocator<Type>;
#else
    void*;
    static_assert(false, "'std::pmr::polymorphic_allocator<Type>' not found.");
#endif


// Sequence containers 

template <typename Type>
using deque = std::deque<Type, polymorphic_allocator<Type>>;

template <typename Type>
using forward_list = std::forward_list<Type, polymorphic_allocator<Type>>;

template <typename Type>
using list = std::list<Type, polymorphic_allocator<Type>>;

template <typename Type>
using vector = std::vector<Type, polymorphic_allocator<Type>>;

// Associative containers

template <typename Key, typename Compare = std::less<Key>>
using set = std::set<Key, Compare, polymorphic_allocator<Key>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using map = std::map<Key,
                     T,
                     Compare,
                     polymorphic_allocator<std::pair<const Key, T>>>;

template <typename Key, typename Compare = std::less<Key>>
using multiset = std::multiset<Key, Compare, polymorphic_allocator<Key>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using multimap = std::multimap<Key,
                               T,
                               Compare,
                               polymorphic_allocator<std::pair<const Key, T>>>;

// Unordered associative containers

template <typename Key,
          typename Hash = std::hash<Key>,
          typename Pred = std::equal_to<Key>>
using unordered_set = std::unordered_set<Key,
                                         Hash,
                                         Pred,
                                         polymorphic_allocator<Key>>;

template <typename Key,
          typename T,
          typename Hash = std::hash<Key>,
          typename Pred = std::equal_to<Key>>
using unordered_map = std::unordered_map<Key,
                                         T,
                                         Hash,
                                         Pred,
                                         polymorphic_allocator<std::pair<const Key, T>>>;

template <typename Key,
          typename Hash = std::hash<Key>,
          typename Pred = std::equal_to<Key>>
using unordered_multiset = std::unordered_multiset<Key,
                                                   Hash,
                                                   Pred,
                                                   polymorphic_allocator<Key>>;

template <typename Key,
          typename T,
          typename Hash = std::hash<Key>,
          typename Pred = std::equal_to<Key>>
using unordered_multimap = std::unordered_multimap<Key,
                                                   T,
                                                   Hash,
                                                   Pred,
                                                   polymorphic_allocator<std::pair<const Key, T>>>;

// StringStream

template <typename CharType, typename Traits = std::char_traits<CharType>>
using basic_stringbuf = std::basic_stringbuf<CharType,
                                             Traits,
                                             polymorphic_allocator<CharType>>;
using stringbuf = basic_stringbuf<char>;
using wstringbuf = basic_stringbuf<wchar_t>;

template <typename CharType, typename Traits = std::char_traits<CharType>>
using basic_istringstream = std::basic_istringstream<CharType,
                                                     Traits,
                                                     polymorphic_allocator<CharType>>;
using istringstream = basic_istringstream<char>;
using wistringstream = basic_istringstream<wchar_t>;

template <typename CharType, typename Traits = std::char_traits<CharType>>
using basic_ostringstream = std::basic_ostringstream<CharType,
                                                     Traits,
                                                     polymorphic_allocator<CharType>>;
using ostringstream = basic_ostringstream<char>;
using wostringstream = basic_ostringstream<wchar_t>;

template <typename CharType, typename Traits = std::char_traits<CharType>>
using basic_stringstream = std::basic_stringstream<CharType,
                                                   Traits,
                                                   polymorphic_allocator<CharType>>;
using stringstream = basic_stringstream<char>;
using wstringstream = basic_stringstream<wchar_t>;

// Strings

template <typename CharType, typename  Traits = std::char_traits<CharType>>
using basic_string = std::basic_string<CharType,
                                       Traits,
                                       polymorphic_allocator<CharType>>;
using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u8string = basic_string<char8_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

// Regex

template <typename BidirIt>
using match_results = std::match_results<BidirIt,
                                         polymorphic_allocator<std::sub_match<BidirIt>>>;
using cmatch = match_results<const char*>;
using wcmatch = match_results<const wchar_t*>;
using smatch = match_results<std::string::const_iterator>;
using wsmatch = match_results<std::wstring::const_iterator>;

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


  //! Create an empty
  UniquePtrDeleter() noexcept;

  //! Create a memory deleter
  UniquePtrDeleter(const polymorphic_allocator<Type>& alloc) noexcept;

  //! Move a data
  UniquePtrDeleter(UniquePtrDeleter&& other) noexcept;

  //! Move a data from a super class
  template <DerivedFrom<Type> Derived>
  UniquePtrDeleter(UniquePtrDeleter<Derived>&& other) noexcept;


  //! Move a data
  UniquePtrDeleter& operator=(UniquePtrDeleter&& other) noexcept;

  //! Delete the given pointer memory
  void operator()(Pointer memory) noexcept;


  //! Return the underlying memory resource
  memory_resource* resource() noexcept;

 private:
  memory_resource* resource_ = nullptr;
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
unique_ptr<Type> allocateUnique(const polymorphic_allocator<Type> alloc,
                                ArgTypes&&... arguments);

} // namespace pmr 

} // namespace zisc

#include "std_memory_resource-inl.hpp"

#endif // ZISC_STD_MEMORY_RESOURCE_HPP
