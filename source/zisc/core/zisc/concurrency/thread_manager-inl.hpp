/*!
  \file thread_manager-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
#include <bit>
#include <concepts>
#include <cstddef>
#include <functional>
#include <future>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "packaged_task.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"
#include "zisc/memory/memory.hpp"
#include "zisc/memory/monotonic_buffer_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"
#include "zisc/structure/queue.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] t No description.
  \param [in] offset No description.
  \param [in] n No description.
  */
inline
ThreadManager::TaskExceptionData::TaskExceptionData(SharedTaskT&& t,
                                                    const DiffT offset,
                                                    const DiffT n) :
    task_{std::move(t)},
    begin_offset_{offset},
    num_of_iterations_{n}
{
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::TaskExceptionData::beginOffset() const noexcept -> DiffT
{
  return begin_offset_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::TaskExceptionData::numOfIterations() const noexcept -> DiffT
{
  return num_of_iterations_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
PackagedTaskType<Type>& ThreadManager::TaskExceptionData::task() noexcept
{
  auto* t = dynamic_cast<PackagedTaskType<Type>*>(task_.get());
  ZISC_ASSERT(t != nullptr, "The dynamic cast of the packaged task failed.");
  return *t;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
const PackagedTaskType<Type>& ThreadManager::TaskExceptionData::task() const noexcept
{
  const auto* t = dynamic_cast<const PackagedTaskType<Type>*>(task_.get());
  ZISC_ASSERT(t != nullptr, "The dynamic cast of the packaged task failed.");
  return *t;
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
    task_id_count_{0},
    num_of_tasks_{0},
    num_of_active_workers_{0},
    task_queue_{defaultCapacity(), mem_resource},
    task_status_list_{taskStatusSize(), mem_resource},
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
template <typename ...Types, typename WrappedTask> inline
auto ThreadManager::getTask(WrappedTask&& data) noexcept
{
  using FuncT1 = std::remove_volatile_t<std::remove_reference_t<WrappedTask>>;
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
auto ThreadManager::wrapTask(Func&& func) noexcept
{
  using Function = std::remove_volatile_t<std::remove_reference_t<Func>>;
  using ReturnT = InvokeResultT<Function, Types...>;
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
constexpr std::size_t ThreadManager::alignmentMax() noexcept
{
  return kAlignmentMax;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t ThreadManager::capacity() const noexcept
{
  const std::size_t cap = taskQueue().capacity();
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
  task_id_count_.store(0, std::memory_order::release);
  taskQueue().clear();
  taskStatusList().reset();
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
  \param [in] wait_for_precedence No description.
  \return No description
  */
template <Invocable Func> inline
auto ThreadManager::enqueue(Func&& task, const bool wait_for_precedence)
    -> std::future<InvokeResultT<Func>>
{
  using ReturnT = InvokeResultT<Func>;
  auto t = wrapTask(std::forward<Func>(task));
  auto wrapped_task = [data = std::move(t)](const int64b) mutable -> ReturnT
  {
    auto func = getTask(data);
    return std::invoke(func);
  };
  std::future result = enqueueImpl<ReturnT, false>(std::move(wrapped_task),
                                                   0,
                                                   1,
                                                   wait_for_precedence);
  return result;
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] task No description.
  \param [in] wait_for_precedence No description.
  \return No description
  */
template <Invocable<int64b> Func> inline
auto ThreadManager::enqueue(Func&& task, const bool wait_for_precedence)
    -> std::future<InvokeResultT<Func, int64b>>
{
  using ReturnT = InvokeResultT<Func, int64b>;
  auto wrapped_task = wrapTask<int64b>(std::forward<Func>(task));
  std::future result = enqueueImpl<ReturnT, false>(std::move(wrapped_task),
                                                   0,
                                                   1,
                                                   wait_for_precedence);
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
  \param [in] wait_for_precedence No description.
  \return No description
  */
template <typename Func, typename Ite1, typename Ite2>
requires Invocable<Func, ThreadManager::CommonIteT<Ite1, Ite2>> inline
auto ThreadManager::enqueueLoop(Func&& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const bool wait_for_precedence) -> std::future<void>
{
  using IteT = CommonIteT<Ite1, Ite2>;
  auto t = wrapTask<IteT>(std::forward<Func>(task));
  auto wrapped_task = [data = std::move(t)](const IteT it, const int64b) mutable
  {
    auto func = getTask<IteT>(data);
    std::invoke(func, it);
  };
  std::future result = enqueueImpl<void, true>(std::move(wrapped_task),
                                               std::forward<Ite1>(begin),
                                               std::forward<Ite2>(end),
                                               wait_for_precedence);
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
  \param [in] wait_for_precedence No description.
  \return No description
  */
template <typename Func, typename Ite1, typename Ite2>
requires Invocable<Func, ThreadManager::CommonIteT<Ite1, Ite2>, int64b> inline
auto ThreadManager::enqueueLoop(Func&& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const bool wait_for_precedence) -> std::future<void>
{
  using IteT = CommonIteT<Ite1, Ite2>;
  auto wrapped_task = wrapTask<IteT, int64b>(std::forward<Func>(task));
  std::future result = enqueueImpl<void, true>(std::move(wrapped_task),
                                               std::forward<Ite1>(begin),
                                               std::forward<Ite2>(end),
                                               wait_for_precedence);
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
  pmr::memory_resource* mem_resource = worker_list_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
inline
void ThreadManager::setCapacity(const std::size_t cap) noexcept
{
  if (workersAreEnabled())
    waitForCompletion();
  taskQueue().setCapacity(cap);
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

  \tparam TaskImpl No description.
  \param [in] task No description.
  \param [in] it_offset No description.
  */
inline
ThreadManager::WorkerTask::WorkerTask(const SharedTaskT& task,
                                      const DiffT it_offset) noexcept :
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
    it_offset_{other.it_offset_},
    task_{std::move(other.task_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
auto ThreadManager::WorkerTask::operator=(WorkerTask&& other) noexcept -> WorkerTask&
{
  it_offset_ = other.it_offset_;
  task_ = std::move(other.task_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] thread_id No description.
  */
inline
void ThreadManager::WorkerTask::operator()(const int64b thread_id)
{
  run(thread_id);
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool ThreadManager::WorkerTask::isValid() const noexcept
{
  const bool is_valid = cast<bool>(task_);
  return is_valid;
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
  \brief No brief description

  No detailed description.

  \tparam ReturnT No description.
  \tparam Data No description.
  \tparam Ite1 No description.
  \tparam Ite2 No description.
  \tparam kIsLoop No description.
  */
template <typename Return, typename Data, typename Ite1, typename Ite2, bool kIsLoop>
class ThreadManager::TaskImpl : public PackagedTaskType<Return>
{
  using BaseT = PackagedTaskType<Return>;

 public:
  // Type aliases
  using ReturnT = typename BaseT::ReturnT;
  using PromiseT = std::promise<ReturnT>;
  using DataT = std::remove_reference_t<Data>;
  using IteT = std::remove_cv_t<CommonIteT<Ite1, Ite2>>;


  //! Initialize the task
  TaskImpl(const int64b task_info, PromiseT&& p, ThreadManager* manager) noexcept :
      BaseT(task_info),
      promise_{std::move(p)},
      manager_{manager}
  {
  }

  //! Finalize the task
  ~TaskImpl() noexcept override
  {
    if constexpr (kIsLoop) {
      promise_.set_value();
      manager_->taskStatusList().testAndSet(static_cast<std::size_t>(BaseT::id()), true);
    }
  }

  //! Return the future of the underlying task
  std::future<ReturnT> getFuture() noexcept override
  {
    return promise_.get_future();
  }

  //! Run a task
  void run(const int64b thread_id, [[maybe_unused]] const DiffT it_offset) override
  {
    if (BaseT::isNeededToWaitForPrecedence())
      manager_->waitForPrecedence(BaseT::id());
    if constexpr (kIsLoop) { // Loop task
      auto func = getTask<IteT, int64b>(*task_);
      IteT ite = advance(*begin_, it_offset);
      std::invoke(func, ite, thread_id);
    }
    else { // Single task
      auto func = getTask<int64b>(*task_);
      if constexpr (std::is_void_v<ReturnT>) {
        std::invoke(func, thread_id);
        promise_.set_value();
      }
      else {
        ReturnT value = std::invoke(func, thread_id);
        promise_.set_value(std::move(value));
      }
      manager_->taskStatusList().testAndSet(static_cast<std::size_t>(BaseT::id()), true);
    }
  }

  //
  void setData(DataT&& data, Ite1&& b) noexcept
  {
    task_.set(std::move(data));
    begin_.set(std::forward<Ite1>(b));
  }

 private:
  PromiseT promise_;
  DataStorage<DataT> task_;
  DataStorage<IteT> begin_;
  ThreadManager* manager_;
};

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
  Ite ite = begin;
  if constexpr (std::is_arithmetic_v<Ite>)
    ite += cast<Ite>(offset);
  else
    std::advance(ite, offset);
  return ite;
}

/*!
  \details No detailed description

  \tparam Task No description.
  \tparam Data No description.
  \tparam Ite No description.
  \param [in] data No description.
  \param [in] ite No description.
  \param [in] wait_for_precedence No description.
  \return No description
  */
template <std::derived_from<PackagedTask> Task, typename Data, typename Ite> inline
std::shared_ptr<Task> ThreadManager::createSharedTask(Data&& data,
                                                      Ite&& ite,
                                                      const bool wait_for_precedence)
{
  constexpr std::size_t alloc_trial_max = 4;
  std::shared_ptr<Task> task{};
  for (std::size_t i = 0; !task && (i < alloc_trial_max); ++i) {
    // Issue a task ID
    const int64b task_id = issueTaskId();

    // Check resource
    const auto storage_id = static_cast<std::size_t>(task_id);
    TaskResource& task_resource = task_storage_list_[storage_id];
    if (!task_resource.isOccupied())
      task_resource.release();

    // Get a resource for the task
    using ReturnT = typename Task::ReturnT;
    using T = std::conditional_t<std::is_same_v<void, ReturnT>, void*, ReturnT>;
    // The max value is heuristic. 3 * sizeof(ReturnT) is needed for promise
    constexpr std::size_t alloc_max = 3 * sizeof(T) + sizeof(Task);
    const bool use_task_resource = alloc_max <= TaskResource::capacity();
    pmr::memory_resource* mem_resource = use_task_resource 
        ? std::addressof(task_resource)
        : resource();

    // Allocate objects
    try {
      using PromiseT = typename Task::PromiseT;
      pmr::polymorphic_allocator<PromiseT> promise_alloc{mem_resource};
      PromiseT promise{std::allocator_arg, promise_alloc};

      pmr::polymorphic_allocator<Task> task_alloc{mem_resource};
      const int64b info = PackagedTask::encodeInfo(task_id, wait_for_precedence);
      task = std::allocate_shared<Task>(task_alloc, info, std::move(promise), this);
    }
    catch ([[maybe_unused]] const Memory::BadAllocation& error) {
      if (i == (alloc_trial_max - 1))
        throw;
    }
  }
  ZISC_ASSERT(task != nullptr, "The shared task is still null.");
  task->setData(std::forward<Data>(data), std::forward<Ite>(ite));

  return task;
}

/*!
  \details No detailed description

  \param [in] num_of_threads No description.
  */
inline
void ThreadManager::createWorkers(const int64b num_of_threads) noexcept
{
  const std::size_t n = getAvailableNumOfThreads(num_of_threads);
  worker_list_.reserve(n);
  worker_id_list_.reserve(n);
  constexpr DiffT not_ready = -1;
  num_of_tasks_.store(not_ready, std::memory_order::release);
  num_of_active_workers_.store(cast<int>(n), std::memory_order::release);

  auto work = [this]()
  {
    // Wait until all worker creation are completed
    num_of_tasks_.wait(not_ready, std::memory_order::acquire);
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
  num_of_tasks_.store(0, std::memory_order::release);
  num_of_tasks_.notify_all();
  // Wait until underlying threads actually become ready
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
auto ThreadManager::distance(Ite1&& begin, Ite2&& end) noexcept -> DiffT
{
  using IteT = std::remove_cvref_t<CommonIteT<Ite1, Ite2>>;
  DiffT d = 0;
  if constexpr (std::is_arithmetic_v<IteT>)
    d = cast<DiffT>(end - begin);
  else
    d = cast<DiffT>(std::distance(begin, end));
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
    std::optional<WorkerTask> task = fetchTask();
    if (task.has_value() && task->isValid()) {
      (*task)(thread_id);
    }
    else { // Wait for next task
      if (0 < num_of_tasks_.load(std::memory_order::acquire)) {
        // The worker just missed queued tasks, waits a little
        std::this_thread::yield();
      }
      else {
        // There is no task. The worker waits for next task queuing
        num_of_active_workers_.fetch_sub(1, std::memory_order::release);
        num_of_tasks_.wait(0, std::memory_order::acquire);
        num_of_active_workers_.fetch_add(1, std::memory_order::release);
      }
    }
  }
}

/*!
  \details No detailed description

  \tparam ReturnT No description.
  \tparam kIsLoop No description.
  \tparam Data No description.
  \tparam Ite1 No description.
  \tparam Ite2 No description.
  \param [in] task No description.
  \param [in] begin No description.
  \param [in] end No description.
  \param [in] wait_for_precedence No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename ReturnT, bool kIsLoop, typename Data, typename Ite1, typename Ite2>
inline
auto ThreadManager::enqueueImpl(Data&& task,
                                Ite1&& begin,
                                Ite2&& end,
                                const bool wait_for_precedence) -> std::future<ReturnT>
{
  const DiffT num_of_tasks = distance(begin, end);

  // Create a shared task
  using TaskImplT = TaskImpl<ReturnT, Data, Ite1, Ite2, kIsLoop>;
  std::shared_ptr shared_task = createSharedTask<TaskImplT>(std::forward<Data>(task),
                                                            std::forward<Ite1>(begin),
                                                            wait_for_precedence);

  // Enqueue tasks
  num_of_tasks_.fetch_add(num_of_tasks, std::memory_order::relaxed);
  for (DiffT i = 0; i < num_of_tasks; ++i) {
    WorkerTask worker_task{shared_task, i};
    try {
      [[maybe_unused]] const std::optional result = taskQueue().enqueue(std::move(worker_task));
    }
    catch ([[maybe_unused]] const TaskQueue::OverflowError& error) {
      const DiffT rest = num_of_tasks - i;
      num_of_tasks_.fetch_sub(rest, std::memory_order::relaxed);
      num_of_tasks_.notify_all();
      const char* message = "Task queue overflow happened.";
      throw OverflowError{message,
                          resource(),
                          {std::move(shared_task), i, num_of_tasks}};
    }
    num_of_tasks_.notify_one();
  }

  return shared_task->getFuture();
}

/*!
  \details No detailed description
  */
inline
void ThreadManager::exitWorkersRunning() noexcept
{
  num_of_tasks_.store(-1, std::memory_order::release);
  num_of_tasks_.notify_all();
  std::for_each(worker_list_.begin(), worker_list_.end(), [](std::thread& worker)
  {
    worker.join();
  });
  num_of_active_workers_.store(0, std::memory_order::release);
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto ThreadManager::fetchTask() noexcept -> std::optional<WorkerTask>
{
  std::optional<WorkerTask> queued_task = taskQueue().dequeue();
  if (queued_task.has_value())
    num_of_tasks_.fetch_sub(1, std::memory_order::relaxed);
  return queued_task;
}

/*!
  \details No detailed description

  \param [in] s No description.
  \return No description
  */
inline
std::size_t ThreadManager::getAvailableNumOfThreads(const int64b s) noexcept
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
  const std::thread::id id = std::this_thread::get_id();
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
  static_assert(TaskQueue::isConcurrent(), "TaskQueue doesn't support concurrency.");
  createWorkers(num_of_threads);
  setCapacity(defaultCapacity());
  // Initialize resources
  task_storage_list_.clear();
  task_storage_list_.reserve(taskStatusList().size());
  for (std::size_t i = 0; i < taskStatusList().size(); ++i)
    task_storage_list_.emplace_back(resource());
}

/*!
  \details No detailed description

  \return No description
  */
inline
int64b ThreadManager::issueTaskId() noexcept
{
  int64b id = task_id_count_.fetch_add(1, std::memory_order::acq_rel);

  const auto index = static_cast<std::size_t>(id);
  if (taskStatusList().isAll(0, index) || (taskStatusList().size() <= index)) {
    clear();
    id = task_id_count_.fetch_add(1, std::memory_order::acq_rel);
  }

  return id;
}

/*!
  \details No detailed description

  \return No description
  */
inline
Bitset& ThreadManager::taskStatusList() noexcept
{
  return task_status_list_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const Bitset& ThreadManager::taskStatusList() const noexcept
{
  return task_status_list_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t ThreadManager::taskStatusSize() noexcept
{
  return 4 * Bitset::chunkBitSize();
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
  */
inline
void ThreadManager::waitForPrecedence(const int64b task_id) const noexcept
{
  constexpr std::size_t begin = 0;
  const auto end = static_cast<std::size_t>(task_id);
  while (!taskStatusList().isAll(begin, end))
    std::this_thread::yield();
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool ThreadManager::workersAreEnabled() const noexcept
{
  // The state gets minus value when the manager is destroyed.
  const DiffT state = num_of_tasks_.load(std::memory_order::acquire);
  const bool result = 0 <= state;
  return result;
}

} // namespace zisc

#endif // ZISC_THREAD_MANAGER_INL_HPP
