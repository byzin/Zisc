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
#include "scalable_circular_queue.hpp"
#include "std_memory_resource.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] task_id No description.
  \param [in] manager No description.
  */
template <typename T> inline
ThreadManager::Result<T>::Result(const int64b task_id,
                                 ThreadManager* manager) noexcept :
    task_id_{task_id},
    manager_{manager},
    has_value_{kFalse}
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
    // Run another task while waiting for the complesion
    const bool has_task = runAnotherTask();
    if (!has_task)
      std::this_thread::yield();
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

  \return No description
  */
template <typename T> inline
ThreadManager* ThreadManager::Result<T>::manager() const noexcept
{
  return manager_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
bool ThreadManager::Result<T>::runAnotherTask() const
{
  bool has_task = false;
  auto th_manager = manager();
  const int64b thread_id = th_manager->getCurrentThreadId();
  if (thread_id != th_manager->unmanagedThreadId()) {
    WorkerTask task = th_manager->fetchTask();
    has_task = task.hasTask();
    if (has_task) {
      task.run(thread_id);
    }
  }
  return has_task;
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
int64b ThreadManager::Result<T>::taskId() const noexcept
{
  return task_id_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto ThreadManager::Result<T>::value() noexcept -> ResultReference
{
  return *treatAs<ResultT*>(std::addressof(data_));
}

/*!
  \details No detailed description

  \param [in] task_id No description.
  \param [in] parent_task_id No description.
  */
inline
ThreadManager::Task::Task(const int64b task_id, const int64b parent_task_id) noexcept
    : task_id_{task_id},
      parent_task_id_{parent_task_id}
{
}

/*!
  \details No detailed description
  */
inline
ThreadManager::Task::~Task() noexcept
{
}

/*!
  \details No detailed description

  \return No description
  */
inline
int64b ThreadManager::Task::parentTaskId() const noexcept
{
  return parent_task_id_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
int64b ThreadManager::Task::taskId() const noexcept
{
  return task_id_;
}

/*!
  \details No detailed description

  \param [in] what_arg No description.
  \param [in,out] mem_resource No description.
  \param [in] t No description.
  \param [in] begin_offset No description.
  \param [in] num_of_iterations No description.
  */
inline
ThreadManager::OverflowError::OverflowError(const std::string_view what_arg,
                                            pmr::memory_resource* mem_resource,
                                            TaskPointer t,
                                            const DiffType begin_offset,
                                            const DiffType num_of_iterations) :
    SystemError(ErrorCode::kThreadManagerQueueOverflow, what_arg),
    task_data_{std::allocate_shared<TaskData>(
        pmr::polymorphic_allocator<TaskData>{mem_resource},
        mem_resource,
        t)},
    begin_offset_{begin_offset},
    num_of_iterations_{num_of_iterations}
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

  \return No description
  */
inline
auto ThreadManager::OverflowError::beginOffset() const noexcept -> DiffType
{
  return begin_offset_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::OverflowError::numOfIterations() const noexcept -> DiffType
{
  return num_of_iterations_;
}

/*!
  \details No detailed description

  \tparam ReturnType No description.
  \return No description
  */
template <typename ReturnType> inline
auto ThreadManager::OverflowError::result() noexcept -> Result<ReturnType>&
{
  using ResultT = Result<ReturnType>;
  Task& t = task();
  ResultT& r = *cast<ResultT*>(t.result());
  return r;
}

/*!
  \details No detailed description

  \tparam ReturnType No description.
  \return No description
  */
template <typename ReturnType> inline
auto ThreadManager::OverflowError::result() const noexcept -> const Result<ReturnType>&
{
  using ResultT = Result<ReturnType>;
  const Task& t = task();
  const ResultT& r = *cast<const ResultT*>(t.result());
  return r;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::OverflowError::task() noexcept -> Task&
{
  return *task_data_->task_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::OverflowError::task() const noexcept -> const Task&
{
  return *task_data_->task_;
}

/*!
  \details No detailed description

  \param [in] mem_resource No description.
  \param [in] t No description.
  */
inline
ThreadManager::OverflowError::TaskData::TaskData(pmr::memory_resource* mem_resource,
                                                 TaskPointer t) noexcept
    : mem_resource_{mem_resource},
      task_{t}
{
}

/*!
  \details No detailed description
  */
inline
ThreadManager::OverflowError::TaskData::~TaskData() noexcept
{
  task_->deleteInException(mem_resource_);
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
ThreadManager::ThreadManager(const int64b num_of_threads,
                             pmr::memory_resource* mem_resource) noexcept :
    task_queue_{defaultItemCapacity(), mem_resource},
    workers_{pmr::vector<std::thread>::allocator_type{mem_resource}},
    worker_state_set_{getActualNumOfThreads(num_of_threads), mem_resource},
    task_state_set_{defaultIdCapacity(), mem_resource},
    total_tasks_{0},
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
    const int64b num_of_threads,
    const int64b thread_id) noexcept
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
    const int64b thread_id) const noexcept
{
  return calcThreadRange(range, numOfThreads(), thread_id);
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t ThreadManager::idCapacity() const noexcept
{
  const std::size_t cap = task_state_set_.size();
  return cap;
}


/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t ThreadManager::itemCapacity() const noexcept
{
  const std::size_t cap = task_queue_.capacity();
  return cap;
}

/*!
  \details No detailed description
  */
inline
void ThreadManager::clear() noexcept
{
  task_queue_.clear();
  task_state_set_.reset();
  total_tasks_.store(0);
  lock_.set(0);
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::defaultIdCapacity() noexcept
{
  const std::size_t cap = 128;
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::defaultItemCapacity() noexcept
{
  const std::size_t cap = 1024;
  return cap;
}

/*!
  \details No detailed description

  \tparam ReturnType No description.
  \tparam Func No description.
  \param [in] task No description.
  \param [in] parent_task_id No description.
  \return No description
  */
template <typename ReturnType, typename Func> inline
auto ThreadManager::enqueue(Func&& task,
                            const int64b parent_task_id,
                            EnableIfInvocable<Func>)
    -> UniqueResult<ReturnType>
{
  using ResultP = typename UniqueResult<ReturnType>::pointer;
  using Iterator = int;
  auto t = [task_impl = std::forward<Func>(task)]
  (const int64b /* thread_id */, const Iterator /* it */, ResultP result)
  {
    if constexpr (std::is_void_v<ReturnType>) {
      task_impl();
      result->set(0);
    }
    else {
      auto value = task_impl();
      result->set(std::move(value));
    }
  };
  auto result = enqueueImpl<false, ReturnType>(std::move(t), 0, 1, parent_task_id);
  return result;
}

/*!
  \details No detailed description

  \tparam ReturnType No description.
  \tparam Func No description.
  \param [in] task No description.
  \param [in] parent_task_id No description.
  \return No description
  */
template <typename ReturnType, typename Func> inline
auto ThreadManager::enqueue(Func&& task,
                            const int64b parent_task_id,
                            EnableIfInvocable<Func, int64b>)
    -> UniqueResult<ReturnType>
{
  using ResultP = typename UniqueResult<ReturnType>::pointer;
  using Iterator = int;
  auto t = [task_impl = std::forward<Func>(task)]
  (const int64b thread_id, const Iterator /* it */, ResultP result)
  {
    if constexpr (std::is_void_v<ReturnType>) {
      task_impl(thread_id);
      result->set(0);
    }
    else {
      auto value = task_impl(thread_id);
      result->set(std::move(value));
    }
  };
  auto result = enqueueImpl<false, ReturnType>(std::move(t), 0, 1, parent_task_id);
  return result;
}

/*!
  \details No detailed description

  \tparam Func No description.
  \tparam Iterator1 No description.
  \tparam Iterator2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
template <typename Func, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoop(
    Func&& task,
    Iterator1&& begin,
    Iterator2&& end,
    const int64b parent_task_id,
    EnableIfInvocable<Func, CommonIterator<Iterator1, Iterator2>>)
        -> UniqueResult<void>
{
  using ResultP = typename UniqueResult<void>::pointer;
  using Iterator = CommonIterator<Iterator1, Iterator2>;
  auto t = [task_impl = std::forward<Func>(task)]
  (const int64b /* thread_id */, const Iterator it, ResultP /* result */)
  {
    task_impl(it);
  };
  auto result = enqueueImpl<true, void>(std::move(t),
                                        std::forward<Iterator1>(begin),
                                        std::forward<Iterator2>(end),
                                        parent_task_id);
  return result;
}

/*!
  \details No detailed description

  \tparam Func No description.
  \tparam Iterator1 No description.
  \tparam Iterator2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
template <typename Func, typename Iterator1, typename Iterator2> inline
auto ThreadManager::enqueueLoop(
    Func&& task,
    Iterator1&& begin,
    Iterator2&& end,
    const int64b parent_task_id,
    EnableIfInvocable<Func, int64b, CommonIterator<Iterator1, Iterator2>>)
        -> UniqueResult<void>
{
  using ResultP = typename UniqueResult<void>::pointer;
  using Iterator = CommonIterator<Iterator1, Iterator2>;
  auto t = [task_impl = std::forward<Func>(task)]
  (const int64b thread_id, const Iterator it, ResultP /* result */)
  {
    task_impl(thread_id, it);
  };
  auto result = enqueueImpl<true, void>(std::move(t),
                                        std::forward<Iterator1>(begin),
                                        std::forward<Iterator2>(end),
                                        parent_task_id);
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
int64b ThreadManager::logicalCores() noexcept
{
  const int64b cores = cast<int64b>(std::thread::hardware_concurrency());
  return cores;
}

/*!
  \details No detailed description

  \return The number of worker threads
  */
inline
int64b ThreadManager::numOfThreads() const noexcept
{
  const std::size_t num_of_threads = workers_.size();
  return cast<int64b>(num_of_threads);
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
void ThreadManager::setIdCapacity(const std::size_t cap) noexcept
{
  task_state_set_.setSize(cap);
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
inline
void ThreadManager::setItemCapacity(const std::size_t cap) noexcept
{
  task_queue_.setCapacity(cap);
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t ThreadManager::size() const noexcept
{
  const std::size_t s = task_queue_.size();
  return s;
}

/*!
  \details No detailed description
  */
inline
void ThreadManager::waitForCompletion() noexcept
{
  const std::size_t n = cast<std::size_t>(numOfThreads());
  while (!isEmpty() || worker_state_set_.isAny(0, n))
    std::this_thread::yield();
}

/*!
  \details No detailed description
  */
inline
ThreadManager::WorkerTask::WorkerTask() noexcept :
    task_{nullptr},
    it_offset_{0}
{
}

/*!
  \details No detailed description

  \param [in] task No description.
  \param [in] it_offset No description.
  */
inline
ThreadManager::WorkerTask::WorkerTask(TaskPointer task,
                                      const DiffType it_offset) noexcept :
    task_{task},
    it_offset_{it_offset}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
ThreadManager::WorkerTask::WorkerTask(WorkerTask&& other) noexcept :
    task_{other.task_},
    it_offset_{other.it_offset_}
{
  other.task_ = nullptr;
  other.it_offset_ = 0;
}

/*!
  \details No detailed description
  */
inline
ThreadManager::WorkerTask::~WorkerTask() noexcept
{
  if (hasTask())
    done();
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
auto ThreadManager::WorkerTask::operator=(WorkerTask&& other) noexcept
    -> WorkerTask&
{
  std::swap(task_, other.task_);
  std::swap(it_offset_, other.it_offset_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool ThreadManager::WorkerTask::hasTask() const noexcept
{
  const bool result = task_ != nullptr;
  return result;
}

/*!
  \details No detailed description

  \param [in] thread_id No description.
  */
inline
void ThreadManager::WorkerTask::run(const int64b thread_id)
{
  auto th_manager = task_->manager();
  th_manager->waitForParent(task_->taskId(), task_->parentTaskId());
  task_->run(thread_id, it_offset_);
}

/*!
  \details No detailed description
  */
inline
void ThreadManager::WorkerTask::done() noexcept
{
  auto th_manager = task_->manager();
  const std::size_t id = cast<std::size_t>(task_->taskId());
  const bool is_last = task_->done();
  if (is_last && !th_manager->task_state_set_[id]) { // Delete 
    th_manager->finishTask(task_->taskId());
    using UniqueTaskT = pmr::unique_ptr<Task>;
    using Deleter = typename UniqueTaskT::deleter_type;
    Deleter deleter{th_manager->resource()};
    deleter(task_);
  }
}

/*!
  \details No detailed description

  \param [in] num_of_threads No description.
  */
inline
void ThreadManager::createWorkers(const int64b num_of_threads) noexcept
{
  const std::size_t id_max = getActualNumOfThreads(num_of_threads);
  workers_.reserve(id_max);
  Atomic::increment(std::addressof(lock_.get()));
  for (std::size_t i = 0; i < id_max; ++i) {
    auto work = [this]()
    {
      // Wait until all worker creation are completed
      while (lock_.get() != 0)
        std::this_thread::yield();

      const int64b thread_id = getCurrentThreadId();
      while (workersAreEnabled()) {
        WorkerTask task = fetchTask();
        if (task.hasTask()) {
          task.run(thread_id);
        }
        else { // Wait for next task
          if (lock_.get() != 0) {
            std::this_thread::yield();
          }
          else {
            worker_state_set_.set(cast<std::size_t>(thread_id), false);
            Atomic::wait(std::addressof(lock_), 0);
            worker_state_set_.set(cast<std::size_t>(thread_id), true);
          }
        }
      }
    };
    workers_.emplace_back(work);
    worker_state_set_.set(i, true);
  }
  // Sort workers by their IDs
  auto comp = [](const std::thread& lhs, const std::thread& rhs) noexcept
  {
    return lhs.get_id() < rhs.get_id();
  };
  std::sort(workers_.begin(), workers_.end(), comp);

  // Wait until underlying threads become ready
  Atomic::decrement(std::addressof(lock_.get()));
  waitForCompletion();
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
auto ThreadManager::distance(Iterator1&& begin, Iterator2&& end) noexcept
    -> DiffType
{
  using CommonIterator = CommonIterator<Iterator1, Iterator2>;
  using Iterator = std::remove_cv_t<std::remove_reference_t<CommonIterator>>;
  DiffType d = 0;
  if constexpr (std::is_arithmetic_v<Iterator>)
    d = cast<DiffType>(end - begin);
  else
    d = cast<DiffType>(std::distance(begin, end));
  ZISC_ASSERT(0 < d, "The end is ahead of the begin.");
  return d;
}

/*!
  \details No detailed description

  \tparam ReturnType No description.
  \tparam Func No description.
  \tparam Iterator1 No description.
  \tparam Iterator2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] parent_task_id No description.
  \return No description
  \exception OverflowError No description.
  */
template <bool kIsLoopTask, typename ReturnType, typename Func, typename Iterator1, typename Iterator2>
inline
auto ThreadManager::enqueueImpl(
    Func&& task,
    Iterator1&& begin,
    Iterator2&& end,
    const int64b parent_task_id) -> UniqueResult<ReturnType>
{
  using FuncT = std::remove_cv_t<std::remove_reference_t<Func>>;
  using UniqueResultT = UniqueResult<ReturnType>;
  using ResultP = typename UniqueResultT::pointer;
  using Iterator = std::remove_cv_t<CommonIterator<Iterator1, Iterator2>>;
  static_assert(std::is_invocable_v<FuncT, int64b, Iterator, ResultP>,
                "The FuncT isn't invocable.");
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_CLANG || Z_GCC
  class SharedTask : public Task
  {
   public:
    SharedTask(Func&& t, ResultP r, const int64b p, const int64b c, Iterator1&& b)
        noexcept : Task(r->taskId(), p),
                   task_{std::forward<Func>(t)},
                   result_{r},
                   counter_{c},
                   begin_{std::forward<Iterator1>(b)}
    {
    }
    ~SharedTask() noexcept override
    {
    }
    std::atomic<int64b>& counter() noexcept
    {
      return counter_;
    }
    bool done() noexcept override
    {
      if constexpr (kIsLoopTask) {
        const int64b c = --counter_;
        const bool flag = c == 0;
        if (flag)
          result_->set(0);
        return flag;
      }
      else {
        return true;
      }
    }
    void* result() noexcept override
    {
      return result_;
    }
    const void* result() const noexcept override
    {
      return result_;
    }
    void run(const int64b thread_id, const DiffType it_offset) override
    {
      if constexpr (std::is_arithmetic_v<Iterator>) {
        using ItType = std::common_type_t<Iterator, DiffType>;
        auto ite = cast<Iterator>(cast<ItType>(begin_) + cast<ItType>(it_offset));
        task_(thread_id, ite, result_);
      }
      else {
        auto ite = begin_;
        std::advance(ite, it_offset);
        task_(thread_id, ite, result_);
      }
    }
   protected:
    void deleteInException(pmr::memory_resource* mem_resource) noexcept override
    {
      {
        using Deleter = typename UniqueResultT::deleter_type;
        Deleter deleter{mem_resource};
        deleter(result_);
      }
      {
        using UniqueTaskT = pmr::unique_ptr<SharedTask>;
        using Deleter = typename UniqueTaskT::deleter_type;
        Deleter deleter{mem_resource};
        deleter(this);
      }
    }
    ThreadManager* manager() noexcept override
    {
      return result_->manager();
    }

   private:
    FuncT task_;
    ResultP result_;
    std::atomic<int64b> counter_;
    Iterator begin_;
  };
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_CLANG || Z_GCC
  auto mem_resource = resource();

  // Create a result of the given task
  using ResultT = typename UniqueResultT::element_type;
  pmr::polymorphic_allocator<ResultT> result_alloc{mem_resource};
  const int64b task_id = issueTaskId();
  auto result = pmr::allocateUnique(result_alloc, task_id, this);

  // Create a shared task
  const DiffType d = distance(begin, end);
  auto shared_task = pmr::allocateUnique<SharedTask>(mem_resource,
                                                     std::forward<Func>(task),
                                                     result.get(),
                                                     parent_task_id,
                                                     d,
                                                     std::forward<Iterator1>(begin)
                                                     ).release();

  auto throw_exception = [this](auto t, auto& r, auto b, auto e)
  {
    finishTask(r->taskId());
    r.release();
    const char* message = "Task queue overflow happened.";
    throw OverflowError{message, resource(), t, b, e};
  };
  if (idCapacity() <= cast<std::size_t>(task_id))
    throw_exception(shared_task, result, 0, d);

  // Enqueue loop tasks
  {
    auto notify_threads = [this](const DiffType num_of_queued_tasks) noexcept
    {
      if (kIsLoopTask && numOfThreads() <= num_of_queued_tasks) {
        Atomic::notifyAll(std::addressof(lock_));
      }
      else {
        for (DiffType i = 0; i < num_of_queued_tasks; ++i)
          Atomic::notifyOne(std::addressof(lock_));
      }
    };

    Atomic::add(std::addressof(lock_.get()), d);
    for (DiffType i = 0; i < d; ++i) {
      WorkerTask worker_task{shared_task, i};
      try {
        task_queue_.enqueue(std::move(worker_task));
      }
      catch (const Queue::OverflowError& /* error */) {
        const DiffType rest = d - i;
        shared_task->counter() -= rest;
        Atomic::sub(std::addressof(lock_.get()), rest);
        notify_threads(i);
        throw_exception(shared_task, result, i, d);
      }
    }
    notify_threads(d);
  }

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
auto ThreadManager::fetchTask() noexcept -> WorkerTask
{
  auto result = task_queue_.dequeue();
  WorkerTask task{};
  const bool flag = std::get<0>(result);
  if (flag) {
    Atomic::decrement(std::addressof(lock_.get()));
    task = std::move(std::get<1>(result));
  }
  return task;
}

/*!
  \details No detailed description

  \param [in] task_id No description.
  */
inline
void ThreadManager::finishTask(const int64b task_id) noexcept
{
  task_state_set_.set(cast<std::size_t>(task_id), true);
}

/*!
  \details No detailed description

  \param [in] s No description.
  \return No description
  */
inline
std::size_t ThreadManager::getActualNumOfThreads(const int64b s) noexcept
{
  const int64b num_of_threads = (s == 0) ? logicalCores() : s;
  return cast<std::size_t>(num_of_threads);
}

/*!
  \details No detailed description

  \return No description
  */
inline
int64b ThreadManager::getCurrentThreadId() const noexcept
{
  const auto id = std::this_thread::get_id();
  auto comp = [](const std::thread& lhs, const std::thread::id& rhs) noexcept
  {
    return lhs.get_id() < rhs;
  };
  const auto t = std::lower_bound(workers_.begin(), workers_.end(), id, comp);
  const bool result = (t != workers_.end()) && (t->get_id() == id);
  const int64b thread_id = result ? std::distance(workers_.begin(), t)
                                  : unmanagedThreadId();
  return thread_id;
}

/*!
  \details No detailed description

  \param [in] num_of_threads No description.
  */
inline
void ThreadManager::initialize(const int64b num_of_threads) noexcept
{
  clear();
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
int64b ThreadManager::issueTaskId() noexcept
{
  const int64b id = total_tasks_++;
  return id;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr int64b ThreadManager::unmanagedThreadId() noexcept
{
  const int64b id = std::numeric_limits<int64b>::min();
  return id;
}

/*!
  \details No detailed description

  \param [in] task_id No description.
  \param [in] parent_task_id No description.
  */
inline
void ThreadManager::waitForParent(const int64b task_id,
                                  const int64b parent_task_id) const noexcept
{
  while (true) {
    bool flag = false;
    if (parent_task_id == kNoParentId) {
      flag = true;
    }
    else if (parent_task_id == kAllParentId) {
      flag = task_state_set_.isAll(0, cast<std::size_t>(task_id));
    }
    else {
      flag = task_state_set_[cast<std::size_t>(parent_task_id)];
    }
    if (flag)
      break;
    std::this_thread::yield();
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
