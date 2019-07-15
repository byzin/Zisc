/*!
  \file atomic_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/atomic.hpp"
#include "zisc/thread_manager.hpp"
#include "zisc/zisc_config.hpp"

namespace  {

template <typename Type, std::size_t resolution>
void testAtomicAddition()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::ThreadManager thread_manager{100};

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::uint, const std::size_t)
  {
    constexpr Type v = zisc::cast<Type>(1);
    auto old = zisc::Atomic::add(&value, v);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e);
  result->wait();

  const char* error_message = "Atomic addition failed: ";
  for (const auto flag : table)
    ASSERT_TRUE(flag) << error_message << value;
  constexpr Type expected = zisc::cast<Type>(resolution);
  ASSERT_EQ(expected, value) << error_message << value;
}

template <typename Type, std::size_t resolution>
void testAtomicSubtraction()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::ThreadManager thread_manager{100};

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::uint, const std::size_t)
  {
    constexpr Type v = zisc::cast<Type>(1);
    auto old = -zisc::Atomic::sub(&value, v);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e);
  result->wait();

  const char* error_message = "Atomic subtraction failed: ";
  for (const auto flag : table)
    ASSERT_TRUE(flag) << error_message << value;
  constexpr Type expected = -zisc::cast<Type>(resolution);
  ASSERT_EQ(expected, value) << error_message << value;
}

} // namespace 

TEST(AtomicTest, AdditionInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicAddition<zisc::int8b, resolution>();
}

TEST(AtomicTest, AdditionInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicAddition<zisc::int16b, resolution>();
}

TEST(AtomicTest, AdditionInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicAddition<zisc::int32b, resolution>();
}

TEST(AtomicTest, AdditionInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicAddition<zisc::int64b, resolution>();
}

TEST(AtomicTest, AdditionUInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicAddition<zisc::uint8b, resolution>();
}

TEST(AtomicTest, AdditionUInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicAddition<zisc::uint16b, resolution>();
}

TEST(AtomicTest, AdditionUInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicAddition<zisc::uint32b, resolution>();
}

TEST(AtomicTest, AdditionUInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicAddition<zisc::uint64b, resolution>();
}

TEST(AtomicTest, SubtractionInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicSubtraction<zisc::int8b, resolution>();
}

TEST(AtomicTest, SubtractionInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicSubtraction<zisc::int16b, resolution>();
}

TEST(AtomicTest, SubtractionInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicSubtraction<zisc::int32b, resolution>();
}

TEST(AtomicTest, SubtractionInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicSubtraction<zisc::int64b, resolution>();
}

