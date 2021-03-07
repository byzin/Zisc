/*!
  \file simple_memory_resource_test.cpp
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
#include <array>
#include <cstddef>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/memory/simple_memory_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"
#include "zisc/thread/atomic.hpp"
#include "zisc/thread/atomic_word.hpp"

namespace {

struct Data1
{
  std::array<zisc::uint64b, 10> value_;
};

}

TEST(SimpleMemoryResourceTest, MemoryAllocationTest)
{
  zisc::SimpleMemoryResource mem_resource;
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << "SimpleMemoryResource initialization failed.";
  ASSERT_FALSE(mem_resource.peakMemoryUsage())
      << "SimpleMemoryResource initialization failed.";

  using zisc::uint8b;
  using zisc::uint16b;
  using zisc::uint32b;
  using zisc::uint64b;

  constexpr std::size_t header_s = sizeof(zisc::SimpleMemoryResource::Header);
  std::size_t total = 0;
  std::size_t peak = 0;

  auto p1 = mem_resource.allocate(sizeof(uint8b), std::alignment_of_v<uint8b>);
  total += sizeof(uint8b) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);
  {
    const auto header = mem_resource.getHeader(p1);
    ASSERT_TRUE(header->size_) << "Header initialization failed.";
    ASSERT_TRUE(header->alignment_) << "Header initialization failed.";
  }

  auto p2 = mem_resource.allocate(sizeof(uint16b), std::alignment_of_v<uint16b>);
  total += sizeof(uint16b) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p3 = mem_resource.allocate(sizeof(::Data1), std::alignment_of_v<::Data1>);
  total += sizeof(::Data1) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  mem_resource.deallocate(p3, sizeof(::Data1), std::alignment_of_v<::Data1>);
  total -= sizeof(::Data1) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p4 = mem_resource.allocate(sizeof(uint32b), std::alignment_of_v<uint32b>);
  total += sizeof(uint32b) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p5 = mem_resource.allocate(sizeof(uint64b), std::alignment_of_v<uint64b>);
  total += sizeof(uint64b) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  p3 = mem_resource.allocate(sizeof(::Data1), std::alignment_of_v<::Data1>);
  total += sizeof(::Data1) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto p6 = mem_resource.allocate(sizeof(long double),
                                  std::alignment_of_v<long double>);
  total += sizeof(long double) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  mem_resource.deallocate(p1, sizeof(uint8b), std::alignment_of_v<uint8b>);
  mem_resource.deallocate(p2, sizeof(uint16b), std::alignment_of_v<uint16b>);
  mem_resource.deallocate(p3, sizeof(::Data1), std::alignment_of_v<::Data1>);
  mem_resource.deallocate(p4, sizeof(uint32b), std::alignment_of_v<uint32b>);
  mem_resource.deallocate(p5, sizeof(uint64b), std::alignment_of_v<uint64b>);
  mem_resource.deallocate(p6, sizeof(long double), std::alignment_of_v<long double>);

  {
    zisc::SimpleMemoryResource mem_resource1{std::move(mem_resource)};
    zisc::SimpleMemoryResource mem_resource2;
    mem_resource2 = std::move(mem_resource1);
    ASSERT_FALSE(mem_resource2.totalMemoryUsage());
    ASSERT_GE(mem_resource2.peakMemoryUsage(), peak);
  }
}

TEST(SimpleMemoryResourceTest, ResourceComparisonTest)
{
  zisc::SimpleMemoryResource mem_resource1;
  zisc::SimpleMemoryResource mem_resource2;

  ASSERT_TRUE(mem_resource1.is_equal(mem_resource1));
  ASSERT_FALSE(mem_resource1.is_equal(mem_resource2));
}

TEST(SimpleMemoryResourceTest, AlignmentTest)
{
  zisc::SimpleMemoryResource mem_resource;
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << "SimpleMemoryResource initialization failed.";
  ASSERT_FALSE(mem_resource.peakMemoryUsage())
      << "SimpleMemoryResource initialization failed.";

  auto test_allocation = [&mem_resource](const std::size_t alignment)
  {
    std::size_t size = 3 * alignment;
    void* ptr = mem_resource.allocate(size, alignment);
    {
      void* p = ptr;
      ASSERT_EQ(std::align(alignment, size, p, size), ptr)
          << "Allocation with " << alignment << " alignment is failed.";
      ASSERT_EQ(p, ptr)
          << "Allocation with " << alignment << " alignment is failed.";
    }
    mem_resource.deallocate(ptr, alignment, alignment);
    ASSERT_FALSE(mem_resource.totalMemoryUsage())
        << "Allocation with " << alignment << " alignment is failed.";
  };

  // 1 byte alignment
  {
    constexpr std::size_t alignment = 1;
    test_allocation(alignment);
  }
  // 2 bytes alignment
  {
    constexpr std::size_t alignment = 2;
    test_allocation(alignment);
  }
  // 4 bytes alignment
  {
    constexpr std::size_t alignment = 4;
    test_allocation(alignment);
  }
  // 8 bytes alignment
  {
    constexpr std::size_t alignment = 8;
    test_allocation(alignment);
  }
  // std::max_align_t
  {
    constexpr std::size_t alignment = std::alignment_of_v<std::max_align_t>;
    test_allocation(alignment);
  }
  // 64 bytes alignment
  {
    constexpr std::size_t alignment = 64;
    test_allocation(alignment);
  }
  // 256 bytes alignment
  {
    constexpr std::size_t alignment = 256;
    test_allocation(alignment);
  }
  // 1024 bytes alignment
  {
    constexpr std::size_t alignment = 1024;
    test_allocation(alignment);
  }
  // 4096 bytes alignment
  {
    constexpr std::size_t alignment = 4096;
    test_allocation(alignment);
  }
}

TEST(SimpleMemoryResource, MultiThreadTest)
{
  zisc::SimpleMemoryResource mem_resource;
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << "SimpleMemoryResource initialization failed.";
  ASSERT_FALSE(mem_resource.peakMemoryUsage())
      << "SimpleMemoryResource initialization failed.";

  zisc::AtomicWord<true> ready1{0};
  zisc::AtomicWord<true> ready2{0};
  std::atomic<std::size_t> created{0};
  std::atomic<std::size_t> alloc_completed{0};
  std::atomic<std::size_t> dealloc_completed{0};

  constexpr std::size_t header_size = sizeof(zisc::SimpleMemoryResource::Header);
  constexpr std::size_t n_threads = 1024;
  constexpr std::size_t loop = 100;
  constexpr std::size_t alloc_size = 5 * 1024;

  auto alloc =
  [&mem_resource, &ready1, &ready2, &created, &alloc_completed, &dealloc_completed]()
  {
    created.fetch_add(1, std::memory_order::release);

    // Allocate memory
    zisc::atomic_wait(&ready1, 0, std::memory_order::acquire);
    std::array<void*, loop> mem_list;
    for (std::size_t i = 0; i < loop; ++i) {
      constexpr std::size_t alignment = std::alignment_of_v<std::size_t>;
      auto p = mem_resource.allocate(alloc_size, alignment);
      mem_list[i] = p;
    }
    alloc_completed.fetch_add(1, std::memory_order::release);

    // Deallocate memory
    zisc::atomic_wait(&ready2, 0, std::memory_order::acquire);
    for (auto p : mem_list)
      mem_resource.deallocate(p, 0, 0);
    dealloc_completed.fetch_add(1, std::memory_order::release);
  };

  std::array<std::thread, n_threads> thread_list;
  // Create threads
  for (std::size_t i = 0; i < thread_list.size(); ++i)
    thread_list[i] = std::thread{alloc};
  while (created.load(std::memory_order::acquire) < n_threads)
    std::this_thread::yield();
  ready1.store(1, std::memory_order::release);
  zisc::atomic_notify_all(&ready1);
  // Allocation test
  while (alloc_completed.load(std::memory_order::acquire) < n_threads)
    std::this_thread::yield();
  constexpr std::size_t expected_peak = n_threads * loop * (alloc_size + header_size);
  EXPECT_EQ(expected_peak, mem_resource.totalMemoryUsage())
      << "Memory allocation in parallel failed.";
  EXPECT_EQ(expected_peak, mem_resource.peakMemoryUsage())
      << "Memory allocation in parallel failed.";
  ready2.store(1, std::memory_order::release);
  zisc::atomic_notify_all(&ready2);
  // Deallocation test
  while (dealloc_completed.load(std::memory_order::acquire) < n_threads)
    std::this_thread::yield();
  EXPECT_EQ(0, mem_resource.totalMemoryUsage())
      << "Memory allocation in parallel failed.";
  EXPECT_EQ(expected_peak, mem_resource.peakMemoryUsage())
      << "Memory allocation in parallel failed.";
  // Join threads
  for (auto& t : thread_list)
    t.join();
}
