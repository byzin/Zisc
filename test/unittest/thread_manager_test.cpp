/*!
  \file thread_manager_test.cpp
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
#include "zisc/math/math.hpp"
#include "zisc/memory/simple_memory_resource.hpp"
#include "zisc/random/correlated_multi_jittered_engine.hpp"
#include "zisc/random/pcg_engine.hpp"
#include "zisc/thread/atomic.hpp"
#include "zisc/thread/future.hpp"
#include "zisc/thread/thread_manager.hpp"

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
  void operator()([[maybe_unused]] const zisc::int64b thread_id)
  {
    value_.store(kGlobalConstant2, std::memory_order::release);
  }
  std::atomic<int> value_{0};
};

struct MovableSetter1 : private zisc::NonCopyable<MovableSetter1>
{
  MovableSetter1()
  {
  }
  MovableSetter1(MovableSetter1&&)
  {
  }
  int operator()() noexcept
  {
    return kGlobalConstant1;
  }
};

struct MovableSetter2 : private zisc::NonCopyable<MovableSetter2>
{
  MovableSetter2()
  {
  }
  MovableSetter2(MovableSetter2&&)
  {
  }
  int operator()([[maybe_unused]] const zisc::int64b thread_id)
  {
    return kGlobalConstant2;
  }
};

} // namespace

TEST(ThreadManagerTest, EnqueueSingleTaskTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    zisc::ThreadManager thread_manager{1, &mem_resource};
    std::size_t id_counter = 0;
    // Function (void)
    {
      // Init
      ::global_value1.store(0, std::memory_order::release);
      ASSERT_FALSE(::global_value1.load(std::memory_order::acquire));
      // Enqueue a task
      const auto func = ::setGlobal1;
      auto result = thread_manager.enqueue(func);
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
      // Check result
      const int value = setter.value_.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant1, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::MovableSetter1 setter{};
      // Enqueue a task
      auto result = thread_manager.enqueue(std::move(setter));
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      const int value = result->get();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
      // Check result
      const int value = setter.value_.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
    {
      // Init
      ::MovableSetter2 setter{};
      // Enqueue a task
      auto result = thread_manager.enqueue(std::move(setter));
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      const int value = result->get();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(::kGlobalConstant2, value) << "Task enqueuing failed.";
    }
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
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
                  [[maybe_unused]] const zisc::int64b thread_id)
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
  MovableAdder1(MovableAdder1&&)
  {
  }
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
  MovableAdder2(MovableAdder2&&)
  {
  }
  int operator()(std::list<int>::const_iterator ite,
                 [[maybe_unused]] const zisc::int64b thread_id)
  {
    global_value2.fetch_add(*ite, std::memory_order::release);
    return 0;
  }
};

} // namespace

TEST(ThreadManagerTest, EnqueueLoopTaskTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    zisc::ThreadManager thread_manager{1, &mem_resource};
    std::size_t id_counter = 0;

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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->get();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->get();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
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
      ASSERT_TRUE(result->valid()) << "Task enqueueing failed.";
      ASSERT_EQ(id_counter++, result->id()) << "Task enqueueing failed.";
      result->wait();
      // Check result
      const int value = ::global_value2.load(std::memory_order::acquire);
      ASSERT_EQ(expected, value) << "Task enqueuing failed.";
    }
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, EnqueueTaskExceptionTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    zisc::ThreadManager thread_manager{1, &mem_resource};
    ASSERT_EQ(1, thread_manager.numOfThreads());
    // enqueueLoop()
    {
      constexpr std::size_t cap = thread_manager.defaultCapacity();
      ASSERT_EQ(cap, thread_manager.capacity());

      auto task = [&thread_manager](const int index, const zisc::int64b /* id */)
      {
        if (index < thread_manager.numOfThreads()) {
          const std::chrono::milliseconds wait_time{512};
          std::this_thread::sleep_for(wait_time);
        }
      };
      constexpr int begin = 0;
      constexpr int end = zisc::cast<int>(2 * cap);
      zisc::SharedFuture<void> result;
      try {
        result = thread_manager.enqueueLoop(task, begin, end);
        result->wait();
        FAIL() << "This line must not be processed.";
      }
      catch (zisc::ThreadManager::OverflowError& error) {
        const int b = zisc::cast<int>(error.beginOffset());
        const int e = zisc::cast<int>(error.numOfIterations());
        ASSERT_LE(zisc::cast<int>(cap), b);
        ASSERT_EQ(end, e);
        auto& task1 = error.task();
        result = task1.getFuture<void>();
        for (auto it = error.beginOffset(); it < error.numOfIterations(); ++it)
          task1.run(zisc::ThreadManager::unmanagedThreadId(), it);
      }
      catch (...) {
        FAIL() << "This line must not be processed.";
      }
      ASSERT_EQ(0, result->id());
      ASSERT_TRUE(result->valid());
      result->wait();
    }
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

