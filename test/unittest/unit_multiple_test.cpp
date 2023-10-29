/*!
  \file unit_multiple_test.cpp
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
#include <cstdint>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/math/fraction.hpp"
#include "zisc/math/math.hpp"
#include "zisc/math/unit_multiple.hpp"

TEST(UnitMultipleTest, ConstructorTest)
{
  using zisc::cast;
  {
    constexpr zisc::ByteUnit value{};
    ASSERT_EQ(0, value.value().numer());
  }
  {
    constexpr zisc::ByteUnit value{1};
    ASSERT_EQ(1, value.value().numer());
    ASSERT_DOUBLE_EQ(1.0, cast<double>(value.value()));
  }
  {
    constexpr zisc::KibiUnit value1{1};
    {
      constexpr auto value2 = value1.cast<zisc::KibiUnit::exponent()>();
      ASSERT_EQ(1, value2.value().numer());
      ASSERT_EQ(1, value2.value().denom());
    }
    {
      constexpr auto value2 = value1.cast<zisc::ByteUnit::exponent()>();
      ASSERT_EQ(1024, value2.value().numer());
      ASSERT_EQ(1, value2.value().denom());
    }
    {
      constexpr auto value2 = value1.cast<zisc::MebiUnit::exponent()>();
      ASSERT_EQ(1, value2.value().numer());
      ASSERT_EQ(1024, value2.value().denom());
    }
  }
  {
    constexpr zisc::ByteUnit value{2};
    ASSERT_EQ(2, value.value().numer());
  }
  {
    constexpr zisc::MebiUnit v{2};
    constexpr zisc::KibiUnit value{v};
    ASSERT_EQ(2 * zisc::ByteUnit::base(), value.value().numer());
  }
  {
    constexpr auto base = zisc::ByteUnit::base();
    ASSERT_EQ(1024, base);
    constexpr auto exponent = zisc::ByteUnit::exponent();
    ASSERT_EQ(0, exponent);
  }
}

TEST(UnitMultipleTest, OperatorTest)
{
  constexpr zisc::KibiUnit value1{1};
  constexpr zisc::ByteUnit value2{512};
  constexpr zisc::KibiUnit value3{2};

  {
    constexpr auto result = value1 + value1;
    ASSERT_EQ(2, result.value().numer());
    ASSERT_EQ(1, result.value().denom());
  }
  {
    constexpr auto result = value1 + value2;
    ASSERT_EQ(0, result.exponent());
    ASSERT_EQ(512 * 3, result.value().numer());
    ASSERT_EQ(1, result.value().denom());
  }
  {
    constexpr auto result = value1 - value1;
    ASSERT_EQ(0, result.value().numer());
    ASSERT_EQ(1, result.value().denom());
  }
  {
    constexpr auto result = value1 - value2;
    ASSERT_EQ(0, result.exponent());
    ASSERT_EQ(512, result.value().numer());
    ASSERT_EQ(1, result.value().denom());
  }
  {
    constexpr auto result = 2 * value2;
    ASSERT_EQ(1024, result.value().numer());
    ASSERT_EQ(1, result.value().denom());
  }
  {
    constexpr auto result = value2 * 2;
    ASSERT_EQ(1024, result.value().numer());
    ASSERT_EQ(1, result.value().denom());
  }
  {
    constexpr auto result = value2 / 2;
    ASSERT_EQ(256, result.value().numer());
    ASSERT_EQ(1, result.value().denom());
  }
  {
    constexpr bool result1 = value1 == value1;
    ASSERT_TRUE(result1);
    constexpr bool result2 = value1 == value2;
    ASSERT_FALSE(result2);
    constexpr bool result3 = value1 == (2 * value2);
    ASSERT_TRUE(result3);
    constexpr bool result4 = value1 == value3;
    ASSERT_FALSE(result4);
  }
  {
    constexpr bool result1 = value1 != value1;
    ASSERT_FALSE(result1);
    constexpr bool result2 = value1 != value2;
    ASSERT_TRUE(result2);
    constexpr bool result3 = value1 != (2 * value2);
    ASSERT_FALSE(result3);
    constexpr bool result4 = value1 != value3;
    ASSERT_TRUE(result4);
  }
  {
    constexpr bool result1 = value1 < value1;
    ASSERT_FALSE(result1);
    constexpr bool result2 = value1 < value2;
    ASSERT_FALSE(result2);
    constexpr bool result3 = value2 < value1;
    ASSERT_TRUE(result3);
    constexpr bool result4 = (2 * value2) < value1;
    ASSERT_FALSE(result4);
    constexpr bool result5 = value1 < value3;
    ASSERT_TRUE(result5);
  }
  {
    constexpr bool result1 = value1 <= value1;
    ASSERT_TRUE(result1);
    constexpr bool result2 = value1 <= value2;
    ASSERT_FALSE(result2);
    constexpr bool result3 = value2 <= value1;
    ASSERT_TRUE(result3);
    constexpr bool result4 = (2 * value2) <= value1;
    ASSERT_TRUE(result4);
    constexpr bool result5 = value1 <= value3;
    ASSERT_TRUE(result5);
  }
  {
    constexpr bool result1 = value1 > value1;
    ASSERT_FALSE(result1);
    constexpr bool result2 = value1 > value2;
    ASSERT_TRUE(result2);
    constexpr bool result3 = value2 > value1;
    ASSERT_FALSE(result3);
    constexpr bool result4 = (2 * value2) > value1;
    ASSERT_FALSE(result4);
    constexpr bool result5 = value1 > value3;
    ASSERT_FALSE(result5);
  }
}

TEST(UnitMultipleTest, MetricPrefixTest)
{
  using Kilo = zisc::UnitMultiple<10, 3>;
  using Milli = zisc::UnitMultiple<10, -3>;
  using Nano = zisc::UnitMultiple<10, -9>;

  {
    constexpr Kilo kilo{1};
    constexpr Milli milli{kilo};
    ASSERT_EQ(zisc::pow(10, 6), milli.value().numer());
    ASSERT_EQ(1, milli.value().denom());
  }
  {
    constexpr Milli milli{1};
    constexpr Kilo kilo{milli};
    ASSERT_EQ(1, kilo.value().numer());
    ASSERT_EQ(zisc::pow(10, 6), kilo.value().denom());
  }
  {
    constexpr Milli milli{1};
    constexpr Nano nano{milli};
    ASSERT_EQ(zisc::pow(10, 6), nano.value().numer());
    ASSERT_EQ(1, nano.value().denom());
  }
  {
    constexpr Nano nano{1};
    constexpr Milli milli{nano};
    ASSERT_EQ(1, milli.value().numer());
    ASSERT_EQ(zisc::pow(10, 6), milli.value().denom());
  }
}
