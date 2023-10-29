/*!
  \file thread_manager_test.cpp
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
#include <chrono>
#include <functional>
#include <future>
#include <list>
#include <iostream>
#include <memory>
#include <numeric>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/concurrency/thread_manager.hpp"
#include "zisc/math/math.hpp"
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/random/correlated_multi_jittered_engine.hpp"
#include "zisc/random/pcg_engine.hpp"

namespace {

std::atomic<int> global_value1{0};
constexpr int kGlobalConstant1 = 10;

void setGlobal1()
{
  global_value1.store(kGlobalConstant1, std::memory_order::release);
}

std::atomic<int> global_value2{0};
constexpr int kGlobalConstant2 = 20;

void setGlobal2([[maybe_unused]] const zisc::int64b thread_id) noexcept
{
  global_value2.store(kGlobalConstant2, std::memory_order::release);
}

struct NonCopyableSetter1 : private zisc::NonCopyable<NonCopyableSetter1>
{
  NonCopyableSetter1()
  {
    value_.store(0, std::memory_order::release);
  }
  void operator()() noexcept
  {
    value_.store(kGlobalConstant1, std::memory_order::release);
  }
  std::atomic<int> value_{0};
};

struct NonCopyableSetter2 : private zisc::NonCopyable<NonCopyableSetter2>
{
  NonCopyableSetter2()
  {
    value_.store(0, std::memory_order::release);
  }
  void operator()([[maybe_unused]] const zisc::int64b thread_id) noexcept
  {
    value_.store(kGlobalConstant2, std::memory_order::release);
  }
  std::atomic<int> value_{0};
};

struct MovableSetter1 : private zisc::NonCopyable<MovableSetter1>
{
  MovableSetter1() = default;
  MovableSetter1([[maybe_unused]] MovableSetter1&& other) noexcept {}
  int operator()() noexcept
  {
    return kGlobalConstant1;
  }
};

struct MovableSetter2 : private zisc::NonCopyable<MovableSetter2>
{
  MovableSetter2() = default;
  MovableSetter2([[maybe_unused]] MovableSetter2&& other) noexcept {}
  int operator()([[maybe_unused]] const zisc::int64b thread_id) noexcept
  {
    return kGlobalConstant2;
  }
};

} // namespace

TEST(ThreadManagerTest, EnqueueSingleTaskTest)
{
  zisc::AllocFreeResource mem_resource;
  {
    zisc::ThreadManager thread_manager{1, &mem_resource};
    // Function (void)
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = ::setGlobal1;
      auto result = thread_manager.enqueue(func);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueue(::setGlobal1);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    // Function (int)
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = ::setGlobal2;
      auto result = thread_manager.enqueue(func);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueue(::setGlobal2);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
    // Function pointer (void)
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = std::addressof(::setGlobal1);
      auto result = thread_manager.enqueue(func);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueue(std::addressof(::setGlobal1));
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    // Function pointer (int)
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = std::addressof(::setGlobal2);
      auto result = thread_manager.enqueue(func);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueue(std::addressof(::setGlobal2));
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
    // Class with operator()(void)
    {
      // Init
      ::NonCopyableSetter1 setter{};
      ASSERT_FALSE(setter.value_.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueue(setter);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = setter.value_.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::MovableSetter1 setter{};
      // Enqueue a task
      auto result = thread_manager.enqueue(std::move(setter));
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      const int value = result.get();
      // Check result
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    // Class with operator()(int)
    {
      // Init
      ::NonCopyableSetter2 setter{};
      ASSERT_FALSE(setter.value_.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueue(setter);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = setter.value_.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::MovableSetter2 setter{};
      // Enqueue a task
      auto result = thread_manager.enqueue(std::move(setter));
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      const int value = result.get();
      // Check result
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
    // Lambda (void)
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = []()
      {
        ::setGlobal1();
      };
      auto result = thread_manager.enqueue(func);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = []()
      {
        ::setGlobal1();
      };
      auto result = thread_manager.enqueue(std::move(func));
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    // Lambda (int)
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = [](const zisc::int64b thread_id)
      {
        ::setGlobal2(thread_id);
      };
      auto result = thread_manager.enqueue(func);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = [](const zisc::int64b thread_id)
      {
        ::setGlobal2(thread_id);
      };
      auto result = thread_manager.enqueue(std::move(func));
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
  }
  ASSERT_EQ(0, mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

namespace {

void addGlobal1(const int value)
{
  global_value1.fetch_add(value, std::memory_order::release);
}

void addGlobal2(std::list<int>::iterator ite,
                [[maybe_unused]] const zisc::int64b thread_id) noexcept
{
  global_value2.fetch_add(*ite, std::memory_order::release);
}

struct NonCopyableAdder1 : private zisc::NonCopyable<NonCopyableAdder1>
{
  NonCopyableAdder1()
  {
    value_.store(0, std::memory_order::release);
  }
  void operator()(const int value) noexcept
  {
    value_.fetch_add(value, std::memory_order::release);
  }
  std::atomic<int> value_{0};
};

struct NonCopyableAdder2 : private zisc::NonCopyable<NonCopyableAdder2>
{
  NonCopyableAdder2()
  {
    value_.store(0, std::memory_order::release);
  }
  void operator()(std::list<int>::iterator ite,
                  [[maybe_unused]] const zisc::int64b thread_id) noexcept
  {
    value_.fetch_add(*ite, std::memory_order::release);
  }
  std::atomic<int> value_{0};
};

struct MovableAdder1 : private zisc::NonCopyable<MovableAdder1>
{
  MovableAdder1()
  {
    global_value1.store(0, std::memory_order::release);
  }
  MovableAdder1([[maybe_unused]] MovableAdder1&& other) noexcept {}
  int operator()(const int value) noexcept
  {
    global_value1.fetch_add(value, std::memory_order::release);
    return 0;
  }
};

struct MovableAdder2 : private zisc::NonCopyable<MovableAdder2>
{
  MovableAdder2()
  {
    global_value2.store(0, std::memory_order::release);
  }
  MovableAdder2([[maybe_unused]] MovableAdder2&& other) noexcept {}
  int operator()(std::list<int>::const_iterator ite,
                 [[maybe_unused]] const zisc::int64b thread_id) noexcept
  {
    global_value2.fetch_add(*ite, std::memory_order::release);
    return 0;
  }
};

} // namespace

TEST(ThreadManagerTest, EnqueueLoopTaskTest)
{
  zisc::AllocFreeResource mem_resource;
  {
    zisc::ThreadManager thread_manager{1, &mem_resource};

    constexpr int begin = 0;
    constexpr int end = 10;
    constexpr auto sum = []() noexcept
    {
      int s = 0;
      for (int i = begin; i < end; ++i)
        s += i;
      return s;
    };
    constexpr int expected = sum();

    // Function (int)
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = ::addGlobal1;
      auto result = thread_manager.enqueueLoop(func, begin, end);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueueLoop(::addGlobal1, begin, end);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    // Function (int, int)
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      std::list<int> l{};
      l.resize(zisc::cast<std::size_t>(end - begin), 0);
      std::iota(l.begin(), l.end(), begin);
      const auto func = ::addGlobal2;
      auto result = thread_manager.enqueueLoop(func, l.begin(), l.end());
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      std::list<int> l{};
      l.resize(zisc::cast<std::size_t>(end - begin), 0);
      std::iota(l.begin(), l.end(), begin);
      auto result = thread_manager.enqueueLoop(::addGlobal2, l.begin(), l.end());
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    // Function pointer (int)
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = std::addressof(::addGlobal1);
      auto result = thread_manager.enqueueLoop(func, begin, end);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueueLoop(std::addressof(::addGlobal1), begin, end);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    // Function pointer (int, int)
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      std::list<int> l{};
      l.resize(zisc::cast<std::size_t>(end - begin), 0);
      std::iota(l.begin(), l.end(), begin);
      const auto func = std::addressof(::addGlobal2);
      auto result = thread_manager.enqueueLoop(func, l.begin(), l.end());
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      std::list<int> l{};
      l.resize(zisc::cast<std::size_t>(end - begin), 0);
      std::iota(l.begin(), l.end(), begin);
      auto result = thread_manager.enqueueLoop(std::addressof(::addGlobal2), l.begin(), l.end());
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    // Class with operator()(int)
    {
      // Init
      ::NonCopyableAdder1 adder{};
      ASSERT_FALSE(adder.value_.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueueLoop(adder, begin, end);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = adder.value_.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::MovableAdder1 adder{};
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      auto result = thread_manager.enqueueLoop(std::move(adder), begin, end);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.get();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    // Class with operator()(int, int)
    {
      // Init
      ::NonCopyableAdder2 adder{};
      ASSERT_FALSE(adder.value_.load(std::memory_order::acquire));
      // Enqueue a task
      std::list<int> l{};
      l.resize(zisc::cast<std::size_t>(end - begin), 0);
      std::iota(l.begin(), l.end(), begin);
      auto result = thread_manager.enqueueLoop(adder, l.begin(), l.end());
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = adder.value_.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::MovableAdder2 adder{};
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      std::list<int> l{};
      l.resize(zisc::cast<std::size_t>(end - begin), 0);
      std::iota(l.begin(), l.end(), begin);
      auto result = thread_manager.enqueueLoop(std::move(adder), l.begin(), l.end());
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.get();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    // Lambda (int)
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = [](const int value)
      {
        ::addGlobal1(value);
      };
      auto result = thread_manager.enqueueLoop(func, begin, end);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = [](const int value)
      {
        ::addGlobal1(value);
      };
      auto result = thread_manager.enqueueLoop(std::move(func), begin, end);
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value1.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    // Lambda (int, int)
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = [](std::list<int>::iterator ite,
                           const zisc::int64b thread_id)
      {
        ::addGlobal2(ite, thread_id);
      };
      std::list<int> l{};
      l.resize(zisc::cast<std::size_t>(end - begin), 0);
      std::iota(l.begin(), l.end(), begin);
      auto result = thread_manager.enqueueLoop(func, l.begin(), l.end());
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::global_value2.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value2.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = [](std::list<int>::iterator ite,
                           const zisc::int64b thread_id)
      {
        ::addGlobal2(ite, thread_id);
      };
      std::list<int> l{};
      l.resize(zisc::cast<std::size_t>(end - begin), 0);
      std::iota(l.begin(), l.end(), begin);
      auto result = thread_manager.enqueueLoop(std::move(func), l.begin(), l.end());
      ASSERT_TRUE(result.valid()) << "Task enqueueing failed.";
      result.wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
  }
  ASSERT_EQ(0, mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, EnqueueTaskExceptionTest)
{
  zisc::AllocFreeResource mem_resource;
  {
    zisc::ThreadManager thread_manager{1, &mem_resource};
    ASSERT_EQ(1, thread_manager.numOfThreads());
    // enqueueLoop()
    {
      constexpr std::size_t cap = thread_manager.defaultCapacity();
      ASSERT_EQ(cap, thread_manager.capacity());
      std::cout << "## Default task capacity: " << cap << std::endl;

      std::atomic_int worker_lock{-1};
      auto task = [&worker_lock](const int /* index */, const zisc::int64b /* id */)
      {
        // Wiat this thread until all threads become ready
        worker_lock.wait(-1, std::memory_order::acquire);
      };
      constexpr int begin = 0;
      constexpr int end = zisc::cast<int>(2 * cap);
      std::future<void> result;
      try {
        result = thread_manager.enqueueLoop(task, begin, end);
        result.wait();
        FAIL() << "This line must not be processed.";
      }
      catch (zisc::ThreadManager::OverflowError& error) {
        worker_lock.store(1, std::memory_order::release);
        worker_lock.notify_all();
        //
        const int b = zisc::cast<int>(error.get().beginOffset());
        const int e = zisc::cast<int>(error.get().numOfIterations());
        ASSERT_LE(zisc::cast<int>(cap), b);
        ASSERT_EQ(end, e);
        auto& task1 = error.get().task<void>();
        result = task1.getFuture();
        for (auto it = b; it < e; ++it)
          task1.run(zisc::ThreadManager::unmanagedThreadId(), it);
      }
      catch (...) {
        FAIL() << "This line must not be processed.";
      }
      ASSERT_TRUE(result.valid());
      result.wait();
    }
  }
  ASSERT_EQ(0, mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, EnqueueAlignedValueTest)
{
  zisc::AllocFreeResource mem_resource;
  zisc::ThreadManager thread_manager{1, &mem_resource};
  {
    constexpr std::size_t alignment = zisc::Config::l1CacheLineSize();
    struct alignas(alignment) Value1
    {
      int value_;
    };
    static_assert(std::alignment_of_v<Value1> == alignment);

    const Value1 v1 = {1};
    auto task1 = [v1]()
    {
      return Value1{2 * v1.value_};
    };

    std::future<Value1> result = thread_manager.enqueue(std::move(task1));
    const Value1 r1 = result.get();
    ASSERT_EQ(2 * v1.value_, r1.value_) << "The aligned value test failed.";
  }
}

