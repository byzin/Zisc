/*!
  \file thread_pool.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_POOL_HPP
#define ZISC_THREAD_POOL_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <utility>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  ThreadPool class is designed based on the following URL
  http://progsch.net/wordpress/?p=81 and https//github.com/progschj/ThreadPool.
  */
class ThreadPool
{
 public:
  //! Create threads as many CPU threads as
  ThreadPool() noexcept;

  //! Create threads
  ThreadPool(const uint num_of_threads) noexcept;

  //! Terminate threads
  ~ThreadPool();


  //! Add a task
  template <typename ReturnType, typename Task>
  std::future<ReturnType> enqueue(Task&& task) noexcept;

  //! Execute a loop task in parallel
  template <typename Task, typename Iterator>
  std::future<void> enqueueLoop(Task&& task,
                                Iterator begin,
                                Iterator end) noexcept;

  //! Get the number of logical cores
  static uint logicalCores() noexcept;

  //! Get the number of threads
  uint numOfThreads() const noexcept;

 private:
  //! Create worker threads
  void createWorkers(const uint num_of_threads) noexcept;

  //! Append a task to the task queue
  template <typename ReturnType, typename Task>
  std::future<ReturnType> enqueueTask(Task& task) noexcept;

  //! Execute loop task in parallel
  template <typename Task, typename Iterator>
  std::future<void> enqueuLoopTask(Task& task,
                                   Iterator begin,
                                   Iterator end) noexcept;

  //! Check if the thread pool is finished
  bool isFinished() const noexcept;


  std::mutex queue_mutex_;
  std::condition_variable condition_;
  std::queue<std::function<void (int)>> task_queue_;
  std::vector<std::thread> workers_;
  bool is_finished_;
};

} // namespace zisc

#include "thread_pool-inl.hpp"

#endif // ZISC_THREAD_POOL_HPP
