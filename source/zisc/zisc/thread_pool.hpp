/*!
  \file thread_pool.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_POOL_HPP
#define ZISC_THREAD_POOL_HPP

// Standard C++ library
#include <array>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief ThreadPool class provides task parallel and data parallel thread pool
  */
class ThreadPool : public NonCopyable<ThreadPool>
{
 public:
  //! Create threads as many CPU threads as
  ThreadPool() noexcept;

  //! Create threads
  ThreadPool(const uint num_of_threads) noexcept;

  //! Terminate threads
  ~ThreadPool();


  //! Calculate a range of a thread
  static std::array<uint, 2> calcThreadRange(const uint range,
                                             const uint num_of_threads,
                                             const uint thread_id) noexcept;

  //! Calculate a range of a thread
  std::array<uint, 2> calcThreadRange(const uint range, 
                                      const uint thread_id) const noexcept;

  //! A worker thread run a task
  template <typename ReturnType, typename Task>
  std::future<ReturnType> enqueue(Task&& task) noexcept;

  //! Worker threads run a loop task in parallel
  template <typename Task, typename Iterator>
  std::future<void> enqueueLoop(Task&& task,
                                Iterator begin,
                                Iterator end) noexcept;

  //! Get the number of logical cores
  static uint logicalCores() noexcept;

  //! Return the number of threads
  uint numOfThreads() const noexcept;

 private:
  using WorkerTask = std::function<void (uint)>;


  //! Create worker threads
  void createWorkers(const uint num_of_threads) noexcept;

  //! Append a task to the task queue
  template <typename ReturnType, typename Task>
  std::future<ReturnType> enqueueTask(Task&& task) noexcept;

  //! Execute loop task in parallel
  template <typename Task, typename Iterator>
  std::future<void> enqueueLoopTask(Task&& task,
                                    Iterator begin,
                                    Iterator end) noexcept;

  //! Exit workers running
  void exitWorkersRunning() noexcept;

  //! Initialize thread pool
  void initialize(const uint num_of_threads) noexcept;

  //! Take a task from the top of the queue
  WorkerTask takeTask() noexcept;

  //! Check if the workers (threads) are enable running
  bool workersAreEnabled() const noexcept;


  std::mutex lock_;
  std::condition_variable condition_;
  std::queue<WorkerTask> task_queue_;
  std::vector<std::thread> workers_;
  uint8 workers_are_enabled_;
  std::array<uint8, 7> padding_;
};

} // namespace zisc

#include "thread_pool-inl.hpp"

#endif // ZISC_THREAD_POOL_HPP
