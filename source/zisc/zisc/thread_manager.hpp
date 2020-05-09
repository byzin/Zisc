/*!
  \file thread_manager.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_HPP
#define ZISC_THREAD_MANAGER_HPP

// Standard C++ library
#include <array>
#include <atomic>
#include <future>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "atomic.hpp"
#include "bitset.hpp"
#include "error.hpp"
#include "scalable_circular_queue.hpp"
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "type_traits.hpp"
#include "zisc_config.hpp"

namespace zisc {

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_GCC || Z_CLANG

/*!
  \brief ThreadManager class provides task parallel and data parallel thread pool

  No detailed description.
  */
class ThreadManager : private NonCopyable<ThreadManager>
{
  // Forward declaration
  class WorkerTask;

 public:
  //! Result type of tasks
  template <typename T>
  class Result : private NonCopyable<Result<T>>
  {
   public:
    using Type = std::remove_volatile_t<T>;

    //! Create a result of a task
    Result(const int64b task_id, ThreadManager* manager) noexcept;

    //! Destroy a result
    ~Result() noexcept;

    //! Return the result
    Type get();

    //! Return the task ID of the result
    int64b taskId() const noexcept;

    //! Wait for the result to become available
    void wait() const;

   private:
    friend ThreadManager;
    using ResultT = std::conditional_t<std::is_void_v<Type>, uint8b, Type>;
    using ResultData = std::aligned_storage_t<sizeof(ResultT), alignof(ResultT)>;
    using ResultReference = std::add_lvalue_reference_t<ResultT>;

    //! Check if a result has a value
    bool hasValue() const noexcept;

    //! Return the underlying thread manager
    ThreadManager* manager() const noexcept;

    //! Run another task
    bool runAnotherTask() const;

    //! Set a result value
    void set(ResultT&& result) noexcept;

    //! Return a data as a value
    ResultReference value() noexcept;


    int64b task_id_;
    ThreadManager* manager_;
    ResultData data_;
    uint8b has_value_;
  };

  using DiffType = WordType;
  class OverflowError;

  /*!
    \brief No brief description

    No detailed description.
    */
  class Task : private NonCopyable<Task>
  {
   public:
    //! Create a task data
    Task(const int64b task_id, const int64b parent_task_id) noexcept;

    //! Destroy a task data
    virtual ~Task() noexcept;

    //! Perform post-process of the task data
    virtual bool done() noexcept = 0;

    //! Return the parent task ID
    int64b parentTaskId() const noexcept;

    //! Return the result of the underlying task
    virtual void* result() noexcept = 0;

    //! Return the result of the underlying task
    virtual const void* result() const noexcept = 0;

    //! Run a task
    virtual void run(const int64b thread_id, const DiffType it_offset) = 0;

    //! Return the task ID
    int64b taskId() const noexcept;

   protected:
    friend OverflowError;
    friend WorkerTask;


    //! Destroy the task and the underlying result
    virtual void deleteInException(pmr::memory_resource* mem_resource) noexcept = 0;

    //! Return the underlying thread manager
    virtual ThreadManager* manager() noexcept = 0;

   private:
    int64b task_id_;
    int64b parent_task_id_;
  };

  using TaskPointer = std::add_pointer_t<Task>;

  /*!
    \brief No brief description

    No detailed description.
    */
  class OverflowError : public SystemError
  {
   public:
    //! Construct the queue error of the thread manager 
    OverflowError(const std::string_view what_arg,
                  pmr::memory_resource* mem_resource,
                  TaskPointer t,
                  const DiffType begin_offset,
                  const DiffType num_of_iterations);

    //! Finalize the queue error
    ~OverflowError() noexcept override;


    //! Return the offset of iterator
    DiffType beginOffset() const noexcept;

    //! Return the number of iterations
    DiffType numOfIterations() const noexcept;

    //! Return the result of the underlying task
    template <typename ReturnType>
    Result<ReturnType>& result() noexcept;

    //! Return the result of the underlying task
    template <typename ReturnType>
    const Result<ReturnType>& result() const noexcept;

    //! Return the underlying task
    Task& task() noexcept;

    //! Return the underlying task
    const Task& task() const noexcept;

   private:
    struct TaskData : private NonCopyable<TaskData>
    {
      TaskData(pmr::memory_resource* mem_resource, TaskPointer t) noexcept;
      ~TaskData() noexcept;
      pmr::memory_resource* mem_resource_;
      TaskPointer task_;
    };


    std::shared_ptr<TaskData> task_data_;
    DiffType begin_offset_;
    DiffType num_of_iterations_;
  };

  template <typename Type>
  using UniqueResult = pmr::unique_ptr<Result<Type>>;
  template <typename Iterator1, typename Iterator2>
  using CommonIterator = std::common_type_t<std::remove_reference_t<Iterator1>,
                                            std::remove_reference_t<Iterator2>>;


  static constexpr int64b kNoParentId = -1;
  static constexpr int64b kAllParentId = std::numeric_limits<int64b>::min();


