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
#include <optional>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::add(ConstReference value)
    -> std::optional<size_type>
{
  auto& search_tree = ref();
  return search_tree.add(value);
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::add(RReference value)
    -> std::optional<size_type>
{
  auto& search_tree = ref();
  return search_tree.add(std::move(value));
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::capacity() const noexcept
    -> size_type
{
  auto& search_tree = ref();
  return search_tree.capacity();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
constexpr auto SearchTree<SearchTreeClass, Key, T, Compare>::capacityMax() noexcept
    -> size_type
{
  return SearchTreeT::capacityMax();
}

/*!
  \details No detailed description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
void SearchTree<SearchTreeClass, Key, T, Compare>::clear() noexcept
{
  auto& search_tree = ref();
  search_tree.clear();
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::contain(ConstKeyT& key) const noexcept
    -> std::optional<size_type>
{
  auto& search_tree = ref();
  return search_tree.contain(key);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::findMinKey() noexcept
    -> std::optional<Pointer>
{
  auto& search_tree = ref();
  return search_tree.findMinKey();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::findMinKey() const noexcept
    -> std::optional<ConstPointer>
{
  auto& search_tree = ref();
  return search_tree.findMinKey();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
constexpr bool SearchTree<SearchTreeClass, Key, T, Compare>::isBounded() noexcept
{
  return SearchTreeT::isBounded();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
constexpr bool SearchTree<SearchTreeClass, Key, T, Compare>::isConcurrent() noexcept
{
  return SearchTreeT::isConcurrent();
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::remove(ConstKeyT& key)
    -> std::optional<size_type>
{
  auto& search_tree = ref();
  return search_tree.remove(key);
}

/*!
  \details No detailed description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
void SearchTree<SearchTreeClass, Key, T, Compare>::setCapacity(const size_type cap) noexcept
{
  auto& search_tree = ref();
  search_tree.setCapacity(cap);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::size() const noexcept -> size_type
{
  auto& search_tree = ref();
  return search_tree.size();
}

/*!
  \details No detailed description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
SearchTree<SearchTreeClass, Key, T, Compare>::SearchTree() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
SearchTree<SearchTreeClass, Key, T, Compare>::SearchTree([[maybe_unused]] const SearchTree& other) noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::operator=([[maybe_unused]] const SearchTree& other) noexcept
    -> SearchTree&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::getKey(ConstReference value) noexcept
    -> ConstKeyT&
{
  if constexpr (std::is_void_v<T>)
    return value;
  else
    return std::get<0>(value);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::ref() noexcept
    -> SearchTreeReference
{
  auto q = static_cast<SearchTreePtr>(this);
  return *q;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename SearchTreeClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
inline
auto SearchTree<SearchTreeClass, Key, T, Compare>::ref() const noexcept
    -> ConstSearchTreeReference
{
  auto q = static_cast<ConstSearchTreePtr>(this);
  return *q;
}

} // namespace zisc

#endif // ZISC_SEARCH_TREE_INL_HPP
