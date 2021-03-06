/*!
  \file mutex_bst_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <array>
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/stopwatch.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/data_structure/bounded_bst.hpp"
#include "zisc/data_structure/mutex_bst.hpp"
#include "zisc/memory/simple_memory_resource.hpp"

TEST(MutexBstTest, ConstructorTest)
{
  using BstImpl = zisc::MutexBst;
  using Bst = zisc::BoundedBst<BstImpl>;

  zisc::SimpleMemoryResource mem_resource;
  std::unique_ptr<BstImpl> bst_impl;
  Bst* bst = nullptr;

  //
  {
    BstImpl bst_impl1{&mem_resource};
    bst_impl = std::make_unique<BstImpl>(std::move(bst_impl1));
  }
  bst = bst_impl.get();
  ASSERT_EQ(BstImpl::defaultCapacity(), bst->capacity())
      << "Constructing of MutexBst failed.";

  // 
  std::size_t cap = 4096;
  {
    BstImpl bst_impl1{cap, &mem_resource};
    bst_impl = std::make_unique<BstImpl>(std::move(bst_impl1));
  }
  bst = bst_impl.get();
  ASSERT_EQ(cap, bst->capacity())
      << "Constructing of MutexBst failed.";

  //
  cap = 5000;
  {
    *bst_impl = BstImpl{cap, &mem_resource};
  }
  ASSERT_EQ(cap, bst->capacity())
      << "Constructing of MutexBst failed.";
}

TEST(MutexBstTest, OperationTest)
{
  using BstImpl = zisc::MutexBst;
  using Bst = zisc::BoundedBst<BstImpl>;

  zisc::SimpleMemoryResource mem_resource;
  BstImpl bst_impl{&mem_resource};
  Bst* bst = std::addressof(bst_impl);

  std::vector<int> value_list{{7, 6, 4, 8, 2, 1, 5, 3, 15, 100, 0}};
  std::vector<int> value2_list{{10, 25000, -1}};

  for (const int value : value_list) {
    {
      const auto [result, index] = bst->add(value);
      ASSERT_TRUE(result) << "Adding value '" << value << "' into the bst failed.";
    }
    {
      const auto [result, index] = bst->add(value);
      ASSERT_FALSE(result) << "Adding value '" << value << "' into the bst failed.";
    }
  }

  for (const int value : value_list) {
    const bool result = bst->contain(value);
    ASSERT_TRUE(result) << "Quering value '" << value << "' from the bst failed.";
  }
  for (const int value : value2_list) {
    const bool result = bst->contain(value);
    ASSERT_FALSE(result) << "Quering value '" << value << "' from the bst failed.";
  }

  for (const int value : value_list) {
    const auto [result, index] = bst->add(value);
    ASSERT_FALSE(result) << "Adding value '" << value << "' into the bst failed.";
  }

  for (const int value : value_list) {
    {
      const bool result = bst->remove(value);
      ASSERT_TRUE(result) << "Removing value '" << value << "' from the bst failed.";
    }
    {
      const bool result = bst->remove(value);
      ASSERT_FALSE(result) << "Removing value '" << value << "' from the bst failed.";
    }
  }
  for (const int value : value2_list) {
    const bool result = bst->remove(value);
    ASSERT_FALSE(result) << "Removing value '" << value << "' from the bst failed.";
  }

  for (const int value : value_list) {
    const bool result = bst->contain(value);
    ASSERT_FALSE(result) << "Quering value '" << value << "' from the bst failed.";
  }

  for (const int value : value_list) {
    const bool result = bst->remove(value);
    ASSERT_FALSE(result) << "Removing value '" << value << "' from the bst failed.";
  }

  for (const int value : value_list) {
    const auto [result, index] = bst->add(value);
    ASSERT_TRUE(result) << "Adding value '" << value << "' into the bst failed.";
  }
}

TEST(MutexBstTest, AddRemoveTest)
{
  using BstImpl = zisc::MutexBst;
  using Bst = zisc::BoundedBst<BstImpl>;

  const std::array<int, 16> candidate_list{{-171717, -1000, -1, 0, 1, 3, 4, 9, 15, 16, 100, 2000, 10300, 50000, 360000, (std::numeric_limits<int>::max)()}};

  std::mt19937_64 rand_engine{123'567'789};
  std::uniform_int_distribution<std::size_t> sampler1{0, 1};
  std::uniform_int_distribution<std::size_t> sampler2{0, candidate_list.size() - 1};

  std::map<int, bool> value_list;
  for (const auto candidate : candidate_list)
    value_list[candidate] = false;

  zisc::SimpleMemoryResource mem_resource;
  BstImpl bst_impl{candidate_list.size() * 2, &mem_resource};
  Bst* bst = std::addressof(bst_impl);

  constexpr std::size_t n = 30'000'000;
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t op = sampler1(rand_engine);

    const std::size_t index = sampler2(rand_engine);
    const int candidate = candidate_list[index];
    auto& flag = value_list[candidate];

    bool result = false;
    if (op == 0) { // Add
      auto [r, id] = bst->add(candidate);
      result = r;
      if (flag) {
        ASSERT_FALSE(result) << "BST add operation failed.";
      }
      else {
        ASSERT_TRUE(result) << "BST add operation failed.";
        flag = true;
      }
    }
    else { // Remove
      result = bst->remove(candidate);
      if (flag) {
        ASSERT_TRUE(result) << "BST remove operation failed.";
        flag = false;
      }
      else {
        ASSERT_FALSE(result) << "BST remove operation failed.";
      }
    }

    for (auto v : value_list) {
      if (v.second)
        ASSERT_TRUE(bst->contain(v.first)) << "BST contain(" << v.first << ") operation failed.";
      else
        ASSERT_FALSE(bst->contain(v.first)) << "BST contain(" << v.first << ") operation failed.";
    }
  }
}

TEST(MutexBstTest, MultithreadTest)
{
  using BstImpl = zisc::MutexBst;
  using Bst = zisc::BoundedBst<BstImpl>;

  const std::size_t num_threads = 32;
  std::vector<std::thread> thread_list;
  thread_list.reserve(num_threads);

  constexpr int iMin = (std::numeric_limits<int>::min)();
  constexpr int iMax = (std::numeric_limits<int>::max)();
  const std::array<int, 16> candidate_list{{iMin, -600000, -599999, -1000, -524, -10, -2, 0, 4, 15, 89, 100, 5120, 800000, 123456789, iMax}};

  zisc::SimpleMemoryResource mem_resource;
  BstImpl bst_impl{candidate_list.size() * 2, &mem_resource};
  Bst* bst = std::addressof(bst_impl);

  for (std::size_t i = 0; i < num_threads; ++i) {
    std::mt19937_64 rand_engine{123'456'789 * i};
    auto task = [rand_engine, &candidate_list, bst]() mutable
    {
      std::uniform_int_distribution<std::size_t> sampler1{0, 1};
      std::uniform_int_distribution<std::size_t> sampler2{0, candidate_list.size() - 1};
      constexpr std::size_t n = 20'000'0;
      for (std::size_t j = 0; j < n; ++j) {
        const std::size_t op = sampler1(rand_engine);
        const std::size_t index = sampler2(rand_engine);
        const int candidate = candidate_list[index];
        if (op == 0) { // Add
          [[maybe_unused]] auto [result, id] = bst->add(candidate);
        }
        else {
          [[maybe_unused]] const bool result = bst->remove(candidate);
        }
      }
    };
    thread_list.emplace_back(std::move(task));
  }

  for (auto& worker : thread_list)
    worker.join();
}
