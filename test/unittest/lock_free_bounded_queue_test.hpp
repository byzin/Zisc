/*!
  \file lock_free_queue_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_BOUNDED_QUEUE_TEST_HPP
#define ZISC_LOCK_FREE_BOUNDED_QUEUE_TEST_HPP

// Standard C++ library
#include <bitset>
#include <chrono>
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>
// Zisc
#include "zisc/data_structure/queue.hpp"
#include "zisc/stopwatch.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

template <typename Queue, std::size_t kNumOfThreads, std::size_t kNumOfThreadTasks>
class LockFreeBoundedQueueTest
{
  using uint32b = zisc::uint32b;
  using uint64b = zisc::uint64b;

 public:
  template <typename QueueClass, typename T>
  static bool testEnqueue(zisc::Queue<QueueClass, T>& q,
                          uint64b& elapsed_time) noexcept
  {
    std::cout << "    Capacity   : " << q.capacity() << std::endl;
    std::cout << "    CapacityMax: " << q.capacityMax() << std::endl;
    std::cout << "    Size       : " << q.size() << std::endl;
    std::cout << "    Empty      : " << q.isEmpty() << std::endl;

    bool result = true;
    std::mutex m;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    threads.reserve(kNumOfThreads);
    std::bitset<kNumOfThreads> ready;
    ready.reset();
    for (std::size_t id = 0; id < kNumOfThreads; ++id) {
      auto work = [id, &m, &cv, &ready, &result, &q]()
      {
        {
          std::unique_lock<std::mutex> locker{m};
          ready.set(id);
          cv.wait(locker);
        }
        const std::size_t offset = id * kNumOfThreadTasks;
        for (std::size_t i = 0; i < kNumOfThreadTasks; ++i) {
          const uint64b index = zisc::cast<uint64b>(offset + i);
          const auto r = q.enqueue(index);
          if (!r.isSuccess())
            result = false;
        }
      };
      threads.emplace_back(work);
    }
    while (!ready.all())
      std::this_thread::yield();
    {
      std::unique_lock<std::mutex> locker{m};
    }

    zisc::Stopwatch stopwatch;
    stopwatch.start();
    cv.notify_all();
    for (auto& t : threads)
      t.join();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch.elapsedTime());
    elapsed_time = millis.count();
    stopwatch.stop();

    return result;
  }

  template <typename QueueClass, typename T>
  static bool testEnqueueDequeue(zisc::Queue<QueueClass, T>& q,
                                 std::vector<uint32b>& data,
                                 uint64b& elapsed_time) noexcept
  {
    bool result = true;
    std::mutex m;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    threads.reserve(kNumOfThreads);
    std::bitset<kNumOfThreads> ready;
    ready.reset();
    for (std::size_t id = 0; id < (kNumOfThreads / 2); ++id) {
      auto work1 = [id, &m, &cv, &ready, &result, &q]()
      {
        {
          std::unique_lock<std::mutex> locker{m};
          ready.set(2 * id);
          cv.wait(locker);
        }
        constexpr std::size_t num_of_task_threads = 2 * kNumOfThreadTasks;
        const std::size_t offset = id * num_of_task_threads;
        for (std::size_t i = 0; i < num_of_task_threads; ++i) {
          const uint64b index = zisc::cast<uint64b>(offset + i);
          const auto r = q.enqueue(index);
          if (!r.isSuccess())
            result = false;
        }
      };
      auto work2 = [id, &m, &cv, &ready, &result, &q, &data]()
      {
        {
          std::unique_lock<std::mutex> locker{m};
          ready.set(2 * id + 1);
          cv.wait(locker);
        }
        constexpr std::size_t num_of_task_threads = 2 * kNumOfThreadTasks;
        for (std::size_t i = 0; i < num_of_task_threads; ++i) {
          const auto r = q.dequeue();
          if (r.isSuccess()) {
            const std::size_t index = r;
            data[index] = 1;
          }
          else {
            result = false;
          }
        }
      };
      threads.emplace_back(work1);
      threads.emplace_back(work2);
    }
    while (!ready.all())
      std::this_thread::yield();
    {
      std::unique_lock<std::mutex> locker{m};
    }

    zisc::Stopwatch stopwatch;
    stopwatch.start();
    cv.notify_all();
    for (auto& t : threads)
      t.join();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch.elapsedTime());
    elapsed_time = millis.count();
    stopwatch.stop();

    return result;
  }

  template <typename QueueClass, typename T>
  static bool testDequeue(zisc::Queue<QueueClass, T>& q,
                          std::vector<uint32b>& data,
                          uint64b& elapsed_time) noexcept
  {
    std::cout << "    Capacity   : " << q.capacity() << std::endl;
    std::cout << "    CapacityMax: " << q.capacityMax() << std::endl;
    std::cout << "    Size       : " << q.size() << std::endl;
    std::cout << "    Empty      : " << q.isEmpty() << std::endl;

    bool result = true;
    std::mutex m;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    threads.reserve(kNumOfThreads);
    std::bitset<kNumOfThreads> ready;
    ready.reset();
    for (std::size_t id = 0; id < kNumOfThreads; ++id) {
      auto work = [id, &m, &cv, &ready, &result, &q, &data]()
      {
        {
          std::unique_lock<std::mutex> locker{m};
          ready.set(id);
          cv.wait(locker);
        }
        for (std::size_t i = 0; i < kNumOfThreadTasks; ++i) {
          const auto r = q.dequeue();
          if (r.isSuccess()) {
            const std::size_t index = r;
            data[index] = 1;
          }
          else {
            result = false;
          }
        }
      };
      threads.emplace_back(work);
    }
    while (!ready.all())
      std::this_thread::yield();
    {
      std::unique_lock<std::mutex> locker{m};
    }

    zisc::Stopwatch stopwatch;
    stopwatch.start();
    cv.notify_all();
    for (auto& t : threads)
      t.join();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch.elapsedTime());
    elapsed_time = millis.count();
    stopwatch.stop();

    return result;
  }
};

#endif // ZISC_LOCK_FREE_BOUNDED_QUEUE_TEST_HPP
