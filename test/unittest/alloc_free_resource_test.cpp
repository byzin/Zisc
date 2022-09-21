/*!
  \file alloc_free_resource_test.cpp
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
#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <random>
#include <span>
#include <thread>
#include <type_traits>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/concurrency/atomic.hpp"
#include "zisc/concurrency/atomic_word.hpp"
#include "zisc/memory/memory.hpp"
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace {

struct Data1
{
  std::array<zisc::uint64b, 10> value_;
};

}

TEST(AllocFreeResourceTest, MemoryAllocationTest)
{
  zisc::AllocFreeResource mem_resource;
  ASSERT_EQ(0, mem_resource.totalMemoryUsage()) << "Resource initialization failed.";
  ASSERT_EQ(0, mem_resource.peakMemoryUsage()) << "Resource initialization failed.";

  using zisc::uint8b;
  using zisc::uint16b;
  using zisc::uint32b;
  using zisc::uint64b;

  constexpr std::size_t header_s = sizeof(zisc::AllocFreeResource::Header);
  std::size_t total = 0;
  std::size_t peak = 0;

  auto* p1 = mem_resource.allocate(sizeof(uint8b), std::alignment_of_v<uint8b>);
  total += sizeof(uint8b) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);
  {
    const auto* header = mem_resource.getHeader(p1);
    ASSERT_TRUE(header->size_) << "Header initialization failed.";
    ASSERT_TRUE(header->alignment_) << "Header initialization failed.";
  }

  auto* p2 = mem_resource.allocate(sizeof(uint16b), std::alignment_of_v<uint16b>);
  total += sizeof(uint16b) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto* p3 = mem_resource.allocate(sizeof(::Data1), std::alignment_of_v<::Data1>);
  total += sizeof(::Data1) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  mem_resource.deallocate(p3, sizeof(::Data1), std::alignment_of_v<::Data1>);
  total -= sizeof(::Data1) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto* p4 = mem_resource.allocate(sizeof(uint32b), std::alignment_of_v<uint32b>);
  total += sizeof(uint32b) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto* p5 = mem_resource.allocate(sizeof(uint64b), std::alignment_of_v<uint64b>);
  total += sizeof(uint64b) + header_s;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  p3 = mem_resource.allocate(sizeof(::Data1), std::alignment_of_v<::Data1>);
  total += sizeof(::Data1) + header_s;
  peak = total;
  ASSERT_GE(mem_resource.totalMemoryUsage(), total);
  ASSERT_GE(mem_resource.peakMemoryUsage(), peak);

  auto* p6 = mem_resource.allocate(sizeof(long double),
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
    zisc::AllocFreeResource mem_resource1{std::move(mem_resource)};
    zisc::AllocFreeResource mem_resource2;
    mem_resource2 = std::move(mem_resource1);
    ASSERT_FALSE(mem_resource2.totalMemoryUsage());
    ASSERT_GE(mem_resource2.peakMemoryUsage(), peak);
  }
}

TEST(AllocFreeResourceTest, ResourceComparisonTest)
{
  const zisc::AllocFreeResource mem_resource1;
  const zisc::AllocFreeResource mem_resource2;

  ASSERT_TRUE(mem_resource1.is_equal(mem_resource1));
  ASSERT_FALSE(mem_resource1.is_equal(mem_resource2));
}

TEST(AllocFreeResourceTest, AlignmentTest)
{
  zisc::AllocFreeResource mem_resource;
  ASSERT_EQ(0, mem_resource.totalMemoryUsage()) << "Resource initialization failed.";
  ASSERT_EQ(0, mem_resource.peakMemoryUsage()) << "Resource initialization failed.";

  for (std::size_t i = 0; i < 13; ++i) {
    const std::size_t alignment = 1 << i;
    std::size_t size = 3 * alignment;
    // Allocation
    void* ptr = mem_resource.allocate(size, alignment);
    {
      void* p = ptr;
      ASSERT_EQ(std::align(alignment, size, p, size), ptr)
          << "Allocation with " << alignment << " alignment is failed.";
    }
    // Memory access test
    const std::span<std::byte> bytes{static_cast<std::byte*>(ptr), size};
    std::for_each(bytes.begin(), bytes.end(), [](std::byte& in)
    {
      in = std::byte{(std::numeric_limits<zisc::uint8b>::max)()};
    });
    // Deallocation
    mem_resource.deallocate(ptr, size, alignment);
    ASSERT_EQ(0, mem_resource.totalMemoryUsage())
        << "Allocation with " << alignment << " alignment is failed.";
  };
}

TEST(AllocFreeResource, MultiThreadTest)
{
  zisc::AllocFreeResource mem_resource;
  ASSERT_EQ(0, mem_resource.totalMemoryUsage()) << "Resource initialization failed.";
  ASSERT_EQ(0, mem_resource.peakMemoryUsage()) << "Resource initialization failed.";

  constexpr std::size_t n_threads = 128;
  constexpr std::size_t loop = 10'000;

  std::atomic_int worker_lock{-1};
  std::vector<std::thread> worker_list;
  worker_list.reserve(n_threads);
  struct Data
  {
    void* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t alignment_ = 0;
  };
  std::vector<Data> data_list;
  data_list.resize(n_threads * loop);

  // Allocation test
  for (std::size_t i = 0; i < n_threads; ++i) {
    worker_list.emplace_back([i, &mem_resource, &data_list, &worker_lock]()
    {
      // Wait the thread until all threads become ready
      worker_lock.wait(-1, std::memory_order::acquire);
      // Do the actual test
      std::mt19937_64 sampler{i};
      std::uniform_int_distribution<std::size_t> distrib{0, 12};
      for (std::size_t j = 0; j < loop; ++j) {
        const std::size_t alignment = 1 << distrib(sampler);
        const std::size_t size = 3 * alignment;
        const std::size_t index = i * loop + j;
        std::byte* ptr = nullptr;
        try {
          void* data = mem_resource.allocate(size, alignment);
          data_list[index] = {data, size, alignment};
          ptr = static_cast<std::byte*>(data);
        }
        catch (const zisc::Memory::BadAlloc& error) {
          FAIL() << "Memory allocation failed. size=" << error.size() << ", alignment=" << error.alignment();
        }
        // Memory access test
        const std::span<std::byte> bytes{ptr, size};
        std::for_each(bytes.begin(), bytes.end(), [](std::byte& in)
        {
          in = std::byte{(std::numeric_limits<zisc::uint8b>::max)()};
        });
      }
    });
  }
  // Start the test, notify all threads
  worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
  worker_lock.notify_all();

  // Wait the test done
  std::for_each_n(worker_list.begin(), n_threads, [](std::thread& w){w.join();});
  worker_list.clear();

  auto to_mb = [](const std::size_t bytes) noexcept
  {
    const double mb = zisc::cast<double>(bytes) / (1000.0 * 1000.0);
    return mb;
  };
  ASSERT_NE(0, mem_resource.totalMemoryUsage()) << "Allocation test failed.";
  std::cout << "## total memory usage: "
            << std::scientific << to_mb(mem_resource.totalMemoryUsage()) << " MB."
            << std::endl;
  ASSERT_NE(0, mem_resource.peakMemoryUsage()) << "Allocation test failed.";
  std::cout << "##  peak memory usage: "
            << std::scientific << to_mb(mem_resource.peakMemoryUsage()) << " MB."
            << std::endl;

  // Deallocation test
  worker_lock.store(-1, std::memory_order::release);
  for (std::size_t i = 0; i < n_threads; ++i) {
    worker_list.emplace_back([i, &mem_resource, &data_list, &worker_lock]()
    {
      // Wait the thread until all threads become ready
      worker_lock.wait(-1, std::memory_order::acquire);
      // Do the actual test
      for (std::size_t j = 0; j < loop; ++j) {
        const std::size_t index = i * loop + j;
        const Data& data = data_list[index];
        const std::size_t size = data.size_;
        const std::size_t alignment = data.alignment_;
        mem_resource.deallocate(data.data_, size, alignment);
      }
    });
  }
  // Start the test, notify all threads
  worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
  worker_lock.notify_all();

  // Wait the test done
  std::for_each_n(worker_list.begin(), n_threads, [](std::thread& w){w.join();});

  ASSERT_EQ(0, mem_resource.totalMemoryUsage()) << "Deallocation test failed.";
}
