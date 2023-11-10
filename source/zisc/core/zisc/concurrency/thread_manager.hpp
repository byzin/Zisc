/*!
  \file thread_manager.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_HPP
#define ZISC_THREAD_MANAGER_HPP

// Standard C++ library
#include <atomic>
#include <concepts>
#include <cstddef>
#include <future>
#include <limits>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "bitset.hpp"
#include "packaged_task.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/monotonic_buffer_resource.hpp"
#include "zisc/structure/queue.hpp"
#include "zisc/structure/lock_free_queue.hpp"

namespace zisc {

// Forward declaration
template <std::movable> class ContainerOverflowError;

/*!
  \brief ThreadManager class provides task parallel and data parallel thread pool

  No detailed description.
  */
class ThreadManager : private NonCopyable<ThreadManager>
{
 public:
  // Type aliases
  using DiffT = PackagedTask::DiffT;
  using SharedTaskT = std::shared_ptr<PackagedTask>;
  template <typename Func, typename ...Args>
  using InvokeResultT = std::remove_volatile_t<std::invoke_result_t<Func, Args...>>;
  template <typename Ite1, typename Ite2>
  using CommonIteT = std::common_type_t<std::remove_reference_t<Ite1>,
                                        std::remove_reference_t<Ite2>>;


  // Exception
  /*!
    \brief No brief description

    No detailed description.
    */
  class TaskExceptionData
  {
   public:
    //! Initialize the data
    TaskExceptionData(SharedTaskT&& t, const DiffT offset, const DiffT n);


    //! Return the begin offset of the iterator
    [[nodiscard]]
    auto beginOffset() const noexcept -> DiffT;

    //! Return the number of iterations which the task should be ran
    [[nodiscard]]
    auto numOfIterations() const noexcept -> DiffT;

    //! Return the underlying task
    template <typename Type>
    auto task() noexcept -> PackagedTaskType<Type>&;

    //! Return the underlying task
    template <typename Type>
    auto task() const noexcept -> const PackagedTaskType<Type>&;

   private:
    SharedTaskT task_;
    DiffT begin_offset_;
    DiffT num_of_iterations_;
  };

  using OverflowError = ContainerOverflowError<TaskExceptionData>;


  //! Create threads as many as the number of supported concurrent CPU threads
  explicit ThreadManager(std::pmr::memory_resource* mem_resource) noexcept;

  //! Create threads
  ThreadManager(const int64b num_of_threads,
                std::pmr::memory_resource* mem_resource) noexcept;

  //! Terminate threads
  ~ThreadManager();


  //! Return the maximum possible available alignment for task and return value
  static constexpr auto alignmentMax() noexcept -> std::size_t;

  //! Return the maximum possible number of task items
  auto capacity() const noexcept -> std::size_t;

  //! Clear the thread manager state
  void clear() noexcept;

  //! Return the default capacity of the task item queue
  static constexpr auto defaultCapacity() noexcept -> std::size_t;

  //! Run the given task on a worker thread in the thread pool
  template <Invocable Func>
  [[nodiscard]]
  auto enqueue(Func&& task, const bool wait_for_precedence = false)
      -> std::future<InvokeResultT<Func>>;

  //! Run the given task on a worker thread in the thread pool
  template <Invocable<int64b> Func>
  [[nodiscard]]
  auto enqueue(Func&& task, const bool wait_for_precedence = false)
      -> std::future<InvokeResultT<Func, int64b>>;

  //! Run tasks on the worker threads in the thread pool 
  template <typename Func, typename Ite1, typename Ite2>
  requires Invocable<Func, ThreadManager::CommonIteT<Ite1, Ite2>>
  [[nodiscard]]
  auto enqueueLoop(Func&& task, Ite1&& begin, Ite2&& end, const bool wait_for_precedence = false)
      -> std::future<void>;

  //! Run tasks on the worker threads in the thread pool
  template <typename Func, typename Ite1, typename Ite2>
  requires Invocable<Func, ThreadManager::CommonIteT<Ite1, Ite2>, int64b>
  [[nodiscard]]
  auto enqueueLoop(Func&& task, Ite1&& begin, Ite2&& end, const bool wait_for_precedence = false)
      -> std::future<void>;

  //! Check whether the task queue is empty
  auto isEmpty() const noexcept -> bool;

  //! Return the number of logical cores
  static auto logicalCores() noexcept -> int64b;

  //! Return the number of worker threads
  auto numOfThreads() const noexcept -> int64b;

  //! Return a pointer to the underlying memory resource
  auto resource() const noexcept -> std::pmr::memory_resource*;

  //! Change the maximum possible number of task items. The queued tasks are cleared
  void setCapacity(const std::size_t cap);

  //! Return the number of queued tasks
  auto size() const noexcept -> std::size_t;

  //! Return the unmanaged thread ID
  static constexpr auto unmanagedThreadId() noexcept -> int64b;

  //! Wait current thread until all tasks in the queue are completed
  void waitForCompletion() noexcept;

 private:

  // task types
  /*!
    \brief No brief description

    No detailed description.
    */
  class WorkerTask : private NonCopyable<WorkerTask>
  {
   public:
    //! Create an empty task
    WorkerTask() noexcept = default;

