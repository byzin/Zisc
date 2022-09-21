/*!
  \file monotonic_buffer_resource_test.cpp
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
#include <limits>
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
#include "zisc/memory/memory.hpp"
#include "zisc/memory/monotonic_buffer_resource.hpp"
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace {

template <std::size_t kSize, std::size_t kAlighment>
class ResourceCreator
{
 public:
  using MemResource = zisc::MonotonicBufferResource<kSize, kAlighment>;


  //
  ResourceCreator() noexcept
  {
  }

  //
  ~ResourceCreator()
  {
  }

  zisc::pmr::unique_ptr<MemResource> create()
  {
    const zisc::pmr::polymorphic_allocator<MemResource> alloc{&resource_};
    return zisc::pmr::allocateUnique<MemResource>(alloc, &resource_);
  }

 private:
  zisc::AllocFreeResource resource_;
};

} /* namespace */

TEST(MonotonicBufferResourceTest, AllocationTest)
{
  constexpr std::size_t alignment_max = 256;
  constexpr std::size_t capacity_max = 4 * alignment_max;
  using ResourceCreator = ::ResourceCreator<capacity_max, alignment_max>;
  using MemResource = typename ResourceCreator::MemResource;
  ResourceCreator creator;
  zisc::pmr::unique_ptr<MemResource> resource = creator.create();
  ASSERT_LE(alignment_max, resource->alignment()) << "Resource initialization failed.";
  ASSERT_LE(capacity_max, resource->capacity()) << "Resource initialization failed.";
  ASSERT_FALSE(resource->isOccupied()) << "Resource initialization failed.";
  ASSERT_EQ(0, resource->size()) << "Resource initialization failed.";

  constexpr std::size_t n = 3 * (MemResource::capacity() / 8);
  std::array<void*, n> data_list;
  // Allocation test
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t alignment = (i % 3 == 0) ? 1 :
                                  (i % 3 == 1) ? 2
                                               : 4;
    void* data = nullptr;
    try {
      data = resource->allocate(alignment, alignment);
    }
    catch (const MemResource::BadAlloc& error) {
      FAIL() << "Allocation with " << alignment << " alignment is failed."
             << " exception: " << error.what()
             << " size=" << error.size() << ", alignment=" << error.alignment();
    }
    ASSERT_TRUE(zisc::Memory::isAligned(data, alignment))
        << "Allocation with " << alignment << " alignment is failed.";
    data_list[i] = data;
  }
  ASSERT_TRUE(resource->isOccupied()) << "Allocation test failed.";
  ASSERT_EQ(resource->capacity(), resource->size()) << "Allocation test failed.";

  // Bad allocation test
  try {
    [[maybe_unused]] void* data = resource->allocate(1, 1);
    FAIL() << "The allocation unexpectedly successed."; // Never go this line
  }
  catch (const MemResource::BadAlloc& error) {
    std::cout << "## Bad allocation happened expectedly." << std::endl;
  }
  // Deallocation
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t alignment = (i % 3 == 0) ? 1 :
                                  (i % 3 == 1) ? 2
                                               : 4;
    resource->deallocate(data_list[i], alignment, alignment);
  }
  ASSERT_FALSE(resource->isOccupied()) << "Deallocation failed.";

  resource->release();
  ASSERT_FALSE(resource->isOccupied()) << "Releasing the resource failed.";
  ASSERT_EQ(0, resource->size()) << "Releasing the resource failed.";

  resource.reset();
}

