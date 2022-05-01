/*!
  \file queue_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_QUEUE_TEST_HPP
#define TEST_QUEUE_TEST_HPP

// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/structure/queue.hpp"

namespace test {

// Forward declaration
class MovableQValue;

//!
template <typename QueueClass>
void testSimpleBoundedQueue(zisc::Queue<QueueClass, int>* queue);

template <typename QueueClass>
void testMovableValueQueue(zisc::Queue<QueueClass, MovableQValue>* queue);

template <typename QueueClass>
void testTinyCapacityQueue(zisc::Queue<QueueClass, int>* queue);

} /* namespace test */

#include "queue_test-inl.hpp"

#endif /* TEST_QUEUE_TEST_HPP */
