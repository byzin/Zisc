/*!
  \file thread_manager.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_HPP
#define ZISC_THREAD_MANAGER_HPP

// Standard C++ library
#include <array>
#include <deque>
#include <future>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "type_traits.hpp"
#include "unique_memory_pointer.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief ThreadManager class provides task parallel and data parallel thread pool
  */
class ThreadManager : private NonCopyable<ThreadManager>
{
 public:
#if defined(Z_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif // Z_CLANG
  //! Result type of tasks
  template <typename T>
  class Result : private NonCopyable<Result<T>>
  {
   public:
    using Type = std::remove_volatile_t<T>;

    //! Create a result of a task
    Result() noexcept;
    //! Create a result of a task
    Result(ThreadManager* manager, const uint thread_id) noexcept;
    //! Destroy a result
    ~Result() noexcept;
    //! Return the result
    Type get() noexcept;
    //! Wait for the result to become available
    void wait() const noexcept;
   private:
    friend ThreadManager;
    using ResultT = std::conditional_t<std::is_void_v<Type>, uint8b, Type>;
    using ResultData = std::aligned_storage_t<sizeof(ResultT), alignof(ResultT)>;
    using ResultReference = std::add_lvalue_reference_t<ResultT>;

    //! Check if a result has a value
    bool hasValue() const noexcept;
    //! Set a result value
    void set(ResultT&& result) noexcept;
    //! Return a data as a value
    ResultReference value() noexcept;

    ThreadManager* thread_manager_;
    ResultData data_;
    uint8b has_value_;
    uint16b thread_id_;
  };
#if defined(Z_CLANG)
#pragma clang diagnostic pop
#endif // Z_CLANG
  template <typename Type>
  using UniqueResult = UniqueMemoryPointer<Result<Type>>;


  //! Create threads as many CPU threads as
  ThreadManager(std::pmr::memory_resource* mem_resource) noexcept;

  //! Create threads
  ThreadManager(const uint num_of_threads,
                std::pmr::memory_resource* mem_resource) noexcept;

  //! Terminate threads
  ~ThreadManager();


  //! Calculate a range of a thread
  template <typename Integer>
  static std::array<Integer, 2> calcThreadRange(const Integer range,
                                                const uint num_of_threads,
                                                const uint thread_id) noexcept;

  //! Calculate a range of a thread
  template <typename Integer>
  std::array<Integer, 2> calcThreadRange(const Integer range, 
                                         const uint thread_id) const noexcept;

  //! Run the given task on a worker thread in the manager
  template <typename ReturnType, typename Task>
  UniqueResult<ReturnType> enqueue(
      Task&& task,
      std::pmr::memory_resource* mem_resource,
      EnableIf<std::is_invocable_v<Task>> = kEnabler) noexcept;

  //! Run the given task on a worker thread in the manager
  template <typename ReturnType, typename Task>
  UniqueResult<ReturnType> enqueue(
      Task&& task,
      std::pmr::memory_resource* mem_resource,
      EnableIf<std::is_invocable_v<Task, uint>> = kEnabler) noexcept;

  //! Run tasks on the worker threads in the manager
  template <typename Task, typename Iterator1, typename Iterator2>
  UniqueResult<void> enqueueLoop(
      Task&& task,
      Iterator1&& begin,
      Iterator2&& end,
      std::pmr::memory_resource* mem_resource,
      EnableIf<std::is_invocable_v<Task, Iterator1>> = kEnabler) noexcept;

  //! Run tasks on the worker threads in the manager
  template <typename Task, typename Iterator1, typename Iterator2>
  UniqueResult<void> enqueueLoop(
      Task&& task,
      Iterator1&& begin,
      Iterator2&& end,
      std::pmr::memory_resource* mem_resource,
      EnableIf<std::is_invocable_v<Task, uint, Iterator1>> = kEnabler) noexcept;

  //! Return the number of logical cores
  static uint logicalCores() noexcept;

  //! Return the number of threads
  uint numOfThreads() const noexcept;

  //! Return a pointer to the underling memory resource
  std::pmr::memory_resource* resource() const noexcept;

 private:
  //! Base class of worker task
  class WorkerTask
  {
   public:
    virtual ~WorkerTask() noexcept {}
    //! Run a task
    virtual void run(const uint thread_id) noexcept = 0;
  };
  using UniqueTask = UniqueMemoryPointer<WorkerTask>;


  //! Create worker threads
  void createWorkers(const uint num_of_threads) noexcept;

  //! Return the distance of given two iterators
  template <typename Iterator>
  static uint distance(Iterator&& begin, Iterator&& end) noexcept;

  //! Run the given task on a worker thread in the manager
  template <typename ReturnType, typename Task>
  UniqueResult<ReturnType> enqueueTask(
      Task&& task,
      std::pmr::memory_resource* mem_resource) noexcept;

  //! Run tasks on the worker threads in the manager
  template <typename Task, typename Iterator1, typename Iterator2>
  UniqueResult<void> enqueueLoopTask(
      Task&& task,
      Iterator1&& begin,
      Iterator2&& end,
      std::pmr::memory_resource* mem_resource) noexcept;

  //! Exit workers running
  void exitWorkersRunning() noexcept;

  //! Fetch a task from the top of the queue
  UniqueTask fetchTask() noexcept;

  //! Return the thread index if the current thread is one of the threads in pool
  uint getThreadIndex() const noexcept;

  //! Initialize this thread manager
  void initialize(const uint num_of_threads) noexcept;

  //! Run single task
  template <typename Task, typename Iterator>
  static void runLoopTask(Task& task,
                          const uint thread_id,
                          Iterator ite) noexcept;

  //! Run single task
  template <typename ReturnType, typename Task>
  static void runSingleTask(Task& task,
                            const uint thread_id,
                            Result<ReturnType>* result) noexcept;

  //! Check if the workers (threads) are enable running
  bool workersAreEnabled() const noexcept;


  std::queue<UniqueTask, pmr::deque<UniqueTask>> task_queue_;
  pmr::vector<std::thread> workers_;
  std::mutex lock_;
  std::condition_variable condition_;
  uint8b workers_are_enabled_;
};

} // namespace zisc

/*!
  \example thread_manager_example.cpp
  This is an example of how to use ThreadManager class."
  */

#include "thread_manager-inl.hpp"

#endif // ZISC_THREAD_MANAGER_HPP
