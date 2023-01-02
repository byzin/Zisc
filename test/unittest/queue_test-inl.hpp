/*!
  \file queue_test-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_QUEUE_TEST_INL_HPP
#define TEST_QUEUE_TEST_INL_HPP

#include "queue_test.hpp"
// Standard C++ library
#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <memory>
#include <random>
#include <optional>
#include <system_error>
#include <type_traits>
#include <utility>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/structure/queue.hpp"

namespace test {

/*!
  \details No detailed description

  \tparam QueueClass No description.
  \param [in,out] queue No description.
  */
template <typename QueueClass> inline
void testSimpleBoundedQueue(zisc::Queue<QueueClass, int>* queue)
{
  using Queue = std::remove_cvref_t<decltype(*queue)>;
  static_assert(Queue::isBounded(), "The queue isn't bounded queue.");

  ASSERT_EQ(1, queue->capacity()) << "The initial queue capacity isn't 1.";
  queue->clear();
  ASSERT_TRUE(queue->isEmpty()) << "The queue isn't empty.";

  // Create input values
  constexpr std::size_t n = 64;
  std::mt19937_64 sampler{123'456'789};
  std::array<int, n> vlist;
  std::iota(vlist.begin(), vlist.end(), 0);
  std::shuffle(vlist.begin(), vlist.end(), sampler);

  // Set capacity test
  queue->setCapacity(vlist.size());
  ASSERT_EQ(vlist.size(), queue->capacity()) << "Initializing queue capacity failed.";
  ASSERT_TRUE(queue->isEmpty()) << "The queue isn't empty.";

  // Enqueue test
  const char* message = "Enqueuing failed.";
  auto test_enqueue = [queue, message](const int value)
  {
    const std::optional<std::size_t> result = queue->enqueue(value);
    ASSERT_TRUE(result.has_value()) << message;
    const std::size_t storage_id = *result;
    ASSERT_EQ(value, queue->get(storage_id)) << message;
  };
  std::for_each(vlist.begin(), vlist.end(), test_enqueue);
  ASSERT_EQ(vlist.size(), queue->size()) << message;
  ASSERT_FALSE(queue->isEmpty()) << "The queue is still empty.";

  // Enqueue overflow test
  using OverflowError = typename Queue::OverflowError;
  auto enqueue_overflow = [queue](const int value)
  {
    try {
      [[maybe_unused]] const std::optional<std::size_t> result = queue->enqueue(value);
    }
    catch (const OverflowError& error) {
      ASSERT_EQ(value, error.get()) << "The enqueue exception failed.";
      std::cout << error.what() << " value: " << error.get() << std::endl;
      throw;
    }
  };
  constexpr int overflow_value = zisc::cast<int>(vlist.size());
  ASSERT_THROW(enqueue_overflow(overflow_value), OverflowError) << message;

  // Dequeue test
  message = "Dequeuing failed.";
  auto test_dequeue = [queue, message](const int expected)
  {
    const std::optional<int> result = queue->dequeue();
    ASSERT_TRUE(result.has_value()) << message;
    ASSERT_EQ(expected, *result) << message << " Expected: " << expected;
  };
  std::for_each(vlist.begin(), vlist.end(), test_dequeue);
  ASSERT_TRUE(queue->isEmpty()) << "The queue isn't empty.";

  // Empty dequeue test
  {
    const std::optional<int> result = queue->dequeue();
    ASSERT_FALSE(result.has_value()) << "Empty dequeue failed.";
  }

  // Clear queue test
  {
    message = "Cleaning queue data failed.";
    ASSERT_TRUE(queue->isEmpty()) << message;
    std::for_each(vlist.begin(), vlist.end(), test_enqueue);
    ASSERT_FALSE(queue->isEmpty()) << message;
    queue->clear();
    ASSERT_TRUE(queue->isEmpty()) << message;
  }
}

class MovableQValue : public zisc::NonCopyable<MovableQValue>
{
 public:
  MovableQValue() = delete;

  MovableQValue(const int v) : value_{v} {}

  MovableQValue(MovableQValue&& other) noexcept : value_{other.value_}
  {
    other.value_ = -1;
  }

  MovableQValue& operator=(MovableQValue&& other) noexcept
  {
    std::swap(value_, other.value_);
    return *this;
  }

  explicit operator int() const
  {
    return value_;
  }

 private:
  int value_ = -1;
};

/*!
  \details No detailed description

  \tparam QueueClass No description.
  \param [in,out] queue No description.
  */
