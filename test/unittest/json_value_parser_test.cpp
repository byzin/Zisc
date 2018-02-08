/*!
  \file json_value_parser_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <iomanip>
#include <iostream>
#include <string>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/json_value_parser.hpp"
#include "zisc/string.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

TEST(JsonValueParserTest, PatternTest)
{
  // True
  {
    constexpr auto pattern = zisc::JsonValueParser::truePattern();
    std::cout << std::setw(10) << "True: " << pattern.toCString() << std::endl;
  }
  // False
  {
    constexpr auto pattern = zisc::JsonValueParser::falsePattern();
    std::cout << std::setw(10) << "False: " << pattern.toCString() << std::endl;
  }
  // Boolean
  {
    constexpr auto pattern = zisc::JsonValueParser::booleanPattern();
    std::cout << std::setw(10) << "Boolean: " << pattern.toCString() << std::endl;
  }
  // Integer
  {
    constexpr auto pattern = zisc::JsonValueParser::integerPattern();
    std::cout << std::setw(10) << "Integer: " << pattern.toCString() << std::endl;
  }
  // Float 
  {
    constexpr auto pattern = zisc::JsonValueParser::floatPattern();
    std::cout << std::setw(10) << "Float: " << pattern.toCString() << std::endl;
  }
  // String
  {
    constexpr auto pattern = zisc::JsonValueParser::stringPattern();
    std::cout << std::setw(10) << "String: " << pattern.toCString() << std::endl;
  }
}

TEST(JsonValueParserTest, BooleanTest)
{
  auto success_test = [](const bool expected, const std::string& json_value)
  {
    zisc::JsonValueParser parser{};
    const bool result1 = parser.isType<bool>(json_value);
    EXPECT_TRUE(result1)
        << "'" << json_value << "' is not JSON boolean" << std::endl;
    const bool result2 = parser.toCxxType<bool>(json_value);
    if (expected) {
      EXPECT_TRUE(result2)
          << "JSON boolean conversion failed: '" << json_value << "'";
    }
    else {
      EXPECT_FALSE(result2)
          << "JSON boolean conversion failed: '" << json_value << "'";
    }
    if (result1)
      std::cout << "'" << json_value << "' is " << result2 << std::endl;
  };

  auto failure_test = [](const std::string& json_value)
  {
    zisc::JsonValueParser parser{};
    const bool result1 = parser.isBool(json_value);
    EXPECT_FALSE(result1)
        << "'" << json_value << "' is JSON boolean" << std::endl;
    if (!result1)
      std::cout << "'" << json_value << "' isn't boolean." << std::endl;
  };

  // Success tests
  success_test(true, "true");
  success_test(false, "false");
  // Failure tests
  failure_test("True");
  failure_test("False");
  failure_test(" true");
  failure_test("false ");
  failure_test("truefalse");
  failure_test("falsetrue");
}

TEST(JsonValueParserTest, IntegerTest)
{
  auto success_test = [](const int expected, const std::string& json_value)
  {
    // Static
    const bool result1 = zisc::JsonValueParser::isTypeValue<int>(json_value);
    EXPECT_TRUE(result1)
        << "'" << json_value << "' is not JSON integer." << std::endl;
    const auto result2 = zisc::JsonValueParser::toCxxTypeValue<int>(json_value);
    EXPECT_EQ(expected, result2)
        << "JSON integer conversion failed: '" << json_value << "'";
    // Instance
    zisc::JsonValueParser parser{};
    const bool result3 = parser.isType<long>(json_value);
    EXPECT_TRUE(result3)
        << "'" << json_value << "' is not JSON integer." << std::endl;
    const auto result4 = parser.toCxxType<long>(json_value);
    EXPECT_EQ(expected, result4)
        << "JSON integer conversion failed: '" << json_value << "'";
    if (result3)
      std::cout << "'" << json_value << "' is '" << expected << "'" << std::endl;
  };

  auto failure_test = [](const std::string& json_value)
  {
    // Static
    const bool result1 = zisc::JsonValueParser::isIntegerValue(json_value);
    EXPECT_FALSE(result1)
        << "'" << json_value << "' is JSON integer." << std::endl;
    // Instance
    zisc::JsonValueParser parser{};
    const bool result2 = parser.isInteger(json_value);
    EXPECT_FALSE(result2)
        << "'" << json_value << "' is JSON integer." << std::endl;
    if (!result2)
      std::cout << "'" << json_value << "' isn't integer." << std::endl;
  };

  // Success test
  success_test(0, R"(0)");
  success_test(-0, R"(-0)");
  success_test(1, R"(1)");
  success_test(-1, R"(-1)");
  success_test(123, R"(123)");
  success_test(-123, R"(-123)");
  // Failure test
  failure_test(R"(ABC)");
  failure_test(R"(123ABC)");
  failure_test(R"( 0)");
  failure_test(R"(-1 )");
  failure_test(R"(00)");
  failure_test(R"(0123)");
  failure_test(R"(+1)");
  failure_test(R"(0.0)");
  failure_test(R"(1.0)");
}

TEST(JsonValueParserTest, FloatTest)
{
  auto success_test = [](const double expected, const std::string& json_value)
  {
    // Static
    const bool result1 = zisc::JsonValueParser::isTypeValue<double>(json_value);
    EXPECT_TRUE(result1)
        << "'" << json_value << "' is not JSON float." << std::endl;
    const auto result2 = zisc::JsonValueParser::toCxxTypeValue<double>(json_value);
    EXPECT_DOUBLE_EQ(expected, result2)
        << "JSON float conversion failed: '" << json_value << "'";
    // Instance
    zisc::JsonValueParser parser{};
    const bool result3 = parser.isType<double>(json_value);
    EXPECT_TRUE(result3)
        << "'" << json_value << "' is not JSON float." << std::endl;
    const auto result4 = parser.toCxxType<double>(json_value);
    EXPECT_DOUBLE_EQ(expected, result4)
        << "JSON float conversion failed: '" << json_value << "'";
    if (result3)
      std::cout << "'" << json_value << "' is '" << expected << "'" << std::endl;
  };

  auto failure_test = [](const std::string& json_value)
  {
    // Static
    const bool result1 = zisc::JsonValueParser::isFloatValue(json_value);
    EXPECT_FALSE(result1)
        << "'" << json_value << "' is JSON float." << std::endl;
    // Instance
    zisc::JsonValueParser parser{};
    const bool result2 = parser.isFloat(json_value);
    EXPECT_FALSE(result2)
        << "'" << json_value << "' is JSON float." << std::endl;
    if (!result2)
      std::cout << "'" << json_value << "' isn't float." << std::endl;
  };

  // Integer success
  success_test(0, R"(0)");
  success_test(-0, R"(-0)");
  success_test(1, R"(1)");
  success_test(-1, R"(-1)");
  success_test(123, R"(123)");
  success_test(-123, R"(-123)");
  // Success test
  success_test(0.0, R"(0.0)");
  success_test(0.11, R"(0.11)");
  success_test(0.0100, R"(0.0100)");
  success_test(1.234, R"(1.234)");
  success_test(-1.234, R"(-1.234)");
  success_test(1.234e2, R"(1.234e2)");
  success_test(1.234E2, R"(1.234E2)");
  success_test(1.234e+2, R"(1.234e+2)");
  success_test(1.234E+2, R"(1.234E+2)");
  success_test(12340.0e-2, R"(12340.0e-2)");
  success_test(12340.0E-2, R"(12340.0E-2)");
  success_test(1e2, R"(1e2)");
  success_test(-1e2, R"(-1e2)");
  success_test(0.000e-10, R"(0.000e-10)");
  // Failure test
  failure_test(R"(ABC)");
  failure_test(R"(1.23ABC)");
  failure_test(R"(1.)");
  failure_test(R"(.234)");
  failure_test(R"(+1.234)");
  failure_test(R"( 1.234)");
  failure_test(R"(-1.234 )");
  failure_test(R"(00.0 )");
  failure_test(R"(1.0E)");
  failure_test(R"(0.0e+)");
}

TEST(JsonValueParserTest, StringTest)
{
  auto success_test = [](const std::string& expected, const std::string& json_value)
  {
    // Static
    const bool result1 = zisc::JsonValueParser::isTypeValue<std::string>(json_value);
    EXPECT_TRUE(result1)
        << "'" << json_value << "' is not JSON string." << std::endl;
    const auto result2 = zisc::JsonValueParser::toCxxTypeValue<std::string>(json_value);
    EXPECT_EQ(expected, result2)
        << "JSON string conversion failed: '" << json_value << "'";
    // Instance
    zisc::JsonValueParser parser{};
    const bool result3 = parser.isType<std::string>(json_value);
    EXPECT_TRUE(result3)
        << "'" << json_value << "' is not JSON string." << std::endl;
    const auto result4 = parser.toCxxType<std::string>(json_value);
    EXPECT_EQ(expected, result4)
        << "JSON string conversion failed: '" << json_value << "'";
    if (result3)
      std::cout << "'" << json_value << "' is '" << expected << "'" << std::endl;
  };

  auto failure_test = [](const std::string& json_value)
  {
    // Static
    const bool result1 = zisc::JsonValueParser::isStringValue(json_value);
    EXPECT_FALSE(result1)
        << "'" << json_value << "' is JSON string." << std::endl;
    // Instance
    zisc::JsonValueParser parser{};
    const bool result2 = parser.isString(json_value);
    EXPECT_FALSE(result2)
        << "'" << json_value << "' is JSON string." << std::endl;
    if (!result2)
      std::cout << "'" << json_value << "' isn't string." << std::endl;
  };

  // Success test
  success_test("", R"("")");
  success_test("test", R"("test")");
  success_test("テスト", R"("テスト")");
  success_test("test\"test\"test", R"("test\"test\"test")");
  success_test("\"\"", R"("\"\"")");
  success_test("test: \" \\ \"", R"("test: \" \\ \"")");
  success_test("test: \" \b \"", R"("test: \" \b \"")");
  success_test("test: \" \f \"", R"("test: \" \f \"")");
  success_test("test: \" \n \"", R"("test: \" \n \"")");
  success_test("test: \" \r \"", R"("test: \" \r \"")");
  success_test("test: \" \t \"", R"("test: \" \t \"")");
  success_test("\"\\\b\f\n\r\t", R"("\"\\\b\f\n\r\t")");
  // Failure test
  failure_test(R"()");
  failure_test(R"(test)");
  failure_test(R"(t"es"t)");
  failure_test(R"("t"es"t")");
  failure_test(R"(""")");
  failure_test(R"("\")");
  failure_test(R"("\a")");
  failure_test(R"("\c")");
  failure_test(R"("\d")");
  failure_test(R"(123)");
}

//TEST(StringTest, ConstexprStringTest)
//{
//  using namespace zisc;
//
//  // Constexpr string test
//  constexpr char message1[] = "ConstexprStringTest";
//  constexpr uint message1_size = sizeof(message1) / sizeof(message1[0]) - 1;
//  constexpr auto test1 = toString(message1);
//  ASSERT_EQ(message1_size, test1.size()) << "Constexpr string size test failed.";
//  ASSERT_STREQ(message1, test1.toCString()) << "Constexpr string test failed.";
//  static_assert(test1 == message1, "Constexpr string test failed.");
//  ASSERT_EQ(std::string{message1}, test1.toStdString()) 
//      << "Constexpr string test failed.";
//
//  // Concatenation test
//  constexpr auto substring1 = toString("Constexpr");
//  constexpr auto substring2 = toString("StringTest");
//  constexpr auto test2 = substring1 + substring2;
//  ASSERT_EQ(message1_size, test2.size()) << "Concatenation test failed.";
//  ASSERT_STREQ(message1, test2.toCString()) << "Concatenation test failed.";
//  static_assert(test1 == test2, "Concatenation test failed.");
//
//  // Empty string test
//  constexpr char empty_string[] = "";
//  constexpr uint empty_size = 0;
//  constexpr auto test3 = toString(empty_string);
//  ASSERT_EQ(empty_size, test3.size()) << "Empty string test failed.";
//  ASSERT_STREQ(empty_string, test3.toCString()) << "Empty string test failed.";
//}
//
//#define INTEGER_SUCCESS_TEST(value) \
//    { \
//      EXPECT_TRUE(value_string.isInteger(#value)) \
//          << value << " is not integer."; \
//      EXPECT_TRUE(value_string.isInteger(std::string{#value})) \
//          << value << " is not integer."; \
//      EXPECT_TRUE(isIntegerString(#value)) \
//          << value << " is not integer."; \
//      EXPECT_TRUE(isIntegerString(std::string{#value})) \
//          << value << " is not integer."; \
//      EXPECT_EQ(value, toInteger<int>(#value)) \
//          << "Conversion test failed."; \
//      EXPECT_EQ(value, toInteger<int>(std::string{#value})) \
//          << "Conversion test failed."; \
//    }
//
//#define INTEGER_FAILURE_TEST(value) \
//    { \
//      EXPECT_FALSE(value_string.isInteger(value)) \
//          << value << " is integer."; \
//      EXPECT_FALSE(value_string.isInteger(std::string{value})) \
//          << value << " is integer."; \
//      EXPECT_FALSE(isIntegerString(value)) \
//          << value << " is integer."; \
//      EXPECT_FALSE(isIntegerString(std::string{value})) \
//          << value << " is integer."; \
//    }
//
//TEST(StringTest, IntegerStringTest)
//{
//  using namespace zisc;
//  ValueString value_string;
//  // Success test
//  INTEGER_SUCCESS_TEST(0);
//  INTEGER_SUCCESS_TEST(-0);
//  INTEGER_SUCCESS_TEST(1);
//  INTEGER_SUCCESS_TEST(-1);
//  INTEGER_SUCCESS_TEST(123);
//  INTEGER_SUCCESS_TEST(-123);
//  // Failure test
//  INTEGER_FAILURE_TEST("ABC");
//  INTEGER_FAILURE_TEST("123ABC");
//  INTEGER_FAILURE_TEST(" 0");
//  INTEGER_FAILURE_TEST("-1 ");
//  INTEGER_FAILURE_TEST("0123");
//  INTEGER_FAILURE_TEST("+1");
//}
//
//#define FLOAT_SUCCESS_TEST(value) \
//    { \
//      EXPECT_TRUE(value_string.isFloat(#value)) \
//          << value << " is not float."; \
//      EXPECT_TRUE(value_string.isFloat(std::string{#value})) \
//          << value << " is not float."; \
//      EXPECT_TRUE(isFloatString(#value)) \
//          << value << " is not float."; \
//      EXPECT_TRUE(isFloatString(std::string{#value})) \
//          << value << " is not float."; \
//      EXPECT_DOUBLE_EQ(value, toFloat<double>(#value)) \
//          << "Conversion test failed."; \
//      EXPECT_DOUBLE_EQ(value, toFloat<double>(std::string{#value})) \
//          << "Conversion test failed."; \
//    }
//
//#define FLOAT_FAILURE_TEST(value) \
//    { \
//      EXPECT_FALSE(value_string.isFloat(value)) \
//          << value << " is float."; \
//      EXPECT_FALSE(value_string.isFloat(std::string{value})) \
//          << value << " is float."; \
//      EXPECT_FALSE(isFloatString(value)) \
//          << value << " is float."; \
//      EXPECT_FALSE(isFloatString(std::string{value})) \
//          << value << " is float."; \
//    }
//
//TEST(StringTest, FloatStringTest)
//{
//  using namespace zisc;
//  ValueString value_string;
//  // Success test
//  FLOAT_SUCCESS_TEST(1.234);
//  FLOAT_SUCCESS_TEST(-1.234);
//  FLOAT_SUCCESS_TEST(1.234e2);
//  FLOAT_SUCCESS_TEST(1.234E2);
//  FLOAT_SUCCESS_TEST(1.234e+2);
//  FLOAT_SUCCESS_TEST(1.234E+2);
//  FLOAT_SUCCESS_TEST(12340.0e-2);
//  FLOAT_SUCCESS_TEST(12340.0E-2);
//  FLOAT_SUCCESS_TEST(1e2);
//  FLOAT_SUCCESS_TEST(-1e2);
//  // Failure test
//  FLOAT_FAILURE_TEST("ABC");
//  FLOAT_FAILURE_TEST("1.23ABC");
//  FLOAT_FAILURE_TEST("1.");
//  FLOAT_FAILURE_TEST(".234");
//  FLOAT_FAILURE_TEST("+1.234");
//  FLOAT_FAILURE_TEST(" 1.234");
//  FLOAT_FAILURE_TEST("-1.234 ");
//}
//
//#define BOOLEAN_SUCCESS_TEST(value) \
//    { \
//      EXPECT_TRUE(value_string.isBoolean(#value)) \
//          << value << " is not boolean."; \
//      EXPECT_TRUE(value_string.isBoolean(std::string{#value})) \
//          << value << " is not boolean."; \
//      EXPECT_TRUE(isBooleanString(#value)) \
//          << value << " is not boolean."; \
//      EXPECT_TRUE(isBooleanString(std::string{#value})) \
//          << value << " is not boolean."; \
//      EXPECT_EQ(value, toBoolean(#value)) \
//          << "Conversion test failed."; \
//      EXPECT_EQ(value, toBoolean(std::string{#value})) \
//          << "Conversion test failed."; \
//    }
//
//#define BOOLEAN_FAILURE_TEST(value) \
//    { \
//      EXPECT_FALSE(value_string.isBoolean(value)) \
//          << value << " is boolean."; \
//      EXPECT_FALSE(value_string.isBoolean(std::string{value})) \
//          << value << " is boolean."; \
//      EXPECT_FALSE(isBooleanString(value)) \
//          << value << " is boolean."; \
//      EXPECT_FALSE(isBooleanString(std::string{value})) \
//          << value << " is boolean."; \
//    }
//
//TEST(StringTest, BooleanStringTest)
//{
//  using namespace zisc;
//  ValueString value_string;
//  // Success test
//  BOOLEAN_SUCCESS_TEST(true);
//  BOOLEAN_SUCCESS_TEST(false);
//  // Failure test
//  BOOLEAN_FAILURE_TEST("True");
//  BOOLEAN_FAILURE_TEST("False");
//  BOOLEAN_FAILURE_TEST(" true");
//  BOOLEAN_FAILURE_TEST("false ");
//}
//
//#define STRING_SUCCESS_TEST(test, result) \
//    { \
//      EXPECT_TRUE(value_string.isCxxString(test)) \
//          << test << " is not string."; \
//      EXPECT_TRUE(value_string.isCxxString(std::string{test})) \
//          << test << " is not string."; \
//      EXPECT_TRUE(isCxxStringString(test)) \
//          << test << " is not string."; \
//      EXPECT_TRUE(isCxxStringString(std::string{test})) \
//          << test << " is not string."; \
//      EXPECT_STREQ(result, toCxxString(test).c_str()) \
//          << "Conversion test failed."; \
//      EXPECT_STREQ(result, toCxxString(std::string{test}).c_str()) \
//          << "Conversion test failed."; \
//    }
//
//#define STRING_FAILURE_TEST(value) \
//    { \
//      EXPECT_FALSE(value_string.isCxxString(value)) \
//          << value << " is string."; \
//      EXPECT_FALSE(value_string.isCxxString(std::string{value})) \
//          << value << " is string."; \
//      EXPECT_FALSE(isCxxStringString(value)) \
//          << value << " is string."; \
//      EXPECT_FALSE(isCxxStringString(std::string{value})) \
//          << value << " is string."; \
//    }
//
//TEST(StringTest, StringStringTest)
//{
//  using namespace zisc;
//  ValueString value_string;
//  // Success test
//  STRING_SUCCESS_TEST(R"("")", R"()");
//  STRING_SUCCESS_TEST(R"("test")", R"(test)");
//  STRING_SUCCESS_TEST(R"("テスト")", R"(テスト)");
//  STRING_SUCCESS_TEST(R"("test\"test\"test")", R"(test\"test\"test)");
//  STRING_SUCCESS_TEST(R"("\"\\\/\b\n\f\r\t")", R"(\"\\\/\b\n\f\r\t)");
//  // Failure test
//  STRING_FAILURE_TEST(R"()");
//  STRING_FAILURE_TEST(R"(test)");
//  STRING_FAILURE_TEST(R"(t"es"t)");
//  STRING_FAILURE_TEST(R"("t"es"t")");
//  STRING_FAILURE_TEST(R"("\a")");
//  STRING_FAILURE_TEST(R"(123)");
//}
//
//TEST(StringTest, TrimTest)
//{
//  using zisc::trimmed;
//
//  // Test 1
//  const std::string test1{""};
//  const std::string result1{""};
//  EXPECT_STREQ(result1.c_str(), trimmed(test1).c_str());
//  // Test 2
//  const std::string test2{"test"};
//  const std::string result2{"test"};
//  EXPECT_STREQ(result2.c_str(), trimmed(test2).c_str());
//  // Test 3
//  const std::string test3{" test"};
//  EXPECT_STREQ(result2.c_str(), trimmed(test3).c_str());
//  // Test 4
//  const std::string test4{"test "};
//  EXPECT_STREQ(result2.c_str(), trimmed(test4).c_str());
//  // Test 5
//  const std::string test5{"  test   "};
//  EXPECT_STREQ(result2.c_str(), trimmed(test5).c_str());
//  // Test 6
//  const std::string test6{"   t e s t   "};
//  const std::string result6{"t e s t"};
//  EXPECT_STREQ(result6.c_str(), trimmed(test6).c_str());
//}
//
//TEST(StringTest, replaceLineFeedCodeToUnitStyleTest)
//{
//  const std::string reference{"UnitTest\nUnitTest \nUnitTest\nUnitTest \nUnitTest\n"};
//  {
//    auto text = reference;
//    zisc::replaceLineFeedCodeToUnixStyle(&text);
//    ASSERT_EQ(reference, text);
//  }
//  {
//    std::string text{"UnitTest\rUnitTest\r\nUnitTest\rUnitTest\r\nUnitTest\n"};
//    zisc::replaceLineFeedCodeToUnixStyle(&text);
//    ASSERT_EQ(reference, text);
//  }
//}
