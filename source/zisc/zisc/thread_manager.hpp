/*!
  \file thread_manager.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_HPP
#define ZISC_THREAD_MANAGER_HPP

// Standard C++ library
#include <array>
#include <deque>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
// Zisc
#include "memory_resource.hpp"
#include "non_copyable.hpp"
#include "simple_memory_resource.hpp"
#include "unique_memory_pointer.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief ThreadManager class provides task parallel and data parallel thread pool
  */
class ThreadManager : public NonCopyable<ThreadManager>
{
 public:
  //! Create threads as many CPU threads as
  ThreadManager(
      pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Create threads
  ThreadManager(
      const uint num_of_threads,
      pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Terminate threads
  ~ThreadManager();


  //! Calculate a range of a thread
  static std::array<uint, 2> calcThreadRange(const uint range,
                                             const uint num_of_threads,
                                             const uint thread_id) noexcept;

  //! Calculate a range of a thread
  std::array<uint, 2> calcThreadRange(const uint range, 
                                      const uint thread_id) const noexcept;

  //! A worker thread run a task
  template <typename ReturnType, typename Task>
  std::future<ReturnType> enqueue(
      Task&& task,
      pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Worker threads run a loop task in parallel
  template <typename Task, typename Iterator>
  std::future<void> enqueueLoop(
      Task&& task,
      Iterator&& begin,
      Iterator&& end,
      pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Get the number of logical cores
  static uint logicalCores() noexcept;

  //! Return the number of threads
  uint numOfThreads() const noexcept;

 private:
  //! Base class of worker task
  class WorkerTask
  {
   public:
    virtual ~WorkerTask() noexcept {}
    //! Do a task
    virtual void doTask(const uint thread_id) noexcept = 0;
  };
  using TaskPointer = UniqueMemoryPointer<WorkerTask>;


  //! Create worker threads
  void createWorkers(const uint num_of_threads) noexcept;

  //! Exit workers running
  void exitWorkersRunning() noexcept;

  //! Initialize this thread manager
  void initialize(const uint num_of_threads) noexcept;

  //! Take a task from the top of the queue
  TaskPointer takeTask() noexcept;

  //! Check if the workers (threads) are enable running
  bool workersAreEnabled() const noexcept;


  std::mutex lock_;
  std::condition_variable condition_;
  std::queue<TaskPointer, pmr::deque<TaskPointer>> task_queue_;
  pmr::vector<std::thread> workers_;
  uint8 workers_are_enabled_;
  std::array<uint8, 7> padding_;
};

} // namespace zisc

#include "thread_manager-inl.hpp"

#endif // ZISC_THREAD_MANAGER_HPP
