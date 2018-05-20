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
#include "zisc/thread_manager.hpp"
#include "zisc/pcg_engine.hpp"

TEST(ThreadManagerTest, EnqueueTaskTest)
{
  zisc::ThreadManager thread_manager{1};

  // enqueue()
  {
    auto task1 = []() noexcept
    {
      return 1;
    };
    auto result1 = thread_manager.enqueue<int>(task1);

    auto task2 = [](const zisc::uint) noexcept
    {
      return 1;
    };
    auto result2 = thread_manager.enqueue<int>(task2);

    std::function<int ()> task3{task1};
    auto result3 = thread_manager.enqueue<int>(std::move(task3));

    std::function<int (zisc::uint)> task4{task2};
    auto result4 = thread_manager.enqueue<int>(std::move(task4));

    ASSERT_EQ(1, result1.get());
    ASSERT_EQ(1, result2.get());
    ASSERT_EQ(1, result3.get());
    ASSERT_EQ(1, result4.get());
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

    result1.get();
    result2.get();
    result3.get();
    result4.get();
  }
}

TEST(ThreadManagerTest, ParallelTest)
{
  constexpr zisc::uint num_of_threads = 4;
  zisc::ThreadManager thread_manager{num_of_threads};

  ASSERT_EQ(num_of_threads, thread_manager.numOfThreads())
      << "Worker creation failed.";

  // Task parallel
  {
    using Task = std::function<void (zisc::uint)>;
    std::array<zisc::uint, num_of_threads> id_list{{0, 0, 0, 0}};
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
    result1.get();
    result2.get();
    result3.get();
    result4.get();
    ASSERT_EQ(0, id_list[0]) << "Task parallel failed.";
    ASSERT_EQ(1, id_list[1]) << "Task parallel failed.";
    ASSERT_EQ(2, id_list[2]) << "Task parallel failed.";
    ASSERT_EQ(3, id_list[3]) << "Task parallel failed.";
  }
  // Loop parallel test1
  {
    using Task = std::function<void (zisc::uint, int)>;
    std::array<zisc::uint, num_of_threads> id_list{{0, 0, 0, 0}};
    Task task{[&id_list](const zisc::uint id, const int)
    {
      const std::chrono::seconds wait_time{1};
      std::this_thread::sleep_for(wait_time);
      id_list[id] = id;
    }};

    auto result = thread_manager.enqueueLoop(Task{task}, 0, 4);
    result.get();
    ASSERT_EQ(0, id_list[0]) << "Loop parallel failed.";
    ASSERT_EQ(1, id_list[1]) << "Loop parallel failed.";
    ASSERT_EQ(2, id_list[2]) << "Loop parallel failed.";
    ASSERT_EQ(3, id_list[3]) << "Loop parallel failed.";
  }
  // Loop parallel test2
  {
    using Task = std::function<void (zisc::uint, std::list<int>::iterator)>;
    std::list<int> list{{0, 1, 2, 3}};
    std::array<int, num_of_threads> id_list{{0, 0, 0, 0}};
    Task task{[&id_list](const zisc::uint, const std::list<int>::iterator number)
    {
      const std::chrono::seconds wait_time{1};
      std::this_thread::sleep_for(wait_time);
      id_list[*number] = *number;
    }};

    auto result = thread_manager.enqueueLoop(Task{task}, list.begin(), list.end());
    result.get();
    ASSERT_EQ(0, id_list[0]) << "Loop parallel failed.";
    ASSERT_EQ(1, id_list[1]) << "Loop parallel failed.";
    ASSERT_EQ(2, id_list[2]) << "Loop parallel failed.";
    ASSERT_EQ(3, id_list[3]) << "Loop parallel failed.";
  }
}

TEST(ThreadManagerTest, ExitWorkerRunningTest)
{
  {
    zisc::ThreadManager thread_manager{24};
    for (zisc::uint number = 0; number < 1024; ++number) {
      auto task = [/* number */](const zisc::uint)
      {
        const std::chrono::milliseconds wait_time{100};
        std::this_thread::sleep_for(wait_time);
      };
      thread_manager.enqueue<void>(task);
    }
  }
  SUCCEED();
}

TEST(ThreadManagerTest, TaskStressTest)
{
  constexpr zisc::uint num_of_threads = 1024;
  constexpr zisc::uint num_of_tasks = 4'000'000;

  std::vector<std::future<void>> result_list;
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
    result.get();

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
  result.get();

  SUCCEED();
}
