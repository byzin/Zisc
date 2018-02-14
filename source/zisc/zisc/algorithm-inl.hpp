/*!
  \file algorithm-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALGORITHM_INL_HPP
#define ZISC_ALGORITHM_INL_HPP

#include "algorithm.hpp"
// Standard C++ library
#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>
// Zisc
#include "error.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

namespace stl_implementation {

/*!
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value,
    EnableIfRandomAccessIterator<RandomAccessIterator> = kEnabler) noexcept
{
  auto result = std::lower_bound(begin, end, value);
  result = (*result == value) ? result : result - 1;
  return result;
}

} // namespace stl_implementation

namespace zisc_implementation {

/*!
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value,
    EnableIfRandomAccessIterator<RandomAccessIterator> = kEnabler) noexcept
{
  const std::size_t n = cast<std::size_t>(std::distance(begin, end));
  std::size_t index = 0;
  while (index < n) {
    const auto node = begin + index;
    index = (index << 1) + ((value < *node) ? 1 : 2);
  }
  while (isOdd(index))
    index = index >> 1;
  ZISC_ASSERT(2 <= index, "The index is invalid.");
  index = (index >> 1) - 1;
  return begin + index;
}

/*!
  */
template <typename RandomAccessIterator, typename OutputIterator> inline
void toBinaryTree(
    RandomAccessIterator begin, 
    RandomAccessIterator end,
    const std::size_t index,
    OutputIterator first,
    EnableIfRandomAccessIterator<RandomAccessIterator> = kEnabler) noexcept
{
  const std::size_t n = cast<std::size_t>(std::distance(begin, end));
  if (n == 1) {
    auto& leaf = begin;
    *(first + index) = std::move(*leaf);
  }
  else if (1 < n) {
    std::size_t s = 1;
    for (; s <= n; s = s << 1);
    s = s >> 1;
    // Find the center node
    const auto c = ((s >> 1) - 1) +
                   ((3 * (s >> 1) <= n + 1) ? (s >> 1) : (n + 1 - s));
    ZISC_ASSERT(isInBounds(c, 0ul, n),
                "The center index is out of range. n:", n, " i:", index, " c:", c);
    auto center = begin + c; 
    *(first + index) = std::move(*center);
    // Left child
    {
      const std::size_t left_index = (index << 1) + 1;
      toBinaryTree(begin, center, left_index, first);
    }
    // Right child
    {
      const std::size_t right_index = (index << 1) + 2;
      toBinaryTree(center + 1, end, right_index, first);
    }
  }
}

} // namespace zisc_implementation

/*!
  \details
  No detailed.
  */
template <typename RandomAccessIterator, typename Type> inline
RandomAccessIterator searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value,
    EnableIfRandomAccessIterator<RandomAccessIterator>) noexcept
{
  ZISC_ASSERT(0 < std::distance(begin, end), "The end is in advance of the begin.");
#ifdef ZISC_ALGORITHM_BINARY_TREE
  return zisc_implementation::searchBinaryTree(begin, end, value);
#else // ZISC_ALGORITHM_BINARY_TREE
  return stl_implementation::searchBinaryTree(begin, end, value);
#endif // ZISC_ALGORITHM_BINARY_TREE
}

/*!
  \details
  No detailed.
  */
template <typename RandomAccessIterator> inline
void toBinaryTree(
    RandomAccessIterator begin, 
    RandomAccessIterator end,
    EnableIfRandomAccessIterator<RandomAccessIterator>) noexcept
{
  ZISC_ASSERT(std::is_sorted(begin, end), "The array isn't sorted.");

  const auto size = std::distance(begin, end);
  ZISC_ASSERT(0 < size, "The end is in advance of the begin.");
  if (1 < size) {
#ifdef ZISC_ALGORITHM_BINARY_TREE
    // Create a temp array
    using Type = typename std::iterator_traits<RandomAccessIterator>::value_type;
    std::vector<Type> array;
    array.reserve(cast<std::size_t>(size));
    for (auto iterator = begin; iterator != end; ++iterator)
      array.emplace_back(std::move(*iterator));
    // Transform an array to a binary tree
    zisc_implementation::toBinaryTree(array.begin(), array.end(), 0, begin);
#endif // ZISC_ALGORITHM_BINARY_TREE
  }
}

} // namespace zisc

#endif // ZISC_ALGORITHM_INL_HPP