TEST(MonotonicBufferResourceTest, AlignmentTest)
{
  constexpr std::size_t alignment_max = 4096;
  constexpr std::size_t capacity_max = 4 * alignment_max;
  using ResourceCreator = ::ResourceCreator<capacity_max, alignment_max>;
  using MemResource = typename ResourceCreator::MemResource;
  ResourceCreator creator;
  zisc::pmr::unique_ptr<MemResource> resource = creator.create();
  ASSERT_LE(alignment_max, resource->alignment()) << "Resource initialization failed.";
  ASSERT_LE(capacity_max, resource->capacity()) << "Resource initialization failed.";
  ASSERT_FALSE(resource->isOccupied()) << "Resource initialization failed.";
  ASSERT_EQ(0, resource->size()) << "Resource initialization failed.";

  for (std::size_t i = 0; i < 13; ++i) {
    const std::size_t alignment = 1 << i;
    ASSERT_LE(alignment, resource->alignment())
        << "The resource alignment isn't enough.";

    using Data = void*;
    std::array<Data, 4> data_list;
    // Allocation
    std::for_each(data_list.begin(), data_list.end(), [alignment, &resource](Data& data)
    {
      try {
        data = resource->allocate(alignment, alignment);
      }
      catch (const MemResource::BadAlloc& error) {
        FAIL() << "Allocation with " << alignment << " alignment is failed."
               << " exception: " << error.what()
               << " size=" << error.size() << ", alignment=" << error.alignment();
      }
      ASSERT_TRUE(zisc::Memory::isAligned(data, alignment))
          << "Allocation with " << alignment << " alignment is failed.";
    });
    ASSERT_TRUE(resource->isOccupied())
        << "Allocation with " << alignment << " alignment is failed.";
    ASSERT_EQ(data_list.size() * alignment, resource->size())
        << "Allocation with " << alignment << " alignment is failed.";
    // Memory access test
    std::for_each(data_list.begin(), data_list.end(), [alignment](Data data)
    {
      const std::span<std::byte> bytes{static_cast<std::byte*>(data), alignment};
      std::for_each(bytes.begin(), bytes.end(), [](std::byte& in)
      {
        in = std::byte{(std::numeric_limits<zisc::uint8b>::max)()};
      });
    });
    // Deallocatino
    std::for_each(data_list.begin(), data_list.end(), [alignment, &resource](Data data)
    {
      resource->deallocate(data, alignment, alignment);
    });
    ASSERT_FALSE(resource->isOccupied()) << "Deallocation failed.";

    resource->release();
    ASSERT_FALSE(resource->isOccupied()) << "Releasing the resource failed.";
    ASSERT_EQ(0, resource->size()) << "Releasing the resource failed.";
  }

  resource.reset();
}

TEST(MonotonicBufferResourceTest, MultiThreadTest)
{
  constexpr std::size_t n_threads = 128;
  constexpr std::size_t loop = 1 << 13;
  constexpr std::size_t alignment_max = 4;
  constexpr std::size_t capacity_max = alignment_max * n_threads * loop;
  using ResourceCreator = ::ResourceCreator<capacity_max, alignment_max>;
  using MemResource = typename ResourceCreator::MemResource;
  ResourceCreator creator;
  zisc::pmr::unique_ptr<MemResource> resource = creator.create();
  ASSERT_LE(alignment_max, resource->alignment()) << "Resource initialization failed.";
  ASSERT_LE(capacity_max, resource->capacity()) << "Resource initialization failed.";
  ASSERT_FALSE(resource->isOccupied()) << "Resource initialization failed.";
  ASSERT_EQ(0, resource->size()) << "Resource initialization failed.";

  std::atomic_int worker_lock{-1};
  std::vector<std::thread> worker_list;
  worker_list.reserve(n_threads);
  std::vector<void*> data_list;
  data_list.resize(n_threads * loop);

  // Allocation test
  for (std::size_t i = 0; i < n_threads; ++i) {
    worker_list.emplace_back([i, &resource, &data_list, &worker_lock]()
    {
      // Wait the thread until all threads become ready
      worker_lock.wait(-1, std::memory_order::acquire);
      // Do the actual test
      const std::uniform_int_distribution<std::size_t> distrib{0, 12};
      for (std::size_t j = 0; j < loop; ++j) {
        const std::size_t index = i * loop + j;
        std::byte* ptr = nullptr;
        try {
          void* data = resource->allocate(alignment_max, alignment_max);
          data_list[index] = data;
          ptr = static_cast<std::byte*>(data);
        }
        catch (const zisc::Memory::BadAlloc& error) {
          FAIL() << "Memory allocation failed. size=" << error.size() << ", alignment=" << error.alignment();
        }
        // Memory access test
        const std::span<std::byte> bytes{ptr, alignment_max};
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

  ASSERT_TRUE(resource->isOccupied()) << "Allocation test failed.";
  ASSERT_EQ(resource->capacity(), resource->size()) << "Allocation test failed.";

  // Deallocation test
  worker_lock.store(-1, std::memory_order::release);
  for (std::size_t i = 0; i < n_threads; ++i) {
    worker_list.emplace_back([i, &resource, &data_list, &worker_lock]()
    {
      // Wait the thread until all threads become ready
      worker_lock.wait(-1, std::memory_order::acquire);
      // Do the actual test
      for (std::size_t j = 0; j < loop; ++j) {
        const std::size_t index = i * loop + j;
        void* data = data_list[index];
        resource->deallocate(data, alignment_max, alignment_max);
      }
    });
  }
  // Start the test, notify all threads
  worker_lock.store(zisc::cast<int>(worker_list.size()), std::memory_order::release);
  worker_lock.notify_all();

  // Wait the test done
  std::for_each_n(worker_list.begin(), n_threads, [](std::thread& w){w.join();});

  ASSERT_FALSE(resource->isOccupied()) << "Deallocation failed.";

  resource->release();
  ASSERT_FALSE(resource->isOccupied()) << "Releasing the resource failed.";
  ASSERT_EQ(0, resource->size()) << "Releasing the resource failed.";

  resource.reset();
}
