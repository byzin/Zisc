/*!
  \file concepts_test.cpp
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
#include <limits>
#include <type_traits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace {

class [[maybe_unused]] TestBase
{
 public:
};

class [[maybe_unused]] TestDerived : public TestBase
{
 public:
};

[[maybe_unused]] int testFunc1()
{
  return 1;
}

[[maybe_unused]] int testFunc2(const int v, int* ptr) noexcept
{
  const int result = 2 * v + *ptr;
  *ptr = result;
  return result;
}

} // namespace

TEST(ConceptsTest, TypeTest)
{
  // SameAs
  static_assert(zisc::SameAs<zisc::int32b, zisc::int32b>);
  static_assert(!zisc::SameAs<zisc::int32b, zisc::uint32b>);
  // DerivedFrom
  static_assert(zisc::DerivedFrom<::TestDerived, ::TestBase>);
  static_assert(!zisc::DerivedFrom<int, ::TestBase>);
  // Integer
  static_assert(!zisc::Integer<void>);
  static_assert(zisc::Integer<int>);
  static_assert(zisc::Integer<unsigned int>);
  static_assert(!zisc::Integer<bool>);
  static_assert(zisc::Integer<zisc::int8b>);
  static_assert(zisc::Integer<zisc::int16b>);
  static_assert(zisc::Integer<zisc::int32b>);
  static_assert(zisc::Integer<zisc::int64b>);
  static_assert(zisc::Integer<zisc::uint8b>);
  static_assert(zisc::Integer<zisc::uint16b>);
  static_assert(zisc::Integer<zisc::uint32b>);
  static_assert(zisc::Integer<zisc::uint64b>);
  static_assert(!zisc::Integer<float>);
  static_assert(!zisc::Integer<double>);
  static_assert(!zisc::Integer<long double>);
  static_assert(!zisc::Integer<void*>);
  static_assert(!zisc::Integer<::TestDerived>);
  // SignedInteger
  static_assert(!zisc::SignedInteger<void>);
  static_assert(zisc::SignedInteger<int>);
  static_assert(!zisc::SignedInteger<unsigned int>);
  static_assert(!zisc::SignedInteger<bool>);
  static_assert(zisc::SignedInteger<zisc::int8b>);
  static_assert(zisc::SignedInteger<zisc::int16b>);
  static_assert(zisc::SignedInteger<zisc::int32b>);
  static_assert(zisc::SignedInteger<zisc::int64b>);
  static_assert(!zisc::SignedInteger<zisc::uint8b>);
  static_assert(!zisc::SignedInteger<zisc::uint16b>);
  static_assert(!zisc::SignedInteger<zisc::uint32b>);
  static_assert(!zisc::SignedInteger<zisc::uint64b>);
  static_assert(!zisc::SignedInteger<float>);
  static_assert(!zisc::SignedInteger<double>);
  static_assert(!zisc::SignedInteger<long double>);
  static_assert(!zisc::SignedInteger<void*>);
  static_assert(!zisc::SignedInteger<::TestDerived>);
  // UnsignedInteger
  static_assert(!zisc::UnsignedInteger<void>);
  static_assert(!zisc::UnsignedInteger<int>);
  static_assert(zisc::UnsignedInteger<unsigned int>);
  static_assert(!zisc::UnsignedInteger<bool>);
  static_assert(!zisc::UnsignedInteger<zisc::int8b>);
  static_assert(!zisc::UnsignedInteger<zisc::int16b>);
  static_assert(!zisc::UnsignedInteger<zisc::int32b>);
  static_assert(!zisc::UnsignedInteger<zisc::int64b>);
  static_assert(zisc::UnsignedInteger<zisc::uint8b>);
  static_assert(zisc::UnsignedInteger<zisc::uint16b>);
  static_assert(zisc::UnsignedInteger<zisc::uint32b>);
  static_assert(zisc::UnsignedInteger<zisc::uint64b>);
  static_assert(!zisc::UnsignedInteger<float>);
  static_assert(!zisc::UnsignedInteger<double>);
  static_assert(!zisc::UnsignedInteger<long double>);
  static_assert(!zisc::UnsignedInteger<void*>);
  static_assert(!zisc::UnsignedInteger<::TestDerived>);
  // FloatingPoint
  static_assert(!zisc::FloatingPoint<void>);
  static_assert(!zisc::FloatingPoint<int>);
  static_assert(!zisc::FloatingPoint<unsigned int>);
  static_assert(!zisc::FloatingPoint<bool>);
  static_assert(!zisc::FloatingPoint<zisc::int8b>);
  static_assert(!zisc::FloatingPoint<zisc::int16b>);
  static_assert(!zisc::FloatingPoint<zisc::int32b>);
  static_assert(!zisc::FloatingPoint<zisc::int64b>);
  static_assert(!zisc::FloatingPoint<zisc::uint8b>);
  static_assert(!zisc::FloatingPoint<zisc::uint16b>);
  static_assert(!zisc::FloatingPoint<zisc::uint32b>);
  static_assert(!zisc::FloatingPoint<zisc::uint64b>);
  static_assert(zisc::FloatingPoint<float>);
  static_assert(zisc::FloatingPoint<double>);
  static_assert(zisc::FloatingPoint<long double>);
  static_assert(!zisc::FloatingPoint<void*>);
  static_assert(!zisc::FloatingPoint<::TestDerived>);
}

TEST(ConceptsTest, CallbaleTest)
{
  // Invocable
  {
    using Func = decltype(::testFunc1);
    static_assert(zisc::Invocable<Func>);
    static_assert(!zisc::Invocable<Func, int>);
    static_assert(!zisc::Invocable<Func, float>);
  }
  {
    using Func = decltype(::testFunc2);
    static_assert(!zisc::Invocable<Func>);
    static_assert(!zisc::Invocable<Func, int>);
    static_assert(!zisc::Invocable<Func, float>);
    static_assert(zisc::Invocable<Func, int, int*>);
    static_assert(zisc::Invocable<Func, float, int*>);
    static_assert(!zisc::Invocable<Func, float, float*>);
  }
}
