/*!
  \file concepts_test.cpp
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
#include <concepts>
#include <limits>
#include <memory>
#include <type_traits>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/function_reference.hpp"
#include "zisc/ieee_754_binary.hpp"
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
  // std concepts test
  // Core language concepts
  static_assert(std::same_as<zisc::int32b, zisc::int32b>);
  static_assert(!std::same_as<zisc::int32b, zisc::uint32b>);
  static_assert(std::derived_from<::TestDerived, ::TestBase>);
  static_assert(!std::derived_from<int, ::TestBase>);
  static_assert(std::convertible_to<float, zisc::Binary16>);
  static_assert(!std::convertible_to<float, void*>);
  static_assert(std::common_with<int, double>);
  static_assert(!std::common_with<int, void>);
  static_assert(std::integral<int>);
  static_assert(std::integral<bool>);
  static_assert(!std::integral<float>);
  static_assert(std::signed_integral<int>);
  static_assert(!std::signed_integral<unsigned int>);
  static_assert(!std::unsigned_integral<int>);
  static_assert(std::unsigned_integral<unsigned int>);
  static_assert(std::floating_point<double>);
  static_assert(!std::floating_point<int>);
//  static_assert(std::assignable_from<float, int>);
//  static_assert(!std::assignable_from<float, ::TestDerived>);
  static_assert(std::swappable<int>);
  static_assert(!std::swappable<void>);
//  static_assert(std::swappable_with<float, int>);
//  static_assert(!std::swappable_with<float, ::TestDerived>);
  static_assert(std::destructible<::TestDerived>);
  static_assert(!std::destructible<void>);
  static_assert(std::constructible_from<double, unsigned int>);
  static_assert(!std::constructible_from<::TestDerived, unsigned int>);
  static_assert(std::default_initializable<int>);
  static_assert(!std::default_initializable<void>);
  static_assert(std::move_constructible<std::unique_ptr<int>>);
  static_assert(!std::move_constructible<void>);
  static_assert(std::copy_constructible<std::shared_ptr<int>>);
  static_assert(!std::copy_constructible<std::unique_ptr<int>>);
  // Comparison concepts
  static_assert(std::equality_comparable<int>);
  static_assert(!std::equality_comparable<void>);
  static_assert(std::equality_comparable_with<int, float>);
  static_assert(!std::equality_comparable_with<int, void>);
  static_assert(std::totally_ordered<int>);
  static_assert(!std::totally_ordered<::TestDerived>);
  static_assert(std::totally_ordered_with<int, unsigned int>);
  static_assert(!std::totally_ordered_with<int, ::TestDerived>);
//  static_assert(std::three_way_comparable<int>);
//  static_assert(!std::three_way_comparable<void*>);
//  static_assert(std::three_way_comparable_with<int, unsigned int>);
//  static_assert(!std::three_way_comparable<int, void*>);
  // Object concepts
  static_assert(std::movable<std::unique_ptr<int>>);
  static_assert(!std::movable<void>);
  static_assert(std::copyable<std::shared_ptr<int>>);
  static_assert(!std::copyable<std::unique_ptr<int>>);
  static_assert(std::semiregular<int>);
  static_assert(!std::semiregular<void>);
  static_assert(std::regular<int>);
  static_assert(!std::regular<void>);

  // Zisc concepts test
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
}

TEST(ConceptsTest, CallbaleTest)
{
  // std callable concepts
  static_assert(std::invocable<zisc::FunctionReference<int ()>>);
  static_assert(!std::invocable<zisc::FunctionReference<int ()>, int>);
  static_assert(std::invocable<zisc::FunctionReference<int (int)>, int>);
  static_assert(!std::invocable<zisc::FunctionReference<int (int)>, int*>);
  static_assert(std::invocable<zisc::FunctionReference<void (int, double)>, int, float>);
  static_assert(std::regular_invocable<zisc::FunctionReference<int ()>>);
  static_assert(!std::regular_invocable<zisc::FunctionReference<int ()>, int>);
  static_assert(std::regular_invocable<zisc::FunctionReference<int (int)>, int>);
  static_assert(!std::regular_invocable<zisc::FunctionReference<int (int)>, int*>);
  static_assert(std::regular_invocable<zisc::FunctionReference<void (int, double)>, int, float>);

  static_assert(std::predicate<zisc::FunctionReference<bool (int)>, int>);
  static_assert(!std::predicate<zisc::FunctionReference<void (int)>, int>);

  // zisc callable concepts
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
