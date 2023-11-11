/*!
  \file std_memory_resource_test.cpp
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
#include <future>
#include <list>
#include <map>
#include <memory>
#include <stack>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"

TEST(StdMemoryResourceTest, VectorTest)
{
  zisc::AllocFreeResource mem_resource{};

  {
    const std::pmr::polymorphic_allocator<int> alloc{&mem_resource};
    std::pmr::vector<int> values{alloc};
    values = {1, 2, 3, 4, 5, 6, 7, 8};
    ASSERT_EQ(8, values.size()) << "The vector initialization failed.";
  }

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}

TEST(StdMemoryResourceTest, ListTest)
{
  zisc::AllocFreeResource mem_resource{};

  {
    const std::pmr::polymorphic_allocator<int> alloc{&mem_resource};
    std::pmr::list<int> values{alloc};
    values = {1, 2, 3, 4, 5, 6, 7, 8};
    ASSERT_EQ(8, values.size()) << "The list initialization failed.";
  }

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}

TEST(StdMemoryResourceTest, MapTest)
{
  zisc::AllocFreeResource mem_resource{};

  {
    const std::pmr::polymorphic_allocator<int> alloc{&mem_resource};
    std::pmr::map<int, int> values{alloc};
    values = {{1, 1}, {2, 2}, {3, 3}, {4, 4}};
    ASSERT_EQ(4, values.size()) << "The map initialization failed.";
  }

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}

TEST(StdMemoryResourceTest, StackTest)
{
  zisc::AllocFreeResource mem_resource{};

  {
    const std::pmr::polymorphic_allocator<int> alloc{&mem_resource};
    std::stack<int, std::pmr::deque<int>> values{alloc};
    values.push(1);
    values.push(2);
    values.push(3);
    values.push(4);
    ASSERT_EQ(4, values.size()) << "The stack initialization failed.";
  }

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}

TEST(StdMemoryResourceTest, StringTest)
{
  zisc::AllocFreeResource mem_resource{};

  {
    const std::pmr::polymorphic_allocator<char> alloc{&mem_resource};
    std::pmr::string code{alloc};
    code = "ZiscStringTest";
    ASSERT_STREQ(code.data(), "ZiscStringTest");
  }

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}

//TEST(StdMemoryResourceTest, StringStreamTest)
//{
//  zisc::AllocFreeResource mem_resource{};
//
//  {
//    const std::pmr::polymorphic_allocator<char> alloc{&mem_resource};
//    zisc::pmr::stringstream code{std::ios_base::in | std::ios_base::out, alloc};
//    code << "Zisc";
//    code << "StringStream";
//    code << "Test";
//    code << 16;
//    ASSERT_STREQ(code.str().data(), "ZiscStringStreamTest16");
//  }
//
//  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
//  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
//  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
//}

TEST(StdMemoryResourceTest, PromiseTest)
{
  zisc::AllocFreeResource mem_resource{};

  {
    constexpr int expected = 16;
    const std::pmr::polymorphic_allocator<int> alloc{&mem_resource};
    std::promise<int> p{std::allocator_arg, alloc};
    std::future result = p.get_future();
    p.set_value(expected);
    ASSERT_EQ(expected, result.get());
  }

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}

//TEST(StdMemoryResourceTest, PackagedTaskTest)
//{
//  zisc::AllocFreeResource mem_resource{};
//
//  {
//    constexpr int expected = 16;
//    const auto getter = []() noexcept
//    {
//      return expected;
//    };
//    const std::pmr::polymorphic_allocator<int> alloc{&mem_resource};
//    std::packaged_task<int ()> func{std::allocator_arg, alloc, getter};
//    std::future result = func.get_future();
//    func();
//    ASSERT_EQ(expected, result.get());
//  }
//
//  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
//  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
//  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
//}

TEST(StdMemoryResourceTest, SharedPtrTest)
{
  zisc::AllocFreeResource mem_resource{};

  {
    constexpr int expected = 16;
    const std::pmr::polymorphic_allocator<int> alloc{&mem_resource};
    std::shared_ptr ptr = std::allocate_shared<int>(alloc, expected);
    ASSERT_EQ(expected, *ptr);
  }

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}

TEST(StdMemoryResourceTest, UniquePtrConstructionTest)
{
  zisc::AllocFreeResource mem_resource;

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

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}

TEST(StdMemoryResourceTest, UniquePtrDeletionTest)
{
  int value = 0;
  struct MemTest
  {
    explicit MemTest(int* v) noexcept : v_{v} {*v_ = 1;}
    ~MemTest() noexcept {*v_ = 2;}
    int* v_ = nullptr;
  };

  zisc::AllocFreeResource mem_resource;
  const std::pmr::polymorphic_allocator<MemTest> alloc{&mem_resource};

  // Empty data
  {
    const zisc::pmr::unique_ptr<MemTest> ptr{};
  }

  // Null pointer
  {
    const zisc::pmr::unique_ptr<MemTest> ptr{nullptr, alloc};
  }

  ASSERT_FALSE(value);
  {
    auto ptr = zisc::pmr::allocateUnique<MemTest>(alloc, &value);
    ASSERT_EQ(value, 1) << "zisc::pmr::unique_ptr construction failed.";
  }
  ASSERT_EQ(value, 2) << "zisc::pmr::unique_ptr destruction failed.";

  const zisc::Memory::Usage& usage = mem_resource.memoryUsage();
  ASSERT_NE(0, usage.peak()) << "The memory resource isn't used.";
  ASSERT_EQ(0, usage.total()) << "There is un-released memory.";
}
