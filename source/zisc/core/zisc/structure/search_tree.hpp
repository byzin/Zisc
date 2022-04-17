/*!
  \file search_tree.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SEARCH_TREE_HPP
#define ZISC_SEARCH_TREE_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

//! Specify a type is a value type of search tree
template <typename Type>
concept MappedValue = std::is_void_v<Type> || std::movable<Type>;

/*!
  \brief No brief description

  No detailed description.

  \tparam SearchTreeClass No description.
  \tparam Key No description.
  \tparam T No description.
  \tparam Compare No description.
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T = void,
          std::invocable<Key, Key> Compare = std::less<Key>>
class SearchTree : private NonCopyable<SearchTree<SearchTreeClass, Key, T, Compare>>
{
 public:
  // Type aliases
  using KeyT = std::remove_volatile_t<Key>;
  using ConstKeyT = std::add_const_t<KeyT>;
  using MappedT = std::remove_volatile_t<T>;
  using ValueT = std::conditional_t<std::is_void_v<T>, KeyT,
                                                       std::pair<KeyT, MappedT>>;
  using ConstValueT = std::add_const_t<ValueT>;
  using CompareT = std::remove_volatile_t<Compare>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_lvalue_reference_t<ConstValueT>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_pointer_t<ConstValueT>;

  // Type aliases for STL
  using key_type = KeyT;
  using mapped_type = MappedT;
  using value_type = ValueT;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = CompareT;
  using reference = Reference;
  using const_reference = ConstReference;
  using pointer = Pointer;
  using const_pointer = ConstPointer;


  //! Insert the given value into the search tree
  [[nodiscard]] std::optional<size_type> add(ConstReference value);

  //! Insert the given value into the search tree
  [[nodiscard]] std::optional<size_type> add(RReference value);

  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Check if the given value is contained in the search tree
  [[nodiscard]] std::optional<size_type> contain(ConstKeyT& key) const noexcept;

  //! Find the minimum key in the search tree
  [[nodiscard]] std::optional<Pointer> findMinKey() noexcept;

  //! Find the minimum key in the search tree
  [[nodiscard]] std::optional<ConstPointer> findMinKey() const noexcept;

  //! Retrun the value by the given index
  Reference get(const size_type index) noexcept;

  //! Retrun the value by the given index
  ConstReference get(const size_type index) const noexcept;

  //! Check if the search tree is bounded
  static constexpr bool isBounded() noexcept;

  //! Check if the search tree is concurrent
  static constexpr bool isConcurrent() noexcept;

  //! Remove the value from the search tree
  [[nodiscard]] std::optional<size_type> remove(ConstKeyT& key);

  //! Change the maximum possible number of elements. The search tree will be cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements in the search tree
  size_type size() const noexcept;

 protected:
  // Type aliases
  using SearchTreeT = std::remove_volatile_t<SearchTreeClass>;
  using ConstSearchTreeT = std::add_const_t<SearchTreeT>;
  using SearchTreePtr = std::add_pointer_t<SearchTreeT>;
  using ConstSearchTreePtr = std::add_pointer_t<ConstSearchTreeT>;
  using SearchTreeReference = std::add_lvalue_reference_t<SearchTreeT>;
  using ConstSearchTreeReference = std::add_lvalue_reference_t<ConstSearchTreeT>;


  //! Create a search tree
  SearchTree() noexcept;

  //! Move a data
  SearchTree(const SearchTree& other) noexcept;


  //! Move a data
  SearchTree& operator=(const SearchTree& other) noexcept;


  //! Get the key from the given value
  static ConstKeyT& getKey(ConstReference value) noexcept;

  //! Return the reference to the search tree class
  SearchTreeReference ref() noexcept;

  //! Return the reference to the search tree class
  ConstSearchTreeReference ref() const noexcept;
};

} // namespace zisc

#include "search_tree-inl.hpp"

#endif // ZISC_SEARCH_TREE_HPP
