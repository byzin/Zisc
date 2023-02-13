/*!
  \file fixed_array_resource_test.cpp
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
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/memory/fixed_array_resource.hpp"
#include "zisc/memory/memory.hpp"

TEST(FixedArrayResourceTest, MemoryAllocationTest)
{

  using StorageT = zisc::uint32b;
  zisc::AllocFreeResource r{};
  zisc::FixedArrayResource<StorageT> mem_resource{&r};

  // Check default values
  constexpr std::size_t size_max = sizeof(StorageT);
  constexpr std::size_t alignment_max = std::alignment_of_v<StorageT>;
  ASSERT_EQ(size_max, mem_resource.sizeMax());
  ASSERT_EQ(alignment_max, mem_resource.alignmentMax());
  ASSERT_EQ(0, mem_resource.count()) << "Initialization of the resource failed.";
  ASSERT_EQ(1, mem_resource.countMax()) << "Initialization of the resource failed.";

  //
  const std::array<std::size_t, 3> size_list{{1, 10, 10 * 8 * zisc::Config::l1CacheLineSize()}};
  for (const std::size_t s : size_list) {
    std::vector<void*> data_list{};
    data_list.resize(s);
    mem_resource.setCountMax(s);
    ASSERT_EQ(0, mem_resource.count()) << "Initialization of the resource failed.";
    ASSERT_EQ(s, mem_resource.countMax()) << "Initialization of the resource failed.";
    for (std::size_t i = 0; i < s; ++i) {
      void* data = mem_resource.allocate(size_max, alignment_max);
      [[maybe_unused]] StorageT* value = ::new(data) StorageT{0};
      data_list[i] = data;
    }
    ASSERT_EQ(s, mem_resource.count()) << "Initialization of the resource failed.";

    // Exception test
    try {
      [[maybe_unused]] void* d = mem_resource.allocate(size_max, alignment_max);
      FAIL() << "The allocation unexpectedly succeeded."; // Never go this line
    }
    catch (const zisc::Memory::BadAllocation& error) {
      std::cout << "## Bad allocation happened expectedly." << std::endl;
      ASSERT_EQ(size_max, error.size());
      ASSERT_EQ(alignment_max, error.alignment());
    }

    for (void* data : data_list)
      mem_resource.deallocate(data, size_max, alignment_max);
    ASSERT_EQ(0, mem_resource.count()) << "Initialization of the resource failed.";

    // Clear test
    for (std::size_t i = 0; i < s; ++i) {
      void* data = mem_resource.allocate(size_max, alignment_max);
      [[maybe_unused]] StorageT* value = ::new(data) StorageT{0};
      data_list[i] = data;
    }
    ASSERT_EQ(s, mem_resource.count()) << "Initialization of the resource failed.";
    mem_resource.clear();
    ASSERT_EQ(0, mem_resource.count()) << "Initialization of the resource failed.";
  }
}

TEST(FixedArrayResourceTest, MemoryAllocationSizeTest)
{
  using StorageT = zisc::uint64b;
  zisc::AllocFreeResource r{};
  zisc::FixedArrayResource<StorageT> mem_resource{&r};

  const std::array<std::size_t, 4> size_list{{1, 2, 4, 8}};
  for (const std::size_t size : size_list) {
    const std::size_t alignment = size;
    void* data = mem_resource.allocate(size, alignment);
    [[maybe_unused]] auto* value = ::new(data) zisc::uint8b{0};
    ASSERT_EQ(1, mem_resource.count()) << "Allocation failed.";

    mem_resource.deallocate(data, size, alignment);
    ASSERT_EQ(0, mem_resource.count()) << "Deallocation failed.";
  }

  // Bad allocation test
  {
    const std::size_t size = 16;
    const std::size_t alignment = size;
    try {
      [[maybe_unused]] void* data = mem_resource.allocate(size, alignment);
      FAIL() << "The allocation unexpectedly succeeded."; // Never go this line
    }
    catch (const zisc::Memory::BadAllocation& error) {
      std::cout << "## Bad allocation happened expectedly." << std::endl;
      ASSERT_EQ(size, error.size());
      ASSERT_EQ(alignment, error.alignment());
    }
    ASSERT_EQ(0, mem_resource.count()) << "Deallocation failed.";
  }
}

TEST(FixedArrayResourceTest, MultiThreadTest)
{
  using StorageT = zisc::uint64b;
  zisc::AllocFreeResource r{};
  zisc::FixedArrayResource<StorageT> mem_resource{&r};

  constexpr std::size_t n_threads = 128;
  constexpr std::size_t loop = 1 << 18;
  mem_resource.setCountMax(n_threads * loop);

  std::atomic_int worker_lock{-1};
  std::vector<std::thread> worker_list{};
  worker_list.reserve(n_threads);
  std::vector<void*> data_list;
  data_list.resize(mem_resource.countMax());

  // Allocation test
  for (std::size_t i = 0; i < n_threads; ++i) {
    worker_list.emplace_back([i, &mem_resource, &data_list, &worker_lock]()
    {
      // Wait the thread until all threads become ready
      worker_lock.wait(-1, std::memory_order::acquire);
      // Do the actual allocation test
      constexpr std::size_t size = sizeof(StorageT);
      constexpr std::size_t alignment = std::alignment_of_v<StorageT>;
      for (std::size_t j = 0; j < loop; ++j) {
        try {
          void* data = mem_resource.allocate(size, alignment);
          const std::size_t index = i * loop + j;
          data_list[index] = data;
        }
        catch ([[maybe_unused]] const zisc::Memory::BadAllocation& error) {
          FAIL() << "Memory allocation failed.";
        }
      }
    });
  }

  // Start the test, notify all threads
  std::cout << "## Starting allocation test." << std::endl;
  worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
  worker_lock.notify_all();

  // Wait the test done
  std::for_each_n(worker_list.begin(), n_threads, [](std::thread& w){w.join();});

  ASSERT_EQ(mem_resource.countMax(), mem_resource.count()) << "Allocation test failed.";
  worker_lock.store(-1, std::memory_order::release);
  worker_list.clear();

  // Deallocation test
  for (std::size_t i = 0; i < n_threads; ++i) {
    worker_list.emplace_back([i, &mem_resource, &data_list, &worker_lock]()
    {
      // Wait the thread until all threads become ready
      worker_lock.wait(-1, std::memory_order::acquire);
      // Do the actual deallocation test
      constexpr std::size_t size = sizeof(StorageT);
      constexpr std::size_t alignment = std::alignment_of_v<StorageT>;
      for (std::size_t j = 0; j < loop; ++j) {
        const std::size_t index = i * loop + j;
        void* data = data_list[index];
        mem_resource.deallocate(data, size, alignment);
      }
    });
  }

  // Start the test, notify all threads
  std::cout << "## Starting deallocation test." << std::endl;
  worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
  worker_lock.notify_all();

  // Wait the test done
  std::for_each_n(worker_list.begin(), n_threads, [](std::thread& w){w.join();});

  ASSERT_EQ(0, mem_resource.count()) << "Deallocation test failed.";
}