namespace {

int dtask1(const zisc::int64b /* id */) noexcept
{
  return 10;
}

} // namespace

TEST(ThreadManagerTest, TaskDependencyTest)
{
  zisc::AllocFreeResource mem_resource;
  {
    constexpr std::size_t nthreads = 256;
    constexpr std::size_t n = 32;
    zisc::ThreadManager thread_manager{nthreads, &mem_resource};
    std::array<std::atomic_int, n> vlist;
    std::for_each(vlist.begin(), vlist.end(), [](std::atomic_int& value)
    {
      value.store(0, std::memory_order::release);
    });

    std::atomic_int worker_lock1{-1};
    std::atomic_int worker_lock2{-1};

    std::future result1 = thread_manager.enqueue(::dtask1, true);

    auto task2 = [&vlist, &worker_lock1](const std::size_t i,
                                         const zisc::int64b /* id */) noexcept
    {
      worker_lock1.wait(-1, std::memory_order::acquire);
      vlist[i].fetch_add(1, std::memory_order::release);
    };
    constexpr std::size_t begin = 0;
    constexpr std::size_t end = vlist.size();
    const std::future result2 = thread_manager.enqueueLoop(task2, begin, end, true);

    auto task3 = [&vlist]() noexcept
    {
      int total = 0;
      std::for_each(vlist.begin(), vlist.end(), [&total](std::atomic_int& value)
      {
        total += value.fetch_add(1, std::memory_order::acq_rel);
      });
      return total;
    };
    std::future result3 = thread_manager.enqueue(task3, true);

    auto task4 = [&vlist, &worker_lock2](const std::size_t i) noexcept
    {
      worker_lock2.wait(-1, std::memory_order::acquire);
      vlist[i].fetch_add(1, std::memory_order::release);
    };
    const std::future result4 = thread_manager.enqueueLoop(task4, begin, end);

    auto task5 = [&vlist, &result4]() noexcept
    {
      result4.wait();
      int total = 0;
      std::for_each(vlist.begin(), vlist.end(), [&total](std::atomic_int& value)
      {
        total += value.load(std::memory_order::acquire);
      });
      return total;
    };
    std::future result5 = thread_manager.enqueue(task5);
    std::future result6 = thread_manager.enqueue(task5, true);

    // Any task (except task1) shouldn't be completed
    auto task5d = [&vlist]() noexcept
    {
      int total = 0;
      std::for_each(vlist.begin(), vlist.end(), [&total](std::atomic_int& value)
      {
        total += value.load(std::memory_order::acquire);
      });
      return total;
    };
    EXPECT_EQ(0, task5d()) << "Task synchronization failed.";

    ASSERT_EQ(10, result1.get()) << "First task failed.";

    worker_lock2.store(1, std::memory_order::release);
    worker_lock2.notify_all();
    int expected = zisc::cast<int>(n);
    ASSERT_EQ(expected, result5.get()) << "Task synchronization failed.";

    worker_lock1.store(1, std::memory_order::release);
    worker_lock1.notify_all();
    expected = 2 * zisc::cast<int>(n);
    result2.wait();
    ASSERT_EQ(expected, result3.get()) << "Task synchronization failed.";

    expected = 3 * zisc::cast<int>(n);
    ASSERT_EQ(expected, result6.get()) << "Task synchronization failed.";

    thread_manager.waitForCompletion();
    EXPECT_TRUE(thread_manager.isEmpty()) << "The thread manager still has some task.";

    expected = 3 * zisc::cast<int>(n);
    ASSERT_EQ(expected, task5()) << "Task synchronization failed.";
  }
  ASSERT_EQ(0, mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, ExitWorkerRunningTest)
{
  zisc::AllocFreeResource mem_resource;
  constexpr zisc::uint num_of_works = 1024;
  {
    zisc::ThreadManager thread_manager{24, &mem_resource};
    thread_manager.setCapacity(num_of_works);
    ASSERT_EQ(num_of_works, thread_manager.capacity());
    for (zisc::uint number = 0; number < num_of_works; ++number) {
      auto task = [](const zisc::int64b)
      {
        const std::chrono::milliseconds wait_time{256};
        std::this_thread::sleep_for(wait_time);
      };
      [[maybe_unused]] const std::future<void> r = thread_manager.enqueue(std::move(task));
    }
  }
  ASSERT_EQ(0, mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

namespace {

struct MovableTestValue1 : private zisc::NonCopyable<MovableTestValue1>
{
  MovableTestValue1() = default;
  MovableTestValue1(MovableTestValue1&& other) noexcept
  {
    std::swap(flag_, other.flag_);
    const int v = zisc::atomic_load(&other.value_, std::memory_order::acquire);
    zisc::atomic_store(&value_, v, std::memory_order::release);
  }
  ~MovableTestValue1()
  {
    if (flag_ != nullptr)
      zisc::atomic_store(flag_, 1, std::memory_order::release);
  }
  MovableTestValue1& operator=(MovableTestValue1&& other) noexcept
  {
    std::swap(flag_, other.flag_);
    const int v = zisc::atomic_load(&other.value_, std::memory_order::acquire);
    zisc::atomic_store(&value_, v, std::memory_order::release);
    return *this;
  }
  int* flag_ = nullptr;
  int value_ = 0;
  [[maybe_unused]] int padding_ = 0;
};

static_assert(std::is_default_constructible_v<MovableTestValue1>);

struct MovableTestValue2 : private zisc::NonCopyable<MovableTestValue2>
{
  MovableTestValue2() = default;
  MovableTestValue2(MovableTestValue2&& other) noexcept
  {
    std::swap(flag_, other.flag_);
    const int v = zisc::atomic_load(&other.value_, std::memory_order::acquire);
    zisc::atomic_store(&value_, v, std::memory_order::release);
  }
  ~MovableTestValue2()
  {
    if (flag_ != nullptr)
      zisc::atomic_store(flag_, 1, std::memory_order::release);
  }
  MovableTestValue2& operator=(MovableTestValue2&& other) noexcept
  {
    std::swap(flag_, other.flag_);
    const int v = zisc::atomic_load(&other.value_, std::memory_order::acquire);
    zisc::atomic_store(&value_, v, std::memory_order::release);
    return *this;
  }
  int* flag_ = nullptr;
  int value_ = 0;
  [[maybe_unused]] int padding_ = 0;
};

static_assert(std::is_default_constructible_v<MovableTestValue2>);

struct MovableTestValue3 : private zisc::NonCopyable<MovableTestValue3>
{
  MovableTestValue3(int& flag, const int value) : flag_{&flag}, value_{value}
  {
  }
  MovableTestValue3(MovableTestValue3&& other) noexcept
  {
    std::swap(flag_, other.flag_);
    const int v = zisc::atomic_load(&other.value_, std::memory_order::acquire);
    zisc::atomic_store(&value_, v, std::memory_order::release);
  }
  ~MovableTestValue3()
  {
    if (flag_ != nullptr)
      zisc::atomic_store(flag_, 1, std::memory_order::release);
  }
  int* flag_ = nullptr;
  int value_ = 0;
  [[maybe_unused]] int padding_ = 0;
};

static_assert(!std::is_default_constructible_v<MovableTestValue3>);

} // namespace

TEST(ThreadManagerTest, GetValueTest)
{
  zisc::AllocFreeResource mem_resource;
  zisc::ThreadManager thread_manager{1, &mem_resource};

  constexpr int expected = 100;

  {
    int is_task_completed = 0;
    auto task = [&is_task_completed]()
    {
      ::MovableTestValue1 value{};
      zisc::atomic_store(&value.value_, expected, std::memory_order::release);
      value.flag_ = std::addressof(is_task_completed);
      return value;
    };
    auto result = thread_manager.enqueue(task);
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_FALSE(flag) << "The result value was already destructed.";
    }
    {
      const ::MovableTestValue1 value = result.get();
      const int v = zisc::atomic_load(&value.value_, std::memory_order::acquire);
      ASSERT_EQ(expected, v) << "The get value test failed.";
    }
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_TRUE(flag) << "The result value wasn't destructed.";
    }
  }

  {
    int is_task_completed = 0;
    auto task = [&is_task_completed]()
    {
      ::MovableTestValue2 value{};
      zisc::atomic_store(&value.value_, expected, std::memory_order::release);
      value.flag_ = std::addressof(is_task_completed);
      return value;
    };
    auto result = thread_manager.enqueue(task);
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_FALSE(flag) << "The result value was already destructed.";
    }
    {
      const ::MovableTestValue2 value = result.get();
      const int v = zisc::atomic_load(&value.value_, std::memory_order::acquire);
      ASSERT_EQ(expected, v) << "The get value test failed.";
    }
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_TRUE(flag) << "The result value wasn't destructed.";
    }
  }

  {
    int is_task_completed = 0;
    auto task = [&is_task_completed]()
    {
      ::MovableTestValue3 value{is_task_completed, expected};
      return value;
    };
    auto result = thread_manager.enqueue(task);
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_FALSE(flag) << "The result value was already destructed.";
    }
    {
      const ::MovableTestValue3 value = result.get();
      const int v = zisc::atomic_load(&value.value_, std::memory_order::acquire);
      ASSERT_EQ(expected, v) << "The get value test failed.";
    }
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_TRUE(flag) << "The result value wasn't destructed.";
    }
  }
}

