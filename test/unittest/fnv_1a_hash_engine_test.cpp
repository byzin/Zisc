/*!
  \file fnv_1a_hash_engine_test.cpp
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
#include <string_view>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/hash/fnv_1a_hash_engine.hpp"

TEST(Fnv1aHashEngineTest, 32BitHashTest)
{
  using Fnv1aHash = zisc::Fnv1aHash32;
  {
    constexpr char seed[] = "";
    constexpr zisc::uint32b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0x811c9dc5, result) << "32bit hash test failed.";
  }
  {
    constexpr char seed[] = "a";
    constexpr zisc::uint32b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0xe40c292c, result) << "32bit hash test failed.";
  }
  {
    constexpr char seed[] = "b";
    constexpr zisc::uint32b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0xe70c2de5, result) << "32bit hash test failed.";
  }
  {
    constexpr char seed[] = "foobar";
    constexpr zisc::uint32b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0xbf9cf968, result) << "32bit hash test failed.";
  }
  {
    constexpr std::string_view seed{"FNV Hash Test"};
    constexpr zisc::uint32b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0xf38b3db9, result) << "32bit hash test failed.";
  }
}

TEST(Fnv1aHashEngineTest, 64BitHashTest)
{
  using Fnv1aHash = zisc::Fnv1aHash64;
  {
    constexpr char seed[] = "";
    constexpr zisc::uint64b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0xcbf29ce484222325, result) << "64bit hash test failed.";
  }
  {
    constexpr char seed[] = "a";
    constexpr zisc::uint64b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0xaf63dc4c8601ec8c, result) << "64bit hash test failed.";
  }
  {
    constexpr char seed[] = "b";
    constexpr zisc::uint64b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0xaf63df4c8601f1a5, result) << "64bit hash test failed.";
  }
  {
    constexpr char seed[] = "foobar";
    constexpr zisc::uint64b result = Fnv1aHash::hash(seed);
    ASSERT_EQ(0x85944171f73967e8, result) << "64bit hash test failed.";
  }
}
