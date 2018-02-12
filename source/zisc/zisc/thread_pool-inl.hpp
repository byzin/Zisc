/*!
  \file thread_pool-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_POOL_INL_HPP
#define ZISC_THREAD_POOL_INL_HPP

#include "thread_pool.hpp"
// Standard C++ library
#include <array>
#include <atomic>
#include <cstddef>
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
    workers_are_enabled_{kTrue}
{
  initialize(num_of_threads);
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
  */
inline
std::array<uint, 2> ThreadPool::calcThreadRange(const uint range,
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
std::array<uint, 2> ThreadPool::calcThreadRange(const uint range,
                                                const uint thread_id) const noexcept
{
  return calcThreadRange(range, numOfThreads(), thread_id);
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
  const std::size_t id_max = (num_of_threads == 0)
      ? cast<std::size_t>(std::thread::hardware_concurrency())
      : cast<std::size_t>(num_of_threads);
  workers_.reserve(id_max);
  for (std::size_t thread_id = 0; thread_id < id_max; ++thread_id) {
    auto work = [this, thread_id]() noexcept
    {
      WorkerTask task;
      while (workersAreEnabled()) {
        {
          std::unique_lock<std::mutex> locker{lock_};
          task = takeTask();
          if (workersAreEnabled() && !task)
            condition_.wait(locker);
        }
        if (task)
          task(cast<uint>(thread_id));
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
  No detailed.
  */
template <typename ReturnType, typename Task> inline
std::future<ReturnType> ThreadPool::enqueueTask(Task&& task) noexcept
{
  using T = std::remove_reference_t<Task>;

#ifdef Z_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif // Z_CLANG

  //! Resource for task
  struct TaskResource : public NonCopyable<TaskResource>
  {
    TaskResource(T& worker_task) noexcept :
        worker_task_{std::forward<Task>(worker_task)},
        worker_promise_{} {}
    T worker_task_;
    std::promise<ReturnType> worker_promise_;
  };

#ifdef Z_CLANG
#pragma clang diagnostic pop
#endif // Z_CLANG

  // Make a task resource
  auto task_resource = std::make_unique<TaskResource>(task).release();
  auto result = task_resource->worker_promise_.get_future();

  // Make a worker task
  WorkerTask wrapped_task{[task_resource](const uint thread_id) noexcept
  {
    inner::processWorkerTask<ReturnType>(task_resource->worker_task_,
                                         task_resource->worker_promise_,
                                         thread_id);
    std::unique_ptr<TaskResource>{task_resource};
  }};

  // Enqueue the task
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
std::future<void> ThreadPool::enqueueLoopTask(Task&& task,
                                              Iterator begin,
                                              Iterator end) noexcept
{
  using T = std::remove_reference_t<Task>;

#ifdef Z_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif // Z_CLANG

  //! Resource for task
  struct SharedResource : public NonCopyable<SharedResource>
  {
    SharedResource(T& worker_task, const uint initial_count) noexcept :
        worker_task_{std::forward<Task>(worker_task)},
        worker_promise_{},
        counter_{initial_count} {}
    T worker_task_; //!< Invoked with each iterators
    std::promise<void> worker_promise_; //!< Invoked when all tasks are finished
    std::atomic<uint> counter_; //!< Counts uncompleted tasks
    static_assert(sizeof(std::atomic<uint>) <= sizeof(std::promise<void>));
    static_assert(alignof(std::atomic<uint>) <= alignof(std::promise<void>));
  };

  // Make a shared resource
  const uint distance = inner::distance(begin, end);
  auto shared_resource = std::make_unique<SharedResource>(task, distance).release();

  // Make a result
  auto result = shared_resource->worker_promise_.get_future();
  // Make worker tasks
  {

    std::unique_lock<std::mutex> locker{lock_};
    for (auto iterator = begin; iterator != end; ++iterator) {
      WorkerTask wrapped_task{
      [shared_resource, iterator](const uint thread_id) noexcept
      {
        auto& worker_task = shared_resource->worker_task_;
        inner::processLoopTask(worker_task, thread_id, iterator);
        const uint count = --(shared_resource->counter_);
        if (count == 0) {
          shared_resource->worker_promise_.set_value();
          std::unique_ptr<SharedResource>{shared_resource};
        }
      }};
      task_queue_.emplace(std::move(wrapped_task));
    }

#ifdef Z_CLANG
#pragma clang diagnostic pop
#endif // Z_CLANG

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
    workers_are_enabled_ = kFalse;
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
void ThreadPool::initialize(const uint num_of_threads) noexcept
{
  createWorkers(num_of_threads);

  // Avoid padding warning
  static_cast<void>(padding_);

  // Check the alignment of member variables
  static_assert(alignof(std::condition_variable) <=
                alignof(std::mutex));
  static_assert(alignof(std::queue<WorkerTask>) <=
                alignof(std::condition_variable));
  static_assert(alignof(std::vector<std::thread>) <=
                alignof(std::queue<WorkerTask>));
  static_assert(alignof(uint8) <=
                alignof(std::vector<std::thread>));
}

/*!
  */
inline
bool ThreadPool::workersAreEnabled() const noexcept
{
  return workers_are_enabled_ == kTrue;
}

} // namespace zisc

#endif // ZISC_THREAD_POOL_INL_HPP