namespace {

int dtask1(const zisc::int64b /* id */) noexcept
{
  const std::chrono::milliseconds wait_time{1000};
  std::this_thread::sleep_for(wait_time);
  return 10;
}

} // namespace

TEST(ThreadManagerTest, TaskDependencyTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    constexpr std::size_t nthreads = 64;
    constexpr std::size_t n = 16;
    zisc::ThreadManager thread_manager{nthreads, &mem_resource};
    std::array<std::atomic_int, n> vlist;
    for (auto& value : vlist)
      value.store(0);

    auto result1 = thread_manager.enqueue(::dtask1, zisc::ThreadManager::kAllPrecedences);
    ASSERT_EQ(0, result1->id());

    auto task2 = [&vlist](const std::size_t i, const zisc::int64b /* id */) noexcept
    {
      const std::chrono::milliseconds wait_time{1000};
      std::this_thread::sleep_for(wait_time);
      ++vlist[i];
    };
    constexpr std::size_t begin = 0;
    constexpr std::size_t end = vlist.size();
    auto result2 = thread_manager.enqueueLoop(task2, begin, end, result1->id());
    ASSERT_EQ(1, result2->id());

    auto task3 = [&vlist]() noexcept
    {
      int total = 0;
      for (auto& value : vlist)
        total += value++;
      return total;
    };
    auto result3 = thread_manager.enqueue(task3, result2->id());
    ASSERT_EQ(2, result3->id());

    auto task4 = [&vlist](const std::size_t i) noexcept
    {
      ++vlist[i];
    };
    auto result4 = thread_manager.enqueueLoop(task4, begin, end, result1->id());
    ASSERT_EQ(3, result4->id());

    auto task5 = [&vlist]() noexcept
    {
      int total = 0;
      for (auto& value : vlist)
        total += value.load();
      return total;
    };
    auto result5 = thread_manager.enqueue(task5, result4->id());
    ASSERT_EQ(4, result5->id());

    auto result6 = thread_manager.enqueue(task5, zisc::ThreadManager::kAllPrecedences);
    ASSERT_EQ(5, result6->id());

    ASSERT_EQ(0, task5()) << "Task synchronization failed.";
    thread_manager.waitForCompletion();

    int expected = 3 * zisc::cast<int>(n);
    ASSERT_EQ(expected, task5()) << "Task synchronization failed.";

    expected = 2 * zisc::cast<int>(n);
    ASSERT_EQ(expected, result3->get()) << "Task synchronization failed.";

    expected = zisc::cast<int>(n);
    ASSERT_EQ(expected, result5->get()) << "Task synchronization failed.";

    expected = 3 * zisc::cast<int>(n);
    ASSERT_EQ(expected, result6->get()) << "Task synchronization failed.";
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, ExitWorkerRunningTest)
{
  zisc::SimpleMemoryResource mem_resource;
  constexpr zisc::uint num_of_works = 1024;
  {
    std::vector<typename zisc::SharedFuture<void>> results;
    results.reserve(num_of_works);
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
        results.emplace_back(thread_manager.enqueue(std::move(task)));
      }
    }
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

