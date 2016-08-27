/*!
  \file aligned_memory_pool_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <initializer_list>
#include <iostream>
// Zisc
#include "zisc/aligned_memory_pool.hpp"

int main()
{
  // AlignedMemoryPool
  std::cout << "## AlignedMemoryPool example" << std::endl;
  std::cout << "Allocate std::array<int, 10> memory." << std::endl;
  using Array = std::array<int, 10>;
  constexpr auto size = sizeof(Array);
  constexpr auto alignment = std::alignment_of<Array>::value;
  zisc::AlignedMemoryPool<size, alignment> memory_pool;
  std::cout << "Allocated memory size: " << memory_pool.size() << std::endl;
  std::cout << "Used memory: " << memory_pool.usedMemory() << std::endl;
  std::cout << "Initialize array: [ ";
  auto array = memory_pool.allocate<Array>();
  for (Array::size_type i = 0; i < array->size(); ++i)
    array->at(i) = static_cast<int>(i);
  for (const auto value : *array)
    std::cout << value << " ";
  std::cout << "]" << std::endl;
  std::cout << "Used memory: " << memory_pool.usedMemory() << std::endl;
  memory_pool.reset();

  return 0;
}
