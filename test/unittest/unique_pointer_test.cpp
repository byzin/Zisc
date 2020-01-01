/*!
  \file unique_pointer_test.cpp
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
#include <cstddef>
#include <cstdlib>
#include <utility>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/simple_memory_resource.hpp"
#include "zisc/unique_pointer.hpp"
#include "zisc/zisc_config.hpp"

namespace {

class Test
{
 public:
  static int counter_;
  int number_;
  Test()
  {
    number_ = counter_++;
  }
  Test(const int n)
  {
    ++counter_;
    number_ = n;
  }
  ~Test()
  {
    --counter_;
  }
};

class Super : public Test
{
 public:
  Super(const int n) : Test(n) {}
};

int Test::counter_ = 0;

} // namespace

TEST(UniquePointerTest, FunctionTest)
{
  using UniquePointer = zisc::UniquePointer<::Test>;

  // null pointer
  UniquePointer p1;
  ASSERT_FALSE(p1) << "The empty unique pointer should be false.";
  ASSERT_EQ(0, ::Test::counter_);

  zisc::SimpleMemoryResource mem_resource;
  {
    auto p2 = UniquePointer::make(&mem_resource, 100);
    ASSERT_EQ(100, (*p2).number_);
    ASSERT_EQ(1, ::Test::counter_);

    auto p3 = UniquePointer::make(&mem_resource);
    ASSERT_EQ(1, p3->number_);
    ASSERT_EQ(2, ::Test::counter_);

    zisc::swap(p2, p3);

    p1 = std::move(p2);
  }

  ASSERT_TRUE(p1);
  ASSERT_EQ(1, p1->number_);
  ASSERT_EQ(1, ::Test::counter_);

  // Reset
  {
    std::pmr::polymorphic_allocator<::Test> alloc{&mem_resource};
    auto test = alloc.allocate(1);
    alloc.construct(test);

    ASSERT_EQ(2, ::Test::counter_);
    p1.reset(test, &mem_resource);
    ASSERT_EQ(1, ::Test::counter_);
  }

  p1.reset();
  ASSERT_FALSE(p1) << "The empty unique pointer should be false.";
  ASSERT_EQ(0, ::Test::counter_);

  {
    UniquePointer p = zisc::UniquePointer<::Super>::make(&mem_resource, 20);
    ASSERT_EQ(20, p->number_);
    ASSERT_EQ(1, ::Test::counter_);
  }
}
