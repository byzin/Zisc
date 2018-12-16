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
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <deque>
#include <future>
#include <functional>
#include <iterator>
#include <limits>
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
  */
template <typename T> inline
ThreadManager::Result<T>::Result() noexcept :
    Result(nullptr, std::numeric_limits<uint16b>::max())
{
}

/*!
  */
template <typename T> inline
ThreadManager::Result<T>::Result(ThreadManager* manager,
                                 const uint thread_id) noexcept :
    thread_manager_{manager},
    has_value_{kFalse},
    thread_id_{zisc::cast<uint16b>(thread_id)}
{
}

/*!
  */
template <typename T> inline
ThreadManager::Result<T>::~Result() noexcept
{
  if (std::is_destructible_v<ResultType> && hasValue())
    value().~ResultType();
}

/*!
  */
template <typename T> inline
auto ThreadManager::Result<T>::get() noexcept -> Type
{
  if (!hasValue()) {
    wait();
  }
  if constexpr (!std::is_void_v<Type>)
    return std::move(value());
}

/*!
  */
template <typename T> inline
void ThreadManager::Result<T>::wait() const noexcept
{
  while (!hasValue()) {
    UniqueTask task;
    if (thread_manager_ != nullptr) {
      std::unique_lock<std::mutex> locker{thread_manager_->lock_};
      task = thread_manager_->fetchTask();
    }
    if (task) {
      task->run(zisc::cast<uint>(thread_id_));
      task.reset();
    }
    else {
      std::this_thread::yield();
    }
  }
}

/*!
  */
template <typename T> inline
bool ThreadManager::Result<T>::hasValue() const noexcept
{
  return has_value_ == kTrue;
}

/*!
  */
template <typename T> inline
void ThreadManager::Result<T>::set(ResultType&& result) noexcept
{
  ZISC_ASSERT(!hasValue(), "The result already has a value.");
  new (&data_) ResultType{std::move(result)};
  has_value_ = kTrue;
}

/*!
  */
template <typename T> inline
auto ThreadManager::Result<T>::value() noexcept -> ResultReference
{
  return *treatAs<ResultType*>(&data_);
}

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
    task_queue_{pmr::deque<UniqueTask>::allocator_type{mem_resource}},
    workers_{pmr::vector<std::thread>::allocator_type{mem_resource}},
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
template <typename Integer> inline
std::array<Integer, 2> ThreadManager::calcThreadRange(const Integer range,
                                                      const uint num_of_threads,
                                                      const uint thread_id) noexcept
{
  static_assert(kIsInteger<Integer>, "The Integer isn't integer type.");
  ZISC_ASSERT(0 < num_of_threads, "The num of threads is zero.");

  const Integer threads = cast<Integer>(num_of_threads);
  const Integer id = cast<Integer>(thread_id);
  ZISC_ASSERT(threads <= range, "The range is less than the num of threads.");

  const Integer n = range / threads;
  const Integer begin = n * id;
  const Integer end = begin + n + ((id + 1 == threads) ? range - n * threads : 0);
  return {{begin, end}};
}

/*!
  */
template <typename Integer> inline
std::array<Integer, 2> ThreadManager::calcThreadRange(const Integer range,
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
  const uint id_max = (num_of_threads == 0)
      ? cast<uint>(std::thread::hardware_concurrency())
      : num_of_threads;
  workers_.reserve(id_max);
  for (uint thread_id = 0; thread_id < id_max; ++thread_id) {
    auto work = [this, thread_id]() noexcept
    {
      UniqueTask task;
      while (workersAreEnabled()) {
        {
          std::unique_lock<std::mutex> locker{lock_};
          task = fetchTask();
          if (workersAreEnabled() && !task)
            condition_.wait(locker);
        }
        if (task) {
          task->run(thread_id);
          task.reset();
        }
      }
    };
    workers_.emplace_back(work);
  }
  auto comp = [](const std::thread& lhs, const std::thread& rhs)
  {
    return lhs.get_id() < rhs.get_id();
  };
  std::sort(workers_.begin(), workers_.end(), comp);
}

