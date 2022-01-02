/*!
  \file thread_manager-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
#include "zisc/bit.hpp"
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
    num_of_active_workers_{0},
    worker_lock_{-1},
    task_queue_{defaultCapacity(), mem_resource},
    task_id_tree_{defaultCapacity(), mem_resource},
    worker_list_{decltype(worker_list_)::allocator_type{mem_resource}},
    worker_id_list_{decltype(worker_id_list_)::allocator_type{mem_resource}},
    task_storage_list_{decltype(task_storage_list_)::allocator_type{mem_resource}}
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
  const std::size_t cap = taskQueue().capacity();
  ZISC_ASSERT(cap <= taskIdTree().capacity(), "The task ID tree cap is less than cap.");
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
  ZISC_ASSERT(worker_lock_.load(std::memory_order::acquire) <= 0,
              "Some worker threads are stil active.");
  total_queued_task_ids_.store(0, std::memory_order::release);
  taskQueue().clear();
  taskIdTree().clear();
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
    -> Future<InvokeResult<Func>>
{
  using ReturnT = InvokeResult<Func>;
  auto tmp = makeTaskData(std::forward<Func>(task));
  auto wrapped_task = [data = std::move(tmp)](const int64b) mutable -> ReturnT
  {
    auto func = getTaskRef(data);
    return std::invoke(func);
  };
  using FutureT = Future<InvokeResult<Func>>;
  FutureT result = enqueueImpl<ReturnT, false>(wrapped_task, 0, 1, parent_task_id);
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
    -> Future<InvokeResult<Func, int64b>>
{
  using ReturnT = InvokeResult<Func, int64b>;
  auto wrapped_task = makeTaskData<int64b>(std::forward<Func>(task));
  using FutureT = Future<InvokeResult<Func, int64b>>;
  FutureT result = enqueueImpl<ReturnT, false>(wrapped_task, 0, 1, parent_task_id);
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
                                const int64b parent_task_id) -> Future<void>
{
  using Ite = CommonIte<Ite1, Ite2>;
  auto tmp = makeTaskData<Ite>(std::forward<Func>(task));
  auto wrapped_task = [data = std::move(tmp)](const Ite it, const int64b) mutable
  {
    auto func = getTaskRef<Ite>(data);
    std::invoke(func, it);
  };
  Future<void> result = enqueueImpl<void, true>(wrapped_task,
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
                                const int64b parent_task_id) -> Future<void>
{
  using Ite = CommonIte<Ite1, Ite2>;
  auto wrapped_task = makeTaskData<Ite, int64b>(std::forward<Func>(task));
  Future<void> result = enqueueImpl<void, true>(wrapped_task,
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
  const bool result = taskQueue().isEmpty();
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
  taskQueue().setCapacity(cap);
  taskIdTree().setCapacity(cap);
  task_storage_list_.resize(cap);
  ZISC_ASSERT(cap <= task_storage_list_.capacity(), "Allocating task mem failed.");
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t ThreadManager::size() const noexcept
{
  const std::size_t s = taskQueue().size();
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
ThreadManager::WorkerTask::WorkerTask() noexcept :
    QueryValue()
{
  static_assert(sizeof(WorkerTask) == 24);
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
    QueryValue(0),
    it_offset_{it_offset},
    task_{task}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
ThreadManager::WorkerTask::WorkerTask(WorkerTask&& other) noexcept :
    QueryValue(other.get()),
    it_offset_{other.it_offset_},
    task_{std::move(other.task_)}
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
  QueryValue::operator=(other);
  it_offset_ = other.it_offset_;
  task_ = std::move(other.task_);
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
  */
