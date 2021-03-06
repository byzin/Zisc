/*!
  \file thread_manager.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_HPP
#define ZISC_THREAD_MANAGER_HPP

// Standard C++ library
#include <atomic>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "atomic.hpp"
#include "atomic_word.hpp"
#include "packaged_task.hpp"
#include "zisc/concepts.hpp"
#include "zisc/data_structure/scalable_circular_queue.hpp"
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

// Forward declaration
template <NonReference> class Future;
class PackagedTask;

// Type aliases
template <NonReference T>
using SharedFuture = std::shared_ptr<Future<T>>;
using SharedTask = std::shared_ptr<PackagedTask>;

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wweak-vtables"
#endif // Z_GCC || Z_CLANG

/*!
  \brief ThreadManager class provides task parallel and data parallel thread pool

  No detailed description.
  */
class ThreadManager : private NonCopyable<ThreadManager>
{
 public:
  // Type aliases
  using DiffType = PackagedTask::DiffType;
  template <typename Func, typename ...Args>
  using InvokeResult = std::remove_volatile_t<std::invoke_result_t<Func, Args...>>;
  template <typename Ite1, typename Ite2>
  using CommonIte = std::common_type_t<std::remove_reference_t<Ite1>,
                                       std::remove_reference_t<Ite2>>;


  /*!
    \brief No brief description

    No detailed description.
    */
  class OverflowError : public SystemError
  {
   public:
    //! Construct the queue error of the thread manager 
    OverflowError(const std::string_view what_arg,
                  SharedTask&& t,
                  const DiffType begin_offset,
                  const DiffType num_of_iterations);

    //! Move data
    OverflowError(OverflowError&& other);

    //! Finalize the queue error
    ~OverflowError() noexcept override;


    //! Move data
    OverflowError& operator=(OverflowError&& other);


    //! Return the offset of iterator
    DiffType beginOffset() const noexcept;

    //! Return the number of iterations which the task should be ran
    DiffType numOfIterations() const noexcept;

    //! Return the underlying task
    PackagedTask& task() noexcept;

    //! Return the underlying task
    const PackagedTask& task() const noexcept;

   private:
    SharedTask task_;
    DiffType begin_offset_;
    DiffType num_of_iterations_;
  };


  static constexpr int64b kNoTask = (std::numeric_limits<int64b>::max)();
  static constexpr int64b kAllPrecedences = -1;


  //! Create threads as many CPU threads as
  ThreadManager(pmr::memory_resource* mem_resource) noexcept;

  //! Create threads
  ThreadManager(const int64b num_of_threads,
                pmr::memory_resource* mem_resource) noexcept;

  //! Terminate threads
  ~ThreadManager();


  //! Return the maximum possible number of task items
  std::size_t capacity() const noexcept;

  //! Clear the thread manager state
  void clear() noexcept;

  //! Return the default capacity of the task item queue
  static constexpr std::size_t defaultCapacity() noexcept;

  //! Run the given task on a worker thread in the manager
  template <Invocable Func>
  SharedFuture<InvokeResult<Func>> enqueue(
      Func&& task,
      const int64b parent_task_id = kNoTask);

  //! Run the given task on a worker thread in the manager
  template <Invocable<int64b> Func>
  SharedFuture<InvokeResult<Func, int64b>> enqueue(
      Func&& task,
      const int64b parent_task_id = kNoTask);

  //! Run tasks on the worker threads in the manager
  template <typename Func, typename Ite1, typename Ite2>
  requires Invocable<Func, ThreadManager::CommonIte<Ite1, Ite2>>
  SharedFuture<void> enqueueLoop(Func&& task,
                                 Ite1&& begin,
                                 Ite2&& end,
                                 const int64b parent_task_id = kNoTask);

