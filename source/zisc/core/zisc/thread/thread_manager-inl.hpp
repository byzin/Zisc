/*!
  \file thread_manager-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_INL_HPP
#define ZISC_THREAD_MANAGER_INL_HPP

#include "thread_manager.hpp"
// Standard C++ library
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <functional>
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
#include "atomic_word.hpp"
#include "future.hpp"
#include "packaged_task.hpp"
#include "zisc/concepts.hpp"
#include "zisc/data_structure/scalable_circular_queue.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

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

  \param [in,out] other No description.
  */
inline
ThreadManager::OverflowError::OverflowError(OverflowError&& other) :
    SystemError(std::move(other)),
    task_{std::move(other.task_)},
    begin_offset_{other.begin_offset_},
    num_of_iterations_{other.num_of_iterations_}
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

  \param [in,out] other No description.
  */
inline
auto ThreadManager::OverflowError::operator=(OverflowError&& other)
    -> OverflowError&
{
  SystemError::operator=(std::move(other));
  task_ = std::move(other.task_);
  begin_offset_ = other.begin_offset_;
  num_of_iterations_ = other.num_of_iterations_;
  return *this;
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
PackagedTask& ThreadManager::OverflowError::task() noexcept
{
  return *task_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const PackagedTask& ThreadManager::OverflowError::task() const noexcept
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
    total_queued_task_ids_{0},
//    num_of_completed_task_ids_{0},
//    current_min_task_id_{0},
    num_of_active_workers_{0},
    worker_lock_{-1},
    task_queue_{defaultCapacity(), mem_resource},
    worker_list_{decltype(worker_list_)::allocator_type{mem_resource}},
    worker_id_list_{decltype(worker_id_list_)::allocator_type{mem_resource}}
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

  \tparam Types No description.
  \tparam TaskData No description.
  \param [in] data No description.
  \return No description
  */
template <typename ...Types, typename TaskData> inline
auto ThreadManager::getTaskRef(TaskData&& data) noexcept
{
  using FuncT1 = std::remove_volatile_t<std::remove_reference_t<TaskData>>;
  constexpr bool t1_is_invocable = std::is_invocable_v<FuncT1, Types...>;
  using FuncT2 = std::remove_pointer_t<FuncT1>;
  constexpr bool t2_is_invocable = std::is_invocable_v<FuncT2, Types...>;
  if constexpr (t1_is_invocable)
    return std::ref(data);
  else if constexpr (t2_is_invocable)
    return std::ref(*data);
  static_assert(t1_is_invocable || t2_is_invocable, "Unsupported data type.");
}

/*!
  \details No detailed description

  \tparam Types No description.
  \tparam Func No description.
  \param [in] func No description.
  \return No description
  */
template <typename ...Types, Invocable<Types...> Func> inline
auto ThreadManager::makeTaskData(Func&& func) noexcept
{
  using Function = std::remove_volatile_t<std::remove_reference_t<Func>>;
  using ReturnT = InvokeResult<Function, Types...>;
  using FunctionPointer = ReturnT (*)(Types...);
  constexpr bool is_func_ptr = std::is_pointer_v<Function>;
  constexpr bool has_func_ptr = std::is_convertible_v<Function, FunctionPointer>;
  constexpr bool is_fanctor = std::is_object_v<Function>;

  if constexpr (is_func_ptr)
    return func;
  else if constexpr (has_func_ptr)
    return FunctionPointer{func};
  else if constexpr (std::is_lvalue_reference_v<Func>)
    return std::addressof(func);
  else // rvalue functor
    return std::move(func);

  static_assert(is_func_ptr || has_func_ptr || is_fanctor, "Unsupported func type.");
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
  */
inline
void ThreadManager::clear() noexcept
{
  if (workersAreEnabled())
    waitForCompletion();
  ZISC_ASSERT(num_of_active_workers_.load(std::memory_order::acquire) == 0,
              "Some worker threads are stil active.");
  ZISC_ASSERT(worker_lock_.load(std::memory_order::acquire) == 0,
              "Task queue still has some tasks.");
  ZISC_ASSERT(isEmpty(), "Task queue still has some tasks.");
  total_queued_task_ids_.store(0, std::memory_order::release);
//  num_of_completed_task_ids_.store(0, std::memory_order::release);
//  finishTask(kNoTask);
  task_queue_.clear();
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::defaultCapacity() noexcept
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
auto ThreadManager::enqueue(Func&& task, const int64b parent_task_id)
    -> SharedFuture<InvokeResult<Func>>
{
  using ReturnT = InvokeResult<Func>;
  auto task_data = [data = makeTaskData(std::forward<Func>(task))]
  (const int64b) mutable noexcept -> ReturnT
  {
    auto func = getTaskRef(data);
    return std::invoke(func);
  };
  auto result = enqueueImpl<ReturnT, false>(task_data, 0, 1, parent_task_id);
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
auto ThreadManager::enqueue(Func&& task, const int64b parent_task_id)
    -> SharedFuture<InvokeResult<Func, int64b>>
{
  using ReturnT = InvokeResult<Func, int64b>;
  auto task_data = makeTaskData<int64b>(std::forward<Func>(task));
  auto result = enqueueImpl<ReturnT, false>(task_data, 0, 1, parent_task_id);
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
template <typename Func, typename Ite1, typename Ite2>
requires Invocable<Func, ThreadManager::CommonIte<Ite1, Ite2>> inline
auto ThreadManager::enqueueLoop(Func&& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const int64b parent_task_id) -> SharedFuture<void>
{
  using Ite = CommonIte<Ite1, Ite2>;
  auto task_data = [data = makeTaskData<Ite>(std::forward<Func>(task))]
  (const Ite it, const int64b) mutable noexcept
  {
    auto func = getTaskRef<Ite>(data);
    std::invoke(func, it);
  };
  auto result = enqueueImpl<void, true>(task_data,
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
template <typename Func, typename Ite1, typename Ite2>
requires Invocable<Func, ThreadManager::CommonIte<Ite1, Ite2>, int64b> inline
auto ThreadManager::enqueueLoop(Func&& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const int64b parent_task_id) -> SharedFuture<void>
{
  using Ite = CommonIte<Ite1, Ite2>;
  auto task_data = makeTaskData<Ite, int64b>(std::forward<Func>(task));
  auto result = enqueueImpl<void, true>(task_data,
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
  const std::size_t num_of_threads = worker_list_.size();
  return cast<int64b>(num_of_threads);
}

/*!
  \details No detailed description

  \return No description
  */
inline
pmr::memory_resource* ThreadManager::resource() const noexcept
{
  auto mem_resource = worker_list_.get_allocator().resource();
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
  while (!isEmpty() || (num_of_active_workers_.load(std::memory_order::acquire) != 0))
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

  \tparam Ite No description.
  \tparam OffsetT No description.
  \param [in] begin No description.
  \param [in] offset No description.
  \return No description
  */
template <typename Ite, typename OffsetT> inline
Ite ThreadManager::advance(Ite& begin, const OffsetT offset) noexcept
{
  auto ite = begin;
  if constexpr (std::is_arithmetic_v<Ite>)
    ite += cast<Ite>(offset);
  else
    std::advance(ite, offset);
  return ite;
}

/*!
  \details No detailed description

  \param [in] num_of_threads No description.
  */
inline
void ThreadManager::createWorkers(const int64b num_of_threads) noexcept
{
  const std::size_t n = getActualNumOfThreads(num_of_threads);
  worker_list_.reserve(n);
  worker_id_list_.reserve(n);
  worker_lock_.store(-1, std::memory_order::release);
  num_of_active_workers_.store(cast<int>(n), std::memory_order::release);
  for (std::size_t i = 0; i < n; ++i) {
    auto work = [this]() noexcept
    {
      // Wait until all worker creation are completed
      atomic_wait(std::addressof(worker_lock_), -1, std::memory_order::acquire);
      // Run worker tasks
      const int64b thread_id = getCurrentThreadId();
      doWorkerTask(thread_id);
    };
    worker_list_.emplace_back(work);
    worker_id_list_.emplace_back(worker_list_.back().get_id());
  }

  // Sort workers by their underlying IDs
  std::sort(worker_id_list_.begin(), worker_id_list_.end());

  // Activate all workers
  worker_lock_.store(0);
  atomic_notify_all(std::addressof(worker_lock_));

  // Wait until underlying threads become ready
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
void ThreadManager::doWorkerTask(const int64b thread_id) noexcept
{
  while (workersAreEnabled()) {
    WorkerTask task = fetchTask();
    if (task.hasTask()) {
      task.run(thread_id);
    }
    else { // Wait for next task
      const auto num_of_tasks = worker_lock_.load(std::memory_order::acquire);
      if (num_of_tasks != 0) {
        // The worker may just missed queued tasks, waits a little
        std::this_thread::yield();
      }
      else {
        // There is no task. The worker waits for next task queuing
        num_of_active_workers_.fetch_sub(1, std::memory_order::release);
        atomic_wait(std::addressof(worker_lock_), 0, std::memory_order::acquire);
        num_of_active_workers_.fetch_add(1, std::memory_order::release);
      }
    }
  }
}

/*!
  \details No detailed description

  \tparam ReturnT No description.
  \tparam kIsLoopTask No description.
  \tparam TaskData No description.
  \tparam Ite1 No description.
  \tparam Ite2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] parent_task_id No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename ReturnT, bool kIsLoopTask,
          typename TaskData, typename Ite1, typename Ite2> inline
auto ThreadManager::enqueueImpl(TaskData& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const int64b parent_task_id) -> SharedFuture<ReturnT>
{
  using SharedFutureT = SharedFuture<ReturnT>;
  using Ite = std::remove_cv_t<CommonIte<Ite1, Ite2>>;
  using Data = std::remove_reference_t<TaskData>;

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_CLANG || Z_GCC
  class TaskImpl : public PackagedTask
  {
   public:
    //! Initialize the task
    TaskImpl(Data&& data, SharedFutureT& r, Ite1 b, const int64b p) noexcept
        : PackagedTask(r->id(), p),
          task_data_{std::move(data)},
          result_{r},
          begin_{std::forward<Ite1>(b)}
    {
    }
    //! Finalize the task
    ~TaskImpl() noexcept override
    {
      if constexpr (kIsLoopTask || std::is_void_v<ReturnT>)
        result_->set(0);
    }
    //! Retrieve the result of the underlying task
    void getFuture(void* result) noexcept override
    {
      *cast<SharedFutureT*>(result) = result_;
    }
    //! Run a task
    void run(const int64b thread_id, const DiffType it_offset) override
    {
      auto th_manager = result_->manager();
      th_manager->waitForParent(id(), parentId());

      if constexpr (kIsLoopTask) { // Loop task
        auto func = getTaskRef<Ite, int64b>(task_data_);
        auto ite = advance(begin_, it_offset);
        std::invoke(func, ite, thread_id);
      }
      else { // Single task
        auto func = getTaskRef<int64b>(task_data_);
        if constexpr (std::is_void_v<ReturnT>) {
          std::invoke(func, thread_id);
        }
        else {
          auto value = std::invoke(func, thread_id);
          result_->set(std::move(value));
        }
      }
    }

   private:
    Data task_data_;
    SharedFutureT result_;
    Ite begin_;
  };
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_CLANG || Z_GCC

  std::unique_lock<std::mutex> locker{manager_lock_};
  auto mem_resource = resource();

  // Create a result of the given task
  using ResultT = typename SharedFutureT::element_type;
  pmr::polymorphic_allocator<ResultT> result_alloc{mem_resource};
  const int64b task_id = issueTaskId();
  auto shared_result = std::allocate_shared<ResultT>(result_alloc, task_id, this);

  // Create a shared task
  const DiffType num_of_tasks = distance(begin, end);
  pmr::polymorphic_allocator<TaskImpl> task_alloc{mem_resource};
  const int64b parent_id = (task_id == 0) ? kNoTask : parent_task_id;
  auto shared_task = std::allocate_shared<TaskImpl>(task_alloc,
                                                    std::move(task),
                                                    shared_result,
                                                    std::forward<Ite1>(begin),
                                                    parent_id);

  auto throw_exception = [&locker](auto& t, auto& r, auto b, auto e)
  {
    r.reset();
    locker.unlock();
    const char* message = "Task queue overflow happened.";
    throw OverflowError{message, std::move(t), b, e};
  };

  // Enqueue tasks
  {
    auto word_ptr = std::addressof(worker_lock_.get());
    atomic_fetch_add(word_ptr, num_of_tasks, std::memory_order::release);
    for (DiffType i = 0; i < num_of_tasks; ++i) {
      WorkerTask worker_task{shared_task, i};
      try {
        task_queue_.enqueue(std::move(worker_task));
      }
      catch (const TaskQueue::OverflowError& /* error */) {
        const DiffType rest = num_of_tasks - i;
        atomic_fetch_sub(word_ptr, rest, std::memory_order::release);
        atomic_notify_all(std::addressof(worker_lock_));
        throw_exception(shared_task, shared_result, i, num_of_tasks);
      }
      atomic_notify_one(std::addressof(worker_lock_));
    }
    if constexpr (kIsLoopTask)
      atomic_notify_all(std::addressof(worker_lock_));
  }

  return shared_result;
}

/*!
  \details No detailed description
  */
inline
void ThreadManager::exitWorkersRunning() noexcept
{
  worker_lock_.store(-1, std::memory_order::release);
  atomic_notify_all(std::addressof(worker_lock_));
  for (auto& worker : worker_list_)
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
    atomic_fetch_dec(std::addressof(worker_lock_.get()), std::memory_order::release);
    task = std::move(std::get<1>(result));
  }
  return task;
}

/*!
  \details No detailed description

  \param [in] task_id No description.
  */
inline
void ThreadManager::finishTask([[maybe_unused]] const int64b task_id) noexcept
{
//  atomic_store(&current_min_task_id_, kNoTask, std::memory_order::release);
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
  const auto t = std::lower_bound(worker_id_list_.begin(), worker_id_list_.end(), id);
  const bool result = (t != worker_id_list_.end()) && (*t == id);
  const int64b thread_id = result ? std::distance(worker_id_list_.begin(), t)
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
  createWorkers(num_of_threads);
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
inline
int64b ThreadManager::issueTaskId() noexcept
{
  const int64b id = total_queued_task_ids_.fetch_add(1, std::memory_order::acq_rel);
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
    // Update current min task id
    //atomic_fetch_min(&current_min_task_id_, task_id, std::memory_order::release);

    bool flag = false;
    if (parent_task_id == kNoTask) {
      flag = true;
    }
    else if (parent_task_id == kAllPrecedences) {
      ZISC_ASSERT(false, "Not implemented.");
//      const auto current = current_min_task_id_.load(std::memory_order::acquire);
//      flag = current == task_id;
    }
    else {
      ZISC_ASSERT(false, "Not implemented.");
//      flag = task_state_set_[cast<std::size_t>(parent_task_id)];
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
  const auto l = worker_lock_.load(std::memory_order::acquire);
  const bool result = 0 <= l;
  return result;
}

} // namespace zisc

#endif // ZISC_THREAD_MANAGER_INL_HPP
