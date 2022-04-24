/*!
  \file thread_manager_example.cpp
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
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/memory/simple_memory_resource.hpp"
#include "zisc/concurrency/thread_manager.hpp"

int main()
{
  // ThreadManager example
  std::cout << "## ThreadManager example" << std::endl;
  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{4, &mem_resource};
  auto task = [](const zisc::int64b thread_id)
  {
    const std::chrono::seconds wait_time{1 + thread_id * 2};
    std::this_thread::sleep_for(wait_time);
    std::cout << "  Task was processed at thread " << thread_id << std::endl;
  };
  // Task parallel example
  std::cout << "Task parallel example" << std::endl;
  auto task1_result = thread_manager.enqueue(task);
  auto task2_result = thread_manager.enqueue(task);
  task1_result.wait();
  task2_result.wait();
  // Loop parallel
  std::cout << "Loop parallel example" << std::endl;
  auto loop_task = [](const int index, const zisc::int64b thread_id)
  {
    const std::chrono::seconds wait_time{index + 1};
    std::this_thread::sleep_for(wait_time);
    std::cout << "  Task" << index << " was processed at thread " << thread_id << std::endl;
  };
  using Func = std::function<void (zisc::int64b, int)>;
  auto task_result = thread_manager.enqueueLoop(Func{loop_task}, 0, 8);
  task_result.wait();

  return 0;
}
