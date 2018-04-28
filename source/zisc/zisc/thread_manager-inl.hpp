/*!
  \file thread_manager-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_INL_HPP
#define ZISC_THREAD_MANAGER_INL_HPP

#include "thread_manager.hpp"
// Standard C++ library
#include <array>
#include <atomic>
#include <cstddef>
#include <deque>
#include <future>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "error.hpp"
#include "memory_resource.hpp"
#include "non_copyable.hpp"
#include "type_traits.hpp"
#include "simple_memory_resource.hpp"
#include "unique_memory_pointer.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
inline
ThreadManager::ThreadManager(pmr::memory_resource* mem_resource) noexcept :
    ThreadManager(std::thread::hardware_concurrency(), mem_resource)
{
}

/*!
  \details
  No detailed.
  */
inline
ThreadManager::ThreadManager(const uint num_of_threads,
                             pmr::memory_resource* mem_resource) noexcept :
    task_queue_{pmr::polymorphic_allocator<TaskPointer>{mem_resource}},
    workers_{pmr::polymorphic_allocator<std::thread>{mem_resource}},
    workers_are_enabled_{kTrue}
{
  initialize(num_of_threads);
}

/*!
  \details
  No detailed.
  */
inline
ThreadManager::~ThreadManager()
{
  exitWorkersRunning();
}

/*!
  */
inline
std::array<uint, 2> ThreadManager::calcThreadRange(const uint range,
                                                   const uint num_of_threads,
                                                   const uint thread_id) noexcept
{
  const uint n = range / num_of_threads;
  const uint begin = n * thread_id;
  const uint end = begin + ((thread_id + 1 == num_of_threads)
      ? n + (range - n * num_of_threads)
      : n);
  return std::array<uint, 2>{{begin, end}};
}

/*!
  */
inline
std::array<uint, 2> ThreadManager::calcThreadRange(const uint range,
                                                   const uint thread_id) const noexcept
{
  return calcThreadRange(range, numOfThreads(), thread_id);
}

/*!
  \details
  No detailed.
  */
inline
uint ThreadManager::logicalCores() noexcept
{
  return cast<uint>(std::thread::hardware_concurrency());
}

/*!
  \details
  No detailed.

  \return The number of worker threads
  */
inline
uint ThreadManager::numOfThreads() const noexcept
{
  return cast<uint>(workers_.size());
}

/*!
  \details
  No detailed.
  */
inline
void ThreadManager::createWorkers(const uint num_of_threads) noexcept
{
  const std::size_t id_max = (num_of_threads == 0)
      ? cast<std::size_t>(std::thread::hardware_concurrency())
      : cast<std::size_t>(num_of_threads);
  workers_.reserve(id_max);
  for (std::size_t thread_id = 0; thread_id < id_max; ++thread_id) {
    auto work = [this, thread_id]() noexcept
    {
      TaskPointer task;
      while (workersAreEnabled()) {
        {
          std::unique_lock<std::mutex> locker{lock_};
          task = takeTask();
          if (workersAreEnabled() && !task)
            condition_.wait(locker);
        }
        if (task)
          task->doTask(cast<uint>(thread_id));
      }
    };
    workers_.emplace_back(work);
  }
}

namespace inner {

//! Process a task
template <typename ReturnType, typename Task> inline
ReturnType processTask(Task& task, const uint thread_id) noexcept
{
  using TaskI = std::function<ReturnType (uint)>;
  if constexpr (std::is_constructible_v<TaskI, Task>) {
    return task(thread_id);
  }
  else {
    static_cast<void>(thread_id);
    return task();
  }
}

//! Process a worker task
template <typename ReturnType, typename Task> inline
void processWorkerTask(Task& worker_task,
                       std::promise<ReturnType>& worker_promise,
                       const uint thread_id) noexcept
{
  if constexpr (!std::is_void_v<ReturnType>) {
    auto result = processTask<ReturnType>(worker_task, thread_id);
    worker_promise.set_value(std::move(result));
  }
  else {
    processTask<ReturnType>(worker_task, thread_id);
    worker_promise.set_value();
  }
}

//! Process a loop task
template <typename Task, typename Iterator> inline
void processLoopTask(Task& worker_task,
                     const uint thread_id,
                     Iterator iterator) noexcept
{
  using TaskII = std::function<void (uint, Iterator)>;
  if constexpr (std::is_constructible_v<TaskII, Task>) {
    worker_task(thread_id, iterator);
  }
  else {
    static_cast<void>(thread_id);
    worker_task(iterator);
  }
}

//! Return the distance of two iterators
template <typename Iterator> inline
uint distance(Iterator begin, Iterator end) noexcept
{
  if constexpr (kIsIterator<Iterator>) {
    ZISC_ASSERT(begin != end, "The end is same as the begin.");
    return cast<uint>(std::distance(begin, end));
  }
  else {
    ZISC_ASSERT(begin < end, "The end is ahead of the begin.");
    return cast<uint>(end - begin);
  }
}

} // namespace inner

/*!
  \details
  The form of "Task" must be
  "ReturnType task()" or "ReturnType task(int thread_id)".
  */
