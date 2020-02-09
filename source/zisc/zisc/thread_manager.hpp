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
#include <future>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "atomic.hpp"
#include "error.hpp"
#include "lock_free_bounded_queue.hpp"
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "type_traits.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief ThreadManager class provides task parallel and data parallel thread pool

  No detailed description.
  */
class ThreadManager : private NonCopyable<ThreadManager>
{
 public:
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_GCC || Z_CLANG
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
    Type get();
    //! Wait for the result to become available
    void wait() const;

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
#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG
  template <typename Type>
  using UniqueResult = pmr::unique_ptr<Result<Type>>;

  /*!
    \brief No brief description

    No detailed description.
    */
  class OverflowError : public SystemError
  {
   public:
    //! Construct the queue error of the thread manager 
    OverflowError(const std::string_view what_arg);

    //! Finalize the queue error
    ~OverflowError() noexcept override;


    //! Return an iterator to the beginning
    virtual void* begin() noexcept = 0;

    //! Return an iterator to the beginning
    virtual const void* begin() const noexcept = 0;

    //! Return an iterator to the end
    virtual void* end() noexcept = 0;

    //! Return an iterator to the end
    virtual const void* end() const noexcept = 0;


    //! Check if the exception has iterators
    virtual bool hasIterator() const noexcept = 0;

    //! Return the result of the tasks
    virtual void* result() noexcept = 0;

    //! Return the result of the tasks
    virtual const void* result() const noexcept = 0;

    //! Return the task 
    virtual void* task() noexcept = 0;

    //! Return the task
    virtual const void* task() const noexcept = 0;
  };


  //! Create threads as many CPU threads as
  ThreadManager(pmr::memory_resource* mem_resource) noexcept;

  //! Create threads
  ThreadManager(const uint num_of_threads,
                pmr::memory_resource* mem_resource) noexcept;

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

  //! Return the maximum possible number of tasks
  std::size_t capacity() const noexcept;

  //! Return the default capacity of the task queue
  static constexpr std::size_t defaultTaskCapacity() noexcept;

  //! Run the given task on a worker thread in the manager
  template <typename ReturnType, typename Task>
  UniqueResult<ReturnType> enqueue(
      Task&& task,
      EnableIfInvocable<Task> = kEnabler);

  //! Run the given task on a worker thread in the manager
  template <typename ReturnType, typename Task>
  UniqueResult<ReturnType> enqueue(
      Task&& task,
      EnableIfInvocable<Task, uint> = kEnabler);

  //! Run tasks on the worker threads in the manager
  template <typename Task, typename Iterator1, typename Iterator2>
  UniqueResult<void> enqueueLoop(
      Task&& task,
      Iterator1&& begin,
      Iterator2&& end,
      EnableIfInvocable<Task, Iterator1> = kEnabler);

  //! Run tasks on the worker threads in the manager
  template <typename Task, typename Iterator1, typename Iterator2>
  UniqueResult<void> enqueueLoop(
      Task&& task,
      Iterator1&& begin,
      Iterator2&& end,
      EnableIfInvocable<Task, uint, Iterator1> = kEnabler);

  //! Check whether the task queue is empty
  bool isEmpty() const noexcept;

  //! Return the number of logical cores
  static uint logicalCores() noexcept;

  //! Return the number of threads
  uint numOfThreads() const noexcept;

  //! Return a pointer to the underlying memory resource
  pmr::memory_resource* resource() const noexcept;

  //! Change the maximum possible number of tasks. The queued tasks are cleared
  void setCapacity(const std::size_t cap) noexcept;

  //! Return the number of queued tasks
  int size() const noexcept;

 private:
  using Lock = AtomicWord<Config::isAtomicOsSpecifiedWaitUsed()>;
  template <typename Type>
  using ResultPointer = std::add_pointer_t<Result<Type>>;
  //! Base class of worker task
  class WorkerTask
  {
   public:
    virtual ~WorkerTask() noexcept {}
    //! Run a task
    virtual void run(const uint thread_id) = 0;
  };
  using UniqueTask = pmr::unique_ptr<WorkerTask>;


  //! Create worker threads
  void createWorkers(const uint num_of_threads) noexcept;

  //! Return the distance of given two iterators
  template <typename Iterator1, typename Iterator2>
  static uint distance(Iterator1&& begin, Iterator2&& end) noexcept;

  //! Run the given task on a worker thread in the manager
  template <typename ReturnType, typename Task>
  UniqueResult<ReturnType> enqueueImpl(Task&& task);

  //! Run tasks on the worker threads in the manager
  template <typename Task, typename Iterator1, typename Iterator2>
  UniqueResult<void> enqueueLoopImpl(Task&& task,
                                     Iterator1&& begin,
                                     Iterator2&& end);

  //! Exit workers running
  void exitWorkersRunning() noexcept;

  //! Fetch a task from the top of the queue
  UniqueTask fetchTask() noexcept;

  //! Return the thread index if the current thread is one of the threads in pool
  uint getThreadIndex() const noexcept;

  //! Initialize this thread manager
  void initialize(const uint num_of_threads) noexcept;

  //! Return the invalid thread ID
  static constexpr uint invalidId() noexcept;

  //! Return the allocator of the result type
  template <typename Type>
  pmr::polymorphic_allocator<Result<Type>> resultAllocator() const noexcept;

  //! Run single task
  template <typename Task, typename Iterator>
  static void runLoopTask(Task& task,
                          const uint thread_id,
                          Iterator ite);

  //! Run single task
  template <typename ReturnType, typename Task>
  static void runSingleTask(Task& task,
                            const uint thread_id,
                            ResultPointer<ReturnType> result);

  //! Check if the workers (threads) are enable running
  bool workersAreEnabled() const noexcept;


  LockFreeBoundedQueue<UniqueTask> task_queue_;
  pmr::vector<std::thread> workers_;
  Lock lock_;
};

} // namespace zisc

/*!
  \example thread_manager_example.cpp
  This is an example of how to use ThreadManager class."
  */

#include "thread_manager-inl.hpp"

#endif // ZISC_THREAD_MANAGER_HPP
