/*!
  \file algorithm.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
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

//! Sort a array
template <typename Type, typename Function>
void binaryRadixSort(std::vector<Type>* container, Function bits_function) noexcept;

//! Sort a array of uninteger values
template <typename UnsignedInteger>
void binaryRadixSort(std::vector<UnsignedInteger>* container) noexcept;

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

//! Calculate the 32bit hash of string using FNV-1a
constexpr uint32 toHash32(const char* string) noexcept;

//! Calculate the 64bit hash of string using FNV-1a
constexpr uint64 toHash64(const char* string) noexcept;

} // namespace zisc

#include "algorithm-inl.hpp"

#endif // ZISC_ALGORITHM_HPP
