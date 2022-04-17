/*!
  \file lock_free_bounded_queue_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/memory/simple_memory_resource.hpp"
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
  zisc::SimpleMemoryResource mem_resource;
  Queue q{&mem_resource};

  {
    std::cout << "Enqueing values [3, 1, 4, 5, 0, 2]." << std::endl;
    std::vector<int> values{{3, 1, 4, 5, 0, 2}};
    q.setCapacity(values.size());
    for (const int value : values) {
      [[maybe_unused]] const auto result = q.enqueue(value);
    }
    std::cout << "  queue size: " << q.size() << ", elements [";
    for (std::size_t i = 0; i < values.size(); ++i) {
      const auto result = q.dequeue();
      if (result.has_value())
        std::cout << *result;
      if (i == (values.size() - 1))
        break;
      std::cout << ", ";
    }
    std::cout << "]" << std::endl;
  }

  {
    static constexpr std::size_t num_of_threads = 16;
    static constexpr std::size_t works_per_thread = 262144;
    static constexpr std::size_t num_of_works = num_of_threads * works_per_thread;

    std::cout << "Multiple producer: " << num_of_threads << " threads, "
              << num_of_works << " int elements." << std::endl;
    q.setCapacity(num_of_works);

    std::vector<std::thread> workers;
    workers.reserve(num_of_threads);

    std::atomic_size_t counter = 0;
    auto enqueue_job = [&q, &counter]()
    {
      const std::size_t id = counter++;
      for (std::size_t i = 0; i < works_per_thread; ++i) {
        const int value = static_cast<int>(id * works_per_thread + i);
        [[maybe_unused]] const auto result = q.enqueue(value);
      }
    };
    for (std::size_t i = 0; i < num_of_threads; ++i)
      workers.emplace_back(enqueue_job);
    for (std::size_t i = 0; i < num_of_threads; ++i)
      workers[i].join();
    workers.clear();

    std::cout << "Multiple consumer: " << num_of_threads << " threads, "
              << num_of_works << " int elements." << std::endl;
    std::vector<int> results;
    results.resize(num_of_works, 0);
    auto dequeue_job = [&q, &results]() noexcept
    {
      for (std::size_t i = 0; i < works_per_thread; ++i) {
        const auto result = q.dequeue();
        if (result.has_value())
          results[*result] = 1;
      }
    };
    for (std::size_t i = 0; i < num_of_threads; ++i)
      workers.emplace_back(dequeue_job);
    for (std::size_t i = 0; i < num_of_threads; ++i)
      workers[i].join();

    // Validate results
    for (std::size_t i = 0; i < num_of_works; ++i) {
      if (results[i] == 0)
        std::cout << "Multipe producer multiple consumer test failed." << std::endl;
    }
  }

  return 0;
}
