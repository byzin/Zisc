/*!
  \file unique_memory_pointer-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
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
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
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
    std::pmr::memory_resource* mem_resource) noexcept :
        data_{data},
        mem_resource_{mem_resource}
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
template <typename Type> template <typename Super> inline
UniqueMemoryPointer<Type>::UniqueMemoryPointer(UniqueMemoryPointer<Super>&& other,
                                               EnableIfBaseOf<Type, Super>) noexcept
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
auto UniqueMemoryPointer<Type>::make(std::pmr::memory_resource* mem_resource,
                                     Types&&... arguments) noexcept
    -> UniqueMemoryPointer
{
  ZISC_ASSERT(mem_resource != nullptr, "The memory resource is null.");

  Allocator alloc{mem_resource};
  auto data = alloc.allocate(1);
  if (data != nullptr)
    alloc.construct(data, std::forward<Types>(arguments)...);
  return (data != nullptr)
      ? UniqueMemoryPointer{data, mem_resource}
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
void UniqueMemoryPointer<Type>::reset(pointer data,
                                      std::pmr::memory_resource* mem_resource) noexcept
{
  auto prev_data = get();
  if (prev_data != nullptr) {
    ZISC_ASSERT(mem_resource_ != nullptr, "The memory resource is null.");
    Allocator alloc{mem_resource_};
    std::destroy_at<value_type>(prev_data);
    alloc.deallocate(prev_data, 1);
  }
  data_ = data;
  mem_resource_ = mem_resource;
}

/*!
  */
template <typename Type> inline
std::pmr::memory_resource* UniqueMemoryPointer<Type>::memoryResource() noexcept
{
  return mem_resource_;
}

/*!
  */
template <typename Type> inline
const std::pmr::memory_resource* UniqueMemoryPointer<Type>::memoryResource()
    const noexcept
{
  return mem_resource_;
}

/*!
  */
template <typename Type> inline
auto UniqueMemoryPointer<Type>::release() noexcept -> pointer
{
  auto data = get();
  data_ = nullptr;
  mem_resource_ = nullptr;
  return data;
}

/*!
  */
template <typename Type> inline
void UniqueMemoryPointer<Type>::swap(UniqueMemoryPointer& other) noexcept
{
  auto tmp_data = get();
  auto tmp_resource = mem_resource_;

  data_ = other.get();
  mem_resource_ = other.mem_resource_;

  other.data_ = tmp_data;
  other.mem_resource_ = tmp_resource;
}

/*!
  */
template <typename Type> template <typename Super> inline
void UniqueMemoryPointer<Type>::swap(UniqueMemoryPointer<Super>& other,
                                     EnableIfBaseOf<Type, Super>) noexcept
{
  auto tmp_data = get();
  auto tmp_resource = mem_resource_;

  mem_resource_ = other.memoryResource();
  data_ = cast<pointer>(other.release());

  using OtherPointer = UniqueMemoryPointer<Super>;
  other = OtherPointer{cast<typename OtherPointer::pointer>(tmp_data), tmp_resource};
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
