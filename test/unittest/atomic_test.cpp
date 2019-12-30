/*!
  \file atomic_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/atomic.hpp"
#include "zisc/simple_memory_resource.hpp"
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

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

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
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
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

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

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
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic subtraction failed: ";
  for (const auto flag : table)
    ASSERT_TRUE(flag) << error_message << value;
  constexpr Type expected = -zisc::cast<Type>(resolution);
  ASSERT_EQ(expected, value) << error_message << value;
}

template <typename Type, std::size_t resolution>
void testAtomicExchange()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::uint, const Type v)
  {
    auto old = zisc::Atomic::exchange(&value, v);
    table[old] = 1;
  };

  constexpr Type s = zisc::cast<Type>(1);
  constexpr Type e = zisc::cast<Type>(resolution);
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic exchange failed: ";
  std::size_t num_of_failure = 0;
  for (const auto flag : table) {
    if (!flag)
      ++num_of_failure;
  }
  ASSERT_GE(1u, num_of_failure) << error_message << value;
}

template <typename Type, std::size_t resolution>
void testAtomicIncrement()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::uint, const std::size_t)
  {
    auto old = zisc::Atomic::increment(&value);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic increment failed: ";
  for (const auto flag : table)
    ASSERT_TRUE(flag) << error_message << value;
  constexpr Type expected = zisc::cast<Type>(resolution);
  ASSERT_EQ(expected, value) << error_message << value;
}

template <typename Type, std::size_t resolution>
void testAtomicDecrement()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::uint, const std::size_t)
  {
    auto old = -zisc::Atomic::decrement(&value);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic decrement failed: ";
  for (const auto flag : table)
    ASSERT_TRUE(flag) << error_message << value;
  constexpr Type expected = -zisc::cast<Type>(resolution);
  ASSERT_EQ(expected, value) << error_message << value;
}

template <typename Type, std::size_t resolution>
void testAtomicMin()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

  Type value = zisc::cast<Type>(0);

  auto test = [&value](const zisc::uint, const Type v)
  {
    zisc::Atomic::min(&value, -v);
  };

  constexpr Type s = zisc::cast<Type>(0);
  constexpr Type e = zisc::cast<Type>(resolution);
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic min failed: ";
  constexpr Type expected = -zisc::cast<Type>(resolution - 1);
  ASSERT_EQ(expected, value) << error_message << value;
}

template <typename Type, std::size_t resolution>
void testAtomicMax()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

  Type value = zisc::cast<Type>(0);

  auto test = [&value](const zisc::uint, const Type v)
  {
    zisc::Atomic::max(&value, v);
  };

  constexpr Type s = zisc::cast<Type>(0);
  constexpr Type e = zisc::cast<Type>(resolution);
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic min failed: ";
  constexpr Type expected = zisc::cast<Type>(resolution - 1);
  ASSERT_EQ(expected, value) << error_message << value;
}

template <typename Type, std::size_t resolution>
void testAtomicCmpxchg()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::uint, const std::size_t)
  {
    auto inc = [](const Type c)
    {
      const Type result = zisc::cast<Type>(c + 1);
      return result;
    };
    auto old = zisc::Atomic::perform(&value, inc);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic cmpxchg failed: ";
  for (const auto flag : table)
    ASSERT_TRUE(flag) << error_message << value;
  constexpr Type expected = zisc::cast<Type>(resolution);
  ASSERT_EQ(expected, value) << error_message << value;
}

template <typename Type>
void testAtomicAnd()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

  Type value = std::numeric_limits<Type>::max();

  auto test = [&value](const zisc::uint, const std::size_t i)
  {
    const Type v = ~zisc::cast<Type>(zisc::cast<Type>(1) << i);
    zisc::Atomic::andBit(&value, v);
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = 8 * sizeof(Type);
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic and failed: ";
  ASSERT_FALSE(value) << error_message << value;
}

template <typename Type>
void testAtomicOr()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  zisc::SimpleMemoryResource mem_resource;
  zisc::ThreadManager thread_manager{100, &mem_resource};

  Type value = zisc::cast<Type>(0);

  auto test = [&value](const zisc::uint, const std::size_t i)
  {
    const Type v = zisc::cast<Type>(zisc::cast<Type>(1) << i);
    zisc::Atomic::orBit(&value, v);
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = 8 * sizeof(Type);
  auto result = thread_manager.enqueueLoop(test, s, e, &mem_resource);
  result->wait();

  const char* error_message = "Atomic or failed: ";
  constexpr Type expected = ~zisc::cast<Type>(0);
  ASSERT_EQ(expected, value) << error_message << value;
}

template <typename Type>
void testAtomicXor()
{
  std::cout << "Atomic is always lock-free: "
            << zisc::Atomic::isAlwaysLockFree<Type>() << std::endl;
  std::cout << "Atomic is lock-free: "
            << zisc::Atomic::isLockFree<Type>() << std::endl;

  constexpr Type v = std::numeric_limits<Type>::max();
  Type value = v;

  const auto old = zisc::Atomic::xorBit(&value, v);

  const char* error_message = "Atomic xor failed: ";
  ASSERT_EQ(v, old) << error_message << old;
  ASSERT_FALSE(value) << error_message << v;
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

TEST(AtomicTest, ExchangeInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicExchange<zisc::int8b, resolution>();
}

TEST(AtomicTest, ExchangeInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicExchange<zisc::int16b, resolution>();
}

TEST(AtomicTest, ExchangeInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicExchange<zisc::int32b, resolution>();
}

TEST(AtomicTest, ExchangeInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicExchange<zisc::int64b, resolution>();
}

TEST(AtomicTest, ExchangeUInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicExchange<zisc::uint8b, resolution>();
}

TEST(AtomicTest, ExchangeUInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicExchange<zisc::uint16b, resolution>();
}

TEST(AtomicTest, ExchangeUInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicExchange<zisc::uint32b, resolution>();
}

TEST(AtomicTest, ExchangeUInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicExchange<zisc::uint64b, resolution>();
}

TEST(AtomicTest, IncrementInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicIncrement<zisc::int8b, resolution>();
}

TEST(AtomicTest, IncrementInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicIncrement<zisc::int16b, resolution>();
}

TEST(AtomicTest, IncrementInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicIncrement<zisc::int32b, resolution>();
}

TEST(AtomicTest, IncrementInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicIncrement<zisc::int64b, resolution>();
}

TEST(AtomicTest, IncrementUInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicIncrement<zisc::uint8b, resolution>();
}

TEST(AtomicTest, IncrementUInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicIncrement<zisc::uint16b, resolution>();
}

TEST(AtomicTest, IncrementUInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicIncrement<zisc::uint32b, resolution>();
}

TEST(AtomicTest, IncrementUInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicIncrement<zisc::uint64b, resolution>();
}

TEST(AtomicTest, DecrementInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicDecrement<zisc::int8b, resolution>();
}

TEST(AtomicTest, DecrementInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicDecrement<zisc::int16b, resolution>();
}

TEST(AtomicTest, DecrementInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicDecrement<zisc::int32b, resolution>();
}

TEST(AtomicTest, DecrementInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicDecrement<zisc::int64b, resolution>();
}

TEST(AtomicTest, CmpxchgInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicCmpxchg<zisc::int8b, resolution>();
}

TEST(AtomicTest, CmpxchgInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicCmpxchg<zisc::int16b, resolution>();
}

TEST(AtomicTest, CmpxchgInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicCmpxchg<zisc::int32b, resolution>();
}

TEST(AtomicTest, CmpxchgInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicCmpxchg<zisc::int64b, resolution>();
}

TEST(AtomicTest, CmpxchgUInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicCmpxchg<zisc::uint8b, resolution>();
}

TEST(AtomicTest, CmpxchgUInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicCmpxchg<zisc::uint16b, resolution>();
}

TEST(AtomicTest, CmpxchgUInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicCmpxchg<zisc::uint32b, resolution>();
}

TEST(AtomicTest, CmpxchgUInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicCmpxchg<zisc::uint64b, resolution>();
}

TEST(AtomicTest, MinInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMin<zisc::int32b, resolution>();
}

TEST(AtomicTest, MaxInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMax<zisc::int32b, resolution>();
}

TEST(AtomicTest, MaxUInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMax<zisc::uint32b, resolution>();
}

TEST(AtomicTest, AndUInt8Test)
{
  ::testAtomicAnd<zisc::uint8b>();
}

TEST(AtomicTest, AndUInt16Test)
{
  ::testAtomicAnd<zisc::uint16b>();
}

TEST(AtomicTest, AndUInt32Test)
{
  ::testAtomicAnd<zisc::uint32b>();
}

TEST(AtomicTest, AndUInt64Test)
{
  ::testAtomicAnd<zisc::uint64b>();
}

TEST(AtomicTest, OrUInt8Test)
{
  ::testAtomicOr<zisc::uint8b>();
}

TEST(AtomicTest, OrUInt16Test)
{
  ::testAtomicOr<zisc::uint16b>();
}

TEST(AtomicTest, OrUInt32Test)
{
  ::testAtomicOr<zisc::uint32b>();
}

TEST(AtomicTest, OrUInt64Test)
{
  ::testAtomicOr<zisc::uint64b>();
}

TEST(AtomicTest, XorUInt8Test)
{
  ::testAtomicXor<zisc::uint8b>();
}

TEST(AtomicTest, XorUInt16Test)
{
  ::testAtomicXor<zisc::uint16b>();
}

TEST(AtomicTest, XorUInt32Test)
{
  ::testAtomicXor<zisc::uint32b>();
}

TEST(AtomicTest, XorUInt64Test)
{
  ::testAtomicXor<zisc::uint64b>();
}
