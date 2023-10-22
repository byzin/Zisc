/*!
  \file boolean_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/boolean.hpp"
#include "zisc/zisc_config.hpp"

TEST(BooleanTest, ValueTest)
{
  using Bool = zisc::Boolean;
  // Default constructor
  {
    constexpr Bool v1;
    static_assert(not v1);
    ASSERT_FALSE(v1) << "The default value of zisc::Boolean isn't false.";

    constexpr Bool v2{v1};
    static_assert(not v2);
    ASSERT_FALSE(v2) << "The copy operation failed.";
  }
  {
    constexpr Bool v1{false};
    static_assert(not v1);
    ASSERT_FALSE(v1) << "The initialization of zisc::Boolean with false failed";

    constexpr Bool v2{true};
    static_assert(v2);
    ASSERT_TRUE(v2) << "The initialization of zisc::Boolean with true failed";
  }

  constexpr auto to_boolean = [](const bool value) noexcept
  {
    Bool v1{};
    v1 = value;
    Bool v2{v1};
    return v2;
  };
  {
    constexpr Bool v1 = to_boolean(true);
    static_assert(v1);
    ASSERT_TRUE(v1) << "The setter of zisc::Boolean failed.";
  }
}
