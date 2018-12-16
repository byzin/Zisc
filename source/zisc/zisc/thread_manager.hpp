/*!
  \file thread_manager.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_THREAD_MANAGER_HPP
#define ZISC_THREAD_MANAGER_HPP

// Standard C++ library
#include <array>
#include <deque>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "memory_resource.hpp"
#include "non_copyable.hpp"
#include "simple_memory_resource.hpp"
#include "unique_memory_pointer.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief ThreadManager class provides task parallel and data parallel thread pool
  */
class ThreadManager : private NonCopyable<ThreadManager>
{
 public:
  //! Result type of tasks
  template <typename T>
  class Result : private NonCopyable<Result<T>>
  {
   public:
    using Type = std::remove_reference_t<T>;
    Result() noexcept;
    Result(ThreadManager* manager, const uint thread_id) noexcept;
    ~Result() noexcept;
    //! Return the result
    Type get() noexcept;
    //! Wait for the result to become available
    void wait() const noexcept;
   private:
    friend ThreadManager;
    using ResultType = std::conditional_t<std::is_void_v<Type>, uint8b, Type>;
    using ResultReference = std::add_lvalue_reference_t<ResultType>;
    //! Check if a result has a value
    bool hasValue() const noexcept;
    //! Set a result value
    void set(ResultType&& result) noexcept;
    //! Return a data as a value
    ResultReference value() noexcept;
    ThreadManager* thread_manager_;
    std::aligned_storage_t<sizeof(ResultType), alignof(ResultType)> data_;
    uint8b has_value_;
    uint16b thread_id_;
  };
  template <typename Type>
  using UniqueResult = UniqueMemoryPointer<Result<Type>>;


  //! Create threads as many CPU threads as
  ThreadManager(
      pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Create threads
  ThreadManager(
      const uint num_of_threads,
      pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

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

  //! A worker thread run a task
  template <typename ReturnType, typename Task>
  UniqueResult<ReturnType> enqueue(
      Task&& task,
      pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Worker threads run a loop task in parallel
  template <typename Task, typename Iterator>
  UniqueResult<void> enqueueLoop(
      Task&& task,
      Iterator&& begin,
      Iterator&& end,
      pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Get the number of logical cores
  static uint logicalCores() noexcept;

  //! Return the number of threads
  uint numOfThreads() const noexcept;

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

  //! Exit workers running
  void exitWorkersRunning() noexcept;

  //! Fetch a task from the top of the queue
  UniqueTask fetchTask() noexcept;

  //! Return the thread index if the current thread is one of the threads in pool
  uint getThreadIndex() const noexcept;

  //! Initialize this thread manager
  void initialize(const uint num_of_threads) noexcept;

  //! Run a task
  template <typename ReturnType, typename Task>
  static void runTask(Task& task,
                      Result<ReturnType>* result,
                      const uint thread_id) noexcept;

  //! Check if the workers (threads) are enable running
  bool workersAreEnabled() const noexcept;


  std::mutex lock_;
  std::condition_variable condition_;
  std::queue<UniqueTask, pmr::deque<UniqueTask>> task_queue_;
  pmr::vector<std::thread> workers_;
  uint8b workers_are_enabled_;
  std::array<uint8b, 7> padding_;
};

} // namespace zisc

#include "thread_manager-inl.hpp"

#endif // ZISC_THREAD_MANAGER_HPP