namespace {

template <std::size_t kBase, std::size_t kMaxLevel, std::size_t kLevel>
void testThreadPoolNest(zisc::ThreadManager& thread_manager,
                        std::vector<double>& results,
                        const std::size_t index) noexcept
{
  static_assert(1 < kBase);
  if constexpr (kLevel < kMaxLevel) {
    auto task1 = [&thread_manager, &results, index]() noexcept
    {
      constexpr std::size_t level = kLevel + 1;
      const std::size_t child_i = kBase * index + 1;
      testThreadPoolNest<kBase, kMaxLevel, level>(thread_manager, results, child_i);
    };

    auto task2 = [&thread_manager, &results, index](const std::size_t i) noexcept
    {
      constexpr std::size_t level = kLevel + 1;
      const std::size_t child_i = kBase * index + (i + 1);
      testThreadPoolNest<kBase, kMaxLevel, level>(thread_manager, results, child_i);
    };

    constexpr std::size_t start = 1;
    const std::size_t end = kBase;
    auto result1 = thread_manager.enqueue(task1);
    auto result2 = thread_manager.enqueueLoop(task2, start, end);

    if (zisc::isOdd(kLevel)) {
      result1->wait();
      result2->wait();
    }
    else {
      result2->wait();
      result1->wait();
    }
  }
  // Process
  constexpr std::size_t num_of_loops = 1'000'000;
  using Cmj = zisc::CmjN16;
  for (std::size_t i = 0; i < num_of_loops; ++i) {
    results[index] += Cmj::generate1d<double>(zisc::cast<zisc::uint32b>(i),
                                              zisc::cast<zisc::uint32b>(index));
  }
}

} // namespace

