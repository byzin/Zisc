/*!
  \file thread_manager.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_HPP
#define ZISC_THREAD_MANAGER_HPP

// Standard C++ library
#include <atomic>
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "future.hpp"
#include "packaged_task.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/monotonic_buffer_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"
#include "zisc/structure/map.hpp"
#include "zisc/structure/mutex_bst.hpp"
#include "zisc/structure/queue.hpp"
#include "zisc/structure/scalable_circular_queue.hpp"

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
    TaskExceptionData(SharedTask&& t, const DiffT offset, const DiffT n);


    //! Return the begin offset of the iterator
    DiffT beginOffset() const noexcept;

    //! Return the number of iterations which the task should be ran
    DiffT numOfIterations() const noexcept;

    //! Return the underlying task
    PackagedTask& task() noexcept;

    //! Return the underlying task
    const PackagedTask& task() const noexcept;

   private:
    SharedTask task_;
    DiffT begin_offset_;
    DiffT num_of_iterations_;
  };
  using OverflowError = ContainerOverflowError<TaskExceptionData>;


  // Special parent ID flags
  static constexpr int64b kNoTask = (std::numeric_limits<int64b>::max)();
  static constexpr int64b kAllPrecedences = -1;


  //! Create threads as many as the number of supported concurrent CPU threads
  ThreadManager(pmr::memory_resource* mem_resource) noexcept;

  //! Create threads
  ThreadManager(const int64b num_of_threads,
                pmr::memory_resource* mem_resource) noexcept;

  //! Terminate threads
  ~ThreadManager();


  //! Return the maximum possible available alignment for task and return value
  static constexpr std::size_t alignmentMax() noexcept;

  //! Return the maximum possible number of task items
  std::size_t capacity() const noexcept;

  //! Clear the thread manager state
  void clear() noexcept;

  //! Return the default capacity of the task item queue
  static constexpr std::size_t defaultCapacity() noexcept;

  //! Run the given task on a worker thread in the thread pool
  template <Invocable Func>
  [[nodiscard]]
  Future<InvokeResultT<Func>> enqueue(Func&& task,
                                      const int64b parent_task_id = kNoTask);

  //! Run the given task on a worker thread in the thread pool
  template <Invocable<int64b> Func>
  [[nodiscard]]
  Future<InvokeResultT<Func, int64b>> enqueue(Func&& task,
                                              const int64b parent_task_id = kNoTask);

  //! Run tasks on the worker threads in the thread pool 
  template <typename Func, typename Ite1, typename Ite2>
  requires Invocable<Func, ThreadManager::CommonIteT<Ite1, Ite2>>
  [[nodiscard]]
  Future<void> enqueueLoop(Func&& task,
                           Ite1&& begin,
                           Ite2&& end,
                           const int64b parent_task_id = kNoTask);

  //! Run tasks on the worker threads in the thread pool
  template <typename Func, typename Ite1, typename Ite2>
  requires Invocable<Func, ThreadManager::CommonIteT<Ite1, Ite2>, int64b>
  [[nodiscard]]
  Future<void> enqueueLoop(Func&& task,
                           Ite1&& begin,
                           Ite2&& end,
                           const int64b parent_task_id = kNoTask);

  //! Check whether the task queue is empty
  bool isEmpty() const noexcept;

  //! Return the number of logical cores
  static int64b logicalCores() noexcept;

  //! Return the number of worker threads
  int64b numOfThreads() const noexcept;

  //! Return a pointer to the underlying memory resource
  pmr::memory_resource* resource() const noexcept;

  //! Change the maximum possible number of task items. The queued tasks are cleared
  void setCapacity(const std::size_t cap) noexcept;

  //! Return the number of queued tasks
  std::size_t size() const noexcept;

  //! Return the unmanaged thread ID
  static constexpr int64b unmanagedThreadId() noexcept;

  //! Wait current thread until all tasks in the queue are completed
  void waitForCompletion() noexcept;

 private:
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
    WorkerTask(const SharedTask& task, const DiffT it_offset) noexcept;

    //! Move data
    WorkerTask(WorkerTask&& other) noexcept;

    //! Destroy a task
    ~WorkerTask() noexcept = default;

    //! Move data
    WorkerTask& operator=(WorkerTask&& other) noexcept;

    //! Move data
    void operator()(const int64b thread_id);

    //! Check if the underlying task is valid
    bool isValid() const noexcept;

    //! Run a task
    void run(const int64b thread_id);

   private:
    DiffT it_offset_ = 0;
    SharedTask task_;
  };

  // Type aliases
  using TaskQueueImpl = ScalableCircularQueue<WorkerTask>;
  using TaskQueue = Queue<TaskQueueImpl, WorkerTask>;
  using TaskIdSetImpl = MutexBst<int64b>;
  using TaskIdSet = Map<TaskIdSetImpl, int64b>;


  //! Increment the given iterator
  template <typename Ite, typename OffsetT>
  static Ite advance(Ite& it, const OffsetT offset) noexcept;

  //! Create a shared task
  template <typename Task, typename Data, typename Ite>
  std::shared_ptr<Task> createSharedTask(const int64b parent_task_id,
                                         Data&& data,
                                         Ite&& ite);

  //! Create worker threads
  void createWorkers(const int64b num_of_threads) noexcept;

  //! Return the distance of given two iterators
  template <typename Ite1, typename Ite2>
  static DiffT distance(Ite1&& begin, Ite2&& end) noexcept;

  //! Do worker task
  void doWorkerTasks(const int64b thread_id);

  //! Run tasks on the worker threads in the manager
  template <typename ReturnT, bool kIsLoop, typename Data, typename Ite1, typename Ite2>
  Future<ReturnT> enqueueImpl(Data&& task,
                              Ite1&& begin,
                              Ite2&& end,
                              const int64b parent_task_id);

  //! Exit workers running
  void exitWorkersRunning() noexcept;

  //! Fetch a task from the top of the queue
  std::optional<WorkerTask> fetchTask() noexcept;

  //! Return the actual available number of cores from the given hint s
  static std::size_t getAvailableNumOfThreads(const int64b s) noexcept;

  //! Return the thread ID of the working thread
  int64b getCurrentThreadId() const noexcept;

  //! Return the function reference of the given function data
  template <typename ...Types, typename WrappedTask>
  static auto getTask(WrappedTask&& data) noexcept;

  template <typename Return, bool kIsLoop, typename Data, typename Ite1, typename Ite2>
  static auto& getTaskImplType() noexcept;

  //! Initialize this thread manager
  void initialize(const int64b num_of_threads) noexcept;

  //! Issue a new task ID
  int64b issueTaskId() noexcept;

  //! Return the task ID tree
  TaskIdSet& taskIdSet() noexcept;

  //! Return the task ID tree
  const TaskIdSet& taskIdSet() const noexcept;

  //! Return the task queue
  TaskQueue& taskQueue() noexcept;

  //! Return the task queue
  const TaskQueue& taskQueue() const noexcept;

  //! Wait current thread for parent task complesion
  void waitForParent(const int64b task_id, const int64b parent_task_id) const noexcept;

  //! Check if the workers (threads) are enable running
  bool workersAreEnabled() const noexcept;

  //! Wrap task data for share between workers
  template <typename ...Types, Invocable<Types...> Func>
  static auto wrapTask(Func&& func) noexcept;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
  static constexpr std::size_t kAlignmentMax = kCacheLineSize;
  static constexpr std::size_t kTaskStorageSize = 8 * kCacheLineSize;
  using TaskResource = MonotonicBufferResource<kTaskStorageSize, kAlignmentMax>;


  alignas(kCacheLineSize) std::atomic<int64b> total_queued_task_ids_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(total_queued_task_ids_)> pad1_;
  alignas(kCacheLineSize) std::atomic<DiffT> num_of_tasks_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(num_of_tasks_)> pad2_;
  alignas(kCacheLineSize) std::atomic<int> num_of_active_workers_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(num_of_active_workers_)> pad3_;
  TaskQueueImpl task_queue_;
  TaskIdSetImpl task_id_set_;
  pmr::vector<std::thread> worker_list_;
  pmr::vector<std::thread::id> worker_id_list_;
  pmr::vector<TaskResource> task_storage_list_;
  static constexpr std::size_t kManagerSize = sizeof(task_queue_) +
                                              sizeof(task_id_set_) +
                                              sizeof(worker_list_) +
                                              sizeof(worker_id_list_) +
                                              sizeof(task_storage_list_);
  [[maybe_unused]] Padding<kCacheLineSize - (kManagerSize % kCacheLineSize)> pad4_;
};

} // namespace zisc

/*!
  \example thread_manager_example.cpp
  This is an example of how to use ThreadManager class."
  */

#include "thread_manager-inl.hpp"

#endif // ZISC_THREAD_MANAGER_HPP
