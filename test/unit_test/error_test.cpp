/*!
  \file error_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/error.hpp"

#ifdef ZISC_ASSERTION

TEST(ErrorDeathTest, AssertTest)
{
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  ASSERT_DEATH(ZISC_ASSERT(false, "AssertTest1"), "");
  ASSERT_DEATH(ZISC_ASSERT(false, "Assert", "Test", 2), "");
}

#endif // ZISC_ASSERTION
