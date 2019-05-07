/*!
  \file string_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <string>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/string.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

TEST(StringTest, ConstructionTest)
{
  {
    auto make_string = []()
    {
      zisc::String<1> message{};
      return message;
    };
    constexpr auto message = make_string();
    constexpr auto size = message.size();
    EXPECT_EQ(0, size);
    EXPECT_TRUE(message == "");
  }
  {
    auto make_string = []()
    {
      zisc::String<4> message{};
      message[0] = 'a';
      message[1] = 'b';
      message[2] = 'c';
      return message;
    };
    constexpr auto message = make_string();
    constexpr auto size = message.size();
    EXPECT_EQ(3, size);
    EXPECT_TRUE(message == "abc");
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
