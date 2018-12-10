/*!
  \file thread_manager_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <chrono>
#include <functional>
#include <future>
#include <list>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/math.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/thread_manager.hpp"
#include "zisc/pcg_engine.hpp"

TEST(ThreadManagerTest, EnqueueTaskTest)
{
  zisc::ThreadManager thread_manager{1};

  // enqueue()
  {
    auto task1 = []() noexcept
    {
      return 2;
    };
    auto result1 = thread_manager.enqueue<int>(task1);

    auto task2 = [](const zisc::uint) noexcept
    {
      return 3;
    };
    auto result2 = thread_manager.enqueue<int>(task2);

    std::function<int ()> task3{task1};
    auto result3 = thread_manager.enqueue<int>(std::move(task3));

    std::function<int (zisc::uint)> task4{task2};
    auto result4 = thread_manager.enqueue<int>(std::move(task4));

    ASSERT_EQ(2, result1->get());
    ASSERT_EQ(3, result2->get());
    ASSERT_EQ(2, result3->get());
    ASSERT_EQ(3, result4->get());
  }
  // enqueueLoop()
  {
    auto task1 = [](const int /* index */) {};
    auto result1 = thread_manager.enqueueLoop(task1, 0, 10);

    auto task2 = [](const zisc::uint /* thread_number */, const int /* index */) {};
    auto result2 = thread_manager.enqueueLoop(task2, 0, 10);

    std::function<void (int)> task3{task1};
    auto result3 = thread_manager.enqueueLoop(std::move(task3), 0, 10);

    std::function<void (zisc::uint, int)> task4{task2};
    auto result4 = thread_manager.enqueueLoop(std::move(task4), 0, 10);

    result1->wait();
    result2->wait();
    result3->wait();
    result4->wait();
  }
}

