/*!
  \file utility_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
#include <iostream>
#include <limits>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

TEST(UtilityTest, MapTo01FloatTest)
{
  using zisc::uint16b;
  {
    constexpr uint16b x = 0;
    constexpr float y = zisc::mapTo01<float>(x);
    const float expected = 0.0f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint16b x = std::numeric_limits<uint16b>::max();
    constexpr float y = zisc::mapTo01<float>(x);
    const float expected = 0.99998474f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint16b x = std::numeric_limits<uint16b>::max() >> 1;
    constexpr float y = zisc::mapTo01<float>(x);
    const float expected = 0.49998474f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }

  using zisc::uint32b;
  {
    constexpr uint32b x = 0;
    constexpr float y = zisc::mapTo01<float>(x);
    const float expected = 0.0f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::max();
    constexpr float y = zisc::mapTo01<float>(x);
    const float expected = 1.0f - std::numeric_limits<float>::epsilon();
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::max() >> 1;
    constexpr float y = zisc::mapTo01<float>(x);
    const float expected = 0.5f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
}

TEST(UtilityTest, MapTo01DoubleTest)
{
  using zisc::uint16b;
  {
    constexpr uint16b x = 0;
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 0.0;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint16b x = std::numeric_limits<uint16b>::max();
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 0.9999847412109375;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint16b x = std::numeric_limits<uint16b>::max() >> 1;
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 0.4999847412109375;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }

  using zisc::uint32b;
  {
    constexpr uint32b x = 0;
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 0.0;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::max();
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 1.0 - std::numeric_limits<double>::epsilon();
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::max() >> 1;
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 0.5;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }

  using zisc::uint64b;
  {
    constexpr uint64b x = 0;
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 0.0;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint64b x = std::numeric_limits<uint64b>::max();
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 1.0 - std::numeric_limits<double>::epsilon();
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint64b x = std::numeric_limits<uint64b>::max() >> 1;
    constexpr double y = zisc::mapTo01<double>(x);
    const double expected = 0.5;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
}
