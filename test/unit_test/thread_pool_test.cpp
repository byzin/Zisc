/*!
  \file thread_pool_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
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
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/aligned_memory_pool.hpp"
#include "zisc/thread_pool.hpp"

TEST(ThreadPoolTest, EnqueueTaskTest)
{
  zisc::ThreadPool thread_pool{1};

  // enqueue()
  {
    auto task1 = []() noexcept
    {
      return 1;
    };
    auto result1 = thread_pool.enqueue<int>(task1);

    auto task2 = [](const int) noexcept
    {
      return 1;
    };
    auto result2 = thread_pool.enqueue<int>(task2);
  
    std::function<int ()> task3{task1};
    auto result3 = thread_pool.enqueue<int>(std::move(task3));

    std::function<int (int)> task4{task2};
    auto result4 = thread_pool.enqueue<int>(std::move(task4));
  
    ASSERT_EQ(1, result1.get());
    ASSERT_EQ(1, result2.get());
    ASSERT_EQ(1, result3.get());
    ASSERT_EQ(1, result4.get());
  }
  // enqueueLoop()
  {
    auto task1 = [](const int /* index */) {};
    auto result1 = thread_pool.enqueueLoop(task1, 0, 10);

    auto task2 = [](const int /* thread_number */, const int /* index */) {};
    auto result2 = thread_pool.enqueueLoop(task2, 0, 10);

    std::function<void (int)> task3{task1};
    auto result3 = thread_pool.enqueueLoop(std::move(task3), 0, 10);

    std::function<void (int, int)> task4{task2};
    auto result4 = thread_pool.enqueueLoop(std::move(task4), 0, 10);

    result1.get();
    result2.get();
    result3.get();
    result4.get();
  }
}

TEST(ThreadPoolTest, ParallelTest)
{
  constexpr zisc::uint num_of_threads = 4;
  zisc::ThreadPool thread_pool{num_of_threads};

  ASSERT_EQ(num_of_threads, thread_pool.numOfThreads())
      << "Worker creation failed.";

  // Task parallel
  {
    using Task = std::function<void (int)>;
    std::array<int, num_of_threads> id_list{{-1, -1, -1, -1}};
    Task task{[&id_list](const int id)
    {
      const std::chrono::seconds wait_time{1};
      std::this_thread::sleep_for(wait_time);
      id_list[id] = id; 
    }};

    auto result1 = thread_pool.enqueue<void>(Task{task});
    auto result2 = thread_pool.enqueue<void>(Task{task});
    auto result3 = thread_pool.enqueue<void>(Task{task});
    auto result4 = thread_pool.enqueue<void>(Task{task});
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
    using Task = std::function<void (int, int)>;
    std::array<int, num_of_threads> id_list{{-1, -1, -1, -1}};
    Task task{[&id_list](const int id, const int)
    {
      const std::chrono::seconds wait_time{1};
      std::this_thread::sleep_for(wait_time);
      id_list[id] = id;
    }};

    auto result = thread_pool.enqueueLoop(Task{task}, 0, 4);
    result.get();
    ASSERT_EQ(0, id_list[0]) << "Loop parallel failed.";
    ASSERT_EQ(1, id_list[1]) << "Loop parallel failed.";
    ASSERT_EQ(2, id_list[2]) << "Loop parallel failed.";
    ASSERT_EQ(3, id_list[3]) << "Loop parallel failed.";
  }
  // Loop parallel test2
  {
    using Task = std::function<void (int, std::list<int>::iterator)>;
    std::list<int> list{{0, 1, 2, 3}};
    std::array<int, num_of_threads> id_list{{-1, -1, -1, -1}};
    Task task{[&id_list](const int, const std::list<int>::iterator number)
    {
      const std::chrono::seconds wait_time{1};
      std::this_thread::sleep_for(wait_time);
      id_list[*number] = *number;
    }};

    auto result = thread_pool.enqueueLoop(Task{task}, list.begin(), list.end());
    result.get();
    ASSERT_EQ(0, id_list[0]) << "Loop parallel failed.";
    ASSERT_EQ(1, id_list[1]) << "Loop parallel failed.";
    ASSERT_EQ(2, id_list[2]) << "Loop parallel failed.";
    ASSERT_EQ(3, id_list[3]) << "Loop parallel failed.";
  }

}
