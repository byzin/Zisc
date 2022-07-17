/*!
  \file mutable-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_MUTABLE_INL_HPP
#define ZISC_FLOCK_MUTABLE_INL_HPP

#include "mutable.hpp"
// Standard C++ library
#include <atomic>
#include <concepts>
#include <span>
#include <utility>
// Zisc
#include "log.hpp"
#include "tag.hpp"
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \param [in] log_list No description.
  */
template <Pointer Type> inline
Mutable<Type>::Mutable() noexcept :
    Mutable(static_cast<Type>(0))
{
}

/*!
  \details No detailed description

  \param [in] log_list No description.
  */
template <Pointer Type> inline
Mutable<Type>::Mutable(ValueT value) noexcept :
    value_{TaggedT::init(value)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <Pointer Type> inline
Mutable<Type>::Mutable(Mutable&& other) noexcept :
    value_{other.value_.load(std::memory_order::acquire)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <Pointer Type> inline
auto Mutable<Type>::operator=(Mutable&& other) noexcept -> Mutable&
{
  value_ = other.value_.load(std::memory_order::acquire);
  return *this;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <Pointer Type> inline
void Mutable<Type>::init(ValueT value) noexcept
{
  value_.store(TaggedT::init(value), std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] log No description.
  \return No description
  */
template <Pointer Type> inline
auto Mutable<Type>::load(Log* log) const noexcept -> ValueT
{
  return TaggedT::value(getValue(log));
}

/*!
  \details No detailed description

  \param [in] log No description.
  \return No description
  */
template <Pointer Type> inline
auto Mutable<Type>::read() const noexcept -> ValueT
{
  return TaggedT::value(value_.load(std::memory_order::acquire));
}

/*!
  \details No detailed description

  \param [in] value No description.
  \param [in] log No description.
  */
template <Pointer Type> inline
void Mutable<Type>::store(ValueT value,
                          WriteAnnouncements* write_announcements,
                          Log* log) noexcept
{
  TaggedT::cas(value_, getValue(log), value, write_announcements, log);
}

/*!
  \details No detailed description

  \param [in] log No description.
  \return No description
  */
template <Pointer Type> inline
auto Mutable<Type>::getValue(Log* log) const noexcept -> IT
{
  const IT v = value_.load(std::memory_order::acquire);
  return log->commitValue(v).first;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_MUTABLE_INL_HPP */
