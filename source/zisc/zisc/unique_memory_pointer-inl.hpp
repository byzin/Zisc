/*!
  \file unique_memory_pointer-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIQUE_MEMORY_POINTER_INL_HPP
#define ZISC_UNIQUE_MEMORY_POINTER_INL_HPP

#include "unique_memory_pointer.hpp"
// Standard C++ library
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "error.hpp"
#include "memory_resource.hpp"
#include "non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Type> inline
UniqueMemoryPointer<Type>::UniqueMemoryPointer() noexcept
{
}

/*!
  */
template <typename Type> inline
UniqueMemoryPointer<Type>::UniqueMemoryPointer(
    pointer data,
    Allocator* alloc) noexcept :
        data_{data},
        allocator_{alloc}
{
}

/*!
  */
template <typename Type> inline
UniqueMemoryPointer<Type>::UniqueMemoryPointer(UniqueMemoryPointer&& other) noexcept
{
  swap(other);
}

/*!
  */
template <typename Type> inline
UniqueMemoryPointer<Type>::~UniqueMemoryPointer() noexcept
{
  reset();
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::operator=(UniqueMemoryPointer&& other) noexcept
    -> UniqueMemoryPointer&
{
  swap(other);
  return *this;
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::operator*() noexcept -> reference
{
  auto data = get();
  ZISC_ASSERT(data != nullptr, "The data is null.");
  return *data;
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::operator*() const noexcept -> const_reference
{
  const auto data = get();
  ZISC_ASSERT(data != nullptr, "The data is null.");
  return *data;
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::operator->() noexcept -> pointer
{
  return get();
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::operator->() const noexcept -> const_pointer
{
  return get();
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::get() noexcept -> pointer
{
  return data_;
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::get() const noexcept -> const_pointer
{
  return data_;
}

/*!
  */
template <typename Type> template <typename ...Types> inline
auto UniqueMemoryPointer<Type>::make(Allocator* alloc, Types&&... arguments) noexcept
    -> UniqueMemoryPointer
{
  ZISC_ASSERT(alloc != nullptr, "The allocator is null.");
  auto data = alloc->allocate(1);
  if (data != nullptr)
    alloc->construct(data, std::forward<Types>(arguments)...);
  return (data != nullptr)
      ? UniqueMemoryPointer{data, alloc}
      : UniqueMemoryPointer{};
}

/*!
  */
template <typename Type> inline
void UniqueMemoryPointer<Type>::reset() noexcept
{
  reset(nullptr, nullptr);
}

/*!
  */
template <typename Type> inline
void UniqueMemoryPointer<Type>::reset(pointer data, Allocator* alloc) noexcept
{
  auto prev_data = get();
  if (prev_data != nullptr) {
    auto& prev_allocator = getAllocator();
    std::destroy_at<value_type>(prev_data);
    prev_allocator.deallocate(prev_data, 1);
  }
  data_ = data;
  allocator_ = alloc;
}

/*!
  */
template <typename Type> inline
void UniqueMemoryPointer<Type>::swap(UniqueMemoryPointer& other) noexcept
{
  auto tmp_data = get();
  auto tmp_alloc = allocator_;

  data_ = other.get();
  allocator_ = other.allocator_;

  other.data_ = tmp_data;
  other.allocator_ = tmp_alloc;
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::getAllocator() noexcept -> Allocator&
{
  ZISC_ASSERT(allocator_ != nullptr, "The allocator is null.");
  return *allocator_;
}

/*!
  */
template <typename Type> inline
void swap(UniqueMemoryPointer<Type>& lhs, UniqueMemoryPointer<Type>& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace zisc

#endif // ZISC_UNIQUE_MEMORY_POINTER_INL_HPP
