/*!
  \file future-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUTURE_INL_HPP
#define ZISC_FUTURE_INL_HPP

#include "future.hpp"
// Standard C++ library
#include <future>
#include <utility>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] impl No description.
  \param [in] id No description.
  */
template <typename T> inline
Future<T>::Future(std::future<T>&& impl, const int64b id) noexcept :
    impl_{std::move(impl)},
    id_{id}
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <typename T> inline
Future<T>::Future(Future&& other) noexcept :
    impl_{std::move(other.impl_)},
    id_{other.id()}
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
template <typename T> inline
auto Future<T>::operator=(Future&& other) noexcept -> Future&
{
  impl_ = std::move(other.impl_);
  id_ = other.id();
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
T Future<T>::get()
{
  return impl_.get();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
int64b Future<T>::id() const noexcept
{
  return id_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
bool Future<T>::isValid() const noexcept
{
  return impl_.valid();
}

/*!
  \details No detailed description
  */
template <typename T> inline
void Future<T>::wait() const
{
  impl_.wait();
}

} /* namespace zisc */

#endif /* ZISC_FUTURE_INL_HPP */
