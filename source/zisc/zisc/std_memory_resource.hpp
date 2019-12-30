/*!
  \file std_memory_resource.hpp
  \author Sho Ikeda

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
#include <list>
#include <string>
#include <vector>

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

// Containers 

template <typename Type>
using deque = std::deque<Type, std::pmr::polymorphic_allocator<Type>>;

template <typename Type>
using forward_list = std::forward_list<Type,
                                       std::pmr::polymorphic_allocator<Type>>;

template <typename Type>
using list = std::list<Type, std::pmr::polymorphic_allocator<Type>>;

template <typename Type>
using vector = std::vector<Type, std::pmr::polymorphic_allocator<Type>>;

// Strings

template <typename CharType, class Traits = std::char_traits<CharType>>
using basic_string = std::basic_string<CharType,
                                       Traits,
                                       std::pmr::polymorphic_allocator<CharType>>;
using string = basic_string<char>;

} // namespace resource

} // namespace zisc

#endif // ZISC_STD_MEMORY_RESOURCE_HPP
