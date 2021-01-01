/*!
  \file std_memory_resource-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STD_MEMORY_RESOURCE_INL_HPP
#define ZISC_STD_MEMORY_RESOURCE_INL_HPP

#include "std_memory_resource.hpp"
// Standard C++ library
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/concepts.hpp"

namespace zisc {

namespace pmr {

/*!
  \details No detailed description
  */
template <typename Type> inline
UniquePtrDeleter<Type>::UniquePtrDeleter() noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] alloc No description.
  */
template <typename Type> inline
UniquePtrDeleter<Type>::UniquePtrDeleter(
    const zisc::pmr::polymorphic_allocator<Type>& alloc) noexcept :
        resource_{alloc.resource()}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type> inline
UniquePtrDeleter<Type>::UniquePtrDeleter(UniquePtrDeleter&& other) noexcept:
    resource_{other.resource()}
{
}

/*!
  \details No detailed description

  \tparam Derived No description.
  \param [in] other No description.
  */
template <typename Type> template <DerivedFrom<Type> Derived> inline
UniquePtrDeleter<Type>::UniquePtrDeleter(UniquePtrDeleter<Derived>&& other) noexcept
    : resource_{other.resource()}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type> inline
auto UniquePtrDeleter<Type>::operator=(UniquePtrDeleter&& other) noexcept
    -> UniquePtrDeleter&
{
  resource_ = other.resource();
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
    zisc::pmr::polymorphic_allocator<Type> alloc{resource()};
    constexpr std::size_t n = 1;
    alloc.deallocate(memory, n);
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
zisc::pmr::memory_resource* UniquePtrDeleter<Type>::resource() noexcept
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
unique_ptr<Type> allocateUnique(const zisc::pmr::polymorphic_allocator<Type> alloc,
                                ArgTypes&&... arguments)
{
  using Pointer = std::add_pointer_t<Type>;
  constexpr std::size_t n = 1;
  zisc::pmr::polymorphic_allocator<Type> a = alloc;
  Pointer memory = a.allocate(n);
  a.construct(memory, std::forward<ArgTypes>(arguments)...);

  unique_ptr<Type> p{memory, alloc};
  return p;
}

} // namespace pmr

} // namespace zisc

#endif // ZISC_STD_MEMORY_RESOURCE_INL_HPP