  //! Create threads as many CPU threads as
  ThreadManager(pmr::memory_resource* mem_resource) noexcept;

  //! Create threads
  ThreadManager(const int64b num_of_threads,
                pmr::memory_resource* mem_resource) noexcept;

  //! Terminate threads
  ~ThreadManager();


  //! Calculate a range of a thread
  template <typename Integer>
  static std::array<Integer, 2> calcThreadRange(const Integer range,
                                                const int64b num_of_threads,
                                                const int64b thread_id) noexcept;

  //! Calculate a range of a thread
  template <typename Integer>
  std::array<Integer, 2> calcThreadRange(const Integer range, 
                                         const int64b thread_id) const noexcept;

  //! Return the maximum task ID
  std::size_t idCapacity() const noexcept;

  //! Return the maximum possible number of task items
  std::size_t itemCapacity() const noexcept;

  //! Clear the thread manager state
  void clear() noexcept;

  //! Return the default value of task ID max
  static constexpr std::size_t defaultIdCapacity() noexcept;

  //! Return the default capacity of the task item queue
  static constexpr std::size_t defaultItemCapacity() noexcept;

  //! Run the given task on a worker thread in the manager
  template <typename ReturnType, typename Func>
  UniqueResult<ReturnType> enqueue(
      Func&& task,
      const int64b parent_task_id = kNoParentId,
      EnableIfInvocable<Func> = kEnabler);

  //! Run the given task on a worker thread in the manager
  template <typename ReturnType, typename Func>
  UniqueResult<ReturnType> enqueue(
      Func&& task,
      const int64b parent_task_id = kNoParentId,
      EnableIfInvocable<Func, int64b> = kEnabler);

  //! Run tasks on the worker threads in the manager
  template <typename Func, typename Iterator1, typename Iterator2>
  UniqueResult<void> enqueueLoop(
      Func&& task,
      Iterator1&& begin,
      Iterator2&& end,
      const int64b parent_task_id = kNoParentId,
      EnableIfInvocable<Func, CommonIterator<Iterator1, Iterator2>> = kEnabler);

  //! Run tasks on the worker threads in the manager
  template <typename Func, typename Iterator1, typename Iterator2>
  UniqueResult<void> enqueueLoop(
      Func&& task,
      Iterator1&& begin,
      Iterator2&& end,
      const int64b parent_task_id = kNoParentId,
      EnableIfInvocable<Func, int64b, CommonIterator<Iterator1, Iterator2>> = kEnabler);

  //! Check whether the task queue is empty
  bool isEmpty() const noexcept;

  //! Return the number of logical cores
  static int64b logicalCores() noexcept;

  //! Return the number of threads
  int64b numOfThreads() const noexcept;

  //! Return a pointer to the underlying memory resource
  pmr::memory_resource* resource() const noexcept;

  //! Change the maximum task ID. The queued tasks are cleared
  void setIdCapacity(const std::size_t cap) noexcept;

  //! Change the maximum possible number of task items. The queued tasks are cleared
  void setItemCapacity(const std::size_t cap) noexcept;

  //! Return the number of queued tasks
  std::size_t size() const noexcept;

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
    WorkerTask() noexcept;

    //! Create a task
    WorkerTask(TaskPointer task, const DiffType it_offset) noexcept;

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
    void done() noexcept;


    TaskPointer task_;
    DiffType it_offset_;
  };

  using Queue = ScalableCircularQueue<WorkerTask>;
  using Lock = AtomicWord<Config::isAtomicOsSpecifiedWaitUsed()>;


  //! Create worker threads
  void createWorkers(const int64b num_of_threads) noexcept;

  //! Return the distance of given two iterators
  template <typename Iterator1, typename Iterator2>
  static DiffType distance(Iterator1&& begin, Iterator2&& end) noexcept;

  //! Run tasks on the worker threads in the manager
  template <bool kIsLoopTask, typename ReturnType, typename Func, typename Iterator1, typename Iterator2>
  UniqueResult<ReturnType> enqueueImpl(Func&& task,
                                       Iterator1&& begin,
                                       Iterator2&& end,
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

  //! Initialize this thread manager
  void initialize(const int64b num_of_threads) noexcept;

  //! Issue a task ID
  int64b issueTaskId() noexcept;

  //! Return the unmanaged thread ID
  static constexpr int64b unmanagedThreadId() noexcept;

  //! Wait current thread for parent task complesion
  void waitForParent(const int64b task_id, const int64b parent_task_id) const noexcept;

  //! Check if the workers (threads) are enable running
  bool workersAreEnabled() const noexcept;


  Queue task_queue_;
  pmr::vector<std::thread> workers_;
  Bitset worker_state_set_; // 0: worker is waiting, 1: worker is running
  Bitset task_state_set_; // 0: task isn't completed, 1: task is completed
  std::atomic<int64b> total_tasks_;
  Lock lock_;
};

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG

} // namespace zisc

/*!
  \example thread_manager_example.cpp
  This is an example of how to use ThreadManager class."
  */

#include "thread_manager-inl.hpp"

#endif // ZISC_THREAD_MANAGER_HPP
