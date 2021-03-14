/*!
  \file function_reference_test.cpp
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

  constexpr int operator()(const int a, const int b)
  {
    return add(a, b) + c_;
  }

  int c_;
};

struct Adder2 : private zisc::NonCopyable<Adder>
{
  Adder2(const int c) : c_{c} {}

  constexpr void operator()(const int a, const int b, int* result) const
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
      FuncRef ref{::add};
      ASSERT_EQ(::add(a, b), ref(a, b)) << "FunctionRef invocation failed.";
    }
    {
      FuncRef ref{std::addressof(::add)};
      ASSERT_EQ(::add(a, b), ref(a, b)) << "FunctionRef invocation failed.";

      FuncRef ref2{ref};
      ASSERT_EQ(::add(a, b), ref2(a, b)) << "FunctionRef invocation failed.";
    }
    // Type conversion
    {
      FuncRef ref{std::addressof(::addL)};
      ASSERT_EQ(::addL(a, b), ref(a, b)) << "FunctionRef invocation failed.";
    }
    {
      using FuncRef2 = zisc::FunctionReference<int (long, long)>;
      FuncRef2 ref{std::addressof(::addL)};
      ASSERT_EQ(::addL(a, b), ref(a, b)) << "FunctionRef invocation failed.";

      FuncRef ref2{ref};
      ASSERT_EQ(::addL(a, b), ref2(a, b)) << "FunctionRef invocation failed.";
    }
    // Class with operator()
    {
      ::Adder adder{10};
      FuncRef ref{adder};
      ASSERT_EQ(adder(a, b), ref(a, b)) << "FunctionRef invocation failed.";
    }
    // Lambda
    {
      int c = 100;
      auto adder = [&c](const int lhs, const int rhs) noexcept
      {
        return ::add(lhs, rhs) + c;
      };
      FuncRef ref{adder};
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
      FuncRef ref{::add2};
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

      FuncRef ref2{ref};
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

      FuncRef ref2{ref};
      ref2(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";
    }
    // Class with opeartor()
    {
      ::Adder2 adder{10};
      FuncRef ref{adder};
      int expected = 0;
      adder(a, b, &expected);
      int result = 0;
      ref(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";
    }
    // Lambda
    {
      int c = 100;
      auto adder = [c](const int lhs, const int rhs, int* result) noexcept
      {
        ::add2(lhs, rhs, result);
        *result += c;
      };
      FuncRef ref{adder};
      int expected = 0;
      adder(a, b, &expected);
      int result = 0;
      ref(a, b, &result);
      ASSERT_EQ(expected, result) << "FunctionRef invocation failed.";
    }
  }
}

namespace {

// A regular function
int addValue(const int a, const int b)
{
  return a + b;
}

int addValueL(const long a, const long b)
{
  return static_cast<int>(a + b);
}

int zero(const int, const int)
{
  return 0;
}

}

TEST(FunctionReferenceTest, OperatorTest)
{
  using FuncRef = zisc::FunctionReference<int (int, int)>;
  static_assert(FuncRef::kNumOfArgs == 2);

  constexpr int v1 = 1;
  constexpr int v2 = 2;
  const int expected = ::addValue(v1, v2);

  {
    FuncRef func_ref;
    ASSERT_FALSE(static_cast<bool>(func_ref))
        << "The operator bool of FunctionReference is wrong.";
  }

  // A function pointer
  {
    FuncRef func_ref{&::addValue};
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a function pointer failed.";
    ASSERT_TRUE(static_cast<bool>(func_ref))
        << "The operator bool of FunctionReference is wrong.";

    FuncRef other{&::zero};
    ASSERT_EQ(0, other(v1, v2));

    zisc::swap(func_ref, other);
    ASSERT_EQ(0, func_ref(v1, v2));
    ASSERT_EQ(expected, other(v1, v2))
        << "Swapping of function pointers failed.";

    other.clear();
    ASSERT_FALSE(static_cast<bool>(other))
        << "Clearing the function reference failed";

    func_ref.assign(&::addValue);
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a function pointer failed.";
  }

  // A function pointer variable
  {
    FuncRef::FunctionPointer func_ptr = &::addValue;
    FuncRef func_ref{func_ptr};
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a function pointer variable failed.";
  }

  {
    FuncRef func_ref{&::addValueL};
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a function pointer failed.";

    func_ref.clear();
    func_ref.assign(&::addValueL);
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a function pointer failed.";
  }

  {
    int (*func_ptr)(long, long) = &::addValueL;
    FuncRef func_ref{func_ptr};
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a function pointer variable failed.";
  }

  {
    zisc::FunctionReference<int (long, long)> func_ref{&::addValueL};
    FuncRef func_ref2{func_ref};
    ASSERT_EQ(expected, func_ref2(v1, v2))
        << "Refering to a copied function reference failed.";
  }

  // Copy constructor
  {
    FuncRef func_ref{&::addValueL};
    const FuncRef func_ref2 = func_ref;
    ASSERT_EQ(expected, func_ref2(v1, v2))
        << "Refering to a copied function reference failed.";
  }

  // Lambda function1
  {
    auto add_value = [](const int a, const int b) noexcept
    {
      return a + b;
    };
    FuncRef func_ref{add_value};
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a lambda1 variable failed.";
  }

  // Lambda function2
  {
    int c = 1;
    auto add_value = [&c](const int a, const int b) noexcept
    {
      return a + b + c - 1;
    };
    FuncRef func_ref{add_value};
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a lambda2 variable failed.";

    func_ref.clear();
    func_ref.assign(add_value);
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a lambda2 variable failed.";
  }

  // A function object
  {
    std::function<int (int, int)> func_object{&::addValue};
    FuncRef func_ref{func_object};
    ASSERT_EQ(expected, func_ref(v1, v2))
        << "Refering to a function object failed.";

    FuncRef copied_func{func_ref};
    ASSERT_EQ(expected, copied_func(v1, v2))
        << "Refering to a copied function reference failed.";
  }
}

namespace {

class MoveType : public zisc::NonCopyable<MoveType>
{
 public:
  MoveType(const int value) : value_{value} {}
  [[maybe_unused]] MoveType(MoveType&& other) : value_{other.value_} {}
  int value_ = 0;
};

} // namespace 

TEST(FunctionReferenceTest, TypeTest)
{
  auto func = [](const MoveType& a, MoveType&& b) noexcept
  {
    return a.value_ + b.value_;
  };

  using FuncRef = zisc::FunctionReference<int (const MoveType&, MoveType&&)>;
  {
    FuncRef func_ref{func};
    const MoveType a{10};
    MoveType b{6};
    const int expected = a.value_ + b.value_;
    ASSERT_EQ(expected, func_ref(a, std::move(b)))
        << "Handing arguments in zisc::FunctionReference failed.";
  }
}
