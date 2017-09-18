/*!
  \file array_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <algorithm>
#include <iostream>
#include <utility>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/array.hpp"
#include "zisc/utility.hpp"

TEST(ArrayTest, ConstructorTest)
{
  using ArrayBody = zisc::Array<double, 2>;
  zisc::Array<ArrayBody, 2> a0;
  {
    constexpr ArrayBody a1{0.0, 1.0};
    constexpr ArrayBody a2{2.0, 3.0};
    constexpr zisc::Array<ArrayBody, 2> a3{a1, a2};
    ASSERT_DOUBLE_EQ(0.0, a3[0][0]);
    a0 = std::move(a3);
  }
  ASSERT_DOUBLE_EQ(0.0, a0[0][0]);
}
