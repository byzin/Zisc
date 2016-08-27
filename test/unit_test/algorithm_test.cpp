/*!
  \file algorithm_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/arithmetic_array.hpp"
#include "zisc/utility.hpp"

TEST(AlgorithmTest, BinaryTreeTest)
{
  using zisc::cast;

  constexpr zisc::uint n = 10;
  zisc::ArithmeticArray<double, n> array;
  for (zisc::uint i = 0; i < n; ++i)
    array[i] = cast<double>(i);
  zisc::toBinaryTree(array.begin(), array.end());

  auto position = zisc::searchBinaryTree(array.begin(), array.end(), 0.5);
  ASSERT_DOUBLE_EQ(0.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 1.5);
  ASSERT_DOUBLE_EQ(1.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 2.5);
  ASSERT_DOUBLE_EQ(2.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 6.5);
  ASSERT_DOUBLE_EQ(6.0, *position);
  
  position = zisc::searchBinaryTree(array.begin(), array.end(), 5.5);
  ASSERT_DOUBLE_EQ(5.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), cast<double>(n));
  ASSERT_DOUBLE_EQ(cast<double>(n - 1), *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 7.0);
  ASSERT_DOUBLE_EQ(7.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 9.0);
  ASSERT_DOUBLE_EQ(9.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 2.0);
  ASSERT_DOUBLE_EQ(2.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 3.0);
  ASSERT_DOUBLE_EQ(3.0, *position);

  position = zisc::searchBinaryTree(array.begin(), array.end(), 6.0);
  ASSERT_DOUBLE_EQ(6.0, *position);
}

TEST(AlgorithmTest, HashTest)
{
  ASSERT_EQ(0x811c9dc5, zisc::toHash32(""))
      << "32bit hash calculation test failed.";
  ASSERT_EQ(0xcbf29ce484222325, zisc::toHash64(""))
      << "64bit hash calculation test failed.";

  ASSERT_EQ(0xe40c292c, zisc::toHash32("a"))
      << "32bit hash calculation test failed.";
  ASSERT_EQ(0xaf63dc4c8601ec8c, zisc::toHash64("a"))
      << "64bit hash calculation test failed.";

  ASSERT_EQ(0xe70c2de5, zisc::toHash32("b"))
      << "32bit hash calculation test failed.";
  ASSERT_EQ(0xaf63df4c8601f1a5, zisc::toHash64("b"))
      << "64bit hash calculation test failed.";

  ASSERT_EQ(0xbf9cf968, zisc::toHash32("foobar"))
      << "32bit hash calculation test failed.";
  ASSERT_EQ(0x85944171f73967e8, zisc::toHash64("foobar"))
      << "64bit hash calculation test failed.";
}
