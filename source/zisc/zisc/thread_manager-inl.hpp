/*!
  \file thread_manager-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
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
#include "atomic.hpp"
#include "error.hpp"
#include "type_traits.hpp"
#include "std_memory_resource.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename T> inline
ThreadManager::Result<T>::Result() noexcept :
    Result(nullptr, std::numeric_limits<uint16b>::max())
{
}

/*!
  \details No detailed description

  \param [in] manager No description.
  \param [in] thread_id No description.
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
  \details No detailed description
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
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto ThreadManager::Result<T>::get() -> Type
{
  wait();
  if constexpr (!std::is_void_v<Type>)
    return std::move(value());
}

/*!
  \details No detailed description
  */
template <typename T> inline
void ThreadManager::Result<T>::wait() const
{
  while (!hasValue()) {
    bool has_task = false;
    if (thread_manager_ != nullptr) {
      UniqueTask task = thread_manager_->fetchTask();
      has_task = task != nullptr;
      if (has_task) {
        Atomic::decrement(std::addressof(thread_manager_->lock_.get()));
        task->run(cast<uint>(thread_id_));
      }
    }
    if (!has_task) {
      std::this_thread::yield();
    }
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
bool ThreadManager::Result<T>::hasValue() const noexcept
{
  return has_value_ == kTrue;
}

/*!
  \details No detailed description

  \param [in] result No description.
  */
template <typename T> inline
void ThreadManager::Result<T>::set(ResultT&& result) noexcept
{
  ZISC_ASSERT(!hasValue(), "The result already has a value.");
  new (&data_) ResultT{std::move(result)};
  has_value_ = kTrue;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto ThreadManager::Result<T>::value() noexcept -> ResultReference
{
  return *treatAs<ResultT*>(&data_);
}

/*!
  \details No detailed description

  \param [in] what_arg No description.
  */
inline
ThreadManager::OverflowError::OverflowError(const std::string_view what_arg) :
    SystemError(ErrorCode::kThreadManagerQueueOverflow, what_arg)
{
}

/*!
  \details No detailed description
  */
inline
ThreadManager::OverflowError::~OverflowError() noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
ThreadManager::ThreadManager(pmr::memory_resource* mem_resource) noexcept :
    ThreadManager(logicalCores(), mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] num_of_threads No description.
  \param [in,out] mem_resource No description.
  */
inline
ThreadManager::ThreadManager(const uint num_of_threads,
                             pmr::memory_resource* mem_resource) noexcept :
    task_queue_{defaultTaskCapacity(), mem_resource},
    workers_{pmr::vector<std::thread>::allocator_type{mem_resource}},
    lock_{0}
{
  initialize(num_of_threads);
}

/*!
  \details No detailed description
  */
inline
ThreadManager::~ThreadManager()
{
  exitWorkersRunning();
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] range No description.
  \param [in] num_of_threads No description.
  \param [in] thread_id No description.
  \return No description
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
  \details No detailed description

  \tparam Integer No description.
  \param [in] range No description.
  \param [in] thread_id No description.
  \return No description
  */

template <typename Integer> inline
std::array<Integer, 2> ThreadManager::calcThreadRange(
    const Integer range,
    const uint thread_id) const noexcept
{
  return calcThreadRange(range, numOfThreads(), thread_id);
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t ThreadManager::capacity() const noexcept
{
  const std::size_t cap = task_queue_.capacity();
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::defaultTaskCapacity() noexcept
{
  const std::size_t cap = 1024;
  return cap;
}

/*!
  \details No detailed description

  \tparam ReturnType No description.
  \tparam Task No description.
  \param [in] task No description.
  \return No description
  */
template <typename ReturnType, typename Task> inline
auto ThreadManager::enqueue(Task&& task, EnableIfInvocable<Task>)
    -> UniqueResult<ReturnType>
{
  auto result = enqueueImpl<ReturnType>(std::forward<Task>(task));
  return result;
}

/*!
  \details No detailed description

  \tparam ReturnType No description.
  \tparam Task No description.
  \param [in] task No description.
  \return No description
  */
template <typename ReturnType, typename Task> inline
auto ThreadManager::enqueue(Task&& task, EnableIfInvocable<Task, uint>)
    -> UniqueResult<ReturnType>
{
  auto result = enqueueImpl<ReturnType>(std::forward<Task>(task));
  return result;
}

/*!
  \details No detailed description

  \tparam Task No description.
  \tparam Iterator1 No description.
  \tparam Iterator2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
template <typename Task, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoop(Task&& task,
                                Iterator1&& begin,
                                Iterator2&& end,
                                EnableIfInvocable<Task, Iterator1>)
    -> UniqueResult<void>
{
  auto result = enqueueLoopImpl(std::forward<Task>(task),
                                std::forward<Iterator1>(begin),
                                std::forward<Iterator2>(end));
  return result;
}

/*!
  \details No detailed description

  \tparam Task No description.
  \tparam Iterator1 No description.
  \tparam Iterator2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
template <typename Task, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoop(Task&& task,
                                Iterator1&& begin,
                                Iterator2&& end,
                                EnableIfInvocable<Task, uint, Iterator1>)
    -> UniqueResult<void>
{
  auto result = enqueueLoopImpl(std::forward<Task>(task),
                                std::forward<Iterator1>(begin),
                                std::forward<Iterator2>(end));
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool ThreadManager::isEmpty() const noexcept
{
  const bool result = task_queue_.isEmpty();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
uint ThreadManager::logicalCores() noexcept
{
  const uint cores = std::thread::hardware_concurrency();
  return cores;
}

/*!
  \details No detailed description

  \return The number of worker threads
  */
inline
uint ThreadManager::numOfThreads() const noexcept
{
  const std::size_t num_of_threads = workers_.size();
  return cast<uint>(num_of_threads);
}

/*!
  \details No detailed description

  \return No description
  */
inline
pmr::memory_resource* ThreadManager::resource() const noexcept
{
  auto mem_resource = workers_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
inline
void ThreadManager::setCapacity(const std::size_t cap) noexcept
{
  task_queue_.setCapacity(cap);
}

/*!
  \details No detailed description

  \return No description
  */
inline
int ThreadManager::size() const noexcept
{
  const int s = task_queue_.size();
  return s;
}

/*!
  \details No detailed description

  \param [in] num_of_threads No description.
  */
inline
void ThreadManager::createWorkers(const uint num_of_threads) noexcept
{
  const uint id_max = (num_of_threads == 0) ? logicalCores() : num_of_threads;
  workers_.reserve(id_max);
  for (uint thread_id = 0; thread_id < id_max; ++thread_id) {
    int32b padding = 0;
    auto work = [this, thread_id, padding]()
    {
      static_cast<void>(padding);
      while (workersAreEnabled()) {
        UniqueTask task = fetchTask();
        if (task) {
          Atomic::decrement(std::addressof(lock_.get()));
          task->run(thread_id);
        }
        else if (0 < lock_.get()) {
          std::this_thread::yield();
        }
        else {
          Atomic::wait(std::addressof(lock_), 0);
        }
      }
    };
    workers_.emplace_back(work);
  }
  // Sort workers by their IDs
  auto comp = [](const std::thread& lhs, const std::thread& rhs) noexcept
  {
    return lhs.get_id() < rhs.get_id();
  };
  std::sort(workers_.begin(), workers_.end(), comp);
}

/*!
  \details No detailed description

  \tparam Iterator1 No description.
  \tparam Iterator2 No description.
  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
template <typename Iterator1, typename Iterator2> inline
uint ThreadManager::distance(Iterator1&& begin, Iterator2&& end) noexcept
{
  using CommonIterator = std::common_type_t<Iterator1, Iterator2>;
  using Iterator = std::remove_cv_t<std::remove_reference_t<CommonIterator>>;
  if constexpr (std::is_arithmetic_v<Iterator>) {
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
  \details No detailed description

  \tparam ReturnType No description.
  \tparam Task No description.
  \param [in] task No description.
  \return No description
  */
template <typename ReturnType, typename Task> inline
auto ThreadManager::enqueueImpl(Task&& task) -> UniqueResult<ReturnType>
{
  using TaskT = std::remove_cv_t<std::remove_reference_t<Task>>;
  using ResultP = ResultPointer<ReturnType>;
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_CLANG || Z_GCC
  class SingleTask : public WorkerTask
  {
   public:
    SingleTask(Task&& t, ResultP result) noexcept : task_{std::forward<Task>(t)},
                                                    result_{result} {}
    void run(const uint thread_id) override
    {
      runSingleTask<ReturnType, TaskT>(task_, thread_id, result_);
    }

    TaskT task_;
    ResultP result_;
  };

  using UniqueResultT = UniqueResult<ReturnType>;
  class SingleTaskOverflowError : public OverflowError
  {
   public:
    SingleTaskOverflowError(const std::string_view what_arg,
                            pmr::memory_resource* mem_resource,
                            TaskT&& t,
                            UniqueResultT&& result) :
        OverflowError(what_arg),
        task_{std::allocate_shared<TaskT>(
            pmr::polymorphic_allocator<TaskT>{mem_resource},
            std::move(t))},
        result_{std::allocate_shared<UniqueResultT>(
            pmr::polymorphic_allocator<UniqueResultT>{mem_resource},
            std::move(result))} {}
    ~SingleTaskOverflowError() noexcept override
    {
    }
    void* begin() noexcept override
    {
      return nullptr;
    }
    const void* begin() const noexcept override
    {
      return nullptr;
    }
    void* end() noexcept override
    {
      return nullptr;
    }
    const void* end() const noexcept override
    {
      return nullptr;
    }
    bool hasIterator() const noexcept override
    {
      return false;
    }
    void* result() noexcept override
    {
      return result_->get();
    }
    const void* result() const noexcept override
    {
      return result_->get();
    }
    void* task() noexcept override
    {
      return task_.get();
    }
    const void* task() const noexcept override
    {
      return task_.get();
    }

   private:
    std::shared_ptr<TaskT> task_;
    std::shared_ptr<UniqueResultT> result_;
  };
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_CLANG || Z_GCC

  // Create a result of loop tasks
  const uint thread_id = getThreadIndex();
  UniqueResult<ReturnType> result = (thread_id != invalidId())
      ? pmr::allocateUnique(resultAllocator<ReturnType>(), this, thread_id)
      : pmr::allocateUnique(resultAllocator<ReturnType>());

  // Enqueue a task maker
  {
    auto worker_task = pmr::allocateUnique<SingleTask>(resource(),
                                                       std::forward<Task>(task),
                                                       result.get());
    Atomic::increment(std::addressof(lock_.get()));
    try {
      task_queue_.enqueue(std::move(worker_task));
    }
    catch (const LockFreeBoundedQueue<UniqueTask>::OverflowError& /* error */) {
      Atomic::decrement(std::addressof(lock_.get()));
      throw SingleTaskOverflowError{"Task queue overflow happened.",
                                    resource(),
                                    std::move(worker_task->task_),
                                    std::move(result)};
    }
  }
  Atomic::notifyOne(std::addressof(lock_));

  return result;
}

/*!
  \details No detailed description

  \tparam Task No description.
  \tparam Iterator1 No description.
  \tparam Iterator2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
template <typename Task, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoopImpl(Task&& task,
                                    Iterator1&& begin,
                                    Iterator2&& end) -> UniqueResult<void>
{
  using TaskT = std::remove_cv_t<std::remove_reference_t<Task>>;
  using ResultP = ResultPointer<void>;
  using MemoryP = pmr::memory_resource*;
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_CLANG || Z_GCC
  struct SharedTaskData
  {
    SharedTaskData(Task&& t, ResultP r, const int c, MemoryP m) noexcept :
        task_{std::forward<Task>(t)},
        result_{r},
        mem_resource_{m},
        counter_{c} {}
    TaskT task_;
    ResultP result_;
    MemoryP mem_resource_;
    std::atomic_int counter_;
  };

  using CommonIterator = std::common_type_t<Iterator1, Iterator2>;
  using Iterator = std::remove_cv_t<std::remove_reference_t<CommonIterator>>;
  using DataAllocator = pmr::polymorphic_allocator<SharedTaskData>;
  class LoopTask : public WorkerTask
  {
   public:
    LoopTask(SharedTaskData* shared_data, Iterator ite) noexcept :
        shared_data_{shared_data},
        ite_{ite} {}
    ~LoopTask() noexcept override
    {
      // A manager will be notified when all tasks have been completed
      const int c = --(shared_data_->counter_);
      ZISC_ASSERT(0 <= c, "The shared task count has minus value: ", c);
      if (c == 0) {
        MemoryP mem = shared_data_->mem_resource_;
        shared_data_->result_->set(0);
        if (mem != nullptr) {
          pmr::UniquePtrDeleter<SharedTaskData> deleter{DataAllocator{mem}};
          deleter(shared_data_);
        }
      }
    }
    void run(const uint thread_id) override
    {
      runLoopTask<TaskT, Iterator>(shared_data_->task_, thread_id, ite_);
    }

    SharedTaskData* shared_data_;
    Iterator ite_;
  };

  using UniqueTaskData = pmr::unique_ptr<SharedTaskData>;
  using UniqueResultT = UniqueResult<void>;
  class LoopTaskOverflowError : public OverflowError
  {
   public:
    LoopTaskOverflowError(const std::string_view what_arg,
                          pmr::memory_resource* mem_resource,
                          SharedTaskData* shared_data,
                          UniqueResultT&& result,
                          Iterator ite_begin,
                          Iterator ite_end) :
        OverflowError(what_arg),
        shared_data_{std::allocate_shared<UniqueTaskData>(
            pmr::polymorphic_allocator<UniqueTaskData>{mem_resource},
            UniqueTaskData{shared_data, DataAllocator{shared_data->mem_resource_}})},
        result_{std::allocate_shared<UniqueResultT>(
            pmr::polymorphic_allocator<UniqueResultT>{mem_resource},
            std::move(result))},
        begin_{ite_begin},
        end_{ite_end}
    {
      (*shared_data_)->mem_resource_ = nullptr;
    }
    ~LoopTaskOverflowError() noexcept override
    {
    }
    void* begin() noexcept override
    {
      return std::addressof(begin_);
    }
    const void* begin() const noexcept override
    {
      return std::addressof(begin_);
    }
    void* end() noexcept override
    {
      return std::addressof(end_);
    }
    const void* end() const noexcept override
    {
      return std::addressof(end_);
    }
    bool hasIterator() const noexcept override
    {
      return true;
    }
    void* result() noexcept override
    {
      return result_->get();
    }
    const void* result() const noexcept override
    {
      return result_->get();
    }
    void* task() noexcept override
    {
      return std::addressof((*shared_data_)->task_);
    }
    const void* task() const noexcept override
    {
      return std::addressof((*shared_data_)->task_);
    }
   private:
    std::shared_ptr<UniqueTaskData> shared_data_;
    std::shared_ptr<UniqueResultT> result_;
    Iterator begin_,
             end_;
  };
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_CLANG || Z_GCC

  MemoryP mem_resource = resource();

  // Create a result of loop tasks
  const uint thread_id = getThreadIndex();
  UniqueResult<void> result = (thread_id != invalidId())
      ? pmr::allocateUnique(resultAllocator<void>(), this, thread_id)
      : pmr::allocateUnique(resultAllocator<void>());

  // Create a shared data
  const int d = cast<int>(distance(begin, end));
  auto shared_data = pmr::allocateUnique<SharedTaskData>(
      mem_resource,
      std::forward<Task>(task),
      result.get(),
      d,
      mem_resource).release();

  // Enqueue loop tasks
  {
    Atomic::add(std::addressof(lock_.get()), d);
    for (Iterator ite = begin; ite != end; ++ite) {
      auto worker_task = pmr::allocateUnique<LoopTask>(mem_resource,
                                                       shared_data,
                                                       ite);
      try {
        task_queue_.enqueue(std::move(worker_task));
      }
      catch (const LockFreeBoundedQueue<UniqueTask>::OverflowError& /* error */) {
        const int rest = cast<int>(distance(ite, end));
        shared_data->counter_ -= (rest - 1);
        Atomic::sub(std::addressof(lock_.get()), rest);
        Atomic::notifyAll(std::addressof(lock_));
        throw LoopTaskOverflowError{"Task queue overflow happened.",
                                    mem_resource,
                                    shared_data,
                                    std::move(result),
                                    ite,
                                    end};
      }
    }
  }
  Atomic::notifyAll(std::addressof(lock_));

  return result;
}

/*!
  \details No detailed description
  */
inline
void ThreadManager::exitWorkersRunning() noexcept
{
  lock_.set(-1);
  Atomic::notifyAll(std::addressof(lock_));
  for (auto& worker : workers_)
    worker.join();
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::fetchTask() noexcept -> UniqueTask 
{
  auto result = task_queue_.dequeue();
  const bool flag = std::get<0>(result);
  UniqueTask task = flag ? std::move(std::get<1>(result)) : UniqueTask{};
  return task;
}

/*!
  \details No detailed description

  \return No description
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
  \details No detailed description

  \param [in] num_of_threads No description.
  */
inline
void ThreadManager::initialize(const uint num_of_threads) noexcept
{
  createWorkers(num_of_threads);

  // Check the alignment of member variables
  static_assert(std::alignment_of_v<decltype(workers_)> <=
                std::alignment_of_v<decltype(task_queue_)>);
  static_assert(std::alignment_of_v<decltype(lock_)> <=
                std::alignment_of_v<decltype(workers_)>);
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr uint ThreadManager::invalidId() noexcept
{
  const uint id = std::numeric_limits<uint>::max();
  return id;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \return No description
  */
template <typename Type> inline
auto ThreadManager::resultAllocator() const noexcept
    -> pmr::polymorphic_allocator<Result<Type>>
{
  pmr::polymorphic_allocator<Result<Type>> alloc{resource()};
  return alloc;
}

/*!
  \details No detailed description

  \tparam Task No description.
  \tparam Iterator No description.
  \param [in] task No description.
  \param [in] thread_id No description.
  \param [in] ite No description.
  */
template <typename Task, typename Iterator> inline
void ThreadManager::runLoopTask(Task& task,
                                const uint thread_id,
                                Iterator ite)
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
  \details No detailed description

  \tparam ReturnType No description.
  \tparam Task No description.
  \param [in] task No description.
  \param [in] thread_id No description.
  \param [out] result No description.
  */
template <typename ReturnType, typename Task> inline
void ThreadManager::runSingleTask(Task& task,
                                  const uint thread_id,
                                  ResultPointer<ReturnType> result)
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
  \details No detailed description

  \return No description
  */
inline
bool ThreadManager::workersAreEnabled() const noexcept
{
  const bool result = 0 <= lock_.get();
  return result;
}

} // namespace zisc

#endif // ZISC_THREAD_MANAGER_INL_HPP
