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
#include "function_reference.hpp"
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
WorkerThreadManager<kLockType>::Result<T>::Result(const uint c) noexcept :
    Result(c, nullptr, std::numeric_limits<uint16b>::max())
{
}

/*!
  */
template <ThreadManagerLockType kLockType> template <typename T> inline
WorkerThreadManager<kLockType>::Result<T>::Result(
    const uint c,
    WorkerThreadManager* manager,
    const uint thread_id) noexcept :
        thread_manager_{manager},
        counter_{c},
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
  WorkerTaskStorage storage;
  while (!hasValue()) {
    WorkerTaskPointer task = nullptr;
    if (thread_manager_ != nullptr) {
      std::unique_lock<Lock> locker{thread_manager_->lock_};
      task = thread_manager_->fetchTask(&storage);
    }
    if (task) {
      task->run(zisc::cast<uint>(thread_id_));
      std::destroy_at(task);
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
        task_queue_{typename pmr::deque<UniqueTaskMaker>::allocator_type{mem_resource}},
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
      WorkerTaskStorage storage;
      while (workersAreEnabled()) {
        WorkerTaskPointer task = nullptr;
        {
          std::unique_lock<Lock> locker{lock_};
          task = fetchTask(&storage);
          if constexpr (kLockType == ThreadManagerLockType::kStdMutex) {
            if (workersAreEnabled() && !task)
              wait(&locker);
          }
        }
        if (task) {
          task->run(thread_id);
          std::destroy_at(task);
        }
        else {
          if constexpr (kLockType == ThreadManagerLockType::kSpinLock)
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

  using TaskRef = std::conditional_t<is_id_required,
      FunctionReference<ReturnType (uint)>,
      FunctionReference<ReturnType ()>>;

  //! Represent single task
  class SingleTask : public WorkerTask
  {
   public:
    SingleTask(TaskRef task_ref, Result<ReturnType>* result) noexcept :
        task_ref_{task_ref},
        result_{result} {}
    //! Run a task
    void run(const uint thread_id) noexcept override
    {
      if constexpr (is_id_required) {
        if constexpr (std::is_void_v<ReturnType>) {
          task_ref_(thread_id);
          result_->set(0);
        }
        else {
          auto value = task_ref_(thread_id);
          result_->set(std::move(value));
        }
      }
      else {
        static_cast<void>(thread_id);
        if constexpr (std::is_void_v<ReturnType>) {
          task_ref_();
          result_->set(0);
        }
        else {
          auto value = task_ref_();
          result_->set(std::move(value));
        }
      }
    }
   private:
    TaskRef task_ref_;
    Result<ReturnType>* result_;
    static_assert(alignof(Result<void>*) <= alignof(TaskRef));
  };
  static_assert(sizeof(SingleTask) <= sizeof(WorkerTaskStorage));
  static_assert(alignof(SingleTask) <= alignof(WorkerTaskStorage));

  //! Task maker for single task 
  class SingleTaskMaker : public WorkerTaskMaker
  {
   public:
    SingleTaskMaker(TaskRef task_ref, Result<ReturnType>* result) :
        task_ref_{task_ref},
        result_{result} {}
    bool isCompleted() const noexcept override
    {
      const bool is_completed = result_ == nullptr;
      return is_completed;
    }
    WorkerTaskPointer make(void* memory) noexcept override
    {
      auto single_task = ::new (memory) SingleTask{task_ref_, result_};
      result_ = nullptr;
      return single_task;
    }
   private:
    TaskRef task_ref_;
    Result<ReturnType>* result_;
  };

  // Create a result of loop tasks
  UniqueResult<ReturnType> result;
  {
    const uint thread_id = getThreadIndex();
    result = (thread_id != std::numeric_limits<uint>::max())
        ? UniqueResult<ReturnType>::make(mem_resource, 1u, this, thread_id)
        : UniqueResult<ReturnType>::make(mem_resource, 1u);
  }

  // Create a task maker
  UniqueTaskMaker maker = UniqueMemoryPointer<SingleTaskMaker>::make(
      mem_resource,
      task,
      result.get());

  // Enqueue a task maker
  {
    std::unique_lock<Lock> locker{lock_};
    task_queue_.emplace(std::move(maker));
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
  using TaskType = std::remove_volatile_t<std::remove_reference_t<Task>>;
  static_assert(std::is_same_v<Iterator,
                    std::remove_cv_t<std::remove_reference_t<Iterator2>>>,
                "The Iterator1 and Iterator2 aren't same.");

  constexpr bool is_id_required = std::is_invocable_v<TaskType, uint, Iterator>;

  using TaskRef = std::conditional_t<is_id_required,
      FunctionReference<void (uint, Iterator)>,
      FunctionReference<void (Iterator)>>;

  //! Represent one of loop tasks
  class LoopTask : public WorkerTask
  {
   public:
    LoopTask(TaskRef task_ref, Result<void>* result, Iterator ite) noexcept :
        task_ref_{task_ref},
        result_{result},
        ite_{ite} {}
    //! Run a task
    void run(const uint thread_id) noexcept override
    {
      if constexpr (is_id_required) {
        task_ref_(thread_id, ite_);
      }
      else {
        static_cast<void>(thread_id);
        task_ref_(ite_);
      }
      // Last loop task notifies a manager that loop tasks have been completed
      {
        const uint c = --(result_->counter_);
        if (c == 0)
          result_->set(0);
      }
    }
   private:
    TaskRef task_ref_;
    Result<void>* result_;
    Iterator ite_;
    static_assert(alignof(Result<void>*) <= alignof(TaskRef));
    static_assert(alignof(Iterator) <= alignof(Result<void>*));
  };
  static_assert(sizeof(LoopTask) <= sizeof(WorkerTaskStorage));
  static_assert(alignof(LoopTask) <= alignof(WorkerTaskStorage));

  //! Task maker for loop task 
  class LoopTaskMaker : public WorkerTaskMaker
  {
   public:
    LoopTaskMaker(TaskRef task_ref, Result<void>* result, Iterator b, Iterator e) :
        task_ref_{task_ref},
        result_{result},
        current_{b},
        end_{e} {}
    bool isCompleted() const noexcept override
    {
      const bool result = current_ == end_;
      return result;
    }
    WorkerTaskPointer make(void* memory) noexcept override
    {
      auto ite = current_++;
      auto loop_task = ::new (memory) LoopTask{task_ref_, result_, ite};
      return loop_task;
    }
   private:
    TaskRef task_ref_;
    Result<void>* result_;
    Iterator current_;
    Iterator end_;
  };

  // Create a result of loop tasks
  UniqueResult<void> result;
  {
    const uint d = distance(begin, end);
    const uint thread_id = getThreadIndex();
    result = (thread_id != std::numeric_limits<uint>::max())
        ? UniqueResult<void>::make(mem_resource, d, this, thread_id)
        : UniqueResult<void>::make(mem_resource, d);
  }

  // Create a task maker
  UniqueTaskMaker maker = UniqueMemoryPointer<LoopTaskMaker>::make(
      mem_resource,
      task,
      result.get(),
      std::forward<Iterator1>(begin),
      std::forward<Iterator2>(end));

  // Enqueue a task maker
  {
    std::unique_lock<Lock> locker{lock_};
    task_queue_.emplace(std::move(maker));
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
auto WorkerThreadManager<kLockType>::fetchTask(void* memory) noexcept
    -> WorkerTaskPointer
{
  WorkerTaskPointer task = nullptr;
  if (!task_queue_.empty()) {
    auto& maker = task_queue_.front();
    task = maker->make(memory);
    if (maker->isCompleted())
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

  // Avoid padding warning
  static_cast<void>(padding_);

  // Check the alignment of member variables
  static_assert(alignof(pmr::vector<std::thread>) <=
                alignof(std::queue<UniqueTaskMaker, pmr::deque<UniqueTaskMaker>>));
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
