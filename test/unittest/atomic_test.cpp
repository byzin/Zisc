/*!
  \file atomic_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/concurrency/atomic.hpp"
#include "zisc/concurrency/thread_manager.hpp"
#include "zisc/memory/simple_memory_resource.hpp"

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
  thread_manager.setCapacity(resolution);

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::int64b, const std::size_t) noexcept
  {
    constexpr Type v = zisc::cast<Type>(1);
    auto old = zisc::atomic_fetch_add(&value, v);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_fetch_add() failed. value=";
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
  thread_manager.setCapacity(resolution);

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::int64b, const std::size_t) noexcept
  {
    constexpr Type v = zisc::cast<Type>(1);
    auto old = -zisc::atomic_fetch_sub(&value, v);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_fetch_sub() failed. value=";
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
  thread_manager.setCapacity(resolution);

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const Type v, const zisc::int64b) noexcept
  {
    auto old = zisc::atomic_exchange(&value, v);
    table[old] = 1;
  };

  constexpr Type s = zisc::cast<Type>(1);
  constexpr Type e = zisc::cast<Type>(resolution);
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_exchange() failed. value=";
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
  thread_manager.setCapacity(resolution);

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::int64b, const std::size_t) noexcept
  {
    auto old = zisc::atomic_fetch_inc(&value);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_fetch_inc() failed. value=";
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
  thread_manager.setCapacity(resolution);

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::int64b, const std::size_t) noexcept
  {
    auto old = -zisc::atomic_fetch_dec(&value);
    table[old] = 1;
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = resolution;
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_fetch_dec() failed. value=";
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
  thread_manager.setCapacity(resolution);

  Type value = zisc::cast<Type>(0);

  auto test = [&value](const Type v, const zisc::int64b) noexcept
  {
    zisc::atomic_fetch_min(&value, zisc::cast<Type>(-v));
  };

  constexpr Type s = zisc::cast<Type>(0);
  constexpr Type e = zisc::cast<Type>(resolution);
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_fetch_min() failed. value=";
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
  thread_manager.setCapacity(resolution);

  Type value = zisc::cast<Type>(0);

  auto test = [&value](const Type v, const zisc::int64b) noexcept
  {
    zisc::atomic_fetch_max(&value, v);
  };

  constexpr Type s = zisc::cast<Type>(0);
  constexpr Type e = zisc::cast<Type>(resolution);
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_fetch_max() failed. value=";
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
  thread_manager.setCapacity(resolution);

  Type value = zisc::cast<Type>(0);
  std::vector<int> table;
  table.resize(resolution, 0);

  auto test = [&value, &table](const zisc::int64b, const std::size_t) noexcept
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
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_compare_exchange() failed. value=";
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

  auto test = [&value](const std::size_t i, const zisc::int64b) noexcept
  {
    const Type v = ~zisc::cast<Type>(zisc::cast<Type>(1) << i);
    zisc::atomic_fetch_and(&value, v);
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = 8 * sizeof(Type);
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_fetch_and() failed. value=";
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

  auto test = [&value](const std::size_t i, const zisc::int64b) noexcept
  {
    const Type v = zisc::cast<Type>(zisc::cast<Type>(1) << i);
    zisc::atomic_fetch_or(&value, v);
  };

  constexpr std::size_t s = 0;
  constexpr std::size_t e = 8 * sizeof(Type);
  auto result = thread_manager.enqueueLoop(test, s, e);
  result.wait();

  const char* error_message = "zisc::atomic_fetch_or() failed. value=";
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

  const auto old = zisc::atomic_fetch_xor(&value, v);

  const char* error_message = "zisc::atomic_fetch_xor() failed. value=";
  ASSERT_EQ(v, old) << error_message << old;
  ASSERT_FALSE(value) << error_message << v;
}

} // namespace 

TEST(AtomicTest, LoadStoreInt8Test)
{
  using zisc::uint8b;
  uint8b value = 0;
  constexpr uint8b expected = 0b0110'1001;
  {
    zisc::atomic_store(&value, expected);
    ASSERT_EQ(expected, value) << "zisc::atomic_store() failed.";
  }
  {
    const uint8b result = zisc::atomic_load(&value);
    ASSERT_EQ(expected, result) << "zisc::atomic_load() failed.";
  }
}

TEST(AtomicTest, LoadStoreInt16Test)
{
  using zisc::uint16b;
  uint16b value = 0;
  constexpr uint16b expected = 0b1010'0101'1111'0011;
  {
    zisc::atomic_store(&value, expected);
    ASSERT_EQ(expected, value) << "zisc::atomic_store() failed.";
  }
  {
    const uint16b result = zisc::atomic_load(&value);
    ASSERT_EQ(expected, result) << "zisc::atomic_load() failed.";
  }
}

TEST(AtomicTest, LoadStoreInt32Test)
{
  using zisc::uint32b;
  uint32b value = 0;
  constexpr uint32b expected = 0xcb8a0146;
  {
    zisc::atomic_store(&value, expected);
    ASSERT_EQ(expected, value) << "zisc::atomic_store() failed.";
  }
  {
    const uint32b result = zisc::atomic_load(&value);
    ASSERT_EQ(expected, result) << "zisc::atomic_load() failed.";
  }
}

TEST(AtomicTest, LoadStoreInt64Test)
{
  using zisc::uint64b;
  uint64b value = 0;
  constexpr uint64b expected = 0x0ad58011'cb8a0146;
  {
    zisc::atomic_store(&value, expected);
    ASSERT_EQ(expected, value) << "zisc::atomic_store() failed.";
  }
  {
    const uint64b result = zisc::atomic_load(&value);
    ASSERT_EQ(expected, result) << "zisc::atomic_load() failed.";
  }
}

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

/*
TEST(AtomicTest, DecrementInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicDecrement<zisc::int8b, resolution>();
}
*/

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

TEST(AtomicTest, MinInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicMin<zisc::int8b, resolution>();
}

TEST(AtomicTest, MinInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicMin<zisc::int16b, resolution>();
}

TEST(AtomicTest, MinInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMin<zisc::int32b, resolution>();
}

TEST(AtomicTest, MinInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMin<zisc::int64b, resolution>();
}

TEST(AtomicTest, MaxInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicMax<zisc::int8b, resolution>();
}

TEST(AtomicTest, MaxInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicMax<zisc::int16b, resolution>();
}

TEST(AtomicTest, MaxInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMax<zisc::int32b, resolution>();
}

TEST(AtomicTest, MaxInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMax<zisc::int64b, resolution>();
}

TEST(AtomicTest, MaxUInt8Test)
{
  constexpr std::size_t resolution = 100;
  ::testAtomicMax<zisc::uint8b, resolution>();
}

TEST(AtomicTest, MaxUInt16Test)
{
  constexpr std::size_t resolution = 30'000;
  ::testAtomicMax<zisc::uint16b, resolution>();
}

TEST(AtomicTest, MaxUInt32Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMax<zisc::uint32b, resolution>();
}

TEST(AtomicTest, MaxUInt64Test)
{
  constexpr std::size_t resolution = 1'000'000;
  ::testAtomicMax<zisc::uint64b, resolution>();
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
