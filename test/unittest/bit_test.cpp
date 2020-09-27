/*!
  \file bit_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <bit>
#include <limits>
#include <type_traits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

template <zisc::UnsignedInteger Integer>
void testCountlZero()
{
  {
    constexpr Integer x = 0b00000000;
    constexpr int result = std::countl_zero(x);
    static_assert(0 <= result);
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::countl_zero(x);
    static_assert(0 <= result);
  }
  {
    constexpr Integer x = 0b00011100;
    constexpr int result = std::countl_zero(x);
    static_assert(0 <= result);
  }
}

} // namespace 

TEST(Cpp20Test, CountlZeroUint8Test)
{
  ::testCountlZero<zisc::uint8b>();
}

TEST(Cpp20Test, CountlZeroUint16Test)
{
  ::testCountlZero<zisc::uint16b>();
}

TEST(Cpp20Test, CountlZeroUint32Test)
{
  ::testCountlZero<zisc::uint32b>();
}

TEST(Cpp20Test, CountlZeroUint64Test)
{
  ::testCountlZero<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testCountlOne()
{
  {
    constexpr Integer x = 0b00000000;
    constexpr int result = std::countl_one(x);
    static_assert(0 <= result);
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::countl_one(x);
    static_assert(0 <= result);
  }
  {
    constexpr Integer x = 0b11100011;
    constexpr int result = std::countl_one(x);
    static_assert(0 <= result);
  }
}

} // namespace 

TEST(Cpp20Test, CountlOneUint8Test)
{
  ::testCountlOne<zisc::uint8b>();
}

TEST(Cpp20Test, CountlOneUint16Test)
{
  ::testCountlOne<zisc::uint16b>();
}

TEST(Cpp20Test, CountlOneUint32Test)
{
  ::testCountlOne<zisc::uint32b>();
}

TEST(Cpp20Test, CountlOneUint64Test)
{
  ::testCountlOne<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testCountrZero()
{
  {
    constexpr Integer x = 0b00000000;
    constexpr int result = std::countr_zero(x);
    static_assert(0 <= result);
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::countr_zero(x);
    static_assert(0 <= result);
  }
  {
    constexpr Integer x = 0b00011100;
    constexpr int result = std::countr_zero(x);
    static_assert(0 <= result);
  }
}

} // namespace 

TEST(Cpp20Test, CountrZeroUint8Test)
{
  ::testCountrZero<zisc::uint8b>();
}

TEST(Cpp20Test, CountrZeroUint16Test)
{
  ::testCountrZero<zisc::uint16b>();
}

TEST(Cpp20Test, CountrZeroUint32Test)
{
  ::testCountrZero<zisc::uint32b>();
}

TEST(Cpp20Test, CountrZeroUint64Test)
{
  ::testCountrZero<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testCountrOne()
{
  {
    constexpr Integer x = 0b00000000;
    constexpr int result = std::countr_one(x);
    static_assert(0 <= result);
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::countr_one(x);
    static_assert(0 <= result);
  }
  {
    constexpr Integer x = 0b11100011;
    constexpr int result = std::countr_one(x);
    static_assert(0 <= result);
  }
}

} // namespace 

TEST(Cpp20Test, CountrOneUint8Test)
{
  ::testCountrOne<zisc::uint8b>();
}

TEST(Cpp20Test, CountrOneUint16Test)
{
  ::testCountrOne<zisc::uint16b>();
}

TEST(Cpp20Test, CountrOneUint32Test)
{
  ::testCountrOne<zisc::uint32b>();
}

TEST(Cpp20Test, CountrOneUint64Test)
{
  ::testCountrOne<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testPopcount()
{
  {
    constexpr Integer x = 0b00000000;
    constexpr int result = std::popcount(x);
    static_assert(result == 0);
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::popcount(x);
    static_assert(result == 8);
  }
  {
    constexpr Integer x = 0b00011101;
    constexpr int result = std::popcount(x);
    static_assert(result == 4);
  }
}

} // namespace

TEST(Cpp20Test, PopcountUint8Test)
{
  ::testPopcount<zisc::uint8b>();
}

TEST(Cpp20Test, PopcountUint16Test)
{
  ::testPopcount<zisc::uint16b>();
}

TEST(Cpp20Test, PopcountUint32Test)
{
  ::testPopcount<zisc::uint32b>();
}

TEST(Cpp20Test, PopcountUint64Test)
{
  ::testPopcount<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testIsPowerOf2()
{
  {
    constexpr Integer x = 0b00000000;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_FALSE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00000001;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_TRUE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00000010;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_TRUE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00000011;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_FALSE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00000100;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_TRUE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00000101;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_FALSE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00000110;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_FALSE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00000111;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_FALSE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00001000;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_TRUE(result) << "zisc::has_single_bit(" << x << ")";
  }
  {
    constexpr Integer x = 0b00001001;
    constexpr bool result = zisc::has_single_bit(x);
    ASSERT_FALSE(result) << "zisc::has_single_bit(" << x << ")";
  }
}

} // namespace

TEST(BitTest, IsPowerOf2Uint8Test)
{
  ::testIsPowerOf2<zisc::uint8b>();
}

TEST(BitTest, IsPowerOf2Uint16Test)
{
  ::testIsPowerOf2<zisc::uint16b>();
}

TEST(BitTest, IsPowerOf2Uint32Test)
{
  ::testIsPowerOf2<zisc::uint32b>();
}

TEST(BitTest, IsPowerOf2Uint64Test)
{
  ::testIsPowerOf2<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testBitWidth()
{
  using zisc::cast;
  {
    constexpr Integer x = 0b0000;
    constexpr Integer result = zisc::bit_width(x);
    ASSERT_FALSE(result) << "zisc::bit_width(" << x << ")";
  }
  {
    constexpr Integer x = 0b0001;
    constexpr Integer result = zisc::bit_width(x);
    ASSERT_EQ(cast<Integer>(1), result) << "zisc::bit_width(" << x << ")";
  }
  {
    constexpr Integer x = 0b0010;
    constexpr Integer result = zisc::bit_width(x);
    ASSERT_EQ(cast<Integer>(2), result) << "zisc::bit_width(" << x << ")";
  }
  {
    constexpr Integer x = 0b0011;
    constexpr Integer result = zisc::bit_width(x);
    ASSERT_EQ(cast<Integer>(2), result) << "zisc::bit_width(" << x << ")";
  }
  {
    constexpr Integer x = 0b0100;
    constexpr Integer result = zisc::bit_width(x);
    ASSERT_EQ(cast<Integer>(3), result) << "zisc::bit_width(" << x << ")";
  }
  {
    constexpr Integer x = 0b0101;
    constexpr Integer result = zisc::bit_width(x);
    ASSERT_EQ(cast<Integer>(3), result) << "zisc::bit_width(" << x << ")";
  }
  {
    constexpr Integer x = 0b0110;
    constexpr Integer result = zisc::bit_width(x);
    ASSERT_EQ(cast<Integer>(3), result) << "zisc::bit_width(" << x << ")";
  }
  {
    constexpr Integer x = 0b0111;
    constexpr Integer result = zisc::bit_width(x);
    ASSERT_EQ(cast<Integer>(3), result) << "zisc::bit_width(" << x << ")";
  }
}

} // namespace

TEST(BitTest, BitWidthUint8Test)
{
  ::testBitWidth<zisc::uint8b>();
}

TEST(BitTest, BitWidthUint16Test)
{
  ::testBitWidth<zisc::uint16b>();
}

TEST(BitTest, BitWidthUint32Test)
{
  ::testBitWidth<zisc::uint32b>();
}

TEST(BitTest, BitWidthUint64Test)
{
  ::testBitWidth<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testBitCeil()
{
  using zisc::cast;
  {
    constexpr Integer x = 0b0000;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b0001;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b0001;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b0001;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b0010;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b0010;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b0011;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b0100;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b0101;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b0110;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b0111;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b1000;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
  {
    constexpr Integer x = 0b1001;
    constexpr Integer result = zisc::bit_ceil(x);
    constexpr Integer expected = 0b10000;
    ASSERT_EQ(expected, result) << "zisc::bit_ceil(" << x << ")";
  }
}

} // namespace

TEST(BitTest, BitCeilUint8Test)
{
  ::testBitCeil<zisc::uint8b>();
}

TEST(BitTest, BitCeilUint16Test)
{
  ::testBitCeil<zisc::uint16b>();
}

TEST(BitTest, BitCeilUint32Test)
{
  ::testBitCeil<zisc::uint32b>();
}

TEST(BitTest, BitCeilUint64Test)
{
  ::testBitCeil<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testBitFloor()
{
  using zisc::cast;
  {
    constexpr Integer x = 0b0000;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b0000;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b0001;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b0001;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b0010;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b0010;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b0011;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b0010;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b0100;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b0101;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b0110;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b0111;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b1000;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
  {
    constexpr Integer x = 0b1001;
    constexpr Integer result = zisc::bit_floor(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "zisc::bit_floor(" << x << ")";
  }
}

} // namespace

TEST(BitTest, BitFloorUint8Test)
{
  ::testBitFloor<zisc::uint8b>();
}

TEST(BitTest, BitFloorUint16Test)
{
  ::testBitFloor<zisc::uint16b>();
}

TEST(BitTest, BitFloorUint32Test)
{
  ::testBitFloor<zisc::uint32b>();
}

TEST(BitTest, BitFloorUint64Test)
{
  ::testBitFloor<zisc::uint64b>();
}
