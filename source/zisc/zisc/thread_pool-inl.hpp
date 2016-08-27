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
#include <cmath>
#include <functional>
#include <future>
#include <iterator>
#include <memory>
#include <thread>
#include <type_traits>
#include <queue>
#include <utility>
// Zisc
#include "aligned_memory_pool.hpp"
#include "error.hpp"
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
    is_finished_{false}
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
  {
    std::unique_lock<std::mutex> lock{queue_mutex_};
    is_finished_ = true;
  }
  condition_.notify_all();
  for (auto& worker : workers_)
    worker.join();
}

/*!
  \details
  The form of "Task" must be
  "ReturnType task()" or "ReturnType task(int thread_number)".
  */
template <typename ReturnType, typename Task> inline
std::future<ReturnType> ThreadPool::enqueue(Task&& task) noexcept
{
  return enqueueTask<ReturnType>(task);
}

/*!
  \details
  The form of "Task" must be
  "void task(iterator i)" or "void task(int thread_number, iterator i)".
  */
template <typename Task, typename Iterator> inline
std::future<void> ThreadPool::enqueueLoop(Task&& task,
                                          Iterator begin,
                                          Iterator end) noexcept
{
  return enqueuLoopTask(task, begin, end);
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
  uint threads = (num_of_threads == 0)
      ? std::thread::hardware_concurrency()
      : num_of_threads;
  workers_.reserve(threads);
  for (int thread_number = 0; thread_number < cast<int>(threads); ++thread_number) {
    auto work = [this, thread_number]() noexcept
    {
      std::unique_lock<std::mutex> locker{queue_mutex_};
      while (!isFinished()) {
        if (!locker)
          locker.lock();
        if (!task_queue_.empty()) {
          auto task = std::move(task_queue_.front());
          task_queue_.pop();
          locker.unlock();
          task(thread_number);
        }
        else {
          condition_.wait(locker);
        }
      }
    };
    workers_.emplace_back(work);
  }
}

namespace zisc_thread_pool {

//! Process a task
template <typename ReturnType, typename Task> inline
ReturnType processTask(
    Task& task, 
    const int /* thread_number */,
    EnableIfConstructible<std::function<void ()>, Task> = kEnabler)
        noexcept
{
  return task();
}

//! Process a task
template <typename ReturnType, typename Task> inline
ReturnType processTask(
    Task& task, 
    const int thread_number,
    EnableIfConstructible<std::function<void (int)>, Task> = kEnabler)
        noexcept
{
  return task(thread_number);
}
 
//! Process a loop task
template <typename Task, typename Iterator> inline
void processLoopTask(
    Task& task, 
    const int /* thread_number */, 
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
    const int thread_number, 
    Iterator iterator,
    EnableIfConstructible<std::function<void (int, Iterator)>, Task> = kEnabler)
        noexcept
{
  task(thread_number, iterator);
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

} // namespace zisc_thread_pool

/*!
  \details
  No detailed.
  */
template <typename ReturnType, typename Task> inline
std::future<ReturnType> ThreadPool::enqueueTask(Task& task) noexcept
{
  auto t = [task = std::move(task)](const int thread_number) noexcept
  {
    return zisc_thread_pool::processTask<ReturnType>(task, thread_number);
  };
  auto shared_task = std::make_shared<std::packaged_task<ReturnType (int)>>(t);
  auto result = shared_task->get_future();

  std::function<void (int)> wrapped_task{
  [shared_task](const int thread_number) noexcept
  {
    (*shared_task)(thread_number);
  }};

  {
    std::unique_lock<std::mutex> lock{queue_mutex_};
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
std::future<void> ThreadPool::enqueuLoopTask(Task& task, 
                                             Iterator begin, 
                                             Iterator end) noexcept
{
  const uint distance = zisc_thread_pool::distance(begin, end);

  auto shared_task = new Task{std::move(task)};
  auto finish_loop = new std::packaged_task<void ()>{[]() noexcept {}};
  auto shared_counter = new std::atomic<uint>{distance};

  auto result = finish_loop->get_future();

  {
    std::unique_lock<std::mutex> lock{queue_mutex_};
    for (auto iterator = begin; iterator != end; ++iterator) {
      std::function<void (int)> wrapped_task{
      [shared_task, finish_loop, shared_counter, iterator](const int thread_number) noexcept
      {
        zisc_thread_pool::processLoopTask(*shared_task, thread_number, iterator);
        const uint count = --(*shared_counter);
        if (count == 0) {
          (*finish_loop)();
          delete shared_counter;
          delete finish_loop;
          delete shared_task;
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
bool ThreadPool::isFinished() const noexcept
{
  return is_finished_;
}

} // namespace zisc

#endif // ZISC_THREAD_POOL_INL_HPP
