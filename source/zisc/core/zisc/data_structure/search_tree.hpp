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
#include <limits>
#include <type_traits>
// Zisc
#include "query_result.hpp"
#include "query_value.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam SearchTreeClass No description.
  */
template <typename SearchTreeClass>
class SearchTree : private NonCopyable<SearchTree<SearchTreeClass>>
{
 public:
  // Type aliases for STL
  using size_type = std::size_t;

  // Type aliases
  using QueryResultT = QueryResult<QueryValue<size_type>>;
  using QueryResultKeyT = QueryResult<double>;


  //! Insert the given value into the search tree
  template <std::convertible_to<double> Type>
  [[nodiscard]] QueryResultT add(const Type& value);

  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Check if the given value is contained in the search tree
  template <std::convertible_to<double> Type>
  [[nodiscard]] QueryResultT contain(const Type& value) const noexcept;

  //! Find the minimum key in the search tree
  [[nodiscard]] QueryResultKeyT findMinKey() const noexcept;

  //! Check if the search tree is bounded
  static constexpr bool isBounded() noexcept;

  //! Check if the search tree is concurrent
  static constexpr bool isConcurrent() noexcept;

  //! Remove the value from the search tree
  template <std::convertible_to<double> Type>
  [[nodiscard]] QueryResultT remove(const Type& value);

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


  static constexpr size_type kUnboundedCapacityMax = (std::numeric_limits<size_type>::max)();


  //! Create a search tree
  SearchTree() noexcept;

  //! Move a data
  SearchTree(const SearchTree& other) noexcept;


  //! Move a data
  SearchTree& operator=(const SearchTree& other) noexcept;


  //! Return the reference to the search tree class
  SearchTreeReference ref() noexcept;

  //! Return the reference to the search tree class
  ConstSearchTreeReference ref() const noexcept;
};

} // namespace zisc

#include "search_tree-inl.hpp"

#endif // ZISC_SEARCH_TREE_HPP
