/*!
  \file search_tree-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SEARCH_TREE_INL_HPP
#define ZISC_SEARCH_TREE_INL_HPP

#include "search_tree.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <type_traits>
// Zisc
#include "query_result.hpp"
#include "query_value.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <typename SearchTreeClass> template <std::convertible_to<double> Type> inline
auto SearchTree<SearchTreeClass>::add(const Type& value) -> QueryResultT
{
  auto& search_tree = ref();
  return search_tree.add(value);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass> inline
auto SearchTree<SearchTreeClass>::capacity() const noexcept -> size_type
{
  auto& search_tree = ref();
  const size_type cap = search_tree.capacity();
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass> inline
constexpr auto SearchTree<SearchTreeClass>::capacityMax() noexcept -> size_type
{
  const size_type cap = SearchTreeT::capacityMax();
  return cap;
}

/*!
  \details No detailed description
  */
template <typename SearchTreeClass> inline
void SearchTree<SearchTreeClass>::clear() noexcept
{
  auto& search_tree = ref();
  search_tree.clear();
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <typename SearchTreeClass> template <std::convertible_to<double> Type> inline
auto SearchTree<SearchTreeClass>::contain(const Type& value) const noexcept -> QueryResultT
{
  auto& search_tree = ref();
  return search_tree.contain(value);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass> inline
auto SearchTree<SearchTreeClass>::findMinKey() const noexcept -> QueryResultKeyT
{
  auto& search_tree = ref();
  return search_tree.findMinKey();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass> inline
constexpr bool SearchTree<SearchTreeClass>::isBounded() noexcept
{
  const size_type c = capacityMax();
  const bool result = c < kUnboundedCapacityMax;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass> inline
constexpr bool SearchTree<SearchTreeClass>::isConcurrent() noexcept
{
  const bool result = SearchTreeT::isConcurrent();
  return result;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <typename SearchTreeClass> template <std::convertible_to<double> Type> inline
auto SearchTree<SearchTreeClass>::remove(const Type& value) -> QueryResultT
{
  auto& search_tree = ref();
  return search_tree.remove(value);
}

/*!
  \details No detailed description
  */
template <typename SearchTreeClass> inline
void SearchTree<SearchTreeClass>::setCapacity(const size_type cap) noexcept
{
  auto& search_tree = ref();
  search_tree.setCapacity(cap);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass> inline
auto SearchTree<SearchTreeClass>::size() const noexcept -> size_type
{
  auto& search_tree = ref();
  const size_type s = search_tree.size();
  return s;
}

/*!
  \details No detailed description
  */
template <typename SearchTreeClass> inline
SearchTree<SearchTreeClass>::SearchTree() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename SearchTreeClass> inline
SearchTree<SearchTreeClass>::SearchTree([[maybe_unused]] const SearchTree& other) noexcept
{
  static_assert(sizeof(QueryResultT) == sizeof(size_type));
  static_assert(sizeof(QueryResultKeyT) == sizeof(double));
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename SearchTreeClass> inline
auto SearchTree<SearchTreeClass>::operator=([[maybe_unused]] const SearchTree& other) noexcept -> SearchTree&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass> inline
auto SearchTree<SearchTreeClass>::ref() noexcept -> SearchTreeReference
{
  auto q = static_cast<SearchTreePtr>(this);
  return *q;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass> inline
auto SearchTree<SearchTreeClass>::ref() const noexcept -> ConstSearchTreeReference
{
  auto q = static_cast<ConstSearchTreePtr>(this);
  return *q;
}

} // namespace zisc

#endif // ZISC_SEARCH_TREE_INL_HPP
