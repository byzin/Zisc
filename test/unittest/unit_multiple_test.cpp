/*!
  \file unit_multiple_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/fraction.hpp"
#include "zisc/math.hpp"
#include "zisc/unit_multiple.hpp"

TEST(UnitMultipleTest, ConstructorTest)
{
  {
    constexpr zisc::ByteUnit value{};
    ASSERT_EQ(0, value.value().numerator());
  }
  {
    constexpr zisc::ByteUnit value{1};
    ASSERT_EQ(1, value.value().numerator());
    ASSERT_DOUBLE_EQ(1.0, value.value().toFloat<double>());
  }
  {
    constexpr zisc::KibiUnit value1{1};
    {
      constexpr auto value2 = value1.representedAs<zisc::KibiUnit::exponent()>();
      ASSERT_EQ(1, value2.value().numerator());
      ASSERT_EQ(1, value2.value().denominator());
    }
    {
      constexpr auto value2 = value1.representedAs<zisc::ByteUnit::exponent()>();
      ASSERT_EQ(1024, value2.value().numerator());
      ASSERT_EQ(1, value2.value().denominator());
    }
    {
      constexpr auto value2 = value1.representedAs<zisc::MebiUnit::exponent()>();
      ASSERT_EQ(1, value2.value().numerator());
      ASSERT_EQ(1024, value2.value().denominator());
    }
  }
  {
    auto make_value = [](const std::int64_t v)
    {
      zisc::ByteUnit value{};
      value.value().numerator() = v;
      value.setValue(v);
      return value;
    };
    constexpr auto value = make_value(2);
    ASSERT_EQ(2, value.value().numerator());
  }
  {
    auto make_value = [](const std::int64_t v)
    {
      zisc::ByteUnit value{zisc::ByteUnit::FractionType{1}};
      value.setValue(zisc::ByteUnit::FractionType{v});
      return value;
    };
    constexpr auto value = make_value(2);
    ASSERT_EQ(2, value.value().numerator());
  }
  {
    auto make_value = [](const std::int64_t v)
    {
      zisc::ByteUnit value1{10};
      zisc::MebiUnit value2{v};
      zisc::KibiUnit value3{value2};
      value3.setValue(value2);
      return value3;
    };
    constexpr auto value = make_value(2);
    ASSERT_EQ(2 * 1024, value.value().numerator());
  }
  {
    constexpr zisc::ByteUnit byte{};
    constexpr auto base = byte.base();
    ASSERT_EQ(1024, base);
    constexpr auto exponent = byte.exponent();
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
    ASSERT_EQ(2, result.value().numerator());
    ASSERT_EQ(1, result.value().denominator());
  }
  {
    constexpr auto result = value1 + value2;
    ASSERT_EQ(0, result.exponent());
    ASSERT_EQ(512 * 3, result.value().numerator());
    ASSERT_EQ(1, result.value().denominator());
  }
  {
    constexpr auto result = value1 - value1;
    ASSERT_EQ(0, result.value().numerator());
    ASSERT_EQ(1, result.value().denominator());
  }
  {
    constexpr auto result = value1 - value2;
    ASSERT_EQ(0, result.exponent());
    ASSERT_EQ(512, result.value().numerator());
    ASSERT_EQ(1, result.value().denominator());
  }
  {
    constexpr auto result = 2 * value2;
    ASSERT_EQ(1024, result.value().numerator());
    ASSERT_EQ(1, result.value().denominator());
  }
  {
    constexpr auto result = value2 * 2;
    ASSERT_EQ(1024, result.value().numerator());
    ASSERT_EQ(1, result.value().denominator());
  }
  {
    constexpr auto result = value2 / 2;
    ASSERT_EQ(256, result.value().numerator());
    ASSERT_EQ(1, result.value().denominator());
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
    ASSERT_EQ(zisc::power<6>(10), milli.value().numerator());
    ASSERT_EQ(1, milli.value().denominator());
  }
  {
    constexpr Milli milli{1};
    constexpr Kilo kilo{milli};
    ASSERT_EQ(1, kilo.value().numerator());
    ASSERT_EQ(zisc::power<6>(10), kilo.value().denominator());
  }
  {
    constexpr Milli milli{1};
    constexpr Nano nano{milli};
    ASSERT_EQ(zisc::power<6>(10), nano.value().numerator());
    ASSERT_EQ(1, nano.value().denominator());
  }
  {
    constexpr Nano nano{1};
    constexpr Milli milli{nano};
    ASSERT_EQ(1, milli.value().numerator());
    ASSERT_EQ(zisc::power<6>(10), milli.value().denominator());
  }
}
