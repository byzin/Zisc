/*!
  \file concurrent_bounded_queue_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>
// Zisc
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/alloc_free_resource.hpp"
#include "zisc/structure/queue.hpp"
#include "zisc/structure/scalable_circular_queue.hpp"

template <typename Type>
void showAtomicProperties(const std::string_view type_name) noexcept
{
  using Atomic = std::atomic<Type>;
  Atomic v;
  std::cout << "  std::atomic<" << type_name << ">: "
            << "size: " << sizeof(v) << ", "
            << "alignment: " << std::alignment_of_v<Atomic> << ", "
            << "lock free: " << v.is_always_lock_free << std::endl;
}

int main()
{
  std::cout << "Atomic properties:" << std::endl;
  showAtomicProperties<zisc::int32b>("int32_t");
  showAtomicProperties<zisc::uint32b>("uint32_t");
  showAtomicProperties<zisc::int64b>("int64_t");
  showAtomicProperties<zisc::uint64b>("uint64_t");

  using Queue = zisc::ScalableCircularQueue<int>;
  zisc::AllocFreeResource mem_resource;
  Queue q{&mem_resource};

  static_assert(Queue::isBounded(), "The queue isn't bounded queue.");
  static_assert(Queue::isConcurrent(), "The queue doesn't support concurrency.");

  // Bounded queue
  {
    std::cout << "Enqueing values [3, 1, 8, 4, 5, 0, 7, 2, 6]" << std::endl;
    std::vector<int> values{{3, 1, 8, 4, 5, 0, 7, 2, 6}};
    q.setCapacity(values.size() - 1);
    std::cout << "  queue capacity: " << q.capacity() << std::endl;
    try {
      std::for_each(values.begin(), values.end(), [&q](const int& in)
      {
        [[maybe_unused]] const std::optional<std::size_t> result = q.enqueue(in);
        ZISC_ASSERT(result.has_value(), "Enqueing failed. value=", in);
      });
    }
    catch (const Queue::OverflowError& error) {
      std::cout << "  queue overflow happened. value=" << error.get() << std::endl;
    }
    std::cout << "  queue size: " << q.size() << std::endl;

    std::cout << "Dequeing values [";
    for (std::size_t i = 0; i < values.size(); ++i) {
      const std::optional<int> result = q.dequeue();
      if (result.has_value())
        std::cout << *result;
      if (!q.isEmpty())
        std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "  queue size: " << q.size() << std::endl;
  }

  {
    static constexpr std::size_t num_of_threads = 16;
    static constexpr std::size_t works_per_thread = 262144;
    static constexpr std::size_t num_of_works = num_of_threads * works_per_thread;

    // Threads
    std::vector<std::thread> workers;
    workers.reserve(num_of_threads);

    std::cout << "Multiple producer: " << num_of_threads << " threads, "
              << num_of_works << " int elements." << std::endl;
    q.setCapacity(num_of_works);
    std::atomic_int worker_lock{-1};
    std::atomic_size_t counter{0};
    auto enqueue_job = [&q, &worker_lock, &counter]()
    {
      // Wiat until all threads become ready
      worker_lock.wait(-1, std::memory_order::acquire);
      // Do actual jobs
      const std::size_t id = counter++;
      for (std::size_t i = 0; i < works_per_thread; ++i) {
        const int value = static_cast<int>(id * works_per_thread + i);
        [[maybe_unused]] const std::optional<std::size_t> result = q.enqueue(value);
      }
    };
    for (std::size_t i = 0; i < num_of_threads; ++i)
      workers.emplace_back(enqueue_job);
    worker_lock.store(zisc::cast<int>(workers.size()), std::memory_order::release);
    worker_lock.notify_all();
    std::for_each(workers.begin(), workers.end(), [](std::thread& w){w.join();});
    workers.clear();

    std::cout << "Multiple consumer: " << num_of_threads << " threads, "
              << num_of_works << " int elements." << std::endl;
    std::vector<int> results;
    results.resize(num_of_works, 0);
    worker_lock.store(-1, std::memory_order::release);
    auto dequeue_job = [&q, &results, &worker_lock]() noexcept
    {
      // Wiat until all threads become ready
      worker_lock.wait(-1, std::memory_order::acquire);
      // Do actual jobs
      for (std::size_t i = 0; i < works_per_thread; ++i) {
        const std::optional<int> result = q.dequeue();
        if (result.has_value())
          results[*result] = 1;
      }
    };
    for (std::size_t i = 0; i < num_of_threads; ++i)
      workers.emplace_back(dequeue_job);
    worker_lock.store(zisc::cast<int>(workers.size()), std::memory_order::release);
    worker_lock.notify_all();
    std::for_each(workers.begin(), workers.end(), [](std::thread& w){w.join();});
    workers.clear();

    // Validate results
    std::for_each(results.begin(), results.end(), [](const int& result)
    {
      if (result == 0)
        std::cerr << "  multipe producer multiple consumer test failed." << std::endl;
    });
  }

  return 0;
}
