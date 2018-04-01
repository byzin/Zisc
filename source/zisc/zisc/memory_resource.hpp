/*!
  \file memory_resource.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MEMORY_RESOURCE_HPP
#define ZISC_MEMORY_RESOURCE_HPP

// Standard C++ library
#if __has_include(<memory_resource>)
#include <memory_resource>
#elif __has_include(<experimental/memory_resource>)
#include <experimental/memory_resource>
#elif
static_assert(false, "Compiler doesn't have 'memory_resource'.");
#endif
#include <forward_list>
#include <list>
#include <vector>

namespace zisc {

namespace pmr {

#if __has_include(<memory_resource>)
using memory_resource = std::pmr::memory_resource;
template <typename Type>
using polymorphic_allocator = std::pmr::polymorphic_allocator<Type>;
#elif __has_include(<experimental/memory_resource>)
using memory_resource = std::experimental::pmr::memory_resource;
template <typename Type>
using polymorphic_allocator = std::experimental::pmr::polymorphic_allocator<Type>;
#endif

template <typename Type>
using forward_list = std::forward_list<Type, polymorphic_allocator<Type>>;
template <typename Type>
using list = std::list<Type, polymorphic_allocator<Type>>;
template <typename Type>
using vector = std::vector<Type, polymorphic_allocator<Type>>;

} // namespace resource

} // namespace zisc

#endif // ZISC_MEMORY_RESOURCE_HPP
