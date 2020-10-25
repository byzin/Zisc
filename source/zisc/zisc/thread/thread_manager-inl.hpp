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
#include <iterator>
#include <limits>
#include <memory>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "atomic.hpp"
#include "bitset.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"
#include "zisc/queue/scalable_circular_queue.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] task_id No description.
  \param [in] manager No description.
  */
template <NonReference T> inline
ThreadManager::Result<T>::Result(const int64b task_id,
                                 ThreadManager* manager) noexcept :
    task_id_{task_id},
    manager_{manager},
    has_value_{kFalse}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
auto ThreadManager::Result<T>::get() -> Type
{
  wait();
  if constexpr (!std::is_void_v<Type>)
    return std::move(value());
}

/*!
  \details No detailed description
  */
template <NonReference T> inline
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
  */
template <NonReference T> inline
void ThreadManager::Result<T>::destroy() noexcept
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
template <NonReference T> inline
bool ThreadManager::Result<T>::hasValue() const noexcept
{
  return has_value_ == kTrue;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
ThreadManager* ThreadManager::Result<T>::manager() const noexcept
{
  return manager_;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
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
template <NonReference T> inline
void ThreadManager::Result<T>::set(ResultRReference result) noexcept
{
  ZISC_ASSERT(!hasValue(), "The result already has a value.");
  new (&data_) ResultT{std::move(result)};
  has_value_ = kTrue;
  auto th_manager = manager();
  th_manager->finishTask(taskId());
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
int64b ThreadManager::Result<T>::taskId() const noexcept
{
  return task_id_;
}

/*!
  \details No detailed description

  \return No description
  */
template <NonReference T> inline
auto ThreadManager::Result<T>::value() noexcept -> ResultReference
{
  return *reinterp<ResultT*>(std::addressof(data_));
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
  \param [in] t No description.
  \param [in] begin_offset No description.
  \param [in] num_of_iterations No description.
  */
inline
ThreadManager::OverflowError::OverflowError(const std::string_view what_arg,
                                            SharedTask&& t,
                                            const DiffType begin_offset,
                                            const DiffType num_of_iterations) :
    SystemError(ErrorCode::kThreadManagerQueueOverflow, what_arg),
    task_{std::move(t)},
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

  \return No description
  */
inline
auto ThreadManager::OverflowError::task() noexcept -> Task&
{
  return *task_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::OverflowError::task() const noexcept -> const Task&
{
  return *task_;
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
  clear<false>();
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] range No description.
  \param [in] num_of_threads No description.
  \param [in] thread_id No description.
  \return No description
  */
template <Integer Int> inline
std::array<Int, 2> ThreadManager::calcThreadRange(const Int range,
                                                  const int64b num_of_threads,
                                                  const int64b thread_id) noexcept
{
  const Int threads = cast<Int>(num_of_threads);
  const Int id = cast<Int>(thread_id);
  ZISC_ASSERT(0 < threads, "The num of threads is zero.");
  ZISC_ASSERT(threads <= range, "The range is less than the num of threads.");

  const Int n = range / threads;
  const Int begin = n * id;
  const Int end = begin + n + ((id + 1 == threads) ? range - n * threads : 0);
  return {{begin, end}};
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] range No description.
  \param [in] thread_id No description.
  \return No description
  */

template <Integer Int> inline
std::array<Int, 2> ThreadManager::calcThreadRange(const Int range,
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

  \tparam kIsSynchronized No description.
  */
template <bool kIsSynchronized> inline
void ThreadManager::clear() noexcept
{
  if constexpr (kIsSynchronized)
    waitForCompletion();
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

  \tparam Func No description.
  \param [in] task No description.
  \param [in] parent_task_id No description.
  \return No description
  */
template <Invocable Func> inline
auto ThreadManager::enqueue(Func&& task,
                            const int64b parent_task_id)
    -> SharedResult<InvokeResult<Func>>
{
  using ReturnT = InvokeResult<Func>;
  auto func = [t = std::forward<Func>(task)](const int64b /* thread_id */)
  {
    if constexpr (std::is_void_v<ReturnT>)
      t();
    else
      return t();
  };
  auto result = enqueueImpl<ReturnT>(std::move(func), 0, 1, parent_task_id);
  return result;
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] task No description.
  \param [in] parent_task_id No description.
  \return No description
  */
template <Invocable<int64b> Func> inline
auto ThreadManager::enqueue(Func&& task,
                            const int64b parent_task_id)
    -> SharedResult<InvokeResult<Func, int64b>>
{
  using ReturnT = InvokeResult<Func, int64b>;
  auto func = [t = std::forward<Func>(task)](const int64b thread_id)
  {
    if constexpr (std::is_void_v<ReturnT>)
      t(thread_id);
    else
      return t(thread_id);
  };
  auto result = enqueueImpl<ReturnT>(std::move(func), 0, 1, parent_task_id);
  return result;
}

/*!
  \details No detailed description

  \tparam Ite1 No description.
  \tparam Ite2 No description.
  \tparam Func No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] parent_task_id No description.
  \return No description
  */
template <typename Ite1,
          typename Ite2,
          Invocable<ThreadManager::CommonIte<Ite1, Ite2>> Func> inline
auto ThreadManager::enqueueLoop(Func&& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const int64b parent_task_id) -> SharedResult<void>
{
  using Ite = CommonIte<Ite1, Ite2>;
  auto func = [t = std::forward<Func>(task)](const int64b /* thread_id */,
                                             const Ite it)
  {
    t(it);
  };
  auto result = enqueueImpl<void>(std::move(func),
                                  std::forward<Ite1>(begin),
                                  std::forward<Ite2>(end),
                                  parent_task_id);
  return result;
}

/*!
  \details No detailed description

  \tparam Ite1 No description.
  \tparam Ite2 No description.
  \tparam Func No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] parent_task_id No description.
  \return No description
  */
template <typename Ite1,
          typename Ite2,
          Invocable<int64b, ThreadManager::CommonIte<Ite1, Ite2>> Func> inline
auto ThreadManager::enqueueLoop(Func&& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const int64b parent_task_id) -> SharedResult<void>
{
  using Ite = CommonIte<Ite1, Ite2>;
  auto func = [t = std::forward<Func>(task)](const int64b thread_id, const Ite it)
  {
    t(thread_id, it);
  };
  auto result = enqueueImpl<void>(std::move(func),
                                  std::forward<Ite1>(begin),
                                  std::forward<Ite2>(end),
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
  clear();
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
inline
void ThreadManager::setItemCapacity(const std::size_t cap) noexcept
{
  task_queue_.setCapacity(cap);
  clear();
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

  \return No description
  */
inline
constexpr int64b ThreadManager::unmanagedThreadId() noexcept
{
  const int64b id = (std::numeric_limits<int64b>::min)();
  return id;
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
ThreadManager::WorkerTask::WorkerTask() noexcept
{
}

/*!
  \details No detailed description

  \tparam TaskImpl No description.
  \param [in] task No description.
  \param [in] it_offset No description.
  */
template <typename TaskImpl> inline
ThreadManager::WorkerTask::WorkerTask(std::shared_ptr<TaskImpl>& task,
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
    task_{std::move(other.task_)},
    it_offset_{other.it_offset_}
{
}

/*!
  \details No detailed description
  */
inline
ThreadManager::WorkerTask::~WorkerTask() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
auto ThreadManager::WorkerTask::operator=(WorkerTask&& other) noexcept -> WorkerTask&
{
  task_ = std::move(other.task_);
  it_offset_ = other.it_offset_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool ThreadManager::WorkerTask::hasTask() const noexcept
{
  const bool result = cast<bool>(task_);
  return result;
}

/*!
  \details No detailed description

  \param [in] thread_id No description.
  */
inline
void ThreadManager::WorkerTask::run(const int64b thread_id)
{
  task_->run(thread_id, it_offset_);
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
  atomic_fetch_inc(std::addressof(lock_.get()));
  for (std::size_t i = 0; i < id_max; ++i) {
    auto work = [this]() noexcept
    {
      doWorkerTask();
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
  atomic_fetch_dec(std::addressof(lock_.get()));
  waitForCompletion();
}

/*!
  \details No detailed description

  \tparam Ite1 No description.
  \tparam Ite2 No description.
  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
template <typename Ite1, typename Ite2> inline
auto ThreadManager::distance(Ite1&& begin, Ite2&& end) noexcept -> DiffType
{
  using Ite = std::remove_cvref_t<CommonIte<Ite1, Ite2>>;
  DiffType d = 0;
  if constexpr (std::is_arithmetic_v<Ite>)
    d = cast<DiffType>(end - begin);
  else
    d = cast<DiffType>(std::distance(begin, end));
  ZISC_ASSERT(0 < d, "The end is ahead of the begin.");
  return d;
}

/*!
  \details No detailed description
  */
inline
void ThreadManager::doWorkerTask() noexcept
{
  // Wait until all worker creation are completed
  while (atomic_load(std::addressof(lock_.get())) != 0)
    std::this_thread::yield();

  const int64b thread_id = getCurrentThreadId();
  while (workersAreEnabled()) {
    WorkerTask task = fetchTask();
    if (task.hasTask()) {
      task.run(thread_id);
    }
    else { // Wait for next task
      if (atomic_load(std::addressof(lock_.get())) != 0) {
        std::this_thread::yield();
      }
      else {
        worker_state_set_.set(cast<std::size_t>(thread_id), false);
        atomic_wait(std::addressof(lock_), 0);
        worker_state_set_.set(cast<std::size_t>(thread_id), true);
      }
    }
  }
}

/*!
  \details No detailed description

  \tparam ReturnT No description.
  \tparam Func No description.
  \tparam Ite1 No description.
  \tparam Ite2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] parent_task_id No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename ReturnT, typename Func, typename Ite1, typename Ite2> inline
auto ThreadManager::enqueueImpl(Func&& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const int64b parent_task_id) -> SharedResult<ReturnT>
{
  using FuncT = std::remove_cvref_t<Func>;
  using SharedResultT = SharedResult<ReturnT>;
  using Ite = std::remove_cv_t<CommonIte<Ite1, Ite2>>;

  using IsSingle = std::is_invocable<FuncT, int64b>;
  using IsLoop = std::is_invocable<FuncT, int64b, Ite>;
  static_assert(IsSingle::value || IsLoop::value, "The Func isn't invocable.");

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_CLANG || Z_GCC
  class TaskImpl : public Task
  {
   public:
    TaskImpl(Func&& t, SharedResultT& r, Ite1&& b, const int64b p) noexcept
        : Task(r->taskId(), p),
          task_{std::forward<Func>(t)},
          result_{r},
          begin_{std::forward<Ite1>(b)}
    {
    }
    ~TaskImpl() noexcept override
    {
      if constexpr (IsLoop::value)
        result_->set(0);
    }
    void getResult(void* result) noexcept override
    {
      *cast<SharedResultT*>(result) = result_;
    }
    void run(const int64b thread_id, const DiffType it_offset) override
    {
      auto th_manager = result_->manager();
      th_manager->waitForParent(taskId(), parentTaskId());

      if constexpr (IsLoop::value) { // Loop task
        if constexpr (std::is_arithmetic_v<Ite>) {
          using IType = std::common_type_t<Ite, DiffType>;
          auto ite = cast<Ite>(cast<IType>(begin_) + cast<IType>(it_offset));
          task_(thread_id, ite);
        }
        else {
          auto ite = begin_;
          std::advance(ite, it_offset);
          task_(thread_id, ite);
        }
      }
      else { // Single task
        if constexpr (std::is_void_v<ReturnT>) {
          task_(thread_id);
          result_->set(0);
        }
        else {
          auto value = task_(thread_id);
          result_->set(std::move(value));
        }
      }
    }

   private:
    FuncT task_;
    SharedResultT result_;
    Ite begin_;
  };
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_CLANG || Z_GCC
  auto mem_resource = resource();

  // Create a result of the given task
  using ResultT = typename SharedResultT::element_type;
  pmr::polymorphic_allocator<ResultT> result_alloc{mem_resource};
  const int64b task_id = issueTaskId();
  auto result = std::allocate_shared<ResultT>(result_alloc, task_id, this);

  // Create a shared task
  const DiffType d = distance(begin, end);
  pmr::polymorphic_allocator<TaskImpl> task_alloc{mem_resource};
  auto shared_task = std::allocate_shared<TaskImpl>(task_alloc,
                                                    std::forward<Func>(task),
                                                    result,
                                                    std::forward<Ite1>(begin),
                                                    parent_task_id);

  auto throw_exception = [](auto& t, auto& r, auto b, auto e)
  {
    r.reset();
    const char* message = "Task queue overflow happened.";
    throw OverflowError{message, std::move(t), b, e};
  };
  if (idCapacity() <= cast<std::size_t>(task_id))
    throw_exception(shared_task, result, 0, d);

  // Enqueue loop tasks
  {
    auto notify_thread = [this](const DiffType index,
                                const DiffType inactive_threads) noexcept
    {
      if (IsLoop::value && (index < inactive_threads))
        atomic_notify_one(std::addressof(lock_));
    };

    auto notify_threads = [this](const DiffType num_of_queued_tasks) noexcept
    {
      const auto inactive_threads = getNumOfInactiveThreads();
      if (IsLoop::value && (inactive_threads <= num_of_queued_tasks)) {
        atomic_notify_all(std::addressof(lock_));
      }
      else {
        for (DiffType i = 0; i < num_of_queued_tasks; ++i)
          atomic_notify_one(std::addressof(lock_));
      }
    };

    const DiffType inactive_threads = getNumOfInactiveThreads();
    atomic_fetch_add(std::addressof(lock_.get()), d);
    for (DiffType i = 0; i < d; ++i) {
      WorkerTask worker_task{shared_task, i};
      try {
        task_queue_.enqueue(std::move(worker_task));
      }
      catch (const Queue::OverflowError& /* error */) {
        const DiffType rest = d - i;
        atomic_fetch_sub(std::addressof(lock_.get()), rest);
        notify_threads(i);
        throw_exception(shared_task, result, i, d);
      }
      notify_thread(i, inactive_threads);
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
  atomic_notify_all(std::addressof(lock_));
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
    atomic_fetch_dec(std::addressof(lock_.get()));
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

  \return No description
  */
inline
auto ThreadManager::getNumOfInactiveThreads() const noexcept -> DiffType
{
  const auto n = worker_state_set_.size() - worker_state_set_.count();
  return cast<DiffType>(n);
}

/*!
  \details No detailed description

  \param [in] num_of_threads No description.
  */
inline
void ThreadManager::initialize(const int64b num_of_threads) noexcept
{
  clear<false>();
  createWorkers(num_of_threads);
  clear();

  // Check the alignment of member variables
  static_assert(std::alignment_of_v<decltype(workers_)> <=
                std::alignment_of_v<decltype(task_queue_)>);
  static_assert(std::alignment_of_v<decltype(worker_state_set_)> <=
                std::alignment_of_v<decltype(workers_)>);
  static_assert(std::alignment_of_v<decltype(total_tasks_)> <=
                std::alignment_of_v<decltype(worker_state_set_)>);
  static_assert(std::alignment_of_v<decltype(lock_)> <=
                std::alignment_of_v<decltype(total_tasks_)>);
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
  const auto l = atomic_load(std::addressof(lock_.get()));
  const bool result = 0 <= l;
  return result;
}

} // namespace zisc

#endif // ZISC_THREAD_MANAGER_INL_HPP
