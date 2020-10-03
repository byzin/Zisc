/*!
  \file thread_manager_test.cpp
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
#include <array>
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <list>
#include <iostream>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/math.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/simple_memory_resource.hpp"
#include "zisc/thread_manager.hpp"
#include "zisc/random/correlated_multi_jittered_engine.hpp"
#include "zisc/random/pcg_engine.hpp"

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
    results[index] += Cmj::generate1D<double>(zisc::cast<zisc::uint32b>(i),
                                              zisc::cast<zisc::uint32b>(index));
  }
}

struct TestValue : private zisc::NonCopyable<TestValue>
{
  TestValue(const int value) : value_{value} {}
  TestValue(TestValue&& other) : value_{other.value_} {}
  int value_;
};

} // namespace

TEST(ThreadManagerTest, EnqueueTaskTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    zisc::ThreadManager thread_manager{1, &mem_resource};
    // enqueue()
    {
      auto task1 = []() noexcept
      {
        return 2;
      };
      auto result1 = thread_manager.enqueue(task1);

      auto task2 = [](const zisc::int64b) noexcept
      {
        return 3;
      };
      auto result2 = thread_manager.enqueue(std::move(task2));

      std::function<int ()> task3{task1};
      auto result3 = thread_manager.enqueue(task3);

      std::function<int (zisc::int64b)> task4{task2};
      auto result4 = thread_manager.enqueue(std::move(task4));

      ASSERT_EQ(0, result1->taskId());
      ASSERT_EQ(2, result1->get());
      ASSERT_EQ(1, result2->taskId());
      ASSERT_EQ(3, result2->get());
      ASSERT_EQ(2, result3->taskId());
      ASSERT_EQ(2, result3->get());
      ASSERT_EQ(3, result4->taskId());
      ASSERT_EQ(3, result4->get());
    }
    thread_manager.clear();
    // enqueueLoop()
    {
      auto task1 = [](const int /* index */) {};
      auto result1 = thread_manager.enqueueLoop(task1, 0, 10);

      auto task2 = [](const zisc::int64b /* thread_number */, const int /* index */) {};
      auto result2 = thread_manager.enqueueLoop(std::move(task2), 0, 10);

      std::function<void (int)> task3{task1};
      auto result3 = thread_manager.enqueueLoop(task3, 0, 10);

      std::function<void (zisc::int64b, int)> task4{task2};
      auto result4 = thread_manager.enqueueLoop(std::move(task4), 0, 10);

      ASSERT_EQ(0, result1->taskId());
      result1->wait();
      ASSERT_EQ(1, result2->taskId());
      result2->wait();
      ASSERT_EQ(2, result3->taskId());
      result3->wait();
      ASSERT_EQ(3, result4->taskId());
      result4->wait();
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
      constexpr std::size_t id_max = thread_manager.defaultIdCapacity();
      ASSERT_EQ(id_max, thread_manager.idCapacity());
      constexpr std::size_t cap = thread_manager.defaultItemCapacity();
      ASSERT_EQ(cap, thread_manager.itemCapacity());

      auto task = [&thread_manager](const zisc::int64b /* id */, const int index)
      {
        if (index < thread_manager.numOfThreads()) {
          const std::chrono::milliseconds wait_time{256};
          std::this_thread::sleep_for(wait_time);
        }
      };
      constexpr int begin = 0;
      constexpr int end = zisc::cast<int>(2 * cap);
      zisc::ThreadManager::SharedResult<void> result;
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
        task1.getResult(std::addressof(result));
        for (auto it = error.beginOffset(); it < error.numOfIterations(); ++it)
          task1.run(zisc::ThreadManager::unmanagedThreadId(), it);
      }
      catch (...) {
        FAIL() << "This line must not be processed.";
      }
      ASSERT_EQ(0, result->taskId());
      result->wait();
    }
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

