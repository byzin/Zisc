/*!
  \file unique_pointer-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIQUE_POINTER_INL_HPP
#define ZISC_UNIQUE_POINTER_INL_HPP

#include "unique_pointer.hpp"
// Standard C++ library
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "error.hpp"
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename Type> inline
UniquePointer<Type>::UniquePointer() noexcept
{
}

/*!
  \details No detailed description

  \param [in] data No description.
  \param [in,out] mem_resource No description.
  */
template <typename Type> inline
UniquePointer<Type>::UniquePointer(
    pointer data,
    std::pmr::memory_resource* mem_resource) noexcept :
        data_{data},
        mem_resource_{mem_resource}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type> inline
UniquePointer<Type>::UniquePointer(UniquePointer&& other) noexcept
{
  swap(other);
}

/*!
  \details No detailed description

  \tparam Super No description.
  \param [in] other No description.
  */
template <typename Type> template <typename Super> inline
UniquePointer<Type>::UniquePointer(UniquePointer<Super>&& other,
                                   EnableIfBaseOf<Type, Super>) noexcept
{
  reset(other);
}

/*!
  \details No detailed description
  */
template <typename Type> inline
UniquePointer<Type>::~UniquePointer() noexcept
{
  reset();
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename Type> inline
auto UniquePointer<Type>::operator=(UniquePointer&& other) noexcept
    -> UniquePointer&
{
  swap(other);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto UniquePointer<Type>::operator*() noexcept -> reference
{
  auto data = get();
  ZISC_ASSERT(data != nullptr, "The data is null.");
  return *data;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto UniquePointer<Type>::operator*() const noexcept -> const_reference
{
  const auto data = get();
  ZISC_ASSERT(data != nullptr, "The data is null.");
  return *data;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto UniquePointer<Type>::operator->() noexcept -> pointer
{
  return get();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto UniquePointer<Type>::operator->() const noexcept -> const_pointer
{
  return get();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto UniquePointer<Type>::get() noexcept -> pointer
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto UniquePointer<Type>::get() const noexcept -> const_pointer
{
  return data_;
}

/*!
  \details No detailed description

  \tparam Types No description.
  \param [in] mem_resource No description.
  \param [in] arguments No description.
  \return No description
  */
template <typename Type> template <typename ...Types> inline
auto UniquePointer<Type>::make(std::pmr::memory_resource* mem_resource,
                               Types&&... arguments) noexcept -> UniquePointer
{
  //! \todo Exception check
  ZISC_ASSERT(mem_resource != nullptr, "The memory resource is null.");

  Allocator alloc{mem_resource};
  auto data = alloc.allocate(1);
  alloc.construct(data, std::forward<Types>(arguments)...);
  auto ptr = UniquePointer{data, mem_resource};
  return ptr;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
std::pmr::memory_resource* UniquePointer<Type>::memoryResource() noexcept
{
  return mem_resource_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
const std::pmr::memory_resource* UniquePointer<Type>::memoryResource()
    const noexcept
{
  return mem_resource_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
auto UniquePointer<Type>::release() noexcept -> pointer
{
  auto data = get();
  data_ = nullptr;
  mem_resource_ = nullptr;
  return data;
}

/*!
  \details No detailed description
  */
template <typename Type> inline
void UniquePointer<Type>::reset() noexcept
{
  reset(nullptr, nullptr);
}

/*!
  \details No detailed description

  \param [in] data No description.
  \param [in,out] mem_resource No description.
  */
template <typename Type> inline
void UniquePointer<Type>::reset(pointer data,
                                std::pmr::memory_resource* mem_resource) noexcept
{
  auto prev_data = get();
  if (prev_data != nullptr) {
    ZISC_ASSERT(mem_resource_ != nullptr, "The memory resource is null.");
    std::destroy_at<value_type>(prev_data);
    Allocator alloc{mem_resource_};
    alloc.deallocate(prev_data, 1);
  }
  data_ = data;
  mem_resource_ = mem_resource;
}

/*!
  \details No detailed description

  \tparam Super No description.
  \param [in] other No description.
  */
template <typename Type> template <typename Super> inline
void UniquePointer<Type>::reset(UniquePointer<Super>& other,
                                EnableIfBaseOf<Type, Super>) noexcept
{
  auto mem_resource = other.memoryResource();
  auto data = other.release();
  reset(data, mem_resource);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type> inline
void UniquePointer<Type>::swap(UniquePointer& other) noexcept
{
  zisc::swap(data_, other.data_);
  zisc::swap(mem_resource_, other.mem_resource_);
}

/*!
  \details No detailed description

  \param [in,out] lhs No description.
  \param [in,out] rhs No description.
  */
template <typename Type> inline
void swap(UniquePointer<Type>& lhs, UniquePointer<Type>& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace zisc

#endif // ZISC_UNIQUE_POINTER_INL_HPP
