/*!
  \file std_memory_resource_test.cpp
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
#include <memory>
#include <sstream>
#include <string>
#include <utility>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/simple_memory_resource.hpp"
#include "zisc/std_memory_resource.hpp"
#include "zisc/zisc_config.hpp"

TEST(StdMemoryResourceTest, StringStreamTest)
{
  zisc::SimpleMemoryResource mem_resource;

  {
    zisc::pmr::polymorphic_allocator<char> alloc{&mem_resource};
    zisc::pmr::string init{alloc};
    zisc::pmr::stringstream code{init};
    code << "Zisc";
    code << "String";
    code << "Test";
    ASSERT_STREQ(code.str().data(), "ZiscStringTest");
  }
}

TEST(StdMemoryResourceTest, UniquePtrConstructionTest)
{
  zisc::SimpleMemoryResource mem_resource;

  {
    // Constructor without parameter
    auto p1 = zisc::pmr::allocateUnique<int>(&mem_resource);
  }
  {
    // Constructo with a parameter
    constexpr int expected = 16;
    auto p2 = zisc::pmr::allocateUnique<int>(&mem_resource, expected);
    ASSERT_EQ(*p2, expected) << "zisc::pmr::unique_ptr initialization failed.";
    ASSERT_GE(mem_resource.totalMemoryUsage(), 0)
        << "Memory allocation using the memory resource failed.";
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << "Memory deallocation using the memory resource failed.";
}

TEST(StdMemoryResourceTest, UniquePtrDeletionTest)
{
  int value = 0;
  struct Test
  {
    Test(int* v) : v_{v} {*v_ = 1;}
    ~Test() {*v_ = 2;}
    int* v_ = nullptr;
  };

  zisc::SimpleMemoryResource mem_resource;
  zisc::pmr::polymorphic_allocator<Test> alloc{&mem_resource};

  // Empty data
  {
    zisc::pmr::unique_ptr<Test> ptr{};
  }

  // Null pointer
  {
    zisc::pmr::unique_ptr<Test> ptr{nullptr, alloc};
  }

  ASSERT_FALSE(value);
  {
    auto ptr = zisc::pmr::allocateUnique<Test>(alloc, &value);
    ASSERT_EQ(value, 1) << "zisc::pmr::unique_ptr construction failed.";
  }
  ASSERT_EQ(value, 2) << "zisc::pmr::unique_ptr destruction failed.";
}
