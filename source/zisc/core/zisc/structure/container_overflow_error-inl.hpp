/*!
  \file container_overflow_error-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONTAINER_OVERFLOW_ERROR_INL_HPP
#define ZISC_CONTAINER_OVERFLOW_ERROR_INL_HPP

#include "container_overflow_error.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] what_arg No description.
  \param [in,out] mem_resource No description.
  \param [in] value No description.
  */
template <std::movable T> inline
ContainerOverflowError<T>::ContainerOverflowError(const std::string_view what_arg,
                                                  pmr::memory_resource* mem_resource,
                                                  ConstReference value) :
    SystemError(ErrorCode::kBoundedQueueOverflow, what_arg),
    value_{std::allocate_shared<ValueT>(
        pmr::polymorphic_allocator<ValueT>{mem_resource},
        value)}
{
}

/*!
  \details No detailed description

  \param [in] what_arg No description.
  \param [in,out] mem_resource No description.
  \param [in] value No description.
  */
template <std::movable T> inline
ContainerOverflowError<T>::ContainerOverflowError(const std::string_view what_arg,
                                                  pmr::memory_resource* mem_resource,
                                                  RReference value) :
    SystemError(ErrorCode::kBoundedQueueOverflow, what_arg),
    value_{std::allocate_shared<ValueT>(
        pmr::polymorphic_allocator<ValueT>{mem_resource},
        std::move(value))}
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <std::movable T> inline
ContainerOverflowError<T>::ContainerOverflowError(ContainerOverflowError&& other) noexcept
    : SystemError(std::move(other)),
      value_{std::move(other.value_)}
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
template <std::movable T> inline
auto ContainerOverflowError<T>::operator=(ContainerOverflowError&& other) noexcept
    -> ContainerOverflowError&
{
  SystemError::operator=(std::move(other));
  value_ = std::move(other.value_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto ContainerOverflowError<T>::get() noexcept -> Reference
{
  return *value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto ContainerOverflowError<T>::get() const noexcept -> ConstReference
{
  return *value_;
}

} // namespace zisc

#endif // ZISC_CONTAINER_OVERFLOW_ERROR_INL_HPP