inline
ThreadManager::TaskResource::TaskResource() noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
inline
ThreadManager::TaskResource::TaskResource([[maybe_unused]] TaskResource&& other) noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
inline
auto ThreadManager::TaskResource::operator=([[maybe_unused]] TaskResource&& other) noexcept
    -> TaskResource&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::TaskResource::offset() noexcept
{
  const std::size_t o = kOffset;
  return o;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::TaskResource::storageAlignment() noexcept
{
  const std::size_t a = std::alignment_of_v<decltype(storage_)>;
  return a;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::TaskResource::storageSize() noexcept
{
  const std::size_t s = sizeof(storage_);
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::TaskResource::taskSize() noexcept
{
  const std::size_t s = kTaskSize;
  return s;
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

  auto work = [this]()
  {
    // Wait until all worker creation are completed
    atomic_wait(std::addressof(worker_lock_), -1, std::memory_order::acquire);
    // Run worker tasks
    const int64b thread_id = getCurrentThreadId();
    doWorkerTasks(thread_id);
  };

  for (std::size_t i = 0; i < n; ++i) {
    worker_list_.emplace_back(work);
    worker_id_list_.emplace_back(worker_list_.back().get_id());
  }
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

  \param [in] thread_id No description.
  */
inline
void ThreadManager::doWorkerTasks(const int64b thread_id)
{
  while (workersAreEnabled()) {
    WorkerTask task = fetchTask();
    if (task.hasTask()) {
      task.run(thread_id);
    }
    else { // Wait for next task
      const auto num_of_tasks = worker_lock_.load(std::memory_order::acquire);
      if (0 < num_of_tasks) {
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
                                const int64b parent_task_id) -> Future<ReturnT>
{
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
    TaskImpl(const int64b task_id, const int64b parent_id,
             Data&& data, Ite1 b, ThreadManager* manager) noexcept
        : PackagedTask(task_id, parent_id),
          data_{std::move(data)},
          begin_{std::forward<Ite1>(b)},
          manager_{manager}
    {
    }
    //! Finalize the task
    ~TaskImpl() noexcept override
    {
      if constexpr (kIsLoopTask || std::is_void_v<ReturnT>)
        setResult<ReturnT>(0);
      [[maybe_unused]] const auto id_result = manager_->taskIdTree().remove(id());
      ZISC_ASSERT(id_result.isSuccess(), "The given id isn't in the task tree.");
    }
    //! Run a task
    void run(const int64b thread_id, const DiffType it_offset) override
    {
      manager_->waitForParent(id(), parentId());
      if constexpr (kIsLoopTask) { // Loop task
        auto func = getTaskRef<Ite, int64b>(data_);
        auto ite = advance(begin_, it_offset);
        std::invoke(func, ite, thread_id);
      }
      else { // Single task
        auto func = getTaskRef<int64b>(data_);
        if constexpr (std::is_void_v<ReturnT>) {
          std::invoke(func, thread_id);
        }
        else {
          auto value = std::invoke(func, thread_id);
          setResult<ReturnT>(std::move(value));
        }
      }
    }

   private:
    Data data_;
    Ite begin_;
    ThreadManager* manager_;
  };

  class OverflowErrorImpl : public OverflowError
  {
   public:
    //! Construct the queue error of the thread manager
    OverflowErrorImpl(const std::string_view what_arg,
                      SharedTask&& t,
                      Future<ReturnT>&& f,
                      const DiffType begin_offset,
                      const DiffType num_of_iterations) :
        OverflowError(what_arg, std::move(t), begin_offset, num_of_iterations),
        future_{std::move(f)}
    {
    }
    //! Move data
    OverflowErrorImpl(OverflowErrorImpl&& other) :
        OverflowError(std::move(other)),
        future_{std::move(other.future_)}
    {
    }
    //! Finalize the queue error
    ~OverflowErrorImpl() noexcept override
    {
    }
    //! Move data
    OverflowErrorImpl& operator=(OverflowErrorImpl&& other)
    {
      OverflowError::operator=(std::move(other));
      future_ = std::move(other.future_);
      return *this;
    }
   private:
    Future<ReturnT> future_;
  };
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_CLANG || Z_GCC

  // Issue a task ID
  const int64b task_id = issueTaskId();
  const auto id_result = taskIdTree().add(task_id);
  ZISC_ASSERT(id_result.isSuccess(), "Registering the task ID failed: id=", task_id);
  const std::size_t storage_index = id_result.get().get();

  // Create a shared task
  const DiffType num_of_tasks = distance(begin, end);
  auto shared_task = makeSharedTask<TaskImpl>(storage_index, task_id, parent_task_id,
                                              std::move(task), std::forward<Ite1>(begin));
  Future<ReturnT> result{shared_task.get()};

  auto throw_exception = [](auto& t, auto& f, auto b, auto e)
  {
    const char* message = "Task queue overflow happened.";
    throw OverflowErrorImpl{message, std::move(t), std::move(f), b, e};
  };

  // Enqueue tasks
  {
    auto word_ptr = std::addressof(worker_lock_.get());
    atomic_fetch_add(word_ptr, num_of_tasks, std::memory_order::release);
    for (DiffType i = 0; i < num_of_tasks; ++i) {
      WorkerTask worker_task{shared_task, i};
      try {
        [[maybe_unused]] const auto r = taskQueue().enqueue(std::move(worker_task));
      }
      catch (const TaskQueue::OverflowError& /* error */) {
        const DiffType rest = num_of_tasks - i;
        atomic_fetch_sub(word_ptr, rest, std::memory_order::release);
        atomic_notify_all(std::addressof(worker_lock_));
        throw_exception(shared_task, result, i, num_of_tasks);
      }
      atomic_notify_one(std::addressof(worker_lock_));
    }
    if constexpr (kIsLoopTask)
      atomic_notify_all(std::addressof(worker_lock_));
  }

  return result;
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
  num_of_active_workers_.store(0, std::memory_order::release);
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::fetchTask() noexcept -> WorkerTask
{
  auto queued_task = taskQueue().dequeue();
  static_assert(sizeof(queued_task) == sizeof(WorkerTask));
  if (queued_task.isSuccess())
    atomic_fetch_dec(std::addressof(worker_lock_.get()), std::memory_order::release);
  return std::move(queued_task.get());
}

/*!
  \details No detailed description

  \param [in] task_id No description.
  */
inline
void ThreadManager::finishTask([[maybe_unused]] const int64b task_id) noexcept
{
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
  static_assert(TaskQueue::isConcurrent(), "TaskQueue isn't concurrent.");
  static_assert(TaskIdTree::isConcurrent(), "TaskIdTree isn't concurrent.");
  createWorkers(num_of_threads);
  setCapacity(defaultCapacity());
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

  \tparam Task No description.
  \tparam TaskData No description.
  \tparam Ite No description.
  \param [in] storage_index No description.
  \param [in] task_id No description.
  \param [in] parent_task_id No description.
  \param [in] data No description.
  \param [in] ite No description.
  \return No description
  */
template <typename Task, typename TaskData, typename Ite> inline
SharedTask ThreadManager::makeSharedTask(const std::size_t storage_index,
                                         const int64b task_id,
                                         const int64b parent_task_id,
                                         TaskData&& data,
                                         Ite&& ite) noexcept
{
  TaskResource& storage = task_storage_list_[storage_index];
  constexpr std::size_t task_size = sizeof(Task);
  constexpr std::size_t task_alignment = (std::max)(std::alignment_of_v<Task>,
                                                    std::alignment_of_v<void*>);
  constexpr bool can_internal_resource_be_used =
      (task_size <= TaskResource::taskSize()) &&
      has_single_bit(task_alignment) &&
      (task_alignment <= TaskResource::storageAlignment());
  pmr::memory_resource* mem_resource = can_internal_resource_be_used
      ? std::addressof(storage)
      : resource();
  pmr::polymorphic_allocator<Task> task_alloc{mem_resource};
  const int64b parent_id = (task_id == 0) ? kNoTask : parent_task_id;
  SharedTask shared_task = std::allocate_shared<Task>(task_alloc,
                                                      task_id,
                                                      parent_id,
                                                      std::forward<TaskData>(data),
                                                      std::forward<Ite>(ite),
                                                      this);
  return shared_task;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::taskIdTree() noexcept -> TaskIdTree&
{
  return task_id_tree_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::taskIdTree() const noexcept -> const TaskIdTree&
{
  return task_id_tree_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::taskQueue() noexcept -> TaskQueue&
{
  return task_queue_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::taskQueue() const noexcept -> const TaskQueue&
{
  return task_queue_;
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
    if (parent_task_id == kNoTask) {
      flag = true;
    }
    else if (parent_task_id == kAllPrecedences) {
      const double key = cast<double>(task_id);
      const auto min_key = taskIdTree().findMinKey();
      ZISC_ASSERT(min_key.isSuccess(), "Any key wasn't found.");
      flag = equal(key, min_key.get());
    }
    else {
      const auto result = taskIdTree().contain(parent_task_id);
      flag = !result.isSuccess();
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
