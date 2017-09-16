/*!
  \file string_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
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

TEST(StringTest, ConstexprStringTest)
{
  using namespace zisc;

  // Constexpr string test
  constexpr char message1[] = "ConstexprStringTest";
  constexpr uint message1_size = sizeof(message1) / sizeof(message1[0]) - 1;
  constexpr auto test1 = toString(message1);
  ASSERT_EQ(message1_size, test1.size()) << "Constexpr string size test failed.";
  ASSERT_STREQ(message1, test1.toCString()) << "Constexpr string test failed.";
  static_assert(test1 == message1, "Constexpr string test failed.");
  ASSERT_EQ(std::string{message1}, test1.toStdString()) 
      << "Constexpr string test failed.";

  // Concatenation test
  constexpr auto substring1 = toString("Constexpr");
  constexpr auto substring2 = toString("StringTest");
  constexpr auto test2 = substring1 + substring2;
  ASSERT_EQ(message1_size, test2.size()) << "Concatenation test failed.";
  ASSERT_STREQ(message1, test2.toCString()) << "Concatenation test failed.";
  static_assert(test1 == test2, "Concatenation test failed.");

  // Empty string test
  constexpr char empty_string[] = "";
  constexpr uint empty_size = 0;
  constexpr auto test3 = toString(empty_string);
  ASSERT_EQ(empty_size, test3.size()) << "Empty string test failed.";
  ASSERT_STREQ(empty_string, test3.toCString()) << "Empty string test failed.";
}

#define INTEGER_SUCCESS_TEST(value) \
    { \
      EXPECT_TRUE(value_string.isInteger(#value)) \
          << value << " is not integer."; \
      EXPECT_TRUE(value_string.isInteger(std::string{#value})) \
          << value << " is not integer."; \
      EXPECT_TRUE(isIntegerString(#value)) \
          << value << " is not integer."; \
      EXPECT_TRUE(isIntegerString(std::string{#value})) \
          << value << " is not integer."; \
      EXPECT_EQ(value, toInteger<int>(#value)) \
          << "Conversion test failed."; \
      EXPECT_EQ(value, toInteger<int>(std::string{#value})) \
          << "Conversion test failed."; \
    }

#define INTEGER_FAILURE_TEST(value) \
    { \
      EXPECT_FALSE(value_string.isInteger(value)) \
          << value << " is integer."; \
      EXPECT_FALSE(value_string.isInteger(std::string{value})) \
          << value << " is integer."; \
      EXPECT_FALSE(isIntegerString(value)) \
          << value << " is integer."; \
      EXPECT_FALSE(isIntegerString(std::string{value})) \
          << value << " is integer."; \
    }

TEST(StringTest, IntegerStringTest)
{
  using namespace zisc;
  ValueString value_string;
  // Success test
  INTEGER_SUCCESS_TEST(0);
  INTEGER_SUCCESS_TEST(-0);
  INTEGER_SUCCESS_TEST(1);
  INTEGER_SUCCESS_TEST(-1);
  INTEGER_SUCCESS_TEST(123);
  INTEGER_SUCCESS_TEST(-123);
  // Failure test
  INTEGER_FAILURE_TEST("ABC");
  INTEGER_FAILURE_TEST("123ABC");
  INTEGER_FAILURE_TEST(" 0");
  INTEGER_FAILURE_TEST("-1 ");
  INTEGER_FAILURE_TEST("0123");
  INTEGER_FAILURE_TEST("+1");
}

#define FLOAT_SUCCESS_TEST(value) \
    { \
      EXPECT_TRUE(value_string.isFloat(#value)) \
          << value << " is not float."; \
      EXPECT_TRUE(value_string.isFloat(std::string{#value})) \
          << value << " is not float."; \
      EXPECT_TRUE(isFloatString(#value)) \
          << value << " is not float."; \
      EXPECT_TRUE(isFloatString(std::string{#value})) \
          << value << " is not float."; \
      EXPECT_DOUBLE_EQ(value, toFloat<double>(#value)) \
          << "Conversion test failed."; \
      EXPECT_DOUBLE_EQ(value, toFloat<double>(std::string{#value})) \
          << "Conversion test failed."; \
    }

#define FLOAT_FAILURE_TEST(value) \
    { \
      EXPECT_FALSE(value_string.isFloat(value)) \
          << value << " is float."; \
      EXPECT_FALSE(value_string.isFloat(std::string{value})) \
          << value << " is float."; \
      EXPECT_FALSE(isFloatString(value)) \
          << value << " is float."; \
      EXPECT_FALSE(isFloatString(std::string{value})) \
          << value << " is float."; \
    }

TEST(StringTest, FloatStringTest)
{
  using namespace zisc;
  ValueString value_string;
  // Success test
  FLOAT_SUCCESS_TEST(1.234);
  FLOAT_SUCCESS_TEST(-1.234);
  FLOAT_SUCCESS_TEST(1.234e2);
  FLOAT_SUCCESS_TEST(1.234E2);
  FLOAT_SUCCESS_TEST(1.234e+2);
  FLOAT_SUCCESS_TEST(1.234E+2);
  FLOAT_SUCCESS_TEST(12340.0e-2);
  FLOAT_SUCCESS_TEST(12340.0E-2);
  FLOAT_SUCCESS_TEST(1e2);
  FLOAT_SUCCESS_TEST(-1e2);
  // Failure test
  FLOAT_FAILURE_TEST("ABC");
  FLOAT_FAILURE_TEST("1.23ABC");
  FLOAT_FAILURE_TEST("1.");
  FLOAT_FAILURE_TEST(".234");
  FLOAT_FAILURE_TEST("+1.234");
  FLOAT_FAILURE_TEST(" 1.234");
  FLOAT_FAILURE_TEST("-1.234 ");
}

#define BOOLEAN_SUCCESS_TEST(value) \
    { \
      EXPECT_TRUE(value_string.isBoolean(#value)) \
          << value << " is not boolean."; \
      EXPECT_TRUE(value_string.isBoolean(std::string{#value})) \
          << value << " is not boolean."; \
      EXPECT_TRUE(isBooleanString(#value)) \
          << value << " is not boolean."; \
      EXPECT_TRUE(isBooleanString(std::string{#value})) \
          << value << " is not boolean."; \
      EXPECT_EQ(value, toBoolean(#value)) \
          << "Conversion test failed."; \
      EXPECT_EQ(value, toBoolean(std::string{#value})) \
          << "Conversion test failed."; \
    }

#define BOOLEAN_FAILURE_TEST(value) \
    { \
      EXPECT_FALSE(value_string.isBoolean(value)) \
          << value << " is boolean."; \
      EXPECT_FALSE(value_string.isBoolean(std::string{value})) \
          << value << " is boolean."; \
      EXPECT_FALSE(isBooleanString(value)) \
          << value << " is boolean."; \
      EXPECT_FALSE(isBooleanString(std::string{value})) \
          << value << " is boolean."; \
    }

TEST(StringTest, BooleanStringTest)
{
  using namespace zisc;
  ValueString value_string;
  // Success test
  BOOLEAN_SUCCESS_TEST(true);
  BOOLEAN_SUCCESS_TEST(false);
  // Failure test
  BOOLEAN_FAILURE_TEST("True");
  BOOLEAN_FAILURE_TEST("False");
  BOOLEAN_FAILURE_TEST(" true");
  BOOLEAN_FAILURE_TEST("false ");
}

#define STRING_SUCCESS_TEST(test, result) \
    { \
      EXPECT_TRUE(value_string.isCxxString(test)) \
          << test << " is not string."; \
      EXPECT_TRUE(value_string.isCxxString(std::string{test})) \
          << test << " is not string."; \
      EXPECT_TRUE(isCxxStringString(test)) \
          << test << " is not string."; \
      EXPECT_TRUE(isCxxStringString(std::string{test})) \
          << test << " is not string."; \
      EXPECT_STREQ(result, toCxxString(test).c_str()) \
          << "Conversion test failed."; \
      EXPECT_STREQ(result, toCxxString(std::string{test}).c_str()) \
          << "Conversion test failed."; \
    }

#define STRING_FAILURE_TEST(value) \
    { \
      EXPECT_FALSE(value_string.isCxxString(value)) \
          << value << " is string."; \
      EXPECT_FALSE(value_string.isCxxString(std::string{value})) \
          << value << " is string."; \
      EXPECT_FALSE(isCxxStringString(value)) \
          << value << " is string."; \
      EXPECT_FALSE(isCxxStringString(std::string{value})) \
          << value << " is string."; \
    }

TEST(StringTest, StringStringTest)
{
  using namespace zisc;
  ValueString value_string;
  // Success test
  STRING_SUCCESS_TEST(R"("")", R"()");
  STRING_SUCCESS_TEST(R"("test")", R"(test)");
  STRING_SUCCESS_TEST(R"("テスト")", R"(テスト)");
  STRING_SUCCESS_TEST(R"("test\"test\"test")", R"(test\"test\"test)");
  STRING_SUCCESS_TEST(R"("\"\\\/\b\n\f\r\t")", R"(\"\\\/\b\n\f\r\t)");
  // Failure test
  STRING_FAILURE_TEST(R"()");
  STRING_FAILURE_TEST(R"(test)");
  STRING_FAILURE_TEST(R"(t"es"t)");
  STRING_FAILURE_TEST(R"("t"es"t")");
  STRING_FAILURE_TEST(R"("\a")");
  STRING_FAILURE_TEST(R"(123)");
}

TEST(StringTest, TrimTest)
{
  using zisc::trimmed;

  // Test 1
  const std::string test1{""};
  const std::string result1{""};
  EXPECT_STREQ(result1.c_str(), trimmed(test1).c_str());
  // Test 2
  const std::string test2{"test"};
  const std::string result2{"test"};
  EXPECT_STREQ(result2.c_str(), trimmed(test2).c_str());
  // Test 3
  const std::string test3{" test"};
  EXPECT_STREQ(result2.c_str(), trimmed(test3).c_str());
  // Test 4
  const std::string test4{"test "};
  EXPECT_STREQ(result2.c_str(), trimmed(test4).c_str());
  // Test 5
  const std::string test5{"  test   "};
  EXPECT_STREQ(result2.c_str(), trimmed(test5).c_str());
  // Test 6
  const std::string test6{"   t e s t   "};
  const std::string result6{"t e s t"};
  EXPECT_STREQ(result6.c_str(), trimmed(test6).c_str());
}

TEST(StringTest, replaceLineFeedCodeToUnitStyleTest)
{
  const std::string reference{"UnitTest\nUnitTest \nUnitTest\nUnitTest \nUnitTest\n"};
  {
    auto text = reference;
    zisc::replaceLineFeedCodeToUnixStyle(&text);
    ASSERT_EQ(reference, text);
  }
  {
    std::string text{"UnitTest\rUnitTest\r\nUnitTest\rUnitTest\r\nUnitTest\n"};
    zisc::replaceLineFeedCodeToUnixStyle(&text);
    ASSERT_EQ(reference, text);
  }
}
