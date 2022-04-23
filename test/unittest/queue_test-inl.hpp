/*!
  \file queue_test-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_QUEUE_TEST_INL_HPP
#define TEST_QUEUE_TEST_INL_HPP

#include "queue_test.hpp"
// Standard C++ library
#include <cstddef>
#include <iostream>
#include <memory>
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

  // Enqueue test
  queue->setCapacity(8);
  ASSERT_EQ(8, queue->capacity()) << "The initial queue capacity isn't 1.";
  const char* message = "Enqueuing failed.";
  ASSERT_TRUE(queue->isEmpty()) << message;
  auto test_enqueue = [queue, message](const int value)
  {
    const std::optional<std::size_t> result = queue->enqueue(value);
    ASSERT_TRUE(result.has_value()) << message;
    const std::size_t storage_id = *result;
    ASSERT_EQ(value, queue->get(storage_id)) << message;
  };
  test_enqueue(7);
  test_enqueue(6);
  test_enqueue(4);
  test_enqueue(8);
  test_enqueue(2);
  test_enqueue(1);
  test_enqueue(5);
  test_enqueue(3);
  ASSERT_EQ(8, queue->size()) << message;
  ASSERT_FALSE(queue->isEmpty()) << "The queue is empty.";

  // Enqueue overflow test
  auto enqueue_overflow = [queue](const int value)
  {
    try {
      [[maybe_unused]] const std::optional<std::size_t> result = queue->enqueue(value);
    }
    catch (const typename Queue::OverflowError& error) {
      ASSERT_EQ(value, error.get()) << "The enqueue exception failed.";
      throw;
    }
  };
  ASSERT_THROW(enqueue_overflow(9), typename Queue::OverflowError) << message;

  // Dequeue test
  message = "Dequeuing failed.";
  auto test_dequeue = [message](const std::optional<int>& result, const int expected)
  {
    ASSERT_TRUE(result.has_value()) << message;
    ASSERT_EQ(expected, *result) << message << " Expected: " << expected;
  };
  test_dequeue(queue->dequeue(), 7);
  test_dequeue(queue->dequeue(), 6);
  test_dequeue(queue->dequeue(), 4);
  test_dequeue(queue->dequeue(), 8);
  test_dequeue(queue->dequeue(), 2);
  test_dequeue(queue->dequeue(), 1);
  test_dequeue(queue->dequeue(), 5);
  test_dequeue(queue->dequeue(), 3);
  ASSERT_TRUE(queue->isEmpty()) << "The queue isn't empty.";

  // Empty dequeue test
  {
    const std::optional<int> result = queue->dequeue();
    ASSERT_FALSE(result.has_value()) << "Empty dequeue failed.";
  }

  // Clear queue test
  {
    const std::optional<std::size_t> result = queue->enqueue(1);
    ASSERT_TRUE(result.has_value()) << "Enqueuing failed.";
    ASSERT_FALSE(queue->isEmpty()) << "The queue is empty.";
    queue->clear();
    ASSERT_TRUE(queue->isEmpty()) << "The queue isn't empty.";
  }
}

class MovableQValue : public zisc::NonCopyable<MovableQValue>
{
 public:
  MovableQValue() noexcept = default;

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

  // Enqueue test
  queue->setCapacity(8);
  ASSERT_EQ(8, queue->capacity()) << "The initial queue capacity isn't 1.";
  const char* message = "Enqueuing failed.";
  ASSERT_TRUE(queue->isEmpty()) << message;
  auto test_enqueue = [queue, message](const int value)
  {
    const std::optional<std::size_t> result = queue->enqueue(Movable{value});
    ASSERT_TRUE(result.has_value()) << message;
    const std::size_t storage_id = *result;
    ASSERT_EQ(value, static_cast<int>(queue->get(storage_id))) << message;
  };
  test_enqueue(1);
  test_enqueue(5);
  test_enqueue(3);
  test_enqueue(8);
  test_enqueue(2);
  test_enqueue(7);
  test_enqueue(6);
  test_enqueue(4);
  ASSERT_EQ(8, queue->size()) << message;
  ASSERT_FALSE(queue->isEmpty()) << "The queue is empty.";

  // Enqueue overflow test
  auto enqueue_overflow = [queue](const int value)
  {
    try {
      [[maybe_unused]] const std::optional<std::size_t> result = queue->enqueue(Movable{value});
    }
    catch (typename Queue::OverflowError& error) {
      Movable r = std::move(error.get());
      ASSERT_EQ(value, static_cast<int>(r)) << "The enqueue exception failed.";
      std::cout << error.what() << " value: " << static_cast<int>(r) << std::endl;
      throw;
    }
  };
  ASSERT_THROW(enqueue_overflow(9), typename Queue::OverflowError) << message;

  // Dequeue test
  message = "Dequeuing failed.";
  const auto test_dequeue = [message](const std::optional<Movable>& result,
                                      const int expected)
  {
    ASSERT_TRUE(result.has_value()) << message;
    const Movable& v = *result;
    ASSERT_EQ(expected, static_cast<int>(v)) << message << " Expected: " << expected;
  };
  test_dequeue(queue->dequeue(), 1);
  test_dequeue(queue->dequeue(), 5); test_dequeue(queue->dequeue(), 3);
  test_dequeue(queue->dequeue(), 8);
  test_dequeue(queue->dequeue(), 2);
  test_dequeue(queue->dequeue(), 7);
  test_dequeue(queue->dequeue(), 6);
  test_dequeue(queue->dequeue(), 4);

  // Empty dequeue test
  {
    const std::optional<Movable> result = queue->dequeue();
    ASSERT_FALSE(result.has_value()) << "Empty dequeue failed.";
  }

  // Clear queue test
  {
    const std::optional<std::size_t> result = queue->enqueue(Movable{1});
    ASSERT_TRUE(result.has_value()) << "Enqueuing failed.";
    ASSERT_FALSE(queue->isEmpty()) << "The queue is empty.";
    queue->clear();
    ASSERT_TRUE(queue->isEmpty()) << "The queue isn't empty.";
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
