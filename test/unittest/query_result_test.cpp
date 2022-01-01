/*!
  \file query_result_test.cpp
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
#include <cstddef>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/data_structure/query_result.hpp"
#include "zisc/data_structure/query_value.hpp"
#include "zisc/zisc_config.hpp"

TEST(QueryResultTest, Int32bValueTest)
{
  using int32b = zisc::int32b;
  using QueryResult = zisc::QueryResult<int32b>;
  static_assert(sizeof(QueryResult) == 8);

  QueryResult result{};
  // Default value test
  ASSERT_FALSE(result.isSuccess()) << "Default result isn't invalid.";
  // Move valid value
  constexpr int32b v1 = 10;
  int32b tmp = v1;
  result = std::move(tmp);
  ASSERT_TRUE(result.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result) << "The result doesn't have correct value.";
  const int32b v2 = result; // Implicit conversion check
  ASSERT_EQ(v1, v2) << "The result doesn't have correct value.";
  // Move result
  QueryResult result2{};
  result2 = std::move(result);
  ASSERT_TRUE(result2.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result2) << "The result doesn't have correct value.";
  // Move constructor
  QueryResult result3{std::move(result2)};
  ASSERT_TRUE(result3.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result3) << "The result doesn't have correct value.";
}

TEST(QueryResultTest, Int8bValueTest)
{
  using int8b = zisc::int8b;
  using QueryResult = zisc::QueryResult<int8b>;
  static_assert(sizeof(QueryResult) == 2);

  QueryResult result{};
  // Default value test
  ASSERT_FALSE(result.isSuccess()) << "Default result isn't invalid.";
  // Move valid value
  constexpr int8b v1 = 10;
  int8b tmp = v1;
  result = std::move(tmp);
  ASSERT_TRUE(result.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result) << "The result doesn't have correct value.";
  const int8b v2 = result; // Implicit conversion check
  ASSERT_EQ(v1, v2) << "The result doesn't have correct value.";
  // Move result
  QueryResult result2{};
  result2 = std::move(result);
  ASSERT_TRUE(result2.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result2) << "The result doesn't have correct value.";
  // Move constructor
  QueryResult result3{std::move(result2)};
  ASSERT_TRUE(result3.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result3) << "The result doesn't have correct value.";
}

TEST(QueryResultTest, FloatValueTest)
{
  using QueryResult = zisc::QueryResult<float>;
  static_assert(sizeof(QueryResult) == 4);

  QueryResult result{};
  // Default value test
  ASSERT_FALSE(result.isSuccess()) << "Default result isn't invalid.";
  // Move valid value
  constexpr float v1 = 10.0f;
  float tmp = v1;
  result = std::move(tmp);
  ASSERT_TRUE(result.isSuccess()) << "The result isn't valid.";
  ASSERT_FLOAT_EQ(v1, result) << "The result doesn't have correct value.";
  const float v2 = result; // Implicit conversion check
  ASSERT_FLOAT_EQ(v1, v2) << "The result doesn't have correct value.";
  // Move result
  QueryResult result2{};
  result2 = std::move(result);
  ASSERT_TRUE(result2.isSuccess()) << "The result isn't valid.";
  ASSERT_FLOAT_EQ(v1, result2) << "The result doesn't have correct value.";
  // Move constructor
  QueryResult result3{std::move(result2)};
  ASSERT_TRUE(result3.isSuccess()) << "The result isn't valid.";
  ASSERT_FLOAT_EQ(v1, result3) << "The result doesn't have correct value.";
}

TEST(QueryResultTest, QueryValueTest)
{
  using uint32b = zisc::uint32b;
  using QueryValue = zisc::QueryValue<uint32b>;
  using QueryResult = zisc::QueryResult<QueryValue>;
  static_assert(sizeof(QueryResult) == 4);

  QueryResult result{};
  // Default value test
  ASSERT_FALSE(result.isSuccess()) << "Default result isn't invalid.";
  // Move valid value
  const QueryValue v1 = 10;
  QueryValue tmp = v1;
  result = std::move(tmp);
  ASSERT_TRUE(result.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result.get()) << "The result doesn't have correct value.";
  const QueryValue v2 = result; // Implicit conversion check
  ASSERT_EQ(v1, v2) << "The result doesn't have correct value.";
  // Move result
  QueryResult result2{};
  result2 = std::move(result);
  ASSERT_TRUE(result2.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result2.get()) << "The result doesn't have correct value.";
  // Move constructor
  QueryResult result3{std::move(result2)};
  ASSERT_TRUE(result3.isSuccess()) << "The result isn't valid.";
  ASSERT_EQ(v1, result3.get()) << "The result doesn't have correct value.";
}