  //! Run tasks on the worker threads in the manager
  template <typename Func, typename Ite1, typename Ite2>
  requires Invocable<Func, ThreadManager::CommonIte<Ite1, Ite2>, int64b>
  SharedFuture<void> enqueueLoop(Func&& task,
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
  template <NonReference>
  friend class Future;


  /*!
    \brief No brief description

    No detailed description.
    */
  class WorkerTask : private NonCopyable<WorkerTask>
  {
   public:
    //! Create an empty task
    WorkerTask() noexcept;

    //! Create a task
    template <typename TaskImpl>
    WorkerTask(std::shared_ptr<TaskImpl>& task, const DiffType it_offset) noexcept;

    //! Move data
    WorkerTask(WorkerTask&& other) noexcept;

    //! Move data
    WorkerTask& operator=(WorkerTask&& other) noexcept;

    //! Destroy a task
    ~WorkerTask() noexcept;

    //! Check if this has a task
    bool hasTask() const noexcept;

    //! Run a task
    void run(const int64b thread_id);

   private:
    SharedTask task_;
    DiffType it_offset_ = 0;
    [[maybe_unused]] int32b padding_ = 0;
  };

  // Type aliases
  using TaskQueue = ScalableCircularQueue<WorkerTask>;
  using WorkerLock = AtomicWord<Config::isAtomicOsSpecifiedWaitUsed()>;


  //! Increment the given iterator
  template <typename Ite, typename OffsetT>
  static Ite advance(Ite& it, const OffsetT offset) noexcept;

  //! Create worker threads
  void createWorkers(const int64b num_of_threads) noexcept;

  //! Return the distance of given two iterators
  template <typename Ite1, typename Ite2>
  static DiffType distance(Ite1&& begin, Ite2&& end) noexcept;

  //! Do worker task
  void doWorkerTask(const int64b thread_id) noexcept;

  //! Run tasks on the worker threads in the manager
  template <typename ReturnT, bool kIsLoopTask,
            typename TaskData, typename Ite1, typename Ite2>
  SharedFuture<ReturnT> enqueueImpl(TaskData& task,
                                    Ite1&& begin,
                                    Ite2&& end,
                                    const int64b parent_task_id);

  //! Exit workers running
  void exitWorkersRunning() noexcept;

  //! Fetch a task from the top of the queue
  WorkerTask fetchTask() noexcept;

  //! Finish a task
  void finishTask(const int64b task_id) noexcept;

  //! Return the actual number of cores
  static std::size_t getActualNumOfThreads(const int64b s) noexcept;

  //! Return the thread ID of the thread in which this method is called
  int64b getCurrentThreadId() const noexcept;

  //! Return the function reference of the given function data
  template <typename ...Types, typename TaskData>
  static auto getTaskRef(TaskData&& data) noexcept;

  //! Initialize this thread manager
  void initialize(const int64b num_of_threads) noexcept;

  //! Issue a task ID
  int64b issueTaskId() noexcept;

  //! Return the func data
  template <typename ...Types, Invocable<Types...> Func>
  static auto makeTaskData(Func&& func) noexcept;

  //! Wait current thread for parent task complesion
  void waitForParent(const int64b task_id, const int64b parent_task_id) const noexcept;

  //! Check if the workers (threads) are enable running
  bool workersAreEnabled() const noexcept;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();


  alignas(kCacheLineSize) std::atomic<int64b> total_queued_task_ids_;
//  alignas(kCacheLineSize) std::atomic<int64b> num_of_completed_task_ids_;
//  alignas(kCacheLineSize) int64b current_min_task_id_;
  alignas(kCacheLineSize) std::atomic<int> num_of_active_workers_;
  alignas(kCacheLineSize) std::mutex manager_lock_;
  alignas(kCacheLineSize) WorkerLock worker_lock_;
  TaskQueue task_queue_;
  pmr::vector<std::thread> worker_list_;
  pmr::vector<std::thread::id> worker_id_list_;
};

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG

} // namespace zisc

/*!
  \example thread_manager_example.cpp
  This is an example of how to use ThreadManager class."
  */

#include "thread_manager-inl.hpp"

#endif // ZISC_THREAD_MANAGER_HPP
