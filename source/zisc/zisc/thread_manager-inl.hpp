/*!
  \file thread_manager-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
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
#include "spin_lock_mutex.hpp"
#include "unique_memory_pointer.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <ThreadManagerLockType kLockType> template  <typename T> inline
WorkerThreadManager<kLockType>::Result<T>::Result() noexcept :
    Result(nullptr, std::numeric_limits<uint16b>::max())
{
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename T> inline
WorkerThreadManager<kLockType>::Result<T>::Result(
    WorkerThreadManager* manager,
    const uint thread_id) noexcept :
        thread_manager_{manager},
        has_value_{kFalse},
        thread_id_{zisc::cast<uint16b>(thread_id)}
{
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename T> inline
WorkerThreadManager<kLockType>::Result<T>::~Result() noexcept
{
  if constexpr (std::is_destructible_v<ResultType>) {
    if (hasValue()) {
      auto& v = value();
      std::destroy_at(&v);
    }
  }
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename T> inline
auto WorkerThreadManager<kLockType>::Result<T>::get() noexcept -> Type
{
  if (!hasValue())
    wait();
  if constexpr (!std::is_void_v<Type>)
    return std::move(value());
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename T> inline
void WorkerThreadManager<kLockType>::Result<T>::wait() const noexcept
{
  UniqueTask task;
  while (!hasValue()) {
    if (thread_manager_ != nullptr) {
      std::unique_lock<Lock> locker{thread_manager_->lock_};
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
template <ThreadManagerLockType kLockType> template <typename T> inline
bool WorkerThreadManager<kLockType>::Result<T>::hasValue() const noexcept
{
  return has_value_ == kTrue;
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename T> inline
void WorkerThreadManager<kLockType>::Result<T>::set(ResultType&& result) noexcept
{
  ZISC_ASSERT(!hasValue(), "The result already has a value.");
  new (&data_) ResultType{std::move(result)};
  has_value_ = kTrue;
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename T> inline
auto WorkerThreadManager<kLockType>::Result<T>::value() noexcept -> ResultReference
{
  return *treatAs<ResultType*>(&data_);
}

/*!
  \details
  No detailed.
  */
template <ThreadManagerLockType kLockType> inline
WorkerThreadManager<kLockType>::WorkerThreadManager(
    pmr::memory_resource* mem_resource) noexcept :
        WorkerThreadManager(std::thread::hardware_concurrency(), mem_resource)
{
}

/*!
  \details
  No detailed.
  */
template <ThreadManagerLockType kLockType> inline
WorkerThreadManager<kLockType>::WorkerThreadManager(
    const uint num_of_threads,
    pmr::memory_resource* mem_resource) noexcept :
        task_queue_{typename pmr::deque<UniqueTask>::allocator_type{mem_resource}},
        workers_{pmr::vector<std::thread>::allocator_type{mem_resource}},
        workers_are_enabled_{kTrue}
{
  initialize(num_of_threads);
}

/*!
  \details
  No detailed.
  */