TEST(ThreadManagerTest, NestedThreadPoolTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    zisc::ThreadManager thread_manager{&mem_resource};
    constexpr std::size_t base = 4;
    constexpr std::size_t max_level = 8;
    std::size_t n = 0;
    for (std::size_t i = 1; i < max_level; ++i) {
      n = base * (n + 1);
    }
    ++n;
    thread_manager.setCapacity(n);
    std::vector<double> results;
    results.resize(n, 0.0);
    ::testThreadPoolNest<base, max_level, 1>(thread_manager, results, 0);
    thread_manager.waitForCompletion();
    for (std::size_t i = 0; i < results.size(); ++i) {
      ASSERT_TRUE(0.0 < results[i]) << "[" << i << "] is 0.0.";
    }
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

namespace {

struct MovableTestValue1 : private zisc::NonCopyable<MovableTestValue1>
{
  MovableTestValue1()
  {
  }
  MovableTestValue1(MovableTestValue1&& other)
  {
    std::swap(flag_, other.flag_);
    const int v = zisc::atomic_load(&other.value_, std::memory_order::acquire);
    zisc::atomic_store(&value_, v, std::memory_order::release);
  }
  ~MovableTestValue1()
  {
    if (flag_)
      zisc::atomic_store(flag_, 1, std::memory_order::release);
  }
  int* flag_ = nullptr;
  int value_ = 0;
  [[maybe_unused]] int padding_ = 0;
};

static_assert(std::is_default_constructible_v<MovableTestValue1>);

struct MovableTestValue2 : private zisc::NonCopyable<MovableTestValue2>
{
  MovableTestValue2()
  {
  }
  MovableTestValue2(MovableTestValue2&& other)
  {
    std::swap(flag_, other.flag_);
    const int v = zisc::atomic_load(&other.value_, std::memory_order::acquire);
    zisc::atomic_store(&value_, v, std::memory_order::release);
  }
  ~MovableTestValue2()
  {
    if (flag_)
      zisc::atomic_store(flag_, 1, std::memory_order::release);
  }
  MovableTestValue2& operator=(MovableTestValue2&& other)
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
  MovableTestValue3(MovableTestValue3&& other)
  {
    std::swap(flag_, other.flag_);
    const int v = zisc::atomic_load(&other.value_, std::memory_order::acquire);
    zisc::atomic_store(&value_, v, std::memory_order::release);
  }
  ~MovableTestValue3()
  {
    if (flag_)
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
  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{1, &mem_resource};

  constexpr int expected = 100;

  {
    int is_task_completed = 0;
    auto task = [&thread_manager, &is_task_completed]()
    {
      auto nested_task = [&is_task_completed]()
      {
        ::MovableTestValue1 value{};
        zisc::atomic_store(&value.value_, expected, std::memory_order::release);
        value.flag_ = std::addressof(is_task_completed);
        return value;
      };
      auto result = thread_manager.enqueue(nested_task);
      return std::move(result->get());
    };
    auto result = thread_manager.enqueue(task);
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_FALSE(flag) << "The result value was already destructed.";
    }
    {
      auto value = std::move(result->get());
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
    auto task = [&thread_manager, &is_task_completed]()
    {
      auto nested_task = [&is_task_completed]()
      {
        ::MovableTestValue2 value{};
        zisc::atomic_store(&value.value_, expected, std::memory_order::release);
        value.flag_ = std::addressof(is_task_completed);
        return value;
      };
      auto result = thread_manager.enqueue(nested_task);
      return std::move(result->get());
    };
    auto result = thread_manager.enqueue(task);
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_FALSE(flag) << "The result value was already destructed.";
    }
    {
      auto value = std::move(result->get());
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
    auto task = [&thread_manager, &is_task_completed]()
    {
      auto nested_task = [&is_task_completed]()
      {
        ::MovableTestValue3 value{is_task_completed, expected};
        return value;
      };
      auto result = thread_manager.enqueue(nested_task);
      return std::move(result->get());
    };
    auto result = thread_manager.enqueue(task);
    {
      const int flag = zisc::atomic_load(&is_task_completed,
                                         std::memory_order::acquire);
      ASSERT_FALSE(flag) << "The result value was already destructed.";
    }
    {
      auto value = std::move(result->get());
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
  zisc::SimpleMemoryResource mem_resource;
  {
    constexpr zisc::uint num_of_threads = 16;
    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setCapacity(1024);

    ASSERT_EQ(num_of_threads, thread_manager.numOfThreads())
        << "Worker creation failed.";

    // Task parallel
    {
      using Task = std::function<void (zisc::int64b)>;
      std::array<zisc::int64b, num_of_threads> id_list;
      for (std::size_t i = 0; i < num_of_threads; ++i)
        id_list[i] = std::numeric_limits<zisc::int64b>::max();
      Task task{[&id_list](const zisc::int64b id)
      {
        const std::chrono::seconds wait_time{1};
        std::this_thread::sleep_for(wait_time);
        id_list[id] = id; 
      }};

      auto result1 = thread_manager.enqueue(task);
      auto result2 = thread_manager.enqueue(task);
      auto result3 = thread_manager.enqueue(task);
      auto result4 = thread_manager.enqueue(task);
      auto result5 = thread_manager.enqueue(task);
      auto result6 = thread_manager.enqueue(task);
      auto result7 = thread_manager.enqueue(task);
      auto result8 = thread_manager.enqueue(task);
      auto result9 = thread_manager.enqueue(Task{task});
      auto result10 = thread_manager.enqueue(Task{task});
      auto result11 = thread_manager.enqueue(Task{task});
      auto result12 = thread_manager.enqueue(Task{task});
      auto result13 = thread_manager.enqueue(Task{task});
      auto result14 = thread_manager.enqueue(Task{task});
      auto result15 = thread_manager.enqueue(Task{task});
      auto result16 = thread_manager.enqueue(Task{task});
      result1->wait();
      result2->wait();
      result3->wait();
      result4->wait();
      result5->wait();
      result6->wait();
      result7->wait();
      result8->wait();
      result9->wait();
      result10->wait();
      result11->wait();
      result12->wait();
      result13->wait();
      result14->wait();
      result15->wait();
      result16->wait();
      for (zisc::int64b i = 0; i < num_of_threads; ++i)
        ASSERT_EQ(i, id_list[i]) << "Task parallel failed.";
    }
    // Loop parallel test1
    {
      using Task = std::function<void (zisc::int64b, zisc::uint)>;
      std::array<zisc::int64b, num_of_threads> id_list;
      for (std::size_t i = 0; i < num_of_threads; ++i)
        id_list[i] = std::numeric_limits<zisc::int64b>::max();
      Task task{[&id_list](const zisc::uint, const zisc::int64b id)
      {
        const std::chrono::seconds wait_time{1};
        std::this_thread::sleep_for(wait_time);
        id_list[id] = id;
      }};

      constexpr zisc::uint start = 0;
      const zisc::uint end = num_of_threads;
      auto result = thread_manager.enqueueLoop(Task{task}, start, end);
      result->wait();
      for (zisc::int64b i = 0; i < num_of_threads; ++i)
        ASSERT_EQ(i, id_list[i]) << "Loop parallel failed.";
    }
    // Loop parallel test2
    {
      using Task = std::function<void (std::list<zisc::uint>::iterator, zisc::int64b)>;
      std::list<zisc::uint> list;
      std::array<zisc::uint, num_of_threads> id_list;
      for (std::size_t i = 0; i < num_of_threads; ++i) {
        id_list[i] = std::numeric_limits<zisc::uint>::max();
        list.emplace_back(i);
      }
      Task task{[&id_list](const std::list<zisc::uint>::iterator number, const zisc::int64b)
      {
        const std::chrono::seconds wait_time{1};
        std::this_thread::sleep_for(wait_time);
        id_list[*number] = *number;
      }};

      auto result = thread_manager.enqueueLoop(task, list.begin(), list.end());
      result->wait();
      for (zisc::uint i = 0; i < num_of_threads; ++i)
        ASSERT_EQ(i, id_list[i]) << "Loop parallel failed.";
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
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, TaskStressTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    constexpr zisc::uint num_of_threads = 1024u;
    constexpr zisc::uint num_of_tasks = 4'000'000;

    std::vector<typename zisc::SharedFuture<void>> result_list;
    result_list.resize(num_of_tasks);

    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setCapacity(num_of_tasks);
    for (zisc::uint number = 0; number < num_of_tasks; ++number) {
      auto task = [number](const zisc::int64b)
      {
        zisc::PcgLcgRxsMXs32 sampler{number};
        const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::pow(1024.0, 2));
        volatile int value = 0;
        for (int i = 0; i < loop; ++i) {
          value = i;
        }
      };
      result_list[number] = thread_manager.enqueue(std::move(task));
    }
    for (std::size_t i = 0; i < result_list.size(); ++i) {
      auto& result = result_list[i];
      ASSERT_EQ(zisc::cast<zisc::int64b>(i), result->id());
      result->wait();
    }
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, TaskStressPerformanceTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    constexpr zisc::uint num_of_threads = 0u;
    constexpr zisc::uint num_of_tasks = 4'000'000;

    std::vector<typename zisc::SharedFuture<void>> result_list;
    result_list.resize(num_of_tasks);

    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setCapacity(num_of_tasks);
    for (zisc::uint number = 0; number < num_of_tasks; ++number) {
      auto task = [number](const zisc::int64b)
      {
        zisc::PcgLcgRxsMXs32 sampler{number};
        const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::pow(1024.0, 2));
        volatile double value = 0;
        for (int i = 0; i < loop; ++i) {
          value = sampler(0.0, 100.0);
        }
      };
      if (number == 0) {
        std::cout << "sizeof(task)  = " << sizeof(decltype(task)) << std::endl;
        std::cout << "alignof(task) = " << std::alignment_of_v<decltype(task)> << std::endl;
      }
      result_list[number] = thread_manager.enqueue(std::move(task));
    }
    for (std::size_t i = 0; i < result_list.size(); ++i) {
      auto& result = result_list[i];
      ASSERT_EQ(zisc::cast<zisc::int64b>(i), result->id());
      result->wait();
    }
    std::cout << "sizeof(result)  = " << sizeof(decltype(result_list[0])) << std::endl;
    std::cout << "alignof(result) = " << std::alignment_of_v<decltype(result_list[0])> << std::endl;
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, LoopTaskStressTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    constexpr zisc::uint num_of_threads = 1024;
    constexpr zisc::uint num_of_tasks = 4'000'000;

    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setCapacity(num_of_tasks);
    auto task = [](const zisc::uint number, const zisc::int64b)
    {
      zisc::PcgLcgRxsMXs32 sampler{number};
      const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::pow(1024.0, 2));
      volatile double value = 0;
      for (int i = 0; i < loop; ++i) {
        value = sampler(0.0, 100.0);
      }
    };
    std::cout << "sizeof(task)  = " << sizeof(decltype(task)) << std::endl;
    std::cout << "alignof(task) = " << std::alignment_of_v<decltype(task)> << std::endl;
    constexpr zisc::uint begin = 0;
    constexpr zisc::uint end = num_of_tasks;
    auto result = thread_manager.enqueueLoop(task, begin, end);
    result->wait();
    std::cout << "sizeof(result)  = " << sizeof(decltype(result)) << std::endl;
    std::cout << "alignof(result) = " << std::alignment_of_v<decltype(result)> << std::endl;
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, LoopTaskStressPerformanceTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    constexpr zisc::uint num_of_threads = 0u;
    constexpr zisc::uint num_of_tasks = 4'000'000;

    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setCapacity(num_of_tasks);
    auto task = [](const zisc::uint number, const zisc::int64b)
    {
      zisc::PcgLcgRxsMXs32 sampler{number};
      const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::pow(1024.0, 2));
      volatile double value = 0;
      for (int i = 0; i < loop; ++i) {
        value = sampler(0.0, 100.0);
      }
    };
    std::cout << "sizeof(task)  = " << sizeof(decltype(task)) << std::endl;
    std::cout << "alignof(task) = " << std::alignment_of_v<decltype(task)> << std::endl;
    constexpr zisc::uint begin = 0;
    constexpr zisc::uint end = num_of_tasks;
    auto result = thread_manager.enqueueLoop(task, begin, end);
    result->wait();
    std::cout << "sizeof(result)  = " << sizeof(decltype(result)) << std::endl;
    std::cout << "alignof(result) = " << std::alignment_of_v<decltype(result)> << std::endl;
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, LoopTaskStressPerformanceTest2)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    static constexpr zisc::uint num_of_threads = 0u;
    static constexpr zisc::uint tasks_per_thread = 1u << 5u;
    static constexpr zisc::uint num_of_tasks = 1u << 25u;
    std::cout << "Tasks per thread: " << tasks_per_thread << std::endl;
    std::cout << "Num of tasks: " << num_of_tasks << std::endl;

    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    std::cout << "Num of threads: " << thread_manager.numOfThreads() << std::endl;
    std::vector<std::array<double, 2>> results;
    results.resize(thread_manager.numOfThreads() * tasks_per_thread);
    auto task =
    [&results](const zisc::uint number, const zisc::int64b id)
    {
      using Cmj = zisc::CmjN16;
      for (zisc::uint i = 0; i < tasks_per_thread; ++i) {
        const zisc::uint index = zisc::cast<zisc::uint>(id) * tasks_per_thread + i;
        results[index] = Cmj::generate2d<double>(i, number);
      }
    };
    std::cout << "sizeof(task)  = " << sizeof(decltype(task)) << std::endl;
    std::cout << "alignof(task) = " << std::alignment_of_v<decltype(task)> << std::endl;
    thread_manager.setCapacity(num_of_tasks);
    constexpr zisc::uint begin = 0;
    constexpr zisc::uint end = num_of_tasks;
    auto result = thread_manager.enqueueLoop(task, begin, end);
    result->wait();
    std::cout << "sizeof(result)  = " << sizeof(decltype(result)) << std::endl;
    std::cout << "alignof(result) = " << std::alignment_of_v<decltype(result)> << std::endl;
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}
