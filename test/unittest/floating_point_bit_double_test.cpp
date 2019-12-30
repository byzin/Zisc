/*!
  \file floating_point_double_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <bitset>
#include <iomanip>
#include <limits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/floating_point.hpp"
#include "zisc/math.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
// Test
#include "test.hpp"

namespace {

} // namespace 

namespace {

constexpr int start = -512;
constexpr int end = 512;

template <typename Float, int end, int i>
struct BitTest
{
  using FloatBit = zisc::FloatingPointFromBytes<sizeof(Float)>;
  using BitType = typename FloatBit::BitType;

  union F
  {
    Float f_;
    BitType bit_;
    F(const Float f) : f_{f} {}
  };
  static_assert(sizeof(F) == sizeof(BitType), "The size of F is invalid.");

  static constexpr Float zero = zisc::cast<Float>(0.0);
  static constexpr Float x = zisc::cast<Float>(i) / zisc::cast<Float>(end);

  static constexpr Float normal = ::makeNormal(x);
  static_assert((normal == zero) || zisc::isNormal(normal),
                "The normal isn't normal.");

  static constexpr Float subnormal = ::makeSubnormal(x);
  static_assert((subnormal == zero) || zisc::isSubnormal(subnormal),
                "The subnormal isn't subnormal.");

  static void testExponentBit()
  {
    {
      constexpr BitType result = FloatBit::makeExponentBits(normal);
      const F f{normal};
      const BitType expected = f.bit_ & FloatBit::exponentBitMask();
      EXPECT_EQ(expected, result)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal;
    }
    {
      constexpr BitType result = FloatBit::makeExponentBits(subnormal);
      const F f{subnormal};
      const BitType expected = f.bit_ & FloatBit::exponentBitMask();
      EXPECT_EQ(expected, result)
          << "subnormal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << subnormal;
    }
    BitTest<Float, end, i + 1>::testExponentBit();
  }

  static void testSignBit()
  {
    {
      constexpr BitType result = FloatBit::makeSignBit(normal);
      const F f{normal};
      const BitType expected = f.bit_ & FloatBit::signBitMask();
      EXPECT_EQ(expected, result)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal;
    }
    {
      constexpr BitType result = FloatBit::makeSignBit(subnormal);
      const F f{subnormal};
      const BitType expected = f.bit_ & FloatBit::signBitMask();
      EXPECT_EQ(expected, result)
          << "subnormal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << subnormal;
    }
    BitTest<Float, end, i + 1>::testSignBit();
  }

  static void testSignificandBit()
  {
    {
      constexpr BitType result = FloatBit::makeSignificandBits(normal);
      const F f{normal};
      const BitType expected = f.bit_ & FloatBit::significandBitMask();
      EXPECT_EQ(expected, result)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal
          << std::endl
          << "E: " << std::bitset<64>(zisc::cast<zisc::uint64b>(expected)) << std::endl
          << "R: " << std::bitset<64>(zisc::cast<zisc::uint64b>(result)) << std::endl;
    }
    {
      constexpr BitType result = FloatBit::makeSignificandBits(subnormal);
      const F f{subnormal};
      const BitType expected = f.bit_ & FloatBit::significandBitMask();
      EXPECT_EQ(expected, result)
          << "subnormal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << subnormal
          << std::endl
          << "E: " << std::bitset<64>(zisc::cast<zisc::uint64b>(expected)) << std::endl
          << "R: " << std::bitset<64>(zisc::cast<zisc::uint64b>(result)) << std::endl;
    }
    BitTest<Float, end, i + 1>::testSignificandBit();
  }

  static void testBit()
  {
    {
      constexpr BitType result = FloatBit::makeBits(normal);
      const F f{normal};
      const BitType expected = f.bit_;
      EXPECT_EQ(expected, result)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal;
    }
    {
      constexpr BitType result = FloatBit::makeBits(subnormal);
      const F f{subnormal};
      const BitType expected = f.bit_;
      EXPECT_EQ(expected, result)
          << "subnormal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << subnormal;
    }
    BitTest<Float, end, i + 1>::testBit();
  }

  static void testFloatMaking()
  {
    {
      constexpr Float f = FloatBit::makeFloat(FloatBit::makeBits(normal));
      EXPECT_EQ(normal, f)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal;
    }
    {
      constexpr Float f = FloatBit::makeFloat(FloatBit::makeBits(subnormal));
      EXPECT_EQ(subnormal, f)
          << "subnormal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << subnormal;
    }
    BitTest<Float, end, i + 1>::testFloatMaking();
  }
};

template <typename Float, int end>
struct BitTest<Float, end, end>
{
  using FloatBit = zisc::FloatingPointFromBytes<sizeof(Float)>;
  using BitType = typename FloatBit::BitType;

  union F
  {
    Float f_;
    BitType bit_;
    F(const Float f) : f_{f} {}
  };
  static_assert(sizeof(F) == sizeof(BitType), "The size of F is invalid.");

  static void testExponentBit()
  {
    auto test = [](const Float x)
    {
      const BitType result = FloatBit::makeExponentBits(x);
      const F f{x};
      const BitType expected = f.bit_ & FloatBit::exponentBitMask();
      EXPECT_EQ(expected, result)
          << "F: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << x;
    };
    test(zisc::cast<Float>(0.0));
    test(std::numeric_limits<Float>::max());
    test(std::numeric_limits<Float>::lowest());
    test(std::numeric_limits<Float>::min());
    test(-std::numeric_limits<Float>::min());
    test(std::numeric_limits<Float>::infinity());
    test(-std::numeric_limits<Float>::infinity());
    test(std::numeric_limits<Float>::quiet_NaN());
    test(std::numeric_limits<Float>::denorm_min());
  }

  static void testSignBit()
  {
    auto test = [](const Float x)
    {
      const BitType result = FloatBit::makeSignBit(x);
      const F f{x};
      const BitType expected = f.bit_ & FloatBit::signBitMask();
      EXPECT_EQ(expected, result)
          << "F: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << x;
    };
    test(zisc::cast<Float>(0.0));
    test(std::numeric_limits<Float>::max());
    test(std::numeric_limits<Float>::lowest());
    test(std::numeric_limits<Float>::min());
    test(-std::numeric_limits<Float>::min());
    test(std::numeric_limits<Float>::infinity());
    test(-std::numeric_limits<Float>::infinity());
    test(std::numeric_limits<Float>::quiet_NaN());
    test(std::numeric_limits<Float>::denorm_min());
  }

  static void testSignificandBit()
  {
    auto test = [](const Float x)
    {
      const BitType result = FloatBit::makeSignificandBits(x);
      const F f{x};
      const BitType expected = f.bit_ & FloatBit::significandBitMask();
      EXPECT_EQ(expected, result)
          << "F: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << x;
    };
    test(zisc::cast<Float>(0.0));
    test(std::numeric_limits<Float>::max());
    test(std::numeric_limits<Float>::lowest());
    test(std::numeric_limits<Float>::min());
    test(-std::numeric_limits<Float>::min());
    test(std::numeric_limits<Float>::infinity());
    test(-std::numeric_limits<Float>::infinity());
    test(std::numeric_limits<Float>::quiet_NaN());
    test(std::numeric_limits<Float>::denorm_min());
  }

  static void testBit()
  {
    auto test = [](const Float x)
    {
      const BitType result = FloatBit::makeBits(x);
      const F f{x};
      const BitType expected = f.bit_;
      EXPECT_EQ(expected, result)
          << "F: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << x;
    };
    test(zisc::cast<Float>(0.0));
    test(std::numeric_limits<Float>::max());
    test(std::numeric_limits<Float>::lowest());
    test(std::numeric_limits<Float>::min());
    test(-std::numeric_limits<Float>::min());
    test(std::numeric_limits<Float>::infinity());
    test(-std::numeric_limits<Float>::infinity());
    test(std::numeric_limits<Float>::quiet_NaN());
    test(std::numeric_limits<Float>::denorm_min());
  }

  static void testFloatMaking()
  {
    auto test = [](const Float x)
    {
      const Float f = FloatBit::makeFloat(FloatBit::makeBits(x));
      EXPECT_EQ(x, f)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << x;

    };
    test(zisc::cast<Float>(0.0));
    test(zisc::cast<Float>(-0.0));
    test(std::numeric_limits<Float>::max());
    test(std::numeric_limits<Float>::lowest());
    test(std::numeric_limits<Float>::min());
    test(-std::numeric_limits<Float>::min());
    test(std::numeric_limits<Float>::infinity());
    test(-std::numeric_limits<Float>::infinity());
//    test(std::numeric_limits<Float>::quiet_NaN());
    test(std::numeric_limits<Float>::denorm_min());
  }
};

} // namespace 

TEST(FloatingPointBitTest, makeExponentBitsDoubleTest)
{
  ::BitTest<double, ::end, ::start>::testExponentBit();
}

TEST(FloatingPointBitTest, makeSignificandBitsDoubleTest)
{
  ::BitTest<double, ::end, ::start>::testSignificandBit();
}

TEST(FloatingPointBitTest, makeSignBitsDoubleTest)
{
  ::BitTest<double, ::end, ::start>::testSignBit();
}

TEST(FloatingPointBitTest, makeBitsDoubleTest)
{
  ::BitTest<double, ::end, ::start>::testBit();
}

TEST(FloatingPointBitTest, DoubleMakingTest)
{
  ::BitTest<double, ::end, ::start>::testFloatMaking();
}

TEST(FloatingPointBitTest, DoubleMapTest)
{
  using zisc::DoubleFloat;
  {
    constexpr zisc::uint64b x = 0;
    constexpr double result = DoubleFloat::mapTo01(x);
    EXPECT_EQ(0.0, result);
  }
  {
    constexpr zisc::uint64b x = std::numeric_limits<zisc::uint64b>::max() / 2;
    constexpr double result = DoubleFloat::mapTo01(x);
    EXPECT_DOUBLE_EQ(0.5, result);
  }
  {
    constexpr zisc::uint64b x = std::numeric_limits<zisc::uint64b>::max();
    constexpr double result = DoubleFloat::mapTo01(x);
    EXPECT_DOUBLE_EQ(1.0, result);
    EXPECT_GT(1.0, result);
  }
}