template <typename ReturnType, typename Task> inline
std::future<ReturnType> ThreadManager::enqueue(
    Task&& task,
    pmr::memory_resource* mem_resource) noexcept
{
#ifdef Z_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif // Z_CLANG
  //! Task class for single task
  class SingleTask : public WorkerTask
  {
   public:
    //! Create a task
    SingleTask(Task&& worker_task, pmr::memory_resource* mem) noexcept :
        worker_task_{std::forward<Task>(worker_task)},
        worker_promise_{std::allocator_arg,
                        pmr::polymorphic_allocator<ReturnType>{mem}} {}

    //! Do a task
    void doTask(const uint thread_id) noexcept override
    {
      inner::processWorkerTask<ReturnType>(worker_task_, worker_promise_, thread_id);
    }

    Task worker_task_;
    std::promise<ReturnType> worker_promise_;
  };
#ifdef Z_CLANG
#pragma clang diagnostic pop
#endif // Z_CLANG

  // Make a task
  auto worker_task = UniqueMemoryPointer<SingleTask>::make(mem_resource, 
                                                           std::forward<Task>(task),
                                                           mem_resource);
  auto result = worker_task->worker_promise_.get_future();

  // Enqueue the task
  {
    std::unique_lock<std::mutex> locker{lock_};
    task_queue_.emplace(std::move(worker_task));
  }
  condition_.notify_one();

  return result;
}

/*!
  \details
  The form of "Task" must be
  "void task(iterator i)" or "void task(int thread_id, iterator i)".
  */
template <typename Task, typename Iterator> inline
std::future<void> ThreadManager::enqueueLoop(
    Task&& task,
    Iterator&& begin,
    Iterator&& end,
    pmr::memory_resource* mem_resource) noexcept
{
#ifdef Z_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif // Z_CLANG
  //! Resources for shared task
  struct SharedResource : public NonCopyable<SharedResource>
  {
    SharedResource(Task&& worker_task,
                   const uint task_count,
                   pmr::memory_resource* mem) noexcept :
        worker_task_{std::forward<Task>(worker_task)},
        worker_promise_{std::allocator_arg,
                        pmr::polymorphic_allocator<void>{mem}},
        mem_{mem},
        counter_{task_count} {}

    Task worker_task_;
    std::promise<void> worker_promise_;
    pmr::memory_resource* mem_;
    std::atomic<uint> counter_;
    static_assert(sizeof(std::atomic<uint>) <= sizeof(std::promise<void>));
    static_assert(alignof(std::atomic<uint>) <= alignof(std::promise<void>));
    static_assert(alignof(std::atomic<uint>) <= alignof(pmr::memory_resource*));
  };

  //! Task class for shared task
  class SharedTask : public WorkerTask
  {
   public:
    //! Create a shared task
    SharedTask(SharedResource* shared_resource, Iterator&& ite) noexcept :
        shared_resource_{shared_resource},
        iterator_{std::forward<Iterator>(ite)} {}

    //! Destroy a shared task
    ~SharedTask() noexcept override
    {
      const uint c = --(shared_resource_->counter_);
      if (c == 0) {
        UniqueMemoryPointer<SharedResource> r{shared_resource_,
                                              shared_resource_->mem_};
        r->worker_promise_.set_value();
      }
    }

    //! Do a task
    void doTask(const uint thread_id) noexcept override
    {
      inner::processLoopTask(shared_resource_->worker_task_, thread_id, iterator_);
    }

    SharedResource* shared_resource_;
    Iterator iterator_;
  };
#ifdef Z_CLANG
#pragma clang diagnostic pop
#endif // Z_CLANG

  // Make a shared resource
  const uint distance = inner::distance(begin, end);
  auto shared_resource = UniqueMemoryPointer<SharedResource>::make(
      mem_resource,
      std::forward<Task>(task),
      distance,
      mem_resource).release();
  auto result = shared_resource->worker_promise_.get_future();

  // Make worker tasks
  {
    std::unique_lock<std::mutex> locker{lock_};
    for (auto ite = begin; ite != end; ++ite) {
      auto worker_task = UniqueMemoryPointer<SharedTask>::make(
          mem_resource,
          shared_resource,
          std::forward<Iterator>(ite));
      task_queue_.emplace(std::move(worker_task));
    }
  }
  condition_.notify_all();

  return result;
}

/*!
  */
inline
void ThreadManager::exitWorkersRunning() noexcept
{
  {
    std::unique_lock<std::mutex> locker{lock_};
    workers_are_enabled_ = kFalse;
  }
  condition_.notify_all();
  for (auto& worker : workers_)
    worker.join();
}

/*!
  */
inline
auto ThreadManager::takeTask() noexcept -> TaskPointer
{
  TaskPointer task;
  if (!task_queue_.empty()) {
    task = std::move(task_queue_.front());
    task_queue_.pop();
  }
  return task;
}

/*!
  */
inline
void ThreadManager::initialize(const uint num_of_threads) noexcept
{
  createWorkers(num_of_threads);

  // Avoid padding warning
  static_cast<void>(padding_);

  // Check the alignment of member variables
  static_assert(alignof(std::condition_variable) <=
                alignof(std::mutex));
  static_assert(alignof(std::queue<TaskPointer, pmr::deque<TaskPointer>>) <=
                alignof(std::condition_variable));
  static_assert(alignof(pmr::vector<std::thread>) <=
                alignof(std::queue<TaskPointer, pmr::deque<TaskPointer>>));
  static_assert(alignof(uint8) <=
                alignof(pmr::vector<std::thread>));
}

/*!
  */
inline
bool ThreadManager::workersAreEnabled() const noexcept
{
  return workers_are_enabled_ == kTrue;
}

} // namespace zisc

#endif // ZISC_THREAD_MANAGER_INL_HPP
