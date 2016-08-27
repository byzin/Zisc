/*!
  \file algorithm_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <iostream>
#include <iterator>
#include <vector>
// Zisc
#include "zisc/algorithm.hpp"

int main()
{
  // Binary search example
  std::cout << "## Binary search example" << std::endl;
  std::vector<int> tree{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
  std::cout << "Array  : [ ";
  for (const int value : tree)
    std::cout << value << " ";
  std::cout << "]" << std::endl;
  zisc::toBinaryTree(tree.begin(), tree.end());
  std::cout << "To tree: [ ";
  for (const int value : tree)
    std::cout << value << " ";
  std::cout << "]" << std::endl;
  const auto position = zisc::searchBinaryTree(tree.begin(), tree.end(), 7);
  std::cout << "The index of 7 is " 
            << std::distance(tree.begin(), position) << std::endl;

  std::cout << std::endl;

  // Hash example
  std::cout << "## Hash example" << std::endl;
  constexpr char hash_key[] = "AlgorithmExample";
  std::cout << "Hash key: \"" << hash_key << "\"" << std::endl;
  // Hash32
  constexpr auto hash32 = zisc::toHash32(hash_key);
  std::cout << "  Hash32: " << hash32 << std::endl;
  // Hash64
  constexpr auto hash64 = zisc::toHash64(hash_key);
  std::cout << "  Hash64: " << hash64 << std::endl;

  return 0;
}
