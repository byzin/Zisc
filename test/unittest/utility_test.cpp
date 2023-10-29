/*!
  \file utility_test.cpp
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
#include <iostream>
#include <limits>
#include <type_traits>
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
    constexpr auto y = zisc::mapTo01<float>(x);
    const float expected = 0.0f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint16b x = std::numeric_limits<uint16b>::max();
    constexpr auto y = zisc::mapTo01<float>(x);
    const float expected = 0.99998474f;
    ASSERT_GT(1.0f, y) << "zisc::mapTo01(" << x << ") failed.";
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint16b x = std::numeric_limits<uint16b>::max() >> 1;
    constexpr auto y = zisc::mapTo01<float>(x);
    const float expected = 0.49998474f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }

  using zisc::uint32b;
  {
    constexpr uint32b x = 0;
    constexpr auto y = zisc::mapTo01<float>(x);
    const float expected = 0.0f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::max();
    constexpr auto y = zisc::mapTo01<float>(x);
    const float expected = 1.0f - std::numeric_limits<float>::epsilon();
    ASSERT_GT(1.0f, y) << "zisc::mapTo01(" << x << ") failed.";
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint32b x = std::numeric_limits<uint32b>::max() >> 1;
    constexpr auto y = zisc::mapTo01<float>(x);
    const float expected = 0.5f;
    ASSERT_FLOAT_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
}

TEST(UtilityTest, MapTo01DoubleTest)
{
  using zisc::uint16b;
  {
    constexpr uint16b x = 0;
    constexpr auto y = zisc::mapTo01<double>(x);
    const double expected = 0.0;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint16b x = std::numeric_limits<uint16b>::max();
    constexpr auto y = zisc::mapTo01<double>(x);
    const double expected = 0.9999847412109375;
    ASSERT_GT(1.0, y) << "zisc::mapTo01(" << x << ") failed.";
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint16b x = std::numeric_limits<uint16b>::max() >> 1;
    constexpr auto y = zisc::mapTo01<double>(x);
    const double expected = 0.4999847412109375;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }

  using zisc::uint64b;
  {
    constexpr uint64b x = 0;
    constexpr auto y = zisc::mapTo01<double>(x);
    const double expected = 0.0;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint64b x = std::numeric_limits<uint64b>::max();
    constexpr auto y = zisc::mapTo01<double>(x);
    const double expected = 1.0 - std::numeric_limits<double>::epsilon();
    ASSERT_GT(1.0, y) << "zisc::mapTo01(" << x << ") failed.";
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
  {
    constexpr uint64b x = std::numeric_limits<uint64b>::max() >> 1;
    constexpr auto y = zisc::mapTo01<double>(x);
    const double expected = 0.5;
    ASSERT_DOUBLE_EQ(expected, y) << "zisc::mapTo01(" << x << ") failed.";
  }
}

namespace {

struct Value
{
  explicit Value(int* ptr) noexcept : value_{ptr} {}

  Value(Value&& other) noexcept : value_{other.value_}
  {
    *other.value_ = -1;
  }

  int* value_ = nullptr;
};

class Object
{
 public:
  Object() noexcept = default;

  Object& operator=(const int value) noexcept
  {
    value_ = value;
    return *this;
  }

  Object& operator=(Value&& value) noexcept
  {
    value_ = *value.value_;
    *value.value_ = 0;
    return *this;
  }

  [[nodiscard]] const int& get() const noexcept
  {
    return value_;
  }

 private:
  int value_ = 0;
};

} /* namespace */

TEST(UtilityTest, ConditionalUpdateTest)
{
  {
    int object = 0;
    ASSERT_FALSE(zisc::updateIfTrue(false, object, 1)) << "updateIfTrue failed.";
    ASSERT_EQ(0, object) << "updateIfTrue failed.";
    ASSERT_TRUE(zisc::updateIfTrue(true, object, 1)) << "updateIfTrue failed.";
    ASSERT_EQ(1, object) << "updateIfTrue failed.";
  }
  {
    static_assert(!std::is_nothrow_constructible_v<::Object, int>);
    static_assert(!std::is_nothrow_constructible_v<::Object, int&&>);
    static_assert(!std::is_nothrow_constructible_v<::Object, ::Value>);
    static_assert(!std::is_nothrow_constructible_v<::Object, ::Value&&>);
    static_assert(std::is_nothrow_assignable_v<::Object, int>);
    static_assert(std::is_nothrow_assignable_v<::Object, const int&>);
    static_assert(std::is_nothrow_assignable_v<::Object, int&&>);
    static_assert(std::is_nothrow_assignable_v<::Object, ::Value>);
    static_assert(!std::is_nothrow_assignable_v<::Object, const ::Value&>);
    static_assert(std::is_nothrow_assignable_v<::Object, ::Value&&>);
    Object object{};
    ASSERT_EQ(0, object.get()) << "The object is wrong.";
    ASSERT_TRUE(zisc::updateIfTrue(true, object, 1)) << "updateIfTrue failed.";
    ASSERT_EQ(1, object.get()) << "updateIfTrue failed.";
    int v = 2;
    {
      Value value{&v};
      ASSERT_FALSE(zisc::updateIfTrue(false, object, std::move(value))) << "updateIfTrue failed.";
    }
    ASSERT_EQ(2, v) << "updateIfTrue failed.";
    {
      Value value{&v};
      ASSERT_TRUE(zisc::updateIfTrue(true, object, std::move(value))) << "updateIfTrue failed.";
    }
    ASSERT_EQ(2, object.get()) << "updateIfTrue failed.";
    ASSERT_EQ(0, v) << "updateIfTrue failed.";
  }
}

TEST(UtilityTest, ConditionalInvokeTest)
{
  {
    int v = 1;
    double result = 0.0;
    const auto func = [](::Value&& v1, const double v2) noexcept -> double
    {
      return static_cast<double>(*v1.value_) + v2;
    };
    {
      ::Value v1{&v};
      ASSERT_FALSE(zisc::invokeIfTrue(false, result, func, std::move(v1), 2.0)) << "invokeIfTrue failed.";
    }
    ASSERT_EQ(1, v) << "invokeIfTrue failed.";
    ASSERT_DOUBLE_EQ(0.0, result) << "invokeIfTrue failed.";
    {
      ::Value v1{&v};
      ASSERT_TRUE(zisc::invokeIfTrue(true, result, func, std::move(v1), 2.0)) << "invokeIfTrue failed.";
    }
    ASSERT_EQ(1, v) << "invokeIfTrue failed.";
    ASSERT_DOUBLE_EQ(3.0, result) << "invokeIfTrue failed.";
  }
  {
    int v = 1;
    double result = 0.0;
    const auto func = [&result](::Value&& v1, const double v2) noexcept -> void
    {
      result = static_cast<double>(*v1.value_) + v2;
    };
    {
      ::Value v1{&v};
      ASSERT_FALSE(zisc::invokeIfTrue(false, func, std::move(v1), 2.0)) << "invokeIfTrue failed.";
    }
    ASSERT_EQ(1, v) << "invokeIfTrue failed.";
    ASSERT_DOUBLE_EQ(0.0, result) << "invokeIfTrue failed.";
    {
      ::Value v1{&v};
      ASSERT_TRUE(zisc::invokeIfTrue(true, func, std::move(v1), 2.0)) << "invokeIfTrue failed.";
    }
    ASSERT_EQ(1, v) << "invokeIfTrue failed.";
    ASSERT_DOUBLE_EQ(3.0, result) << "invokeIfTrue failed.";
  }
}
