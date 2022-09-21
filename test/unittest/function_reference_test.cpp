/*!
  \file function_reference_test.cpp
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
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/function_reference.hpp"
#include "zisc/non_copyable.hpp"

namespace {

constexpr int add(const int a, const int b)
{
  return a + b;
}

constexpr void add2(const int a, const int b, int* result) noexcept
{
  *result = add(a, b);
}

constexpr int addL(const long a, const long b)
{
  return static_cast<int>(a + b);
}

constexpr void addL2(const long a, const long b, int* result) noexcept
{
  *result = addL(a, b);
}

struct Adder : private zisc::NonCopyable<Adder>
{
  Adder(const int c) : c_{c} {}

  constexpr int operator()(const int a, const int b) const noexcept
  {
    return add(a, b) + c_;
  }

  int c_;
};

struct Adder2 : private zisc::NonCopyable<Adder>
{
  Adder2(const int c) : c_{c} {}

  constexpr void operator()(const int a, const int b, int* result) const noexcept
  {
    add2(a, b, result);
    *result += c_;
  }

  int c_;
};

} // namespace 

TEST(FunctionReferenceTest, InvocationTest)
{
  // Non void return case
  {
    using FuncRef = zisc::FunctionReference<int (int, int)>;
    static_assert(FuncRef::kNumOfArgs == 2);

    constexpr int a = 1;
    constexpr int b = 2;

    // Function pointer
    {
      const FuncRef ref{::add};
      ASSERT_EQ(::add(a, b), ref(a, b)) << "FunctionRef invocation failed.";
    }
    {
      FuncRef ref{std::addressof(::add)};
      ASSERT_EQ(::add(a, b), ref(a, b)) << "FunctionRef invocation failed.";

      const FuncRef ref2{ref};
      ASSERT_EQ(::add(a, b), ref2(a, b)) << "FunctionRef invocation failed.";
    }
    // Type conversion
    {
      const FuncRef ref{std::addressof(::addL)};
      ASSERT_EQ(::addL(a, b), ref(a, b)) << "FunctionRef invocation failed.";
    }
    {
      using FuncRef2 = zisc::FunctionReference<int (long, long)>;
      FuncRef2 ref{std::addressof(::addL)};
      ASSERT_EQ(::addL(a, b), ref(a, b)) << "FunctionRef invocation failed.";

      const FuncRef ref2{ref};
      ASSERT_EQ(::addL(a, b), ref2(a, b)) << "FunctionRef invocation failed.";
    }
    // Class with operator()
    {
      ::Adder adder{10};
      const FuncRef ref{adder};
      ASSERT_EQ(adder(a, b), ref(a, b)) << "FunctionRef invocation failed.";
    }
    // Lambda
    {
      int c = 100;
      auto adder = [&c](const int lhs, const int rhs) noexcept
      {
        return ::add(lhs, rhs) + c;
      };
      const FuncRef ref{adder};
      ASSERT_EQ(adder(a, b), ref(a, b)) << "FunctionRef invocation failed.";
    }
  }
  // Void case
  {
    using FuncRef = zisc::FunctionReference<void (const int, const int, int*)>;

    constexpr int a = 1;
    constexpr int b = 2;

    // Function pointer
    {
      const FuncRef ref{::add2};
      int expected = 0;
      ::add2(a, b, &expected);
      int result = 0;
      ref(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";
    }
    {
      FuncRef ref{std::addressof(::add2)};
      int expected = 0;
      ::add2(a, b, &expected);
      int result = 0;
      ref(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";

      const FuncRef ref2{ref};
      ref2(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";
    }
    {
      using FuncRef2 = zisc::FunctionReference<void (long, long, int*)>;
      FuncRef2 ref{::addL2};
      int expected = 0;
      ::addL2(a, b, &expected);
      int result = 0;
      ref(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";

      const FuncRef ref2{ref};
      ref2(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";
    }
    // Class with opeartor()
    {
      ::Adder2 adder{10};
      const FuncRef ref{adder};
      int expected = 0;
      adder(a, b, &expected);
      int result = 0;
      ref(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";
    }
    // Lambda
    {
      constexpr int c = 100;
      auto adder = [](const int lhs, const int rhs, int* result) noexcept
      {
        ::add2(lhs, rhs, result);
        *result += c;
      };
      const FuncRef ref{adder};
      int expected = 0;
      adder(a, b, &expected);
      int result = 0;
      ref(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";
    }
  }
}

TEST(FunctionReferenceTest, OperatorTest)
{
  using FuncRef = zisc::FunctionReference<int (int, int)>;

  constexpr int a = 1;
  constexpr int b = 2;

  // Function pointer
  {
    FuncRef ref{};
    ASSERT_FALSE(zisc::cast<bool>(ref)) << "Operator bool is wrong.";

    ref = ::add;
    ASSERT_TRUE(zisc::cast<bool>(ref)) << "Operator bool is wrong.";
    ASSERT_EQ(::add(a, b), ref(a, b)) << "FunctionRef invocation failed.";

    ref.clear();
    ASSERT_FALSE(zisc::cast<bool>(ref)) << "Operator bool is wrong.";
  }
  {
    FuncRef ref{std::addressof(::add)};
    // Move constructor
    FuncRef ref2{ref};
    ASSERT_TRUE(zisc::cast<bool>(ref2)) << "Operator bool is wrong.";
    ASSERT_EQ(::add(a, b), ref2(a, b)) << "FunctionRef invocation failed.";
    // Move assignment
    FuncRef ref3{};
    ASSERT_FALSE(zisc::cast<bool>(ref3)) << "Operator bool is wrong.";
    ref3 = ref2;
    ASSERT_EQ(::add(a, b), ref3(a, b)) << "FunctionRef invocation failed.";
    ASSERT_TRUE(zisc::cast<bool>(ref3)) << "Operator bool is wrong.";
    // Copy assignment
    FuncRef ref4{};
    ASSERT_FALSE(zisc::cast<bool>(ref4)) << "Operator bool is wrong.";
    ref4 = ref3;
    ASSERT_EQ(::add(a, b), ref4(a, b)) << "FunctionRef invocation failed.";
    ASSERT_TRUE(zisc::cast<bool>(ref4)) << "Operator bool is wrong.";
  }
}

TEST(FunctionReferenceTest, SwapTest)
{
  using FuncRef = zisc::FunctionReference<int (int, int)>;

  constexpr int a = 1;
  constexpr int b = 2;

  {
    FuncRef ref1{::add};
    ::Adder adder{10};
    FuncRef ref2{adder};

    ASSERT_EQ(::add(a, b), ref1(a, b)) << "FunctionRef invocation failed.";
    ASSERT_NE(::add(a, b), ref2(a, b)) << "FunctionRef invocation failed.";
    ASSERT_NE(adder(a, b), ref1(a, b)) << "FunctionRef invocation failed.";
    ASSERT_EQ(adder(a, b), ref2(a, b)) << "FunctionRef invocation failed.";

    zisc::swap(ref1, ref2);
    ASSERT_NE(::add(a, b), ref1(a, b)) << "FunctionRef swap failed.";
    ASSERT_EQ(::add(a, b), ref2(a, b)) << "FunctionRef swap failed.";
    ASSERT_EQ(adder(a, b), ref1(a, b)) << "FunctionRef swap failed.";
    ASSERT_NE(adder(a, b), ref2(a, b)) << "FunctionRef swap failed.";
  }
}

namespace {

class NonCopyable : private zisc::NonCopyable<NonCopyable>
{
 public:
  NonCopyable(const int value) : value_{value} {}
  int value_ = 0;
};

class Movable : public zisc::NonCopyable<Movable>
{
 public:
  Movable(const int value) : value_{value} {}
  Movable(Movable&& other) noexcept : value_{other.value_} {}
  int value_ = 0;
};

} // namespace 

TEST(FunctionReferenceTest, ArgumentTest)
{
  auto func = [](Movable a, const NonCopyable& b, NonCopyable& c, Movable&& d) 
  {
    const int result = a.value_ + b.value_ + c.value_ + d.value_;
    c.value_ = 100;
    return result;
  };

  using FuncRef = zisc::FunctionReference<int (::Movable, const ::NonCopyable&, ::NonCopyable&, ::Movable&&)>;
  {
    const FuncRef func_ref{func};
    Movable a{1};
    const NonCopyable b{2};
    NonCopyable c{3};
    Movable d{4};
    const int expected = a.value_ + b.value_ + c.value_ + d.value_;
    const int result = func_ref(std::move(a), b, c, std::move(d));
    ASSERT_EQ(expected, result) << "Passing arguments failed";
    ASSERT_EQ(100, c.value_) << "Passing arguments failed";
  }
}
