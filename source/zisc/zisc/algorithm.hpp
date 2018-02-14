/*!
  \file algorithm.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALGORITHM_HPP
#define ZISC_ALGORITHM_HPP

// Standard C++ library
#include <vector>
// Zisc
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

//! Return the iterator pointing that the element is greatest <= value
template <typename RandomAccessIterator, typename Type>
RandomAccessIterator searchBinaryTree(
    const RandomAccessIterator begin,
    const RandomAccessIterator end,
    const Type& value,
    EnableIfRandomAccessIterator<RandomAccessIterator> = kEnabler) noexcept;

//! Transform a sorted unique array to a binary tree
template <typename RandomAccessIterator>
void toBinaryTree(
    RandomAccessIterator begin,
    RandomAccessIterator end,
    EnableIfRandomAccessIterator<RandomAccessIterator> = kEnabler) noexcept;

} // namespace zisc

#include "algorithm-inl.hpp"

#endif // ZISC_ALGORITHM_HPP
