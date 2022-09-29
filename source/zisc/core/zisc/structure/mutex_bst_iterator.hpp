/*!
  \file mutex_bst_iterator.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_BST_ITERATOR_HPP
#define ZISC_MUTEX_BST_ITERATOR_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
// Zisc
#include "map.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam Key No description.
  \tparam T No description.
  */
template <typename Key, typename T = void>
class MutexBstIterator
{
 public:
  // Type aliases
  using KeyT = std::remove_volatile_t<Key>;
  using ConstKeyT = std::add_const_t<KeyT>;
  using MappedT = std::remove_volatile_t<T>;
  using ValueT = std::conditional_t<std::is_void_v<T>, KeyT,
                                                       std::pair<KeyT, MappedT>>;
  using ConstValueT = std::add_const_t<ValueT>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_const_t<std::add_lvalue_reference_t<ConstValueT>>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_const_t<std::add_pointer_t<ConstValueT>>;
  using PointerPtr = std::add_pointer_t<Pointer>;
  using ConstPointerPtr = std::add_const_t<std::add_pointer_t<ConstPointer>>;

  // Type aliases for STL
  using value_type = ValueT;
  using difference_type = std::ptrdiff_t;
  using reference = Reference;
  using pointer = Pointer;
  using iterator_category = std::forward_iterator_tag;
  using iterator_concept = iterator_category;


  //! Create an iterator
  MutexBstIterator(PointerPtr ptr) noexcept;


  //! Dereference pointer to the underlying object
  Reference operator*() noexcept;

  //! Dereference pointer to the underlying object
  ConstReference operator*() const noexcept;

  //! Dereference pointer to the underlying object
  Pointer operator->() noexcept;

  //! Dereference pointer to the underlying object
  ConstPointer operator->() const noexcept;

  //! Increment the iterator
  MutexBstIterator& operator++() noexcept;

  //! Increment the iterator
  MutexBstIterator operator++(int) noexcept;


  //! Return the underlying pointer
  PointerPtr get() noexcept;

  //! Return the underlying pointer
  ConstPointerPtr get() const noexcept;

 private:
  PointerPtr ptr_ = nullptr;
};

//! Compare the values
template <typename Key, typename T>
bool operator==(const MutexBstIterator<Key, T>& lhs,
                const MutexBstIterator<Key, T>& rhs) noexcept;

//! Compare the values
template <typename Key, typename T>
bool operator!=(const MutexBstIterator<Key, T>& lhs,
                const MutexBstIterator<Key, T>& rhs) noexcept;

} /* namespace z */

#include "mutex_bst_iterator-inl.hpp"

#endif /* ZISC_MUTEX_BST_ITERATOR_HPP */
