/*!
  \file thread_pool-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_POOL_INL_HPP
#define ZISC_THREAD_POOL_INL_HPP

#include "thread_pool.hpp"
// Standard C++ library
#include <atomic>
#include <functional>
#include <future>
#include <iterator>
#include <memory>
#include <thread>
#include <type_traits>
#include <queue>
#include <utility>
#include <vector>
// Zisc
#include "error.hpp"
#include "non_copyable.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
inline
ThreadPool::ThreadPool() noexcept :
    ThreadPool(std::thread::hardware_concurrency())
{
}

/*!
  \details
  No detailed.
  */
inline
ThreadPool::ThreadPool(const uint num_of_threads) noexcept :
    workers_are_enabled_{true}
{
  createWorkers(num_of_threads);
}

/*!
  \details
  No detailed.
  */
inline
ThreadPool::~ThreadPool()
{
  exitWorkersRunning();
}

/*!
  \details
  The form of "Task" must be
  "ReturnType task()" or "ReturnType task(int thread_id)".
  */
template <typename ReturnType, typename Task> inline
std::future<ReturnType> ThreadPool::enqueue(Task&& task) noexcept
{
  return enqueueTask<ReturnType>(task);
}

/*!
  \details
  The form of "Task" must be
  "void task(iterator i)" or "void task(int thread_id, iterator i)".
  */
template <typename Task, typename Iterator> inline
std::future<void> ThreadPool::enqueueLoop(Task&& task,
                                          Iterator begin,
                                          Iterator end) noexcept
{
  return enqueueLoopTask(task, begin, end);
}

/*!
  \details
  No detailed.
  */
inline
uint ThreadPool::logicalCores() noexcept
{
  return cast<uint>(std::thread::hardware_concurrency());
}

/*!
  \details
  No detailed.

  \return The number of worker threads
  */
inline
uint ThreadPool::numOfThreads() const noexcept
{
  return cast<uint>(workers_.size());
}

/*!
  \details
  No detailed.
  */
inline
void ThreadPool::createWorkers(const uint num_of_threads) noexcept
{
  const uint threads = (num_of_threads == 0)
      ? std::thread::hardware_concurrency()
      : num_of_threads;
  workers_.reserve(threads);
  for (int thread_id = 0; thread_id < cast<int>(threads); ++thread_id) {
    auto work = [this, thread_id]() noexcept
    {
      WorkerTask task;
      while (workersAreEnabled()) {
        {
          std::unique_lock<std::mutex> locker{lock_};
          task = takeTask();
        }
        if (task) {
          task(thread_id);
        }
        else if (workersAreEnabled()){
          std::unique_lock<std::mutex> locker{lock_};
          condition_.wait(locker);
        }
      }
    };
    workers_.emplace_back(work);
  }
}

namespace inner {

//! Process a task
template <typename ReturnType, typename Task> inline
ReturnType processTask(
    Task& task,
    const int /* thread_id */,
    EnableIfConstructible<std::function<void ()>, Task> = kEnabler)
        noexcept
{
  return task();
}

//! Process a task
template <typename ReturnType, typename Task> inline
ReturnType processTask(
    Task& task,
    const int thread_id,
    EnableIfConstructible<std::function<void (int)>, Task> = kEnabler)
        noexcept
{
  return task(thread_id);
}
 
//! Process a loop task
template <typename Task, typename Iterator> inline
void processLoopTask(
    Task& task,
    const int /* thread_id */,
    Iterator iterator,
    EnableIfConstructible<std::function<void (Iterator)>, Task> = kEnabler)
        noexcept
{
  task(iterator);
}

//! Process a loop task
template <typename Task, typename Iterator> inline
void processLoopTask(
    Task& task,
    const int thread_id,
    Iterator iterator,
    EnableIfConstructible<std::function<void (int, Iterator)>, Task> = kEnabler)
        noexcept
{
  task(thread_id, iterator);
}

//! Return the distance of two iterators
template <typename Iterator> inline
uint distance(Iterator begin,
              Iterator end,
              EnableIfIterator<Iterator> = kEnabler) noexcept
{
  ZISC_ASSERT(begin != end, "The end is same as the begin.");
  return cast<uint>(std::distance(begin, end));
}

//! Return the distance of two iterators
template <typename Iterator> inline
uint distance(Iterator begin,
              Iterator end,
              EnableIfInteger<Iterator> = kEnabler) noexcept
{
  ZISC_ASSERT(begin < end, "The end is ahead of the begin.");
  return cast<uint>(end - begin);
}

} // namespace inner

/*!
  \details
  No detailed.
  */
template <typename ReturnType, typename Task> inline
std::future<ReturnType> ThreadPool::enqueueTask(Task& task) noexcept
{
  // Define the shared resource
  auto shared_task = new std::packaged_task<ReturnType (int)>{
  [task = std::move(task)](const int thread_id) noexcept
  {
    return inner::processTask<ReturnType>(task, thread_id);
  }};
  auto result = shared_task->get_future();
  // Make a task
  WorkerTask wrapped_task{[shared_task](const int thread_id) noexcept
  {
    (*shared_task)(thread_id);
    delete shared_task;
  }};
  {
    std::unique_lock<std::mutex> lock{lock_};
    task_queue_.emplace(std::move(wrapped_task));
  }
  condition_.notify_one();

  return result;
}

/*!
  \details
  No detailed.
  */
template <typename Task, typename Iterator> inline
std::future<void> ThreadPool::enqueueLoopTask(Task& task,
                                              Iterator begin,
                                              Iterator end) noexcept
{
  // Define the shared resource
  struct SharedResource : public NonCopyable<SharedResource>
  {
    SharedResource(Task& shared_task, const uint initial_count) noexcept :
        exit_loop_{[]() noexcept {}},
        shared_task_{std::move(shared_task)},
        counter_{initial_count} {}
    std::packaged_task<void ()> exit_loop_; //!< Invoked when all tasks are finished
    Task shared_task_; //!< Invoked with each iterators
    std::atomic<uint> counter_; //!< Counts uncompleted tasks
  };
  // Make a shared resource
  const uint distance = inner::distance(begin, end);
  auto shared_resource = new SharedResource{task, distance};
  // Make a result
  auto result = shared_resource->exit_loop_.get_future();
  // Make tasks
  {
    std::unique_lock<std::mutex> locker{lock_};
    for (auto iterator = begin; iterator != end; ++iterator) {
      WorkerTask wrapped_task{
      [shared_resource, iterator](const int thread_id) noexcept
      {
        auto& shared_task = shared_resource->shared_task_;
        inner::processLoopTask(shared_task, thread_id, iterator);
        const uint count = --(shared_resource->counter_);
        if (count == 0) {
          shared_resource->exit_loop_();
          delete shared_resource;
        }
      }};
      task_queue_.emplace(std::move(wrapped_task));
    }
  }
  condition_.notify_all();

  return result;
}

/*!
  */
inline
void ThreadPool::exitWorkersRunning() noexcept
{
  {
    std::unique_lock<std::mutex> locker{lock_};
    workers_are_enabled_ = false;
  }
  condition_.notify_all();
  for (auto& worker : workers_)
    worker.join();
}

/*!
  */
inline
auto ThreadPool::takeTask() noexcept -> WorkerTask
{
  WorkerTask task;
  if (!task_queue_.empty()) {
    task = std::move(task_queue_.front());
    task_queue_.pop();
  }
  return task;
}

/*!
  */
inline
bool ThreadPool::workersAreEnabled() const noexcept
{
  return workers_are_enabled_;
}

} // namespace zisc

#endif // ZISC_THREAD_POOL_INL_HPP
