/*!
  \file unique_memory_pointer_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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
#include "zisc/unique_memory_pointer.hpp"
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

int Test::counter_ = 0;

} // namespace

TEST(UniqueMemoryPointerTest, FunctionTest)
{
  using UniquePointer = zisc::UniqueMemoryPointer<::Test>;

  // null pointer
  UniquePointer p1;
  ASSERT_FALSE(p1) << "The empty unique pointer should be false.";
  ASSERT_EQ(0, ::Test::counter_);

  auto shared_resource = zisc::SimpleMemoryResource::sharedResource();
  ASSERT_TRUE(shared_resource->is_equal(*shared_resource));

  {
    auto p2 = UniquePointer::make(shared_resource, 100);
    ASSERT_EQ(100, (*p2).number_);
    ASSERT_EQ(1, ::Test::counter_);

    auto p3 = UniquePointer::make(shared_resource);
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
    zisc::pmr::polymorphic_allocator<::Test> alloc{shared_resource};
    auto test = alloc.allocate(1);
    alloc.construct(test);

    ASSERT_EQ(2, ::Test::counter_);
    p1.reset(test, shared_resource);
    ASSERT_EQ(1, ::Test::counter_);
  }

  p1.reset();
  ASSERT_FALSE(p1) << "The empty unique pointer should be false.";
  ASSERT_EQ(0, ::Test::counter_);
}
