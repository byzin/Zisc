/*!
  \file thread_manager_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
// Zisc
#include "zisc/simple_memory_resource.hpp"
#include "zisc/thread_manager.hpp"
#include "zisc/zisc_config.hpp"

int main()
{
  // ThreadManager example
  std::cout << "## ThreadManager example" << std::endl;
  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{4, &mem_resource};
  auto task = [](const zisc::uint thread_id)
  {
    const std::chrono::seconds wait_time{1 + thread_id * 2};
    std::this_thread::sleep_for(wait_time);
    std::cout << "  Task processed at thread " << thread_id << std::endl;
  };
  // Task parallel example
  std::cout << "Task parallel example" << std::endl;
  auto task1_result = thread_manager.enqueue<void>(task, &mem_resource);
  auto task2_result = thread_manager.enqueue<void>(task, &mem_resource);
  task1_result.get();
  task2_result.get();
  // Loop parallel
  std::cout << "Loop parallel example" << std::endl;
  auto loop_task = [](const zisc::uint thread_id, const int index)
  {
    const std::chrono::seconds wait_time{index + 1};
    std::this_thread::sleep_for(wait_time);
    std::cout << "  Task" << index << " processed at thread " << thread_id << std::endl;
  };
  auto task_result = thread_manager.enqueueLoop(std::function<void (int,int)>{loop_task}, 0, 8, &mem_resource);
  task_result.get();

  return 0;
}
