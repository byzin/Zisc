/*!
  \file bitset_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <bitset>
#include <cstddef>
#include <memory>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/memory/simple_memory_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"
#include "zisc/thread/bitset.hpp"

TEST(BitsetTest, ConstructionTest)
{
  using Bitset = zisc::Bitset;
  zisc::SimpleMemoryResource mem_resource;
  {
    Bitset bits{&mem_resource};
    ASSERT_EQ(Bitset::defaultSize(), bits.size()) << "Bitset construction failed.";
  }
  {
    constexpr std::size_t n = 300;
    Bitset bits{n, &mem_resource};
    ASSERT_EQ(n, bits.size()) << "Bitset construction failed.";
  }
  {
    constexpr std::size_t n = 300;
    Bitset bits{&mem_resource};
    {
      Bitset bits2{n, &mem_resource};
      bits = std::move(bits2);
    }
    ASSERT_EQ(n, bits.size()) << "Bitset construction failed.";
  }
}

TEST(BitsetTest, BitManipulationTest)
{
  using Bitset = zisc::Bitset;
  zisc::SimpleMemoryResource mem_resource;
  constexpr std::size_t n = 300;
  Bitset bits{n, &mem_resource};

  ASSERT_TRUE(bits.isNone());
  ASSERT_FALSE(bits.isAny());
  ASSERT_FALSE(bits.isAll());
  ASSERT_FALSE(bits[5]);
  ASSERT_FALSE(bits[98]);
  ASSERT_EQ(0, bits.count());

  ASSERT_TRUE(bits.isNone(0, 0));
  ASSERT_FALSE(bits.isAny(0, 0));
  ASSERT_FALSE(bits.isAll(0, 0));

  auto test_result = bits.testAndSet(5, true);
  ASSERT_FALSE(test_result);
  test_result = bits.testAndSet(98, true);
  ASSERT_FALSE(test_result);
  ASSERT_FALSE(bits.isNone());
  ASSERT_TRUE(bits.isAny());
  ASSERT_FALSE(bits.isAll());
  ASSERT_TRUE(bits[5]);
  ASSERT_TRUE(bits[98]);
  ASSERT_EQ(2, bits.count());

  test_result = bits.testAndSet(n - 1, true);
  ASSERT_FALSE(test_result);
  ASSERT_TRUE(bits[n-1]);
  ASSERT_EQ(3, bits.count());

  {
    const std::size_t b = 100;
    const std::size_t e = 200;
    ASSERT_TRUE(bits.isNone(b, e));
    ASSERT_FALSE(bits.isAny(b, e));
    ASSERT_FALSE(bits.isAll(b, e));
    for (std::size_t i = b; i < e; ++i) {
      ASSERT_FALSE(bits[i]);
      test_result = bits.testAndSet(i, true);
      ASSERT_FALSE(test_result);
      ASSERT_TRUE(bits[i]);
      test_result = bits.testAndSet(i, true);
      ASSERT_TRUE(test_result);
    }
    ASSERT_FALSE(bits.isNone(b, e));
    ASSERT_TRUE(bits.isAny(b, e));
    ASSERT_TRUE(bits.isAll(b, e));

    ASSERT_FALSE(bits.isNone(b - 1, e));
    ASSERT_TRUE(bits.isAny(b - 1, e));
    ASSERT_FALSE(bits.isAll(b - 1, e));

    ASSERT_FALSE(bits.isNone(b, e + 1));
    ASSERT_TRUE(bits.isAny(b, e + 1));
    ASSERT_FALSE(bits.isAll(b, e + 1));

    ASSERT_EQ(e - b, bits.count(b, e));
    ASSERT_EQ(e - b + 1, bits.count(b - 2, e));
    ASSERT_EQ(e - b + 3, bits.count());

    ASSERT_TRUE(bits.isNone(b, b));
    ASSERT_FALSE(bits.isAny(b, b));
    ASSERT_FALSE(bits.isAll(b, b));
    ASSERT_TRUE(bits.isNone(e, e));
    ASSERT_FALSE(bits.isAny(e, e));
    ASSERT_FALSE(bits.isAll(e, e));
    ASSERT_FALSE(bits.count(b, b));
    ASSERT_FALSE(bits.count(e, e));

    test_result = bits.testAndSet(b, false);
    ASSERT_TRUE(test_result);
    test_result = bits.testAndSet(b, false);
    ASSERT_FALSE(test_result);

    bits.reset(b, e);
    ASSERT_FALSE(bits.isNone());
    ASSERT_TRUE(bits.isAny());
    ASSERT_FALSE(bits.isAll());
    ASSERT_TRUE(bits.isNone(b, e));
    ASSERT_FALSE(bits.isAny(b, e));
    ASSERT_FALSE(bits.isAll(b, e));

    ASSERT_EQ(0, bits.count(b, e));
    ASSERT_EQ(3, bits.count());

    bits.reset(b, e, true);
    ASSERT_FALSE(bits.isNone(b, e));
    ASSERT_TRUE(bits.isAny(b, e));
    ASSERT_TRUE(bits.isAll(b, e));

    ASSERT_FALSE(bits.isNone(b - 1, e));
    ASSERT_TRUE(bits.isAny(b - 1, e));
    ASSERT_FALSE(bits.isAll(b - 1, e));

    ASSERT_FALSE(bits.isNone(b, e + 1));
    ASSERT_TRUE(bits.isAny(b, e + 1));
    ASSERT_FALSE(bits.isAll(b, e + 1));

    ASSERT_EQ(e - b, bits.count(b, e));
    ASSERT_EQ(e - b + 3, bits.count());
  }

  ASSERT_FALSE(bits.count(0, 0));

  bits.reset();
  ASSERT_TRUE(bits.isNone());
  ASSERT_FALSE(bits.isAny());
  ASSERT_FALSE(bits.isAll());
  ASSERT_EQ(0, bits.count());
}
