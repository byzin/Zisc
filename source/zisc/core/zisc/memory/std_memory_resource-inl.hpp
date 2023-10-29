/*!
  \file std_memory_resource-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STD_MEMORY_RESOURCE_INL_HPP
#define ZISC_STD_MEMORY_RESOURCE_INL_HPP

#include "std_memory_resource.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace zisc::pmr {

/*!
  \details No detailed description

  \param [in,out] alloc No description.
  */
template <typename Type> inline
UniquePtrDeleter<Type>::UniquePtrDeleter(const polymorphic_allocator<Type>& alloc) noexcept :
    resource_{alloc.resource()}
{
}

/*!
  \details No detailed description

  \tparam Derived No description.
  \param [in] other No description.
  */
template <typename Type> template <std::derived_from<Type> Derived> inline
UniquePtrDeleter<Type>::UniquePtrDeleter(UniquePtrDeleter<Derived>&& other) noexcept
    : resource_{std::move(other).resource()}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type> template <std::derived_from<Type> Derived> inline
auto UniquePtrDeleter<Type>::operator=(UniquePtrDeleter<Derived>&& other) noexcept
    -> UniquePtrDeleter&
{
  resource_ = std::move(other).resource();
  return *this;
}

/*!
  \details No detailed description

  \param [in,out] memory No description.
  */
template <typename Type> inline
void UniquePtrDeleter<Type>::operator()(Pointer memory) noexcept
{
  if (memory != nullptr) {
    std::destroy_at(memory);
    polymorphic_allocator<Type> alloc{resource()};
    constexpr std::size_t n = 1;
    alloc.deallocate(memory, n);
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto UniquePtrDeleter<Type>::resource() noexcept -> memory_resource*
{
  return resource_;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam ArgTypes No description.
  \param [in,out] alloc No description.
  \param [in,out] arguments No description.
  \return No description
  */
template <typename Type, typename ...ArgTypes> inline
auto allocateUnique(const polymorphic_allocator<Type> alloc,
                    ArgTypes&&... arguments) -> unique_ptr<Type>
{
  using Pointer = std::add_pointer_t<Type>;
  constexpr std::size_t n = 1;
  polymorphic_allocator<Type> a = alloc;
  Pointer memory = a.allocate(n);
  a.construct(memory, std::forward<ArgTypes>(arguments)...);

  return {memory, typename unique_ptr<Type>::deleter_type{alloc}};
}

} // namespace zisc::pmr

#endif // ZISC_STD_MEMORY_RESOURCE_INL_HPP