template <typename QueueClass> inline
void testMovableValueQueue(zisc::Queue<QueueClass, MovableQValue>* queue)
{
  using Queue = std::remove_cvref_t<decltype(*queue)>;
  using Movable = MovableQValue;
  static_assert(Queue::isBounded(), "The queue isn't bounded queue.");

  ASSERT_EQ(1, queue->capacity()) << "The initial queue capacity isn't 1.";
  queue->clear();
  ASSERT_TRUE(queue->isEmpty()) << "The queue isn't empty.";

  // Create input values
  constexpr std::size_t n = 64;
  std::mt19937_64 sampler{123'456'789};
  std::array<int, n> vlist;
  std::iota(vlist.begin(), vlist.end(), 0);
  std::shuffle(vlist.begin(), vlist.end(), sampler);

  // Set capacity test
  queue->setCapacity(vlist.size());
  ASSERT_EQ(vlist.size(), queue->capacity()) << "Initializing queue capacity failed.";
  ASSERT_TRUE(queue->isEmpty()) << "The queue isn't empty.";

  // Enqueue test
  const char* message = "Enqueuing failed.";
  auto test_enqueue = [queue, message](const int value)
  {
    const std::optional<std::size_t> result = queue->enqueue(Movable{value});
    ASSERT_TRUE(result.has_value()) << message;
    const std::size_t storage_id = *result;
    ASSERT_EQ(value, static_cast<int>(queue->get(storage_id))) << message;
  };
  std::for_each(vlist.begin(), vlist.end(), test_enqueue);
  ASSERT_EQ(vlist.size(), queue->size()) << message;
  ASSERT_FALSE(queue->isEmpty()) << "The queue is still empty.";

  // Enqueue overflow test
  using OverflowError = typename Queue::OverflowError;
  auto enqueue_overflow = [queue](const int value)
  {
    try {
      [[maybe_unused]] const std::optional<std::size_t> result = queue->enqueue(Movable{value});
    }
    catch (const OverflowError& error) {
      const int v = static_cast<int>(error.get());
      ASSERT_EQ(value, v) << "The enqueue exception failed.";
      std::cout << error.what() << " value: " << v << std::endl;
      throw;
    }
  };
  constexpr int overflow_value = zisc::cast<int>(vlist.size());
  ASSERT_THROW(enqueue_overflow(overflow_value), OverflowError) << message;

  // Dequeue test
  message = "Dequeuing failed.";
  const auto test_dequeue = [queue, message](const int expected)
  {
    const std::optional<Movable>& result = queue->dequeue();
    ASSERT_TRUE(result.has_value()) << message;
    const Movable& v = *result;
    ASSERT_EQ(expected, static_cast<int>(v)) << message << " Expected: " << expected;
  };
  std::for_each(vlist.begin(), vlist.end(), test_dequeue);

  // Empty dequeue test
  {
    const std::optional<Movable> result = queue->dequeue();
    ASSERT_FALSE(result.has_value()) << "Empty dequeue failed.";
  }

  // Clear queue test
  {
    message = "Cleaning queue data failed.";
    ASSERT_TRUE(queue->isEmpty()) << message;
    const std::optional<std::size_t> result = queue->enqueue(Movable{1});
    ASSERT_TRUE(result.has_value()) << message;
    ASSERT_FALSE(queue->isEmpty()) << message;
    queue->clear();
    ASSERT_TRUE(queue->isEmpty()) << message;
  }
}

/*!
  \details No detailed description

  \tparam QueueClass No description.
  \param [in,out] queue No description.
  */
template <typename QueueClass> inline
void testTinyCapacityQueue(zisc::Queue<QueueClass, int>* queue)
{
  ASSERT_EQ(1, queue->capacity()) << "Queue capacity isn't 1.";
  constexpr std::size_t n = 65536;
  for (std::size_t i = 0; i < n; ++i) {
    ASSERT_EQ(0, queue->size()) << "Queue isn't empty.";
    ASSERT_TRUE(queue->isEmpty()) << "Queue isn't empty.";
    {
      const auto value = zisc::cast<int>(i);
      const std::optional<std::size_t> result = queue->enqueue(value);
      ASSERT_TRUE(result.has_value()) << "Queueing an index failed.";
    }
    ASSERT_EQ(1, queue->size()) << "Enqueuing failed.";
    ASSERT_FALSE(queue->isEmpty()) << "Enqueuing failed.";
    {
      const std::optional<int> result = queue->dequeue();
      ASSERT_TRUE(result.has_value()) << "Dequeuing failed.";
      const auto index = zisc::cast<std::size_t>(*result);
      ASSERT_EQ(i, index);
    }
    ASSERT_EQ(0, queue->size()) << "Dequeuing failed.";
    ASSERT_TRUE(queue->isEmpty()) << "Dequeuing failed.";
  }
}

} /* namespace test */

#endif /* TEST_QUEUE_TEST_INL_HPP */
