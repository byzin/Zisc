/*!
  \file write_once-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_WRITE_ONCE_INL_HPP
#define ZISC_FLOCK_WRITE_ONCE_INL_HPP

#include "write_once.hpp"
// Standard C++ library
#include <atomic>
#include <type_traits>
// Zisc
#include "log.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \details No detailed description
  */
template <typename Type> inline
WriteOnce<Type>::WriteOnce() noexcept
{
}

/*!
  \details No detailed description
  */
template <typename Type> inline
WriteOnce<Type>::WriteOnce(ValueT initial) noexcept :
    value_{initial}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type> inline
WriteOnce<Type>::WriteOnce(WriteOnce&& other) noexcept :
    value_{other.value_.load(std::memory_order::acquire)}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename Type> inline
auto WriteOnce<Type>::operator=(ValueT value) noexcept -> WriteOnce&
{
  store(value);
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename Type> inline
auto WriteOnce<Type>::operator=(WriteOnce&& other) noexcept -> WriteOnce&
{
  ValueT v = other.value_.load(std::memory_order::acquire);
  value_.store(v, std::memory_order::release);
  return *this;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <typename Type> inline
void WriteOnce<Type>::init(ValueT value) noexcept
{
  store(value);
}

/*!
  \details No detailed description

  \param [in] log No description.
  \return No description
  */
template <typename Type> inline
auto WriteOnce<Type>::load(Log* log) const noexcept -> ValueT
{
  return log->commitValueSafe(value_.load(std::memory_order::acquire)).first;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <typename Type> inline
void WriteOnce<Type>::store(ValueT value) noexcept
{
  value_.store(value, std::memory_order::release);
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_WRITE_ONCE_INL_HPP */