TEST(ThreadManagerTest, ParallelTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    constexpr zisc::uint num_of_threads = 16;
    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setItemCapacity(1024);

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
      Task task{[&id_list](const zisc::int64b id, const zisc::uint)
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
      using Task = std::function<void (zisc::int64b, std::list<zisc::uint>::iterator)>;
      std::list<zisc::uint> list;
      std::array<zisc::uint, num_of_threads> id_list;
      for (std::size_t i = 0; i < num_of_threads; ++i) {
        id_list[i] = std::numeric_limits<zisc::uint>::max();
        list.emplace_back(i);
      }
      Task task{[&id_list](const zisc::int64b, const std::list<zisc::uint>::iterator number)
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
        auto thread_range = thread_manager.calcThreadRange(range, 0);
        ASSERT_EQ(thread_range[0], 0) << "calcThreadRange() is wrong.";
        ASSERT_EQ(thread_range[1], 1) << "calcThreadRange() is wrong.";
        thread_range = thread_manager.calcThreadRange(range, num_of_threads - 1);
        ASSERT_EQ(thread_range[0], num_of_threads - 1) << "calcThreadRange() is wrong.";
        ASSERT_EQ(thread_range[1], num_of_threads) << "calcThreadRange() is wrong.";
      }
      {
        const zisc::uint range = num_of_threads + 1;
        auto thread_range = thread_manager.calcThreadRange(range, 0);
        ASSERT_EQ(thread_range[0], 0) << "calcThreadRange() is wrong.";
        ASSERT_EQ(thread_range[1], 1) << "calcThreadRange() is wrong.";
        thread_range = thread_manager.calcThreadRange(range, num_of_threads - 1);
        ASSERT_EQ(thread_range[0], num_of_threads - 1) << "calcThreadRange() is wrong.";
        ASSERT_EQ(thread_range[1], num_of_threads + 1) << "calcThreadRange() is wrong.";
      }
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

    auto result1 = thread_manager.enqueue(::dtask1);
    ASSERT_EQ(0, result1->taskId());

    auto task2 = [&vlist](const zisc::int64b /* id */, const std::size_t i) noexcept
    {
      const std::chrono::milliseconds wait_time{1000};
      std::this_thread::sleep_for(wait_time);
      ++vlist[i];
    };
    constexpr std::size_t begin = 0;
    constexpr std::size_t end = vlist.size();
    auto result2 = thread_manager.enqueueLoop(task2, begin, end, result1->taskId());
    ASSERT_EQ(1, result2->taskId());

    auto task3 = [&vlist]() noexcept
    {
      int total = 0;
      for (auto& value : vlist)
        total += value++;
      return total;
    };
    auto result3 = thread_manager.enqueue(task3, result2->taskId());
    ASSERT_EQ(2, result3->taskId());

    auto task4 = [&vlist](const std::size_t i) noexcept
    {
      ++vlist[i];
    };
    auto result4 = thread_manager.enqueueLoop(task4, begin, end, result1->taskId());
    ASSERT_EQ(3, result4->taskId());

    auto task5 = [&vlist]() noexcept
    {
      int total = 0;
      for (auto& value : vlist)
        total += value.load();
      return total;
    };
    auto result5 = thread_manager.enqueue(task5, result4->taskId());
    ASSERT_EQ(4, result5->taskId());

    auto result6 = thread_manager.enqueue(task5, zisc::ThreadManager::kAllParentId);
    ASSERT_EQ(5, result6->taskId());

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
  {
    constexpr zisc::uint num_of_works = 1024;
    std::vector<typename zisc::ThreadManager::SharedResult<void>> results;
    results.reserve(num_of_works);
    {
      zisc::ThreadManager thread_manager{24, &mem_resource};
      thread_manager.setIdCapacity(num_of_works);
      ASSERT_EQ(num_of_works, thread_manager.idCapacity());
      thread_manager.setItemCapacity(num_of_works);
      ASSERT_EQ(num_of_works, thread_manager.itemCapacity());
      for (zisc::uint number = 0; number < num_of_works; ++number) {
        auto task = [/* number */](const zisc::int64b)
        {
          const std::chrono::milliseconds wait_time{100};
          std::this_thread::sleep_for(wait_time);
        };
        results.emplace_back(thread_manager.enqueue(task));
      }
    }
  }
  ASSERT_FALSE(mem_resource.totalMemoryUsage())
      << mem_resource.totalMemoryUsage() << " bytes isn't deallocated.";
}

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
    thread_manager.setIdCapacity(n);
    thread_manager.setItemCapacity(n);
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

TEST(ThreadManagerTest, GetValueTest)
{
  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{1, &mem_resource};
  auto task = [&thread_manager]()
  {
    auto nested_task = []()
    {
      ::TestValue value{100};
      return value;
    };
    auto result = thread_manager.enqueue(nested_task);
    return result->get();
  };
  auto result = thread_manager.enqueue(task);
  auto value = result->get();
  ASSERT_EQ(100, value.value_) << "The get value test failed.";
}

TEST(ThreadManagerTest, TaskStressTest)
{
  zisc::SimpleMemoryResource mem_resource;
  {
    constexpr zisc::uint num_of_threads = 1024u;
    constexpr zisc::uint num_of_tasks = 4'000'000;

    std::vector<typename zisc::ThreadManager::SharedResult<void>> result_list;
    result_list.resize(num_of_tasks);

    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setIdCapacity(num_of_tasks);
    thread_manager.setItemCapacity(num_of_tasks);
    for (zisc::uint number = 0; number < num_of_tasks; ++number) {
      auto task = [number](const zisc::int64b)
      {
        zisc::PcgLcgRxsMXs32 sampler{number};
        const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::power<2>(1024.0));
        volatile int value = 0;
        for (int i = 0; i < loop; ++i) {
          value = i;
        }
      };
      result_list[number] = thread_manager.enqueue(task);
    }
    for (std::size_t i = 0; i < result_list.size(); ++i) {
      auto& result = result_list[i];
      ASSERT_EQ(zisc::cast<zisc::int64b>(i), result->taskId());
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

    std::vector<typename zisc::ThreadManager::SharedResult<void>> result_list;
    result_list.resize(num_of_tasks);

    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    thread_manager.setIdCapacity(num_of_tasks);
    thread_manager.setItemCapacity(num_of_tasks);
    for (zisc::uint number = 0; number < num_of_tasks; ++number) {
      auto task = [number](const zisc::int64b)
      {
        zisc::PcgLcgRxsMXs32 sampler{number};
        const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::power<2>(1024.0));
        volatile int value = 0;
        for (int i = 0; i < loop; ++i) {
          value = i;
        }
      };
      if (number == 0) {
        std::cout << "sizeof(task)  = " << sizeof(decltype(task)) << std::endl;
        std::cout << "alignof(task) = " << std::alignment_of_v<decltype(task)> << std::endl;
      }
      result_list[number] = thread_manager.enqueue(task);
    }
    for (std::size_t i = 0; i < result_list.size(); ++i) {
      auto& result = result_list[i];
      ASSERT_EQ(zisc::cast<zisc::int64b>(i), result->taskId());
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
    thread_manager.setItemCapacity(num_of_tasks);
    auto task = [](const zisc::int64b, const zisc::uint number)
    {
      zisc::PcgLcgRxsMXs32 sampler{number};
      const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::power<2>(1024.0));
      volatile int value = 0;
      for (int i = 0; i < loop; ++i) {
        value = i;
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
    thread_manager.setItemCapacity(num_of_tasks);
    auto task = [](const zisc::int64b, const zisc::uint number)
    {
      zisc::PcgLcgRxsMXs32 sampler{number};
      const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::power<2>(1024.0));
      volatile int value = 0;
      for (int i = 0; i < loop; ++i) {
        value = i;
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
    static constexpr zisc::uint tasks_per_thread = 1u << 4u;
    static constexpr zisc::uint num_of_tasks = 1u << 25u;
    std::cout << "Tasks per thread: " << tasks_per_thread << std::endl;
    std::cout << "Num of tasks: " << num_of_tasks << std::endl;

    zisc::ThreadManager thread_manager{num_of_threads, &mem_resource};
    std::cout << "Num of threads: " << thread_manager.numOfThreads() << std::endl;
    std::vector<std::array<double, 2>> results;
    results.resize(thread_manager.numOfThreads() * tasks_per_thread);
    auto task =
    [&results](const zisc::int64b id, const zisc::uint number)
    {
      using Cmj = zisc::CmjN16;
      for (zisc::uint i = 0; i < tasks_per_thread; ++i) {
        const zisc::uint index = zisc::cast<zisc::uint>(id) * tasks_per_thread + i;
        results[index] = Cmj::generate2D<double>(i, number);
      }
    };
    std::cout << "sizeof(task)  = " << sizeof(decltype(task)) << std::endl;
    std::cout << "alignof(task) = " << std::alignment_of_v<decltype(task)> << std::endl;
    thread_manager.setItemCapacity(num_of_tasks);
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