TEST(ThreadManagerTest, ParallelTest)
{
  zisc::AllocFreeResource mem_resource;
  {
    constexpr zisc::uint num_of_threads = 16;
    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setCapacity(1024);

    ASSERT_EQ(num_of_threads, thread_manager.numOfThreads())
        << "Worker creation failed.";

    constexpr std::chrono::milliseconds wait_time{256};
    // Task parallel
    {
      using Task = std::function<void (zisc::int64b)>;
      using Future = std::future<void>;

      std::array<zisc::int64b, num_of_threads> id_list{};
      std::for_each(id_list.begin(), id_list.end(), [](zisc::int64b& value)
      {
        value = std::numeric_limits<zisc::int64b>::max();
      });

      std::atomic_int worker_lock{-1};
      Task task{[&id_list, &worker_lock](const zisc::int64b id)
      {
        worker_lock.wait(-1, std::memory_order::acquire);
        id_list[id] = id; 
      }};

      std::array<std::future<void>, num_of_threads> result_list;
      std::for_each(result_list.begin(), result_list.end(),
      [&thread_manager, &task](Future& result)
      {
        result = thread_manager.enqueue(task);
      });

      std::this_thread::sleep_for(wait_time);
      worker_lock.store(1, std::memory_order::release);
      worker_lock.notify_all();
      std::for_each(result_list.begin(), result_list.end(), [](Future& result)
      {
        result.wait();
      });

      for (zisc::int64b i = 0; i < num_of_threads; ++i) {
        ASSERT_EQ(i, id_list[i]) << "Task parallel failed.";
      }
    }
    // Loop parallel test1
    {
      using Task = std::function<void (zisc::int64b, zisc::uint)>;

      std::array<zisc::int64b, num_of_threads> id_list{};
      std::for_each(id_list.begin(), id_list.end(), [](zisc::int64b& value)
      {
        value = std::numeric_limits<zisc::int64b>::max();
      });

      std::atomic_int worker_lock{-1};
      const Task task{[&id_list, &worker_lock](const zisc::uint, const zisc::int64b id)
      {
        worker_lock.wait(-1, std::memory_order::acquire);
        id_list[id] = id;
      }};

      constexpr zisc::uint start = 0;
      const zisc::uint end = num_of_threads;
      auto result = thread_manager.enqueueLoop(Task{task}, start, end);

      std::this_thread::sleep_for(wait_time);
      worker_lock.store(1, std::memory_order::release);
      worker_lock.notify_all();
      result.wait();

      for (zisc::int64b i = 0; i < num_of_threads; ++i) {
        ASSERT_EQ(i, id_list[i]) << "Loop parallel failed.";
      }
    }
    // Loop parallel test2
    {
      using Task = std::function<void (std::list<zisc::uint>::iterator, zisc::int64b)>;

      std::list<zisc::uint> list;
      std::array<zisc::uint, num_of_threads> id_list{};
      for (std::size_t i = 0; i < num_of_threads; ++i) {
        id_list[i] = std::numeric_limits<zisc::uint>::max();
        list.emplace_back(i);
      }

      std::atomic_int worker_lock{-1};
      Task task{[&id_list, &worker_lock](const std::list<zisc::uint>::iterator number, const zisc::int64b)
      {
        worker_lock.wait(-1, std::memory_order::acquire);
        id_list[*number] = *number;
      }};

      auto result = thread_manager.enqueueLoop(task, list.begin(), list.end());

      std::this_thread::sleep_for(wait_time);
      worker_lock.store(1, std::memory_order::release);
      worker_lock.notify_all();
      result.wait();

      for (zisc::uint i = 0; i < num_of_threads; ++i) {
        ASSERT_EQ(i, id_list[i]) << "Loop parallel failed.";
      }
    }
    // Thread range computation
    {
      {
        const zisc::uint range = num_of_threads;
        auto thread_range = zisc::divideRange(range, num_of_threads, 0);
        ASSERT_EQ(thread_range[0], 0) << "calcThreadRange() is wrong.";
        ASSERT_EQ(thread_range[1], 1) << "calcThreadRange() is wrong.";
        thread_range = zisc::divideRange(range, num_of_threads, num_of_threads - 1);
        ASSERT_EQ(thread_range[0], num_of_threads - 1) << "calcThreadRange() is wrong.";
        ASSERT_EQ(thread_range[1], num_of_threads) << "calcThreadRange() is wrong.";
      }
      {
        const zisc::uint range = num_of_threads + 1;
        auto thread_range = zisc::divideRange(range, num_of_threads, 0);
        ASSERT_EQ(thread_range[0], 0) << "calcThreadRange() is wrong.";
        ASSERT_EQ(thread_range[1], 1) << "calcThreadRange() is wrong.";
        thread_range = zisc::divideRange(range, num_of_threads, num_of_threads - 1);
        ASSERT_EQ(thread_range[0], num_of_threads - 1) << "calcThreadRange() is wrong.";
        ASSERT_EQ(thread_range[1], num_of_threads + 1) << "calcThreadRange() is wrong.";
      }
    }
  }
  ASSERT_EQ(0, mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

namespace {

void testThreadManagerThroughput(const std::size_t num_of_samples,
                                 const std::size_t num_of_rounds,
                                 const zisc::int64b trial_time,
                                 const bool has_task_dependencies,
                                 zisc::ThreadManager* thread_manager)
{
  thread_manager->setCapacity(num_of_samples);
  ASSERT_LE(num_of_samples, thread_manager->capacity()) << "Setting capacity failed.";
  ASSERT_TRUE(thread_manager->isEmpty()) << "The thread manager has some task.";

  constexpr std::size_t chunk_size = 1 << 8;
  constexpr std::size_t batch_size = 1 << 6;

  double average_throughput = 0.0;
  for (std::size_t round = 0; round < num_of_rounds; ++round) {
    std::vector<int> value_list;
    value_list.resize(num_of_samples, 0);
    std::atomic_size_t num_of_ops{0};
    auto task = [&value_list, &num_of_ops](const std::size_t /* it */) noexcept
    {
      for (std::size_t i = 0; i < batch_size; ++i) {
        const std::size_t index = num_of_ops.fetch_add(1, std::memory_order::acq_rel);
        value_list[index] = 1;
      }
    };

    // Do throughput test
    ASSERT_EQ(0, num_of_samples % (chunk_size * batch_size));

    using Clock = std::chrono::system_clock;
    const auto start_time = Clock::now();
    for (std::size_t i = 0; i < (num_of_samples / (chunk_size * batch_size)); ++i) {
      if ((i % 100) == 99) {
        const auto current_time = Clock::now();
        if (std::chrono::milliseconds{trial_time} < (current_time - start_time))
          break;
      }
      constexpr std::size_t begin = 0;
      constexpr std::size_t end = chunk_size;
      [[maybe_unused]] const std::future<void> r = thread_manager->enqueueLoop(
          task,
          begin,
          end,
          has_task_dependencies);
    }
    thread_manager->waitForCompletion();
    const auto end_time = Clock::now();

    // Calc throughput
    auto calc_mop = [num_of_samples](const Clock::time_point s,
                                     const Clock::time_point e,
                                     const std::atomic_size_t& num_of_ops) noexcept
    {
      using microsec = std::chrono::microseconds;
      const auto elapsed_time = std::chrono::duration_cast<microsec>(e - s);
      const std::size_t total_op = num_of_ops.load(std::memory_order::acquire);
      const double throughput = zisc::cast<double>(total_op) /
                                zisc::cast<double>(elapsed_time.count());
      if (total_op == num_of_samples) {
        std::cout << "  [Warning] out of samples, finished in "
                  << (elapsed_time.count() / 1000) << " ms." << std::endl;
      }
      return throughput;
    };
    {
      const double throughput = calc_mop(start_time, end_time, num_of_ops);
      std::cout << "  [" << round << "] throughput=" << std::scientific << throughput
                << " (Mop/s)." << std::endl;
      average_throughput += throughput;
    }

    thread_manager->clear();
    ASSERT_TRUE(thread_manager->isEmpty()) << "The thread manager has some task.";
  }
  average_throughput /= zisc::cast<double>(num_of_rounds);
  std::cout << "  avg throughput=" << std::scientific << average_throughput
            << " (Mop/s)." << std::endl;
}

} /* namespace */

TEST(ThreadManagerTest, ConcurrentThroughputTest1)
{
  zisc::AllocFreeResource mem_resource;
  auto thread_manager = std::make_unique<zisc::ThreadManager>(&mem_resource);

  constexpr std::size_t num_of_samples = 1 << 24;
  constexpr std::size_t num_of_rounds = 4;
  constexpr zisc::int64b trial_time = 2'500; // milli seconds

  ::testThreadManagerThroughput(num_of_samples,
                                num_of_rounds,
                                trial_time,
                                false,
                                thread_manager.get());
}

TEST(ThreadManagerTest, ConcurrentThroughputTest2)
{
  zisc::AllocFreeResource mem_resource;
  auto thread_manager = std::make_unique<zisc::ThreadManager>(&mem_resource);

  constexpr std::size_t num_of_samples = 1 << 24;
  constexpr std::size_t num_of_rounds = 4;
  constexpr zisc::int64b trial_time = 2'500; // milli seconds

  ::testThreadManagerThroughput(num_of_samples,
                                num_of_rounds,
                                trial_time,
                                true,
                                thread_manager.get());
}
