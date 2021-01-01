/*!
  \file string_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <string_view>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/string/constant_string.hpp"

TEST(ConstantStringTest, ConstructionTest)
{
  {
    auto make_string = []()
    {
      zisc::CString<1> message{};
      return message;
    };
    constexpr auto message = make_string();
    constexpr auto size = message.size();
    EXPECT_EQ(0, size);
    EXPECT_TRUE(message == "");
    EXPECT_FALSE(message == "stringtest");
  }
  {
    auto make_string = []()
    {
      zisc::CString<4> message{};
      message[0] = 'a';
      message[1] = 'b';
      message[2] = 'c';
      return message;
    };
    constexpr auto message = make_string();
    constexpr auto size = message.size();
    EXPECT_EQ(3, size);
    EXPECT_TRUE(message == "abc");
    EXPECT_FALSE(message == "stringtest");
  }
  {
    auto make_string = []()
    {
      auto message = zisc::toString("test");
      return message;
    };
    constexpr auto message = make_string();
    EXPECT_TRUE(message == "test");
    auto data = message.data().data();
    EXPECT_STREQ("test", data);
  }
}
