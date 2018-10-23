/*!
  \file floating_point_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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
#include "zisc/floating_point_bit.hpp"
#include "zisc/math.hpp"
#include "zisc/sip_hash_engine.hpp"
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
  using FloatBit = zisc::FloatingPointBit<Float>;
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
      constexpr BitType result = FloatBit::getExponentBits(normal);
      const F f{normal};
      const BitType expected = f.bit_ & FloatBit::exponentBitMask();
      EXPECT_EQ(expected, result)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal;
    }
    {
      constexpr BitType result = FloatBit::getExponentBits(subnormal);
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
      constexpr BitType result = FloatBit::getSignBit(normal);
      const F f{normal};
      const BitType expected = f.bit_ & FloatBit::signBitMask();
      EXPECT_EQ(expected, result)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal;
    }
    {
      constexpr BitType result = FloatBit::getSignBit(subnormal);
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
      constexpr BitType result = FloatBit::getSignificandBits(normal);
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
      constexpr BitType result = FloatBit::getSignificandBits(subnormal);
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
      constexpr BitType result = FloatBit::getBits(normal);
      const F f{normal};
      const BitType expected = f.bit_;
      EXPECT_EQ(expected, result)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal;
    }
    {
      constexpr BitType result = FloatBit::getBits(subnormal);
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
      constexpr BitType s = FloatBit::getSignBit(normal);
      constexpr BitType e = FloatBit::getExponentBits(normal);
      constexpr BitType m = FloatBit::getSignificandBits(normal);
      constexpr Float f = FloatBit::makeFloat(s, e, m);
      EXPECT_EQ(normal, f)
          << "normal: "
          << std::setprecision(std::numeric_limits<Float>::digits10)
          << normal;
    }
    {
      constexpr BitType s = FloatBit::getSignBit(subnormal);
      constexpr BitType e = FloatBit::getExponentBits(subnormal);
      constexpr BitType m = FloatBit::getSignificandBits(subnormal);
      constexpr Float f = FloatBit::makeFloat(s, e, m);
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
  using FloatBit = zisc::FloatingPointBit<Float>;
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
      const BitType result = FloatBit::getExponentBits(x);
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
      const BitType result = FloatBit::getSignBit(x);
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
      const BitType result = FloatBit::getSignificandBits(x);
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
      const BitType result = FloatBit::getBits(x);
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
      const BitType s = FloatBit::getSignBit(x);
      const BitType e = FloatBit::getExponentBits(x);
      const BitType m = FloatBit::getSignificandBits(x);
      const Float f = FloatBit::makeFloat(s, e, m);
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

TEST(FloatingPointBitTest, getExponentBitsFloatTest)
{
  ::BitTest<float, ::end, ::start>::testExponentBit();
}

TEST(FloatingPointBitTest, getExponentBitsDoubleTest)
{
  ::BitTest<double, ::end, ::start>::testExponentBit();
}

TEST(FloatingPointBitTest, getSignificandBitsFloatTest)
{
  ::BitTest<float, ::end, ::start>::testSignificandBit();
}

TEST(FloatingPointBitTest, getSignificandBitsDoubleTest)
{
  ::BitTest<double, ::end, ::start>::testSignificandBit();
}

TEST(FloatingPointBitTest, getSignBitsFloatTest)
{
  ::BitTest<float, ::end, ::start>::testSignBit();
}

TEST(FloatingPointBitTest, getSignBitsDoubleTest)
{
  ::BitTest<double, ::end, ::start>::testSignBit();
}

TEST(FloatingPointBitTest, getBitsFloatTest)
{
  ::BitTest<float, ::end, ::start>::testBit();
}

TEST(FloatingPointBitTest, getBitsDoubleTest)
{
  ::BitTest<double, ::end, ::start>::testBit();
}

TEST(FloatingPointBitTest, FloatMakingTest)
{
  ::BitTest<float, ::end, ::start>::testFloatMaking();
}

TEST(FloatingPointBitTest, DoubleMakingTest)
{
  ::BitTest<double, ::end, ::start>::testFloatMaking();
}

TEST(FloatingPointBitTest, FloatMapTest)
{
  using zisc::FloatBit;
  {
    constexpr zisc::uint32b x = 0;
    constexpr float result = FloatBit::mapTo01Float(x);
    EXPECT_EQ(0.0f, result);
  }
  {
    constexpr zisc::uint32b x = std::numeric_limits<zisc::uint32b>::max() / 2;
    constexpr float result = FloatBit::mapTo01Float(x);
    EXPECT_FLOAT_EQ(0.5f, result);
  }
  {
    constexpr zisc::uint32b x = std::numeric_limits<zisc::uint32b>::max();
    constexpr float result = FloatBit::mapTo01Float(x);
    EXPECT_FLOAT_EQ(1.0f, result);
    EXPECT_GT(1.0f, result);
  }
}

TEST(FloatingPointBitTest, DoubleMapTest)
{
  using zisc::DoubleBit;
  {
    constexpr zisc::uint64b x = 0;
    constexpr double result = DoubleBit::mapTo01Float(x);
    EXPECT_EQ(0.0, result);
  }
  {
    constexpr zisc::uint64b x = std::numeric_limits<zisc::uint64b>::max() / 2;
    constexpr double result = DoubleBit::mapTo01Float(x);
    EXPECT_DOUBLE_EQ(0.5, result);
  }
  {
    constexpr zisc::uint64b x = std::numeric_limits<zisc::uint64b>::max();
    constexpr double result = DoubleBit::mapTo01Float(x);
    EXPECT_DOUBLE_EQ(1.0, result);
    EXPECT_GT(1.0, result);
  }
}
