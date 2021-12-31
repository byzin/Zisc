/*!
  \file std_memory_resource_test.cpp
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
#include <memory>
#include <sstream>
#include <string>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/memory/simple_memory_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"

TEST(StdMemoryResourceTest, StringStreamTest)
{
  zisc::SimpleMemoryResource mem_resource;

  {
    zisc::pmr::polymorphic_allocator<char> alloc{&mem_resource};
    zisc::pmr::string init{zisc::pmr::string::allocator_type{alloc}};
    zisc::pmr::stringstream code{init};
    code << "Zisc";
    code << "String";
    code << "Test";
    ASSERT_STREQ(code.str().data(), "ZiscStringTest");
  }

  std::cout << "total: " << mem_resource.totalMemoryUsage() << " bytes" << std::endl;
  std::cout << "peak : " << mem_resource.peakMemoryUsage() << " bytes" << std::endl;
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

  // Derivation test
  {
    struct Base
    {
      virtual ~Base() = default;
      virtual int getValue()
      {
        return 0;
      }
    };
    struct MemTest : public Base
    {
      int getValue() override
      {
        return 1;
      }
    };
    using UniqueBase = zisc::pmr::unique_ptr<Base>;
    UniqueBase ptr = zisc::pmr::allocateUnique<MemTest>(&mem_resource);
    ASSERT_TRUE(ptr->getValue());
  }
}

TEST(StdMemoryResourceTest, UniquePtrDeletionTest)
{
  int value = 0;
  struct MemTest
  {
    MemTest(int* v) noexcept : v_{v} {*v_ = 1;}
    ~MemTest() noexcept {*v_ = 2;}
    int* v_ = nullptr;
  };

  zisc::SimpleMemoryResource mem_resource;
  zisc::pmr::polymorphic_allocator<MemTest> alloc{&mem_resource};

  // Empty data
  {
    zisc::pmr::unique_ptr<MemTest> ptr{};
  }

  // Null pointer
  {
    zisc::pmr::unique_ptr<MemTest> ptr{nullptr, alloc};
  }

  ASSERT_FALSE(value);
  {
    auto ptr = zisc::pmr::allocateUnique<MemTest>(alloc, &value);
    ASSERT_EQ(value, 1) << "zisc::pmr::unique_ptr construction failed.";
  }
  ASSERT_EQ(value, 2) << "zisc::pmr::unique_ptr destruction failed.";
}
