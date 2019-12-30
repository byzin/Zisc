/*!
  \file thread_manager-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
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
#include <future>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "error.hpp"
#include "non_copyable.hpp"
#include "type_traits.hpp"
#include "std_memory_resource.hpp"
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
  if constexpr (std::is_destructible_v<ResultT>) {
    if (hasValue()) {
      ResultReference v = value();
      std::destroy_at(std::addressof(v));
    }
  }
}

/*!
  */
template <typename T> inline
auto ThreadManager::Result<T>::get() noexcept -> Type
{
  wait();
  if constexpr (!std::is_void_v<Type>)
    return std::move(value());
}

/*!
  */
template <typename T> inline
void ThreadManager::Result<T>::wait() const noexcept
{
  UniqueTask task;
  while (!hasValue()) {
    if (thread_manager_ != nullptr)
      task = thread_manager_->fetchTask();
    if (task) {
      task->run(cast<uint>(thread_id_));
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
void ThreadManager::Result<T>::set(ResultT&& result) noexcept
{
  ZISC_ASSERT(!hasValue(), "The result already has a value.");
  new (&data_) ResultT{std::move(result)};
  has_value_ = kTrue;
}

/*!
  */
template <typename T> inline
auto ThreadManager::Result<T>::value() noexcept -> ResultReference
{
  return *treatAs<ResultT*>(&data_);
}

/*!
  \details
  No detailed.
  */
inline
ThreadManager::ThreadManager(std::pmr::memory_resource* mem_resource) noexcept :
    ThreadManager(logicalCores(), mem_resource)
{
}

/*!
  \details
  No detailed.
  */
inline
ThreadManager::ThreadManager(const uint num_of_threads,
                             std::pmr::memory_resource* mem_resource) noexcept :
    task_queue_{defaultTaskCapacity(), mem_resource},
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
std::array<Integer, 2> ThreadManager::calcThreadRange(
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
template <typename Integer> inline
std::array<Integer, 2> ThreadManager::calcThreadRange(
    const Integer range,
    const uint thread_id) const noexcept
{
  return calcThreadRange(range, numOfThreads(), thread_id);
}

/*!
  */
inline
std::size_t ThreadManager::capacity() const noexcept
{
  const std::size_t cap = task_queue_.capacity();
  return cap;
}

/*!
  */
inline
bool ThreadManager::isEmpty() const noexcept
{
  const bool result = task_queue_.isEmpty();
  return result;
}

/*!
  \details
  No detailed.
  */
inline
uint ThreadManager::logicalCores() noexcept
{
  const uint cores = std::thread::hardware_concurrency();
  return cores;
}

/*!
  \details
  No detailed.

  \return The number of worker threads
  */
inline
uint ThreadManager::numOfThreads() const noexcept
{
  const std::size_t num_of_threads = workers_.size();
  return cast<uint>(num_of_threads);
}

/*!
  */
inline
std::pmr::memory_resource* ThreadManager::resource() const noexcept
{
  auto mem_resource = workers_.get_allocator().resource();
  return mem_resource;
}

/*!
  */
inline
void ThreadManager::setCapacity(const std::size_t cap) noexcept
{
  task_queue_.setCapacity(cap);
}

/*!
  */
inline
int ThreadManager::size() const noexcept
{
  const int s = task_queue_.size();
  return s;
}

/*!
  \details
  No detailed.
  */
inline
void ThreadManager::createWorkers(const uint num_of_threads) noexcept
{
  const uint id_max = (num_of_threads == 0) ? logicalCores() : num_of_threads;
  workers_.reserve(id_max);
  for (uint thread_id = 0; thread_id < id_max; ++thread_id) {
    int32b padding = 0;
    auto work = [this, thread_id, padding]() noexcept
    {
      (void)padding;
      UniqueTask task;
      while (workersAreEnabled()) {
        {
          task = fetchTask();
          if (!task) {
            std::unique_lock<std::mutex> locker{lock_};
            if (workersAreEnabled())
              condition_.wait(locker);
          }
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

/*!
  */
inline
constexpr std::size_t ThreadManager::defaultTaskCapacity() noexcept
{
  const std::size_t cap = 128;
  return cap;
}

/*!
  */
template <typename ReturnType, typename Task> inline
auto ThreadManager::enqueue(
    Task&& task,
    std::pmr::memory_resource* mem_resource,
    EnableIf<std::is_invocable_v<Task>>) noexcept -> UniqueResult<ReturnType>
{
  auto result = enqueueBridge<ReturnType>(std::forward<Task>(task), mem_resource);
  return result;
}

/*!
  */
template <typename ReturnType, typename Task> inline
auto ThreadManager::enqueue(
    Task&& task,
    std::pmr::memory_resource* mem_resource,
    EnableIf<std::is_invocable_v<Task, uint>>) noexcept -> UniqueResult<ReturnType>
{
  auto result = enqueueBridge<ReturnType>(std::forward<Task>(task), mem_resource);
  return result;
}
/*!
  */
template <typename Task, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoop(
    Task&& task,
    Iterator1&& begin,
    Iterator2&& end,
    std::pmr::memory_resource* mem_resource,
    EnableIf<std::is_invocable_v<Task, Iterator1>>)
        noexcept -> UniqueResult<void>
{
  auto result = enqueueLoopBridge1(std::forward<Task>(task),
                                   std::forward<Iterator1>(begin),
                                   std::forward<Iterator2>(end),
                                   mem_resource);
  return result;
}

/*!
  */
template <typename Task, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoop(
    Task&& task,
    Iterator1&& begin,
    Iterator2&& end,
    std::pmr::memory_resource* mem_resource,
    EnableIf<std::is_invocable_v<Task, uint, Iterator1>>)
        noexcept -> UniqueResult<void>
{
  auto result = enqueueLoopBridge1(std::forward<Task>(task),
                                   std::forward<Iterator1>(begin),
                                   std::forward<Iterator2>(end),
                                   mem_resource);
  return result;
}

/*!
  */
template <typename Iterator> inline
uint ThreadManager::distance(Iterator&& begin, Iterator&& end)
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
template <typename ReturnType, typename Task> inline
auto ThreadManager::enqueueBridge(
    Task&& task,
    std::pmr::memory_resource* mem_resource) noexcept -> UniqueResult<ReturnType>
{
  using TaskT = std::remove_cv_t<std::remove_reference_t<Task>>;
  using ResultP = Result<ReturnType>*;

  constexpr std::size_t size = sizeof(TaskT) + sizeof(ResultP);
  constexpr std::size_t alignment = std::max(alignof(TaskT), alignof(ResultP));
  constexpr std::size_t padding_size = ((size % alignment) != 0)
      ? alignment - (size % alignment)
      : 0;

  if constexpr (0 < padding_size) {
    class SingleTask : public WorkerTask
    {
     public:
      SingleTask(Task&& t, ResultP result) noexcept :
          task_{std::forward<Task>(t)},
          result_{result} {(void)padding_;}
      void run(const uint thread_id) noexcept override
      {
        runSingleTask<ReturnType, TaskT>(task_, thread_id, result_);
      }
     private:
      TaskT task_;
      std::array<uint8b, padding_size> padding_;
      ResultP result_;
    };
    return enqueueImpl<SingleTask, ReturnType>(std::forward<Task>(task),
                                               mem_resource);
  }
  else {
    class SingleTask : public WorkerTask
    {
     public:
      SingleTask(Task&& t, ResultP result) noexcept :
          task_{std::forward<Task>(t)},
          result_{result} {}
      void run(const uint thread_id) noexcept override
      {
        runSingleTask<ReturnType, TaskT>(task_, thread_id, result_);
      }
     private:
      TaskT task_;
      ResultP result_;
    };
    return enqueueImpl<SingleTask, ReturnType>(std::forward<Task>(task),
                                               mem_resource);
  }
}

/*!
  */
template <typename SingleTask, typename ReturnType, typename Task> inline
auto ThreadManager::enqueueImpl(
    Task&& task,
    std::pmr::memory_resource* mem_resource) noexcept -> UniqueResult<ReturnType>
{
  // Create a result of loop tasks
  const uint thread_id = getThreadIndex();
  UniqueResult<ReturnType> result = (thread_id != invalidId())
      ? UniqueResult<ReturnType>::make(mem_resource, this, thread_id)
      : UniqueResult<ReturnType>::make(mem_resource);

  // Enqueue a task maker
  using UniqueSingleTask = UniqueMemoryPointer<SingleTask>;
  {
    UniqueTask worker_task = UniqueSingleTask::make(mem_resource,
                                                    std::forward<Task>(task),
                                                    result.get());
    task_queue_.enqueue(std::move(worker_task));
  }
  condition_.notify_one();

  return result;
}

/*!
  */
template <typename Task, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoopBridge1(
    Task&& task,
    Iterator1&& begin,
    Iterator2&& end,
    std::pmr::memory_resource* mem_resource) noexcept -> UniqueResult<void>
{
  using TaskT = std::remove_cv_t<std::remove_reference_t<Task>>;
  using ResultP = Result<void>*;
  using MemoryP = std::pmr::memory_resource*;
  using AtomicT = std::atomic_uint;
  static_assert(alignof(ResultP) == alignof(MemoryP));
  static_assert(alignof(AtomicT) == 4);

  constexpr std::size_t size = sizeof(TaskT) +
                               sizeof(ResultP) +
                               sizeof(MemoryP) +
                               sizeof(AtomicT);
  constexpr std::size_t alignment = std::max({alignof(TaskT),
                                              alignof(ResultP),
                                              alignof(MemoryP),
                                              alignof(AtomicT)});
  constexpr std::size_t padding_size = (size % alignment != 0)
      ? alignment - (size % alignment)
      : 0;

  if constexpr ((alignof(ResultP) <= alignof(TaskT)) && (0 < padding_size)) {
    struct SharedTaskData
    {
      SharedTaskData(Task&& t, ResultP r, const uint c, MemoryP m) noexcept :
          task_{std::forward<Task>(t)},
          result_{r},
          mem_resource_{m},
          counter_{c} {(void)padding_;}
      TaskT task_;
      ResultP result_;
      MemoryP mem_resource_;
      AtomicT counter_;
      std::array<uint8b, padding_size> padding_;
    };
    return enqueueLoopBridge2<SharedTaskData>(std::forward<Task>(task),
                                              std::forward<Iterator1>(begin),
                                              std::forward<Iterator2>(end),
                                              mem_resource);
  }
  else if constexpr ((alignof(TaskT) < alignof(AtomicT)) && (0 < padding_size)) {
    struct SharedTaskData
    {
      SharedTaskData(Task&& t, ResultP r, const uint c, MemoryP m) noexcept :
          result_{r},
          mem_resource_{m},
          counter_{c},
          task_{std::forward<Task>(t)} {(void)padding_;}
      ResultP result_;
      MemoryP mem_resource_;
      AtomicT counter_;
      TaskT task_;
      std::array<uint8b, padding_size> padding_;
    };
    return enqueueLoopBridge2<SharedTaskData>(std::forward<Task>(task),
                                              std::forward<Iterator1>(begin),
                                              std::forward<Iterator2>(end),
                                              mem_resource);
  }
  else {
    struct SharedTaskData
    {
      SharedTaskData(Task&& t, ResultP r, const uint c, MemoryP m) noexcept :
          result_{r},
          mem_resource_{m},
          task_{std::forward<Task>(t)},
          counter_{c} {}
      ResultP result_;
      MemoryP mem_resource_;
      TaskT task_;
      AtomicT counter_;
    };
    return enqueueLoopBridge2<SharedTaskData>(std::forward<Task>(task),
                                              std::forward<Iterator1>(begin),
                                              std::forward<Iterator2>(end),
                                              mem_resource);
  }
}

/*!
  */
template <typename SharedTaskData,
          typename Task, typename Iterator1, typename Iterator2 > inline
auto ThreadManager::enqueueLoopBridge2(
    Task&& task,
    Iterator1&& begin,
    Iterator2&& end,
    std::pmr::memory_resource* mem_resource) noexcept -> UniqueResult<void>
{
  using Iterator = std::remove_cv_t<std::remove_reference_t<Iterator1>>;
  using TaskT = std::remove_cv_t<std::remove_reference_t<Task>>;
  using UniqueSharedData = UniqueMemoryPointer<SharedTaskData>;

  constexpr std::size_t size = sizeof(SharedTaskData*) + sizeof(Iterator);
  constexpr std::size_t alignment = std::max(alignof(SharedTaskData*),
                                             alignof(Iterator));
  constexpr std::size_t padding_size = (size % alignment != 0)
      ? alignment - (size % alignment)
      : 0;

  if constexpr (0 < padding_size) {
    class LoopTask : public WorkerTask
    {
     public:
      LoopTask(SharedTaskData* shared_data, Iterator ite) noexcept :
          shared_data_{shared_data},
          ite_{ite} {(void)padding_;}
      void run(const uint thread_id) noexcept override
      {
        runLoopTask<TaskT, Iterator>(shared_data_->task_, thread_id, ite_);
        // A manager will be notified when all tasks have been completed
        const uint c = --(shared_data_->counter_);
        if (c == 0) {
          UniqueSharedData data{shared_data_, shared_data_->mem_resource_};
          data->result_->set(0);
        }
      }
     private:
      SharedTaskData* shared_data_;
      std::array<uint8b, padding_size> padding_;
      Iterator ite_;
    };
    return enqueueLoopImpl<SharedTaskData, LoopTask>(
        std::forward<Task>(task),
        std::forward<Iterator1>(begin),
        std::forward<Iterator2>(end),
        mem_resource);
  }
  else {
    class LoopTask : public WorkerTask
    {
     public:
      LoopTask(SharedTaskData* shared_data, Iterator ite) noexcept :
          shared_data_{shared_data},
          ite_{ite} {}
      void run(const uint thread_id) noexcept override
      {
        runLoopTask<TaskT, Iterator>(shared_data_->task_, thread_id, ite_);
        // A manager will be notified when all tasks have been completed
        const uint c = --(shared_data_->counter_);
        if (c == 0) {
          UniqueSharedData data{shared_data_, shared_data_->mem_resource_};
          data->result_->set(0);
        }
      }
     private:
      SharedTaskData* shared_data_;
      Iterator ite_;
    };
    return enqueueLoopImpl<SharedTaskData, LoopTask>(
        std::forward<Task>(task),
        std::forward<Iterator1>(begin),
        std::forward<Iterator2>(end),
        mem_resource);
  }
}

/*!
  */
template <typename SharedTaskData, typename LoopTask,
          typename Task, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoopImpl(
    Task&& task,
    Iterator1&& begin,
    Iterator2&& end,
    std::pmr::memory_resource* mem_resource) noexcept -> UniqueResult<void>
{
  // Create a result of loop tasks
  const uint thread_id = getThreadIndex();
  UniqueResult<void> result = (thread_id != invalidId())
      ? UniqueResult<void>::make(mem_resource, this, thread_id)
      : UniqueResult<void>::make(mem_resource);

  // Create a shared data
  using UniqueSharedData = UniqueMemoryPointer<SharedTaskData>;
  const uint d = distance(begin, end);
  auto shared_data = UniqueSharedData::make(mem_resource,
                                            std::forward<Task>(task),
                                            result.get(),
                                            d,
                                            mem_resource).release();

  // Enqueue loop tasks
  using UniqueLoopTask = UniqueMemoryPointer<LoopTask>;
  {
    for (auto ite = begin; ite != end; ++ite) {
      UniqueTask worker_task = UniqueLoopTask::make(mem_resource,
                                                    shared_data,
                                                    ite);
      task_queue_.enqueue(std::move(worker_task));
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
  auto result = task_queue_.dequeue();
  const bool flag = std::get<0>(result);
  if (flag)
    task = std::move(std::get<1>(result));
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

  // Check the alignment of member variables
  static_assert(alignof(pmr::vector<std::thread>) <=
                alignof(LockFreeBoundedQueue<UniqueTask>));
  static_assert(alignof(std::mutex) <= alignof(pmr::vector<std::thread>));
  static_assert(alignof(std::condition_variable) <= alignof(std::mutex));
  static_assert(alignof(uint8b) <= alignof(std::condition_variable));
}

/*!
  */
inline
constexpr uint ThreadManager::invalidId() noexcept
{
  const uint id = std::numeric_limits<uint>::max();
  return id;
}

/*!
  */
template <typename Task, typename Iterator> inline
void ThreadManager::runLoopTask(
    Task& task,
    const uint thread_id,
    Iterator ite) noexcept
{
  if constexpr (std::is_invocable_v<Task, uint, Iterator>) {
    task(thread_id, ite);
  }
  else {
    static_cast<void>(thread_id);
    task(ite);
  }
}

/*!
  */
template <typename ReturnType, typename Task> inline
void ThreadManager::runSingleTask(
    Task& task,
    const uint thread_id,
    Result<ReturnType>* result) noexcept
{
  if constexpr (std::is_invocable_v<Task, uint>) {
    if constexpr (std::is_void_v<ReturnType>) {
      task(thread_id);
      result->set(0);
    }
    else {
      auto value = task(thread_id);
      result->set(std::move(value));
    }
  }
  else {
    static_cast<void>(thread_id);
    if constexpr (std::is_void_v<ReturnType>) {
      task();
      result->set(0);
    }
    else {
      auto value = task();
      result->set(std::move(value));
    }
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
