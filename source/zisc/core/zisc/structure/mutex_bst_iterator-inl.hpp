/*!
  \file mutex_bst_iterator-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_BST_ITERATOR_INL_HPP
#define ZISC_MUTEX_BST_ITERATOR_INL_HPP

#include "mutex_bst_iterator.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace zisc {

/*!
  \details No detailed description

  \param [in] ptr No description.
  */
template <typename Key, typename T> inline
MutexBstIterator<Key, T>::MutexBstIterator(PointerPtr ptr) noexcept : ptr_{ptr}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Key, typename T> inline
auto MutexBstIterator<Key, T>::operator*() noexcept -> Reference
{
  return **ptr_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Key, typename T> inline
auto MutexBstIterator<Key, T>::operator*() const noexcept -> ConstReference
{
  return **ptr_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Key, typename T> inline
auto MutexBstIterator<Key, T>::operator->() noexcept -> Pointer
{
  return *ptr_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Key, typename T> inline
auto MutexBstIterator<Key, T>::operator->() const noexcept -> ConstPointer
{
  return *ptr_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Key, typename T> inline
auto MutexBstIterator<Key, T>::operator++() noexcept -> MutexBstIterator&
{
  ++ptr_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Key, typename T> inline
auto MutexBstIterator<Key, T>::operator++(int) noexcept -> MutexBstIterator
{
  MutexBstIterator ite{ptr_};
  ++(*this);
  return ite;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Key, typename T> inline
auto MutexBstIterator<Key, T>::get() noexcept -> PointerPtr
{
  return ptr_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Key, typename T> inline
auto MutexBstIterator<Key, T>::get() const noexcept -> ConstPointerPtr
{
  return ptr_;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Key, typename T> inline
auto operator==(const MutexBstIterator<Key, T>& lhs,
                const MutexBstIterator<Key, T>& rhs) noexcept -> bool
{
  return lhs.get() == rhs.get();
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Key, typename T> inline
auto operator!=(const MutexBstIterator<Key, T>& lhs,
                const MutexBstIterator<Key, T>& rhs) noexcept -> bool
{
  return !(lhs == rhs);
}

} /* namespace zisc */

#endif /* ZISC_MUTEX_BST_ITERATOR_INL_HPP */