    //! Create a task
    WorkerTask(SharedTaskT task, const DiffT it_offset) noexcept;

    //! Move data
    WorkerTask(WorkerTask&& other) noexcept;

    //! Destroy a task
    ~WorkerTask() noexcept = default;

    //! Move data
    auto operator=(WorkerTask&& other) noexcept -> WorkerTask&;

    //! Move data
    void operator()(const int64b thread_id);

    //! Check if the underlying task is valid
    [[nodiscard]]
    auto isValid() const noexcept -> bool;

    //! Run a task
    void run(const int64b thread_id);

   private:
    DiffT it_offset_ = 0;
    SharedTaskT task_;
  };

  //! Implementation of shared task
  template <typename Return, typename Data, typename Ite1, typename Ite2, bool kIsLoop>
  class TaskImpl;

  // Type aliases
  using TaskQueueImpl = PortableRingQueue<WorkerTask>;
  using TaskQueue = Queue<TaskQueueImpl, WorkerTask>;


  //! Increment the given iterator
  template <typename Ite, typename OffsetT>
  static auto advance(Ite& begin, const OffsetT offset) noexcept -> Ite;

  //! Create a shared task
  template <std::derived_from<PackagedTask> Task, typename Data, typename Ite>
  auto createSharedTask(Data&& data, Ite&& ite, const bool wait_for_precedence)
      -> std::shared_ptr<Task>;

  //! Create worker threads
  void createWorkers(const int64b num_of_threads);

  //! Return the distance of given two iterators
  template <typename Ite1, typename Ite2>
  static auto distance(const Ite1& begin, const Ite2& end) noexcept -> DiffT;

  //! Do worker task
  void doWorkerTasks(const int64b thread_id);

  //! Run tasks on the worker threads in the manager
  template <typename ReturnT, bool kIsLoop, typename Data, typename Ite1, typename Ite2>
  auto enqueueImpl(Data&& task, Ite1&& begin, Ite2&& end, const bool wait_for_precedence = false)
      -> std::future<ReturnT>;

  //! Exit workers running
  void exitWorkersRunning() noexcept;

  //! Fetch a task from the top of the queue
  auto fetchTask() noexcept -> std::optional<WorkerTask>;

  //! Return the actual available number of cores from the given hint s
  static auto getAvailableNumOfThreads(const int64b s) noexcept -> std::size_t;

  //! Return the thread ID of the working thread
  auto getCurrentThreadId() const noexcept -> int64b;

  //! Return the function reference of the given function data
  template <typename ...Types, typename WrappedTask>
  static auto getTask(WrappedTask&& data) noexcept;

  //! Initialize this thread manager
  void initialize(const int64b num_of_threads) noexcept;

  //! Issue a new task ID
  auto issueTaskId() noexcept -> int64b;

  //! Return the task status list
  auto taskStatusList() noexcept -> Bitset&;

  //! Return the task status list
  auto taskStatusList() const noexcept -> const Bitset&;

  //! Return the size of task status list
  static constexpr auto taskStatusSize() noexcept -> std::size_t;

  //! Return the task queue
  auto taskQueue() noexcept -> TaskQueue&;

  //! Return the task queue
  auto taskQueue() const noexcept -> const TaskQueue&;

  //! Wait current thread for all precedence task are completed
  void waitForPrecedence(const int64b task_id) const noexcept;

  //! Check if the workers (threads) are enable running
  auto workersAreEnabled() const noexcept -> bool;

  //! Wrap task data for share between workers
  template <typename ...Types, Invocable<Types...> Func>
  static auto wrapTask(Func&& func) noexcept;


  static constexpr std::size_t kCacheLineSize = 2 * Config::l1CacheLineSize();
  static constexpr std::size_t kAlignmentMax = kCacheLineSize;
  static constexpr std::size_t kTaskStorageSize = 8 * kCacheLineSize;
  using TaskResource = MonotonicBufferResource<kTaskStorageSize, kAlignmentMax>;


  alignas(kCacheLineSize) std::atomic<int64b> task_id_count_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(task_id_count_)> pad1_{};
  alignas(kCacheLineSize) std::atomic<DiffT> num_of_tasks_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(num_of_tasks_)> pad2_{};
  alignas(kCacheLineSize) std::atomic<int> num_of_active_workers_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(num_of_active_workers_)> pad3_{};
  TaskQueueImpl task_queue_;
  Bitset task_status_list_;
  std::pmr::vector<std::thread> worker_list_;
  std::pmr::vector<std::thread::id> worker_id_list_;
  std::pmr::vector<TaskResource> task_storage_list_;
  static constexpr std::size_t kManagerSize = sizeof(task_queue_) +
                                              sizeof(task_status_list_) +
                                              sizeof(decltype(worker_list_)) +
                                              sizeof(decltype(worker_id_list_)) +
                                              sizeof(decltype(task_storage_list_));
  [[maybe_unused]] Padding<kCacheLineSize - (kManagerSize % kCacheLineSize)> pad4_{};
};

} // namespace zisc

/*!
  \example thread_manager_example.cpp
  This is an example of how to use ThreadManager class."
  */

#include "thread_manager-inl.hpp"

#endif // ZISC_THREAD_MANAGER_HPP