TEST(ThreadManagerTest, ParallelTest)
{
  constexpr zisc::uint num_of_threads = 16;
  zisc::ThreadManager thread_manager{num_of_threads};

  ASSERT_EQ(num_of_threads, thread_manager.numOfThreads())
      << "Worker creation failed.";

  // Task parallel
  {
    using Task = std::function<void (zisc::uint)>;
    std::array<zisc::uint, num_of_threads> id_list;
    for (std::size_t i = 0; i < num_of_threads; ++i)
      id_list[i] = std::numeric_limits<zisc::uint>::max();
    Task task{[&id_list](const zisc::uint id)
    {
      const std::chrono::seconds wait_time{1};
      std::this_thread::sleep_for(wait_time);
      id_list[id] = id; 
    }};

    auto result1 = thread_manager.enqueue<void>(Task{task});
    auto result2 = thread_manager.enqueue<void>(Task{task});
    auto result3 = thread_manager.enqueue<void>(Task{task});
    auto result4 = thread_manager.enqueue<void>(Task{task});
    auto result5 = thread_manager.enqueue<void>(Task{task});
    auto result6 = thread_manager.enqueue<void>(Task{task});
    auto result7 = thread_manager.enqueue<void>(Task{task});
    auto result8 = thread_manager.enqueue<void>(Task{task});
    auto result9 = thread_manager.enqueue<void>(Task{task});
    auto result10 = thread_manager.enqueue<void>(Task{task});
    auto result11 = thread_manager.enqueue<void>(Task{task});
    auto result12 = thread_manager.enqueue<void>(Task{task});
    auto result13 = thread_manager.enqueue<void>(Task{task});
    auto result14 = thread_manager.enqueue<void>(Task{task});
    auto result15 = thread_manager.enqueue<void>(Task{task});
    auto result16 = thread_manager.enqueue<void>(Task{task});
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
    for (zisc::uint i = 0; i < num_of_threads; ++i)
      ASSERT_EQ(i, id_list[i]) << "Task parallel failed.";
  }
  // Loop parallel test1
  {
    using Task = std::function<void (zisc::uint, zisc::uint)>;
    std::array<zisc::uint, num_of_threads> id_list;
    for (std::size_t i = 0; i < num_of_threads; ++i)
      id_list[i] = std::numeric_limits<zisc::uint>::max();
    Task task{[&id_list](const zisc::uint id, const zisc::uint)
    {
      const std::chrono::seconds wait_time{1};
      std::this_thread::sleep_for(wait_time);
      id_list[id] = id;
    }};

    constexpr zisc::uint start = 0;
    const zisc::uint end = num_of_threads;
    auto result = thread_manager.enqueueLoop(Task{task}, start, end);
    result->wait();
    for (zisc::uint i = 0; i < num_of_threads; ++i)
      ASSERT_EQ(i, id_list[i]) << "Loop parallel failed.";
  }
  // Loop parallel test2
  {
    using Task = std::function<void (zisc::uint, std::list<zisc::uint>::iterator)>;
    std::list<zisc::uint> list;
    std::array<zisc::uint, num_of_threads> id_list;
    for (std::size_t i = 0; i < num_of_threads; ++i) {
      id_list[i] = std::numeric_limits<zisc::uint>::max();
      list.emplace_back(i);
    }
    Task task{[&id_list](const zisc::uint, const std::list<zisc::uint>::iterator number)
    {
      const std::chrono::seconds wait_time{1};
      std::this_thread::sleep_for(wait_time);
      id_list[*number] = *number;
    }};

    auto result = thread_manager.enqueueLoop(Task{task}, list.begin(), list.end());
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

TEST(ThreadManagerTest, ExitWorkerRunningTest)
{
  constexpr zisc::uint num_of_works = 1024;
  std::vector<zisc::ThreadManager::UniqueResult<void>> results;
  results.reserve(num_of_works);
  {
    zisc::ThreadManager thread_manager{24};
    for (zisc::uint number = 0; number < num_of_works; ++number) {
      auto task = [/* number */](const zisc::uint)
      {
        const std::chrono::milliseconds wait_time{100};
        std::this_thread::sleep_for(wait_time);
      };
      results.emplace_back(thread_manager.enqueue<void>(task));
    }
  }
  SUCCEED();
}

TEST(ThreadManagerTest, TaskStressTest)
{
  constexpr zisc::uint num_of_threads = 1024;
  constexpr zisc::uint num_of_tasks = 4'000'000;

  std::vector<zisc::ThreadManager::UniqueResult<void>> result_list;
  result_list.resize(num_of_tasks);

  zisc::ThreadManager thread_manager{num_of_threads};
  for (zisc::uint number = 0; number < num_of_tasks; ++number) {
    auto task = [number](const zisc::uint)
    {
      zisc::PcgLcgRxsMXs32 sampler{number};
      const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::power<2>(1024.0));
      volatile int value = 0;
      for (int i = 0; i < loop; ++i) {
        value = i;
      }
    };
    result_list[number] = thread_manager.enqueue<void>(task);
  }
  for (auto& result : result_list)
    result->wait();

  SUCCEED();
}

TEST(ThreadManagerTest, LoopTaskStressTest)
{
  constexpr zisc::uint num_of_threads = 1024;
  constexpr zisc::uint num_of_tasks = 4'000'000;

  zisc::ThreadManager thread_manager{num_of_threads};
  auto task = [](const zisc::uint, const zisc::uint number)
  {
    zisc::PcgLcgRxsMXs32 sampler{number};
    const auto loop = static_cast<int>(sampler(0.0, 1.0) * zisc::power<2>(1024.0));
    volatile int value = 0;
    for (int i = 0; i < loop; ++i) {
      value = i;
    }
  };
  constexpr zisc::uint begin = 0;
  constexpr zisc::uint end = num_of_tasks;
  auto result = thread_manager.enqueueLoop(task, begin, end);
  result->wait();

  SUCCEED();
}

namespace {

void testThreadPoolNest(zisc::ThreadManager& thread_manager, const int level)
{
  constexpr int max_level = 3;
  if (level < max_level) {
    auto task1 = [&thread_manager, level]()
    {
      const int next_level = level + 1;
      testThreadPoolNest(thread_manager, next_level);
    };

    auto task2 = [&thread_manager, level](const zisc::uint)
    {
      const int next_level = level + 1;
      testThreadPoolNest(thread_manager, next_level);
    };

    auto result1 = thread_manager.enqueue<void>(task1);
    constexpr zisc::uint start = 0;
    const zisc::uint end = thread_manager.numOfThreads();
    auto result2 = thread_manager.enqueueLoop(task2, start, end);
    if (zisc::isOdd(level)) {
      result1->wait();
      result2->wait();
    }
    else {
      result2->wait();
      result1->wait();
    }
  }

  const std::chrono::milliseconds wait_time{150};
  std::this_thread::sleep_for(wait_time);
}

struct TestValue : private zisc::NonCopyable<TestValue>
{
  TestValue(const int value) : value_{value} {}
  TestValue(TestValue&& other) : value_{other.value_} {}
  int value_;
};

} // namespace 

TEST(ThreadManagerTest, NestedThreadPoolTest)
{
  zisc::ThreadManager thread_manager{16};
  ::testThreadPoolNest(thread_manager, 0);
}

TEST(ThreadManagerTest, GetValueTest)
{
  zisc::ThreadManager thread_manager{1};
  auto task = [&thread_manager]()
  {
    auto nested_task = []()
    {
      ::TestValue value{100};
      return value;
    };
    auto result = thread_manager.enqueue<::TestValue>(nested_task);
    return result->get();
  };
  auto result = thread_manager.enqueue<::TestValue>(task);
  auto value = result->get();
  ASSERT_EQ(100, value.value_) << "The get value test failed.";
}
