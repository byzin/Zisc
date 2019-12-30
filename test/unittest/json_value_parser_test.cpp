/*!
  \file json_value_parser_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
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
