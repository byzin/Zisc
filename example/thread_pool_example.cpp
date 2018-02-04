/*!
  \file thread_pool_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
// Zisc
#include "zisc/thread_pool.hpp"
#include "zisc/zisc_config.hpp"

int main()
{
  // ThreadPool example
  std::cout << "## ThreadPool example" << std::endl;
  zisc::ThreadPool thread_pool{4};
  auto task = [](const zisc::uint thread_id)
  {
    const std::chrono::seconds wait_time{1 + thread_id * 2};
    std::this_thread::sleep_for(wait_time);
    std::cout << "  Task processed at thread " << thread_id << std::endl;
  };
  // Task parallel example
  std::cout << "Task parallel example" << std::endl;
  auto task1_result = thread_pool.enqueue<void>(task);
  auto task2_result = thread_pool.enqueue<void>(task);
  task1_result.get();
  task2_result.get();
  // Loop parallel
//  std::cout << "Loop parallel example" << std::endl;
//  auto loop_task = [](const zisc::uint thread_id, const int index)
//  {
//    const std::chrono::seconds wait_time{index + 1};
//    std::this_thread::sleep_for(wait_time);
//    std::cout << "  Task" << index << " processed at thread " << thread_id << std::endl;
//  };
//  auto task_result = thread_pool.enqueueLoop(std::function<void (int,int)>{loop_task}, 0, 8);
//  task_result.get();

  return 0;
}