namespace inner {

//! Run a task
template <typename ReturnType, typename Task> inline
ReturnType runTask(Task& task, const uint thread_id) noexcept
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

//! Run a loop task
template <typename Task, typename Iterator> inline
void runTask(Task& worker_task, const uint thread_id, Iterator iterator) noexcept
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
auto ThreadManager::enqueue(
    Task&& task,
    pmr::memory_resource* mem_resource) noexcept -> UniqueResult<ReturnType>
{
  //! Task class for single task
  class SingleTask : public WorkerTask
  {
   public:
    //! Create a task
    SingleTask(Task&& worker_task, Result<ReturnType>* result) noexcept :
        worker_task_{std::forward<Task>(worker_task)},
        result_{result} {}
    //! Run a task
    void run(const uint thread_id) noexcept override
    {
      runTask(worker_task_, result_, thread_id);
    }
    std::remove_reference_t<Task> worker_task_;
    Result<ReturnType>* result_;
  };

  UniqueResult<ReturnType> result;
  const uint thread_id = getThreadIndex();
  // Enqueue the task
  {
    std::unique_lock<std::mutex> locker{lock_};
    result = (thread_id != std::numeric_limits<uint>::max())
        ? UniqueResult<ReturnType>::make(mem_resource, this, thread_id)
        : UniqueResult<ReturnType>::make(mem_resource);
    auto worker_task = UniqueMemoryPointer<SingleTask>::make(
        mem_resource, 
        std::forward<Task>(task),
        result.get());
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
auto ThreadManager::enqueueLoop(
    Task&& task,
    Iterator&& begin,
    Iterator&& end,
    pmr::memory_resource* mem_resource) noexcept -> UniqueResult<void>
{
  //! Resources for shared task
  struct SharedResource : private NonCopyable<SharedResource>
  {
    SharedResource(Task&& worker_task,
                   Result<void>* result,
                   pmr::memory_resource* mem,
                   const uint task_count) noexcept :
        worker_task_{std::forward<Task>(worker_task)},
        result_{result},
        mem_{mem},
        counter_{task_count} {}
    std::remove_reference_t<Task> worker_task_;
    Result<void>* result_;
    pmr::memory_resource* mem_;
    std::atomic<uint> counter_;
    static_assert(alignof(std::atomic<uint>) <= alignof(void*));
  };

  //! Task class for shared task
  class SharedTask : public WorkerTask
  {
   public:
    //! Create a shared task
    SharedTask(SharedResource* shared_resource, Iterator ite) noexcept :
        shared_resource_{shared_resource},
        iterator_{ite} {}
    //! Destroy a shared task
    ~SharedTask() noexcept override
    {
      const uint c = --(shared_resource_->counter_);
      if (c == 0) {
        UniqueMemoryPointer<SharedResource> r{shared_resource_,
                                              shared_resource_->mem_};
        r->result_->set(0);
      }
    }
    //! Run a task
    void run(const uint thread_id) noexcept override
    {
      inner::runTask(shared_resource_->worker_task_, thread_id, iterator_);
    }
    SharedResource* shared_resource_;
    std::remove_reference_t<Iterator> iterator_;
  };

  UniqueResult<void> result;
  const uint thread_id = getThreadIndex();
  const uint distance = inner::distance(begin, end);
  // Make worker tasks
  {
    std::unique_lock<std::mutex> locker{lock_};
    result = (thread_id != std::numeric_limits<uint>::max())
        ? UniqueResult<void>::make(mem_resource, this, thread_id)
        : UniqueResult<void>::make(mem_resource);
    // Make a shared resource
    auto shared_resource = UniqueMemoryPointer<SharedResource>::make(
        mem_resource,
        std::forward<Task>(task),
        result.get(),
        mem_resource,
        distance).release();
    for (auto ite = begin; ite != end; ++ite) {
      auto worker_task = UniqueMemoryPointer<SharedTask>::make(
          mem_resource,
          shared_resource,
          ite);
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
auto ThreadManager::fetchTask() noexcept -> UniqueTask
{
  UniqueTask task;
  if (!task_queue_.empty()) {
    task = std::move(task_queue_.front());
    task_queue_.pop();
  }
  return task;
}

/*!
  */
inline
uint ThreadManager::getThreadIndex() const noexcept
{
  const auto id = std::this_thread::get_id();
  auto comp = [](const std::thread& lhs, const std::thread::id& rhs)
  {
    return lhs.get_id() < rhs;
  };
  const auto t = std::lower_bound(workers_.begin(), workers_.end(), id, comp);
  const bool result = (t != workers_.end()) && (t->get_id() == id);
  const uint thread_id = result
      ? cast<uint>(std::distance(workers_.begin(), t))
      : std::numeric_limits<uint>::max();
  return thread_id;
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
  static_assert(alignof(std::queue<UniqueTask, pmr::deque<UniqueTask>>) <=
                alignof(std::condition_variable));
  static_assert(alignof(pmr::vector<std::thread>) <=
                alignof(std::queue<UniqueTask, pmr::deque<UniqueTask>>));
  static_assert(alignof(uint8b) <=
                alignof(pmr::vector<std::thread>));
}

/*!
  */
template <typename ReturnType, typename Task> inline
void ThreadManager::runTask(Task& task,
                            Result<ReturnType>* result,
                            const uint thread_id) noexcept
{
  if constexpr (std::is_void_v<ReturnType>) {
    inner::runTask<ReturnType>(task, thread_id);
    result->set(0);
  }
  else {
    auto value = inner::runTask<ReturnType>(task, thread_id);
    result->set(std::move(value));
  }
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
