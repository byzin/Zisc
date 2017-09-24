/*!
  \file memory_pool_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/error.hpp"
#include "zisc/memory_chunk.hpp"
#include "zisc/memory_pool.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"


namespace {

class TestBase
{
 public:
  TestBase() {}
  virtual ~TestBase() {}

  static void* operator new(std::size_t) = delete;
  static void* operator new[](std::size_t) = delete;
  static void* operator new(std::size_t, zisc::MemoryChunk* chunk)
  {
    chunk->setFree(false);
    return chunk->data<void>();
  }

  static void operator delete(void*)
  {
    ZISC_ASSERT(false, "Invalid delete function is called.");
  }
  static void operator delete[](void*) = delete;
  static void operator delete(void* ptr, zisc::MemoryChunk* chunk)
  {
    zisc::cast<TestBase*>(ptr)->~TestBase();
    chunk->setFree(true);
  }
};

class TestSuper1 : public TestBase
{
 public:
  class Inner
  {
   public:
    Inner(int* value) : value_{value} {++(*value_);}
    ~Inner() {--(*value_);}
    int* value_;
  };
  TestSuper1(int* value) : inner_{value} {}
  ~TestSuper1() override {}

 private:
  Inner inner_;
};

class TestSuper2 : public TestBase
{
 public:
  TestSuper2(int* value) : inner1_{value}, inner2_{value} {}
  ~TestSuper2() override {}

 private:
  TestSuper1::Inner inner1_,
                    inner2_;
};

}

TEST(MemoryPoolTest, InitTest)
{
  constexpr std::size_t memory_size1 = 1024; // 1024 bytes
  zisc::MemoryPool pool{memory_size1};
  zisc::MemoryPool memory_pool{std::move(pool)};

  ASSERT_EQ(memory_size1, memory_pool.size()) << "The size of the pool is wrong.";
  ASSERT_EQ(0u, memory_pool.usedMemory()) << "";
  ASSERT_EQ(0u, memory_pool.numOfChunks()) << "";

  // Resize the memory pool
  constexpr std::size_t memory_size2 = 2048;
  memory_pool.setSize(memory_size2);
  ASSERT_EQ(memory_size2, memory_pool.size()) << "The size of the pool is wrong.";
}

TEST(MemoryPoolTest, StressTest)
{
  constexpr std::size_t memory_size = 1024u * 1024u * 1024u; // 1 Gbytes
  zisc::MemoryPool memory_pool{memory_size};
  ASSERT_EQ(memory_size, memory_pool.size()) << "The size of the pool is wrong.";
}

TEST(MemoryPoolTest, AllocationTest)
{
  constexpr std::size_t memory_size = 1024; // 1024 bytes
  zisc::MemoryPool memory_pool{memory_size};

  // Test Error case
  {
    int tmp = 0;
    ASSERT_FALSE(memory_pool.hasContained(&tmp)) << "The 'hasContained' is broken.";
    auto c = memory_pool.getChunk(0u);
    ASSERT_TRUE(c == nullptr) << "The 'getChunk(index)' is broken.";
    c = memory_pool.getChunk(&tmp);
    ASSERT_TRUE(c == nullptr) << "The 'getChunk(ptr)' is broken.";
  }

  auto test_memory_pool = [](zisc::MemoryPool& pool)
  {
    // Create TestSuper1 instance
    int value = 0;
    auto chunk1 = pool.allocate<::TestSuper1>(1);
    ::TestBase* instance1 = new(chunk1) ::TestSuper1{&value};
    const std::size_t instance1_size = zisc::MemoryChunk::headerSize() +
                                       chunk1->stride();

    ASSERT_EQ(instance1_size, pool.usedMemory()) << "Invalid memory usage.";
    ASSERT_TRUE(pool.hasContained(instance1)) << "The 'hasContained' is broken.";
    ASSERT_EQ(1u, pool.numOfChunks()) << "Invalid num of chunks.";
    auto c = pool.getChunk(0u);
    ASSERT_EQ(chunk1, c) << "The 'getChunk(index)' is broken.";
    c = pool.getChunk(instance1);
    ASSERT_EQ(chunk1, c) << "The 'getChunk(ptr)' is broken.";
    ASSERT_TRUE(c->isValid()) << "The chunk is invalid.";
    ASSERT_EQ(0u, c->id()) << "The chunk ID isn't set.";
    ASSERT_FALSE(c->isFreed()) << "The chunk free isn't set.";
    ASSERT_EQ(1, value) << "The new operator is broken.";

    // Create TestSuper2 instance
    auto chunk2 = pool.allocate<::TestSuper2>(1);
    ::TestBase* instance2 = new(chunk2) ::TestSuper2{&value};
    const std::size_t instance2_size = zisc::MemoryChunk::headerSize() +
                                       chunk2->stride();

    ASSERT_EQ(instance1_size + instance2_size,
              pool.usedMemory()) << "Invalid memory usage.";
    ASSERT_TRUE(pool.hasContained(instance2)) << "The 'hasContained' is broken.";
    ASSERT_EQ(2u, pool.numOfChunks()) << "Invalid num of chunks.";
    c = pool.getChunk(1u);
    ASSERT_EQ(chunk2, c) << "The 'getChunk(index)' is broken.";
    c = pool.getChunk(instance2);
    ASSERT_EQ(chunk2, c) << "The 'getChunk(ptr)' is broken.";
    ASSERT_TRUE(c->isValid()) << "The chunk is invalid.";
    ASSERT_EQ(1u, c->id()) << "The chunk ID isn't set.";
    ASSERT_FALSE(c->isFreed()) << "The chunk free isn't set.";
    ASSERT_EQ(3, value) << "The new operator is broken.";

    // Create integer instances
    constexpr zisc::uint num_of_int = 2;
    auto chunk3 = pool.allocate<int>(num_of_int);
    new(chunk3) int[num_of_int];
    ASSERT_EQ(sizeof(int) * 2, chunk3->size()) << "The array allocation failed.";

    // Destroy TestSuper2 instance
    ::TestBase::operator delete(instance2, chunk2);

    ASSERT_TRUE(chunk2->isFreed()) << "The chunk free isn't set.";
    ASSERT_EQ(1, value) << "The delete operator is broken.";

    // Destroy TestSuper1 instance
    ::TestBase::operator delete(instance1, chunk1);

    ASSERT_TRUE(chunk1->isFreed()) << "The chunk free isn't set.";
    ASSERT_EQ(0, value) << "The delete operator is broken.";
  };

  std::cout << "First time test" << std::endl;
  test_memory_pool(memory_pool);

  memory_pool.setSize(memory_size * 2);

  std::cout << "Second time test" << std::endl;
  test_memory_pool(memory_pool);
}
