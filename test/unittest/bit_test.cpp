/*!
  \file bit_test.cpp
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
#include <bit>
#include <concepts>
#include <limits>
#include <type_traits>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

template <zisc::UnsignedInteger Integer, std::floating_point Float>
void testCastBit()
{
  static_assert(sizeof(Integer) == sizeof(Float));
  union Data
  {
    Data(const Integer v) : u_{v} {}
    Data(const Float v) : f_{v} {}
    Integer u_;
    Float f_;
  };

  auto reinterpu = [](const Float f) noexcept
  {
    const Data b{f};
    return b.u_;
  };

  using FLimits = std::numeric_limits<Float>;
  {
    constexpr Float f = FLimits::min();
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = FLimits::lowest();
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = FLimits::max();
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = FLimits::epsilon();
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = FLimits::round_error();
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = FLimits::infinity();
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = -FLimits::infinity();
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    const Float f = FLimits::quiet_NaN();
    const Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
//  {
//    const Float f = FLimits::signaling_NaN();
//    const Integer u = zisc::bit_cast<Integer>(f);
//    const Integer expected = reinterpu(f);
//    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
//  }
  {
    constexpr Float f = FLimits::denorm_min();
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = FLimits::min() - FLimits::denorm_min(); // denorm_max
    static_assert(f != FLimits::min());
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = static_cast<Float>(0.0);
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = static_cast<Float>(1.0);
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }
  {
    constexpr Float f = static_cast<Float>(123456.0);
    constexpr Integer u = zisc::bit_cast<Integer>(f);
    const Integer expected = reinterpu(f);
    ASSERT_EQ(expected, u) << "zisc::bit_cast(" << f << ") failed.";
  }

  auto reinterpf = [](const Integer u) noexcept
  {
    const Data b{u};
    return b.f_;
  };

  {
    constexpr Integer u = static_cast<Integer>(0);
    constexpr Float f = zisc::bit_cast<Float>(u);
    const Float expected = reinterpf(u);
    ASSERT_EQ(expected, f) << "zisc::bit_cast(" << u << ") failed.";
  }
  {
    constexpr Integer u = static_cast<Integer>(1);
    constexpr Float f = zisc::bit_cast<Float>(u);
    const Float expected = reinterpf(u);
    ASSERT_EQ(expected, f) << "zisc::bit_cast(" << u << ") failed.";
  }
  {
    constexpr Integer u = static_cast<Integer>(123456);
    constexpr Float f = zisc::bit_cast<Float>(u);
    const Float expected = reinterpf(u);
    ASSERT_EQ(expected, f) << "zisc::bit_cast(" << u << ") failed.";
  }
}

} // namespace

TEST(BitTest, CastBitFloatTest)
{
  ::testCastBit<zisc::uint32b, float>();
}

TEST(BitTest, CastBitDoubleTest)
{
  ::testCastBit<zisc::uint64b, double>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testCountlZero()
{
  constexpr int offset = 8 * (sizeof(Integer) - sizeof(zisc::uint8b));
  {
    constexpr Integer x = 0b00000000;
    constexpr int result = std::countl_zero(x);
    constexpr int expected = 8 + offset;
    ASSERT_EQ(expected, result) << "std::countl_zero(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::countl_zero(x);
    constexpr int expected = 0 + offset;
    ASSERT_EQ(expected, result) << "std::countl_zero(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00011100;
    constexpr int result = std::countl_zero(x);
    constexpr int expected = 3 + offset;
    ASSERT_EQ(expected, result) << "std::countl_zero(" << x << ") failed.";
  }
}

} // namespace 

TEST(BitTest, CountlZeroUint8Test)
{
  ::testCountlZero<zisc::uint8b>();
}

TEST(BitTest, CountlZeroUint16Test)
{
  ::testCountlZero<zisc::uint16b>();
}

TEST(BitTest, CountlZeroUint32Test)
{
  ::testCountlZero<zisc::uint32b>();
}

TEST(BitTest, CountlZeroUint64Test)
{
  ::testCountlZero<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testCountlOne()
{
  constexpr int offset = 8 * (sizeof(Integer) - sizeof(zisc::uint8b));
  using zisc::cast;
  {
    constexpr Integer x = cast<Integer>(Integer{0b00000000} << offset);
    constexpr int result = std::countl_one(x);
    constexpr int expected = 0;
    ASSERT_EQ(expected, result) << "std::countl_one(" << x << ") failed.";
  }
  {
    constexpr Integer x = cast<Integer>(Integer{0b11111111} << offset);
    constexpr int result = std::countl_one(x);
    constexpr int expected = 8;
    ASSERT_EQ(expected, result) << "std::countl_one(" << x << ") failed.";
  }
  {
    constexpr Integer x = cast<Integer>(Integer{0b11100011} << offset);
    constexpr int result = std::countl_one(x);
    constexpr int expected = 3;
    ASSERT_EQ(expected, result) << "std::countl_one(" << x << ") failed.";
  }
}

} // namespace 

TEST(BitTest, CountlOneUint8Test)
{
  ::testCountlOne<zisc::uint8b>();
}

TEST(BitTest, CountlOneUint16Test)
{
  ::testCountlOne<zisc::uint16b>();
}

TEST(BitTest, CountlOneUint32Test)
{
  ::testCountlOne<zisc::uint32b>();
}

TEST(BitTest, CountlOneUint64Test)
{
  ::testCountlOne<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testCountrZero()
{
  constexpr int offset = 8 * (sizeof(Integer) - sizeof(zisc::uint8b));
  {
    constexpr Integer x = 0b00000000;
    constexpr int result = std::countr_zero(x);
    constexpr int expected = 8 + offset;
    ASSERT_EQ(expected, result) << "std::countr_zero(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::countr_zero(x);
    constexpr int expected = 0;
    ASSERT_EQ(expected, result) << "std::countr_zero(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00011100;
    constexpr int result = std::countr_zero(x);
    constexpr int expected = 2;
    ASSERT_EQ(expected, result) << "std::countr_zero(" << x << ") failed.";
  }
}

} // namespace 

TEST(BitTest, CountrZeroUint8Test)
{
  ::testCountrZero<zisc::uint8b>();
}

TEST(BitTest, CountrZeroUint16Test)
{
  ::testCountrZero<zisc::uint16b>();
}

TEST(BitTest, CountrZeroUint32Test)
{
  ::testCountrZero<zisc::uint32b>();
}

TEST(BitTest, CountrZeroUint64Test)
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
    constexpr int expected = 0;
    ASSERT_EQ(expected, result) << "std::countr_one(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::countr_one(x);
    constexpr int expected = 8;
    ASSERT_EQ(expected, result) << "std::countr_one(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b11100011;
    constexpr int result = std::countr_one(x);
    constexpr int expected = 2;
    ASSERT_EQ(expected, result) << "std::countr_one(" << x << ") failed.";
  }
}

} // namespace 

TEST(BitTest, CountrOneUint8Test)
{
  ::testCountrOne<zisc::uint8b>();
}

TEST(BitTest, CountrOneUint16Test)
{
  ::testCountrOne<zisc::uint16b>();
}

TEST(BitTest, CountrOneUint32Test)
{
  ::testCountrOne<zisc::uint32b>();
}

TEST(BitTest, CountrOneUint64Test)
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
    constexpr int expected = 0;
    ASSERT_EQ(expected, result) << "std::popcount(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b11111111;
    constexpr int result = std::popcount(x);
    constexpr int expected = 8;
    ASSERT_EQ(expected, result) << "std::popcount(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00011101;
    constexpr int result = std::popcount(x);
    constexpr int expected = 4;
    ASSERT_EQ(expected, result) << "std::popcount(" << x << ") failed.";
  }
}

} // namespace

TEST(BitTest, PopcountUint8Test)
{
  ::testPopcount<zisc::uint8b>();
}

TEST(BitTest, PopcountUint16Test)
{
  ::testPopcount<zisc::uint16b>();
}

TEST(BitTest, PopcountUint32Test)
{
  ::testPopcount<zisc::uint32b>();
}

TEST(BitTest, PopcountUint64Test)
{
  ::testPopcount<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testIsPowerOf2()
{
  {
    constexpr Integer x = 0b00000000;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_FALSE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00000001;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_TRUE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00000010;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_TRUE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00000011;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_FALSE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00000100;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_TRUE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00000101;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_FALSE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00000110;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_FALSE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00000111;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_FALSE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00001000;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_TRUE(result) << "std::has_single_bit(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b00001001;
    constexpr bool result = std::has_single_bit(x);
    ASSERT_FALSE(result) << "std::has_single_bit(" << x << ") failed.";
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
    constexpr Integer result = std::bit_width(x);
    ASSERT_FALSE(result) << "std::bit_width(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0001;
    constexpr Integer result = std::bit_width(x);
    ASSERT_EQ(cast<Integer>(1), result) << "std::bit_width(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0010;
    constexpr Integer result = std::bit_width(x);
    ASSERT_EQ(cast<Integer>(2), result) << "std::bit_width(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0011;
    constexpr Integer result = std::bit_width(x);
    ASSERT_EQ(cast<Integer>(2), result) << "std::bit_width(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0100;
    constexpr Integer result = std::bit_width(x);
    ASSERT_EQ(cast<Integer>(3), result) << "std::bit_width(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0101;
    constexpr Integer result = std::bit_width(x);
    ASSERT_EQ(cast<Integer>(3), result) << "std::bit_width(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0110;
    constexpr Integer result = std::bit_width(x);
    ASSERT_EQ(cast<Integer>(3), result) << "std::bit_width(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0111;
    constexpr Integer result = std::bit_width(x);
    ASSERT_EQ(cast<Integer>(3), result) << "std::bit_width(" << x << ") failed.";
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
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b0001;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0001;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b0001;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0010;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b0010;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0011;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0100;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0101;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0110;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0111;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b1000;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b1001;
    constexpr Integer result = std::bit_ceil(x);
    constexpr Integer expected = 0b10000;
    ASSERT_EQ(expected, result) << "std::bit_ceil(" << x << ") failed.";
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
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b0000;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0001;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b0001;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0010;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b0010;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0011;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b0010;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0100;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0101;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0110;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b0111;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b0100;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b1000;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
  }
  {
    constexpr Integer x = 0b1001;
    constexpr Integer result = std::bit_floor(x);
    constexpr Integer expected = 0b1000;
    ASSERT_EQ(expected, result) << "std::bit_floor(" << x << ") failed.";
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

namespace {

template <zisc::UnsignedInteger Integer>
void testBitwiseLeftRotating()
{
  using zisc::cast;
  constexpr std::size_t nbits = 8 * sizeof(Integer);
  constexpr Integer a = 0b0101;
  constexpr Integer b = 0b1010;
  constexpr Integer x = cast<Integer>((b << (nbits - 4)) | a);
  {
    constexpr Integer result = std::rotl(x, 0);
    constexpr Integer expected = x;
    ASSERT_EQ(expected, result) << "std::rotl(" << x << ") failed.";
  }
  {
    constexpr Integer result = std::rotl(x, 1);
    constexpr Integer expected = cast<Integer>((x << 1) | (x >> (nbits - 1)));
    ASSERT_EQ(expected, result) << "std::rotl(" << x << ") failed.";
  }
  {
    constexpr Integer result = std::rotl(x, 4);
    constexpr Integer expected = cast<Integer>((x << 4) | (x >> (nbits - 4)));
    ASSERT_EQ(expected, result) << "std::rotl(" << x << ") failed.";
  }
  {
    constexpr Integer result = std::rotl(x, 9);
    if constexpr (nbits < 9)
    {
      constexpr Integer expected = cast<Integer>((x << 1) | (x >> (nbits - 1)));
      ASSERT_EQ(expected, result) << "std::rotl(" << x << ") failed.";
    }
    else
    {
      constexpr Integer expected = cast<Integer>((x << 9) | (x >> (nbits - 9)));
      ASSERT_EQ(expected, result) << "std::rotl(" << x << ") failed.";
    }
  }
  {
    constexpr Integer result = std::rotl(x, -1);
    constexpr Integer expected = cast<Integer>((x >> 1) | (x << (nbits - 1)));
    ASSERT_EQ(expected, result) << "std::rotl(" << x << ") failed.";
  }
}

}

TEST(BitTest, BitwiseLeftRotating8Test)
{
  ::testBitwiseLeftRotating<zisc::uint8b>();
}

TEST(BitTest, BitwiseLeftRotating16Test)
{
  ::testBitwiseLeftRotating<zisc::uint16b>();
}

TEST(BitTest, BitwiseLeftRotating32Test)
{
  ::testBitwiseLeftRotating<zisc::uint32b>();
}

TEST(BitTest, BitwiseLeftRotating64Test)
{
  ::testBitwiseLeftRotating<zisc::uint64b>();
}

namespace {

template <zisc::UnsignedInteger Integer>
void testBitwiseRightRotating()
{
  using zisc::cast;
  constexpr std::size_t nbits = 8 * sizeof(Integer);
  constexpr Integer a = 0b0101;
  constexpr Integer b = 0b1010;
  constexpr Integer x = cast<Integer>((b << (nbits - 4)) | a);
  {
    constexpr Integer result = std::rotr(x, 0);
    constexpr Integer expected = x;
    ASSERT_EQ(expected, result) << "std::rotr(" << x << ") failed.";
  }
  {
    constexpr Integer result = std::rotr(x, 1);
    constexpr Integer expected = cast<Integer>((x >> 1) | (x << (nbits - 1)));
    ASSERT_EQ(expected, result) << "std::rotr(" << x << ") failed.";
  }
  {
    constexpr Integer result = std::rotr(x, 4);
    constexpr Integer expected = cast<Integer>((x >> 4) | (x << (nbits - 4)));
    ASSERT_EQ(expected, result) << "std::rotr(" << x << ") failed.";
  }
  {
    constexpr Integer result = std::rotr(x, 9);
    if constexpr (nbits < 9)
    {
      constexpr Integer expected = cast<Integer>((x >> 1) | (x << (nbits - 1)));
      ASSERT_EQ(expected, result) << "std::rotr(" << x << ") failed.";
    }
    else
    {
      constexpr Integer expected = cast<Integer>((x >> 9) | (x << (nbits - 9)));
      ASSERT_EQ(expected, result) << "std::rotr(" << x << ") failed.";
    }
  }
  {
    constexpr Integer result = std::rotr(x, -1);
    constexpr Integer expected = cast<Integer>((x << 1) | (x >> (nbits - 1)));
    ASSERT_EQ(expected, result) << "std::rotr(" << x << ") failed.";
  }
}

}

TEST(BitTest, BitwiseRightRotating8Test)
{
  ::testBitwiseRightRotating<zisc::uint8b>();
}

TEST(BitTest, BitwiseRightRotating16Test)
{
  ::testBitwiseRightRotating<zisc::uint16b>();
}

TEST(BitTest, BitwiseRightRotating32Test)
{
  ::testBitwiseRightRotating<zisc::uint32b>();
}

TEST(BitTest, BitwiseRightRotating64Test)
{
  ::testBitwiseRightRotating<zisc::uint64b>();
}