template <ThreadManagerLockType kLockType> inline
WorkerThreadManager<kLockType>::~WorkerThreadManager()
{
  exitWorkersRunning();
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename Integer> inline
std::array<Integer, 2> WorkerThreadManager<kLockType>::calcThreadRange(
    const Integer range,
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
template <ThreadManagerLockType kLockType> template <typename Integer> inline
std::array<Integer, 2> WorkerThreadManager<kLockType>::calcThreadRange(
    const Integer range,
    const uint thread_id) const noexcept
{
  return calcThreadRange(range, numOfThreads(), thread_id);
}

/*!
  \details
  No detailed.
  */
template <ThreadManagerLockType kLockType> inline
uint WorkerThreadManager<kLockType>::logicalCores() noexcept
{
  return cast<uint>(std::thread::hardware_concurrency());
}

/*!
  \details
  No detailed.

  \return The number of worker threads
  */
template <ThreadManagerLockType kLockType> inline
uint WorkerThreadManager<kLockType>::numOfThreads() const noexcept
{
  return cast<uint>(workers_.size());
}

/*!
  \details
  No detailed.
  */
template <ThreadManagerLockType kLockType> inline
void WorkerThreadManager<kLockType>::createWorkers(const uint num_of_threads)
    noexcept
{
  const uint id_max = (num_of_threads == 0)
      ? cast<uint>(std::thread::hardware_concurrency())
      : num_of_threads;
  workers_.reserve(id_max);
  for (uint thread_id = 0; thread_id < id_max; ++thread_id) {
    auto work = [this, thread_id]() noexcept
    {
      using LockType = ThreadManagerLockType;
      UniqueTask task;
      while (workersAreEnabled()) {
        {
          std::unique_lock<Lock> locker{lock_};
          task = fetchTask();
          if ((kLockType == LockType::kStdMutex) && workersAreEnabled() && !task)
              wait(&locker);
        }
        if (task) {
          task->run(thread_id);
          task.reset();
        }
        else if (kLockType == LockType::kSpinLock) {
          wait();
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

/*!
  */
template <ThreadManagerLockType kLockType>
template <typename ReturnType, typename Task> inline
auto WorkerThreadManager<kLockType>::enqueue(
    Task&& task,
    pmr::memory_resource* mem_resource,
    EnableIf<std::is_invocable_v<Task>>) noexcept -> UniqueResult<ReturnType>
{
  auto result = enqueueTask<ReturnType>(std::forward<Task>(task), mem_resource);
  return result;
}

/*!
  */
template <ThreadManagerLockType kLockType>
template <typename ReturnType, typename Task> inline
auto WorkerThreadManager<kLockType>::enqueue(
    Task&& task,
    pmr::memory_resource* mem_resource,
    EnableIf<std::is_invocable_v<Task, uint>>) noexcept -> UniqueResult<ReturnType>
{
  auto result = enqueueTask<ReturnType>(std::forward<Task>(task), mem_resource);
  return result;
}
/*!
  */
template <ThreadManagerLockType kLockType>
template <typename Task, typename Iterator1, typename Iterator2> inline
auto WorkerThreadManager<kLockType>::enqueueLoop(
    Task&& task,
    Iterator1&& begin,
    Iterator2&& end,
    pmr::memory_resource* mem_resource,
    EnableIf<std::is_invocable_v<Task, Iterator1>>)
        noexcept -> UniqueResult<void>
{
  auto result = enqueueLoopTask(std::forward<Task>(task),
                                std::forward<Iterator1>(begin),
                                std::forward<Iterator2>(end),
                                mem_resource);
  return result;
}

/*!
  */
template <ThreadManagerLockType kLockType>
template <typename Task, typename Iterator1, typename Iterator2> inline
auto WorkerThreadManager<kLockType>::enqueueLoop(
    Task&& task,
    Iterator1&& begin,
    Iterator2&& end,
    pmr::memory_resource* mem_resource,
    EnableIf<std::is_invocable_v<Task, uint, Iterator1>>)
        noexcept -> UniqueResult<void>
{
  auto result = enqueueLoopTask(std::forward<Task>(task),
                                std::forward<Iterator1>(begin),
                                std::forward<Iterator2>(end),
                                mem_resource);
  return result;
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename Iterator> inline
uint WorkerThreadManager<kLockType>::distance(Iterator&& begin, Iterator&& end)
    noexcept
{
  using IteratorType = std::remove_cv_t<std::remove_reference_t<Iterator>>;
  if constexpr (std::is_arithmetic_v<IteratorType>) {
    ZISC_ASSERT(begin < end, "The end is ahead of the begin.");
    const auto d = cast<uint>(end - begin);
    return d;
  }
  else {
    ZISC_ASSERT(begin != end, "The end is same as the begin.");
    const auto d = cast<uint>(std::distance(begin, end));
    return d;
  }
}

/*!
  */
template <ThreadManagerLockType kLockType>
template <typename ReturnType, typename Task> inline
auto WorkerThreadManager<kLockType>::enqueueTask(
    Task&& task,
    pmr::memory_resource* mem_resource) noexcept -> UniqueResult<ReturnType>
{
  using TaskType = std::remove_volatile_t<std::remove_reference_t<Task>>;
  constexpr bool is_id_required = std::is_invocable_v<TaskType, uint>;

  //! Represent single task
  class SingleTask : public WorkerTask
  {
   public:
    SingleTask(Task&& t, Result<ReturnType>* result) noexcept :
        task_{std::forward<Task>(t)},
        result_{result} {}
    //! Run a task
    void run(const uint thread_id) noexcept override
    {
      if constexpr (is_id_required) {
        if constexpr (std::is_void_v<ReturnType>) {
          task_(thread_id);
          result_->set(0);
        }
        else {
          auto value = task_(thread_id);
          result_->set(std::move(value));
        }
      }
      else {
        static_cast<void>(thread_id);
        if constexpr (std::is_void_v<ReturnType>) {
          task_();
          result_->set(0);
        }
        else {
          auto value = task_();
          result_->set(std::move(value));
        }
      }
    }
   private:
    TaskType task_;
    Result<ReturnType>* result_;
  };
  using UniqueSingleTask = UniqueMemoryPointer<SingleTask>;

  // Create a result of loop tasks
  UniqueResult<ReturnType> result;
  {
    const uint thread_id = getThreadIndex();
    result = (thread_id != std::numeric_limits<uint>::max())
        ? UniqueResult<ReturnType>::make(mem_resource, this, thread_id)
        : UniqueResult<ReturnType>::make(mem_resource);
  }

  // Enqueue a task maker
  {
    UniqueTask worker_task = UniqueSingleTask::make(mem_resource,
                                                    std::forward<Task>(task),
                                                    result.get());
    std::unique_lock<Lock> locker{lock_};
    task_queue_.emplace(std::move(worker_task));
  }
  notifyOne();

  return result;
}

/*!
  */
template <ThreadManagerLockType kLockType>
template <typename Task, typename Iterator1, typename Iterator2> inline
auto WorkerThreadManager<kLockType>::enqueueLoopTask(
    Task&& task,
    Iterator1&& begin,
    Iterator2&& end,
    pmr::memory_resource* mem_resource) noexcept -> UniqueResult<void>
{
  using Iterator = std::remove_cv_t<std::remove_reference_t<Iterator1>>;
  using TaskType = std::remove_cv_t<std::remove_reference_t<Task>>;
  constexpr bool is_id_required = std::is_invocable_v<TaskType, uint, Iterator>;

  //! Shared data by loop tasks
  struct SharedTaskData
  {
    //! Create a shared task data
    SharedTaskData(Task&& t,
                   Result<void>* result,
                   const uint c,
                   pmr::memory_resource* mem_resource) noexcept :
        task_{std::forward<Task>(t)},
        result_{result},
        mem_resource_{mem_resource},
        counter_{c} {}
    TaskType task_;
    Result<void>* result_;
    pmr::memory_resource* mem_resource_;
    std::atomic<uint> counter_;
    static_assert(alignof(pmr::memory_resource) <= alignof(Result<void>*));
    static_assert(alignof(std::atomic<uint>) <= alignof(pmr::memory_resource));
  };

  //! Represent one of loop tasks
  class LoopTask : public WorkerTask
  {
   public:
    LoopTask(SharedTaskData* shared_data, Iterator ite) noexcept :
        shared_data_{shared_data},
        ite_{ite} {}
    //! Run a task
    void run(const uint thread_id) noexcept override
    {
      if constexpr (is_id_required) {
        shared_data_->task_(thread_id, ite_);
      }
      else {
        static_cast<void>(thread_id);
        shared_data_->task_(ite_);
      }
      // A manager will be notified when all tasks have been completed
      {
        const uint c = --(shared_data_->counter_);
        if (c == 0) {
          UniqueMemoryPointer<SharedTaskData> r{shared_data_,
                                                shared_data_->mem_resource_};
          r->result_->set(0);
        }
      }
    }
   private:
    SharedTaskData* shared_data_;
    Iterator ite_;
  };
  using UniqueLoopTask = UniqueMemoryPointer<LoopTask>;

  // Create a result of loop tasks
  UniqueResult<void> result;
  {
    const uint thread_id = getThreadIndex();
    result = (thread_id != std::numeric_limits<uint>::max())
        ? UniqueResult<void>::make(mem_resource, this, thread_id)
        : UniqueResult<void>::make(mem_resource);
  }

  // Create a shared data
  using UniqueSharedData = UniqueMemoryPointer<SharedTaskData>;
  const uint d = distance(begin, end);
  auto shared_data = UniqueSharedData::make(mem_resource,
                                            std::forward<Task>(task),
                                            result.get(),
                                            d,
                                            mem_resource).release();

  // Enqueue loop tasks
  {
    std::unique_lock<Lock> locker{lock_};
    for (auto ite = begin; ite != end; ++ite) {
      UniqueTask worker_task = UniqueLoopTask::make(mem_resource, shared_data, ite);
      task_queue_.emplace(std::move(worker_task));
    }
  }
  notifyAll();

  return result;
}

/*!
  */
template <ThreadManagerLockType kLockType> inline
void WorkerThreadManager<kLockType>::exitWorkersRunning() noexcept
{
  {
    std::unique_lock<Lock> locker{lock_};
    workers_are_enabled_ = kFalse;
  }
  notifyAll();
  for (auto& worker : workers_)
    worker.join();
}

/*!
  */
template <ThreadManagerLockType kLockType> inline
auto WorkerThreadManager<kLockType>::fetchTask() noexcept -> UniqueTask 
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
template <ThreadManagerLockType kLockType> inline
uint WorkerThreadManager<kLockType>::getThreadIndex() const noexcept
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
template <ThreadManagerLockType kLockType> inline
void WorkerThreadManager<kLockType>::initialize(const uint num_of_threads) noexcept
{
  createWorkers(num_of_threads);

  // Check the alignment of member variables
  static_assert(alignof(pmr::vector<std::thread>) <=
                alignof(std::queue<UniqueTask, pmr::deque<UniqueTask>>));
  static_assert(alignof(Lock) <= alignof(pmr::vector<std::thread>));
  static_assert(alignof(Condition) <= alignof(Lock));
  static_assert(alignof(uint8b) <= alignof(Condition));
}

/*!
  */
template <ThreadManagerLockType kLockType> inline
void WorkerThreadManager<kLockType>::notifyAll() noexcept
{
  if constexpr (kLockType == ThreadManagerLockType::kSpinLock) {
  }
  else {
    // std::mutex
    condition_.notify_all();
  }
}

/*!
  */
template <ThreadManagerLockType kLockType> inline
void WorkerThreadManager<kLockType>::notifyOne() noexcept
{
  if constexpr (kLockType == ThreadManagerLockType::kSpinLock) {
  }
  else {
    // std::mutex
    condition_.notify_one();
  }
}

/*!
  */
template <ThreadManagerLockType kLockType> inline
void WorkerThreadManager<kLockType>::wait(std::unique_lock<Lock>* locker) noexcept
{
  if constexpr (kLockType == ThreadManagerLockType::kSpinLock) {
    static_cast<void>(locker);
    std::this_thread::yield();
  }
  else {
    // std::mutex
    condition_.wait(*locker);
  }
}

/*!
  */
template <ThreadManagerLockType kLockType> inline
bool WorkerThreadManager<kLockType>::workersAreEnabled() const noexcept
{
  return workers_are_enabled_ == kTrue;
}

} // namespace zisc

#endif // ZISC_THREAD_MANAGER_INL_HPP
