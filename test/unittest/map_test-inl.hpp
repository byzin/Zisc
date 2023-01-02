/*!
  \file map_test-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_MAP_TEST_INL_HPP
#define TEST_MAP_TEST_INL_HPP

#include "map_test.hpp"
// Standard C++ library
#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <random>
#include <type_traits>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/structure/map.hpp"

namespace test {

/*!
  \details No detailed description

  \tparam MapClass No description.
  \param [in,out] map No description.
  */
template <typename MapClass> inline
void testSimpleBoundedMap(zisc::Map<MapClass, int>* map)
{
  using Map = std::remove_cvref_t<decltype(*map)>;
  static_assert(Map::isBounded(), "The map isn't bounded map.");

  ASSERT_EQ(1, map->capacity()) << "The initial map capacity isn't 1.";
  map->clear();
  ASSERT_TRUE(map->isEmpty()) << "The map isn't empty.";

  // Check iterator
  using Iterator = decltype(map->begin());
  static_assert(std::is_same_v<std::ptrdiff_t,
                               std::iter_difference_t<Iterator>>);
  static_assert(std::is_same_v<std::forward_iterator_tag,
                               typename Iterator::iterator_category>);
  static_assert(std::is_same_v<std::forward_iterator_tag,
                               typename Iterator::iterator_concept>);
  ASSERT_EQ(map->begin(), map->end()) << "Iterator is wrong.";

  // Create input values
  constexpr std::size_t n = 64;
  std::mt19937_64 sampler{123'456'789};
  std::array<int, n> vlist;
  constexpr int min_value = -zisc::cast<int>(n / 2);
  std::iota(vlist.begin(), vlist.end(), min_value);
  std::shuffle(vlist.begin(), vlist.end(), sampler);

  // Set capacity test
  map->setCapacity(vlist.size());
  ASSERT_EQ(vlist.size(), map->capacity()) << "Initializing map capacity failed.";
  ASSERT_TRUE(map->isEmpty()) << "The map isn't empty.";

  // Add test
  const char* message = "Adding value failed.";
  auto test_add = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->add(value);
    ASSERT_TRUE(result.has_value()) << message;
    const std::size_t storage_id = *result;
    ASSERT_EQ(value, map->get(storage_id)) << message;
  };
  std::for_each(vlist.begin(), vlist.end(), test_add);
  ASSERT_EQ(vlist.size(), map->size()) << message;
  ASSERT_FALSE(map->isEmpty()) << "The map is still empty.";

  // Add overflow test
  using OverflowError = typename Map::OverflowError;
  auto add_overflow = [map](const int value)
  {
    try {
      [[maybe_unused]] const std::optional<std::size_t> result = map->add(value);
    }
    catch (const OverflowError& error) {
      ASSERT_EQ(value, error.get()) << "The map overflow exception failed.";
      std::cout << error.what() << " value: " << error.get() << std::endl;
      throw;
    }
  };
  constexpr int overflow_value = zisc::cast<int>(vlist.size());
  ASSERT_THROW(add_overflow(overflow_value), OverflowError) << message;

  // Add overlap test
  std::array vlist2 = vlist;
  std::shuffle(vlist2.begin(), vlist2.end(), sampler);
  auto test_add_overlap = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->add(value);
    ASSERT_FALSE(result.has_value()) << message;
  };
  std::for_each(vlist2.begin(), vlist2.end(), test_add_overlap);

  // Query test
  message = "Query a value from the map failed.";
  auto test_query = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->contain(value);
    ASSERT_TRUE(result.has_value()) << message;
  };
  std::for_each(vlist.begin(), vlist.end(), test_query);
  {
    const auto result = map->findMinKey();
    ASSERT_TRUE(result.has_value()) << message;
    ASSERT_EQ(min_value, **result) << message;
  }

  // Check iterator
  {
    std::array vlist3 = vlist;
    std::sort(vlist3.begin(), vlist3.end());
    std::size_t index = 0;
    for (auto ite = map->begin(); ite != map->end();) {
      ASSERT_EQ(vlist3[index++], *ite) << "Iterator is wrong.";
      ++ite;
    }
  }
  {
    std::array vlist3 = vlist;
    std::sort(vlist3.begin(), vlist3.end());
    std::size_t index = 0;
    for (auto ite = map->begin(); ite != map->end();) {
      auto i = ite++;
      ASSERT_EQ(vlist3[index++], *i) << "Iterator is wrong.";
    }
  }

  // Remove test
  message = "Removing value failed.";
  auto test_remove = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->remove(value);
    ASSERT_TRUE(result.has_value()) << message;
  };
  std::for_each(vlist.begin(), vlist.end(), test_remove);
  ASSERT_TRUE(map->isEmpty()) << message;

  // Remove overlap test
  auto test_remove_overlap = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->remove(value);
    ASSERT_FALSE(result.has_value()) << message;
  };
  std::for_each(vlist2.begin(), vlist2.end(), test_remove_overlap);

  // Query test2
  message = "Query a value from the map failed.";
  auto test_query2 = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->contain(value);
    ASSERT_FALSE(result.has_value()) << message;
  };
  std::for_each(vlist2.begin(), vlist2.end(), test_query2);
  {
    const auto result = map->findMinKey();
    ASSERT_FALSE(result.has_value()) << message;
  }

  // Check iterator
  ASSERT_EQ(map->begin(), map->end()) << "Iterator is wrong.";

  // Clean map data test
  message = "Cleaning map data failed.";
  ASSERT_TRUE(map->isEmpty()) << message;
  std::for_each(vlist.begin(), vlist.end(), test_add);
  ASSERT_FALSE(map->isEmpty()) << message;
  map->clear();
  ASSERT_TRUE(map->isEmpty()) << message;
}

class MovableMValue : public zisc::NonCopyable<MovableMValue>
{
 public:
  MovableMValue() = default;

  MovableMValue(const int v) : value_{v} {}

  MovableMValue(MovableMValue&& other) noexcept : value_{other.value_}
  {
    other.value_ = -1;
  }

  MovableMValue& operator=(MovableMValue&& other) noexcept
  {
    std::swap(value_, other.value_);
    return *this;
  }

  explicit operator int() const
  {
    return value_;
  }

  int value() const noexcept
  {
    return value_;
  }

 private:
  int value_ = -1;
};

/*!
  \details No detailed description

  \tparam MapClass No description.
  \param [in,out] map No description.
  */
template <typename MapClass> inline
void testMovableValueMap(zisc::Map<MapClass, int, MovableMValue>* map)
{
  using Map = std::remove_cvref_t<decltype(*map)>;
  using Movable = MovableMValue;
  static_assert(Map::isBounded(), "The map isn't bounded map.");

  ASSERT_EQ(1, map->capacity()) << "The initial map capacity isn't 1.";
  map->clear();
  ASSERT_TRUE(map->isEmpty()) << "The map isn't empty.";

  // Check iterator
  using Iterator = decltype(map->begin());
  static_assert(std::is_same_v<std::ptrdiff_t,
                               std::iter_difference_t<Iterator>>);
  static_assert(std::is_same_v<std::forward_iterator_tag,
                               typename Iterator::iterator_category>);
  static_assert(std::is_same_v<std::forward_iterator_tag,
                               typename Iterator::iterator_concept>);
  ASSERT_EQ(map->begin(), map->end()) << "Iterator is wrong.";

  // Create input values
  constexpr std::size_t n = 64;
  std::mt19937_64 sampler{123'456'789};
  std::array<int, n> vlist;
  constexpr int min_value = -zisc::cast<int>(n / 2);
  std::iota(vlist.begin(), vlist.end(), min_value);
  std::shuffle(vlist.begin(), vlist.end(), sampler);

  // Set capacity test
  map->setCapacity(vlist.size());
  ASSERT_EQ(vlist.size(), map->capacity()) << "Initializing map capacity failed.";
  ASSERT_TRUE(map->isEmpty()) << "The map isn't empty.";

  // Add test
  const char* message = "Adding value failed.";
  auto test_add = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->add(value, Movable{value});
    ASSERT_TRUE(result.has_value()) << message;
    const std::size_t storage_id = *result;
    const auto& r = map->get(storage_id);
    ASSERT_EQ(value, r.first) << message;
    ASSERT_EQ(value, static_cast<int>(r.second)) << message;
  };
  std::for_each(vlist.begin(), vlist.end(), test_add);
  ASSERT_EQ(vlist.size(), map->size()) << message;
  ASSERT_FALSE(map->isEmpty()) << "The map is still empty.";

  // Add overflow test
  using OverflowError = typename Map::OverflowError;
  auto add_overflow = [map](const int value)
  {
    try {
      [[maybe_unused]] const std::optional<std::size_t> result = map->add(value, Movable{value});
    }
    catch (const OverflowError& error) {
      const auto& r = error.get();
      ASSERT_EQ(value, r.first) << "The map overflow exception failed.";
      const int v = static_cast<int>(r.second);
      ASSERT_EQ(value, v) << "The map overflow exception failed.";
      std::cout << error.what() << " value: " << v << std::endl;
      throw;
    }
  };
  constexpr int overflow_value = zisc::cast<int>(vlist.size());
  ASSERT_THROW(add_overflow(overflow_value), OverflowError) << message;

  // Add overlap test
  std::array vlist2 = vlist;
  std::shuffle(vlist2.begin(), vlist2.end(), sampler);
  auto test_add_overlap = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->add(value, Movable{value});
    ASSERT_FALSE(result.has_value()) << message;
  };
  std::for_each(vlist2.begin(), vlist2.end(), test_add_overlap);

  // Query test
  message = "Query a value from the map failed.";
  auto test_query = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->contain(value);
    ASSERT_TRUE(result.has_value()) << message;
  };
  std::for_each(vlist.begin(), vlist.end(), test_query);
  {
    const auto result = map->findMinKey();
    const auto& r = *result;
    ASSERT_TRUE(result.has_value()) << message;
    ASSERT_EQ(min_value, r->first) << message;
    ASSERT_EQ(min_value, static_cast<int>(r->second)) << message;
  }

  // Check iterator
  {
    std::array vlist3 = vlist;
    std::sort(vlist3.begin(), vlist3.end());
    std::size_t index = 0;
    for (auto ite = map->begin(); ite != map->end();) {
      ASSERT_EQ(vlist3[index++], ite->second.value()) << "Iterator is wrong.";
      ++ite;
    }
  }
  {
    std::array vlist3 = vlist;
    std::sort(vlist3.begin(), vlist3.end());
    std::size_t index = 0;
    for (auto ite = map->begin(); ite != map->end();) {
      auto i = ite++;
      ASSERT_EQ(vlist3[index++], i->second.value()) << "Iterator is wrong.";
    }
  }

  // Remove test
  message = "Removing value failed.";
  auto test_remove = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->remove(value);
    ASSERT_TRUE(result.has_value()) << message;
  };
  std::for_each(vlist.begin(), vlist.end(), test_remove);
  ASSERT_TRUE(map->isEmpty()) << message;

  // Remove overlap test
  auto test_remove_overlap = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->remove(value);
    ASSERT_FALSE(result.has_value()) << message;
  };
  std::for_each(vlist2.begin(), vlist2.end(), test_remove_overlap);

  // Query test2
  message = "Query a value from the map failed.";
  auto test_query2 = [map, message](const int value)
  {
    const std::optional<std::size_t> result = map->contain(value);
    ASSERT_FALSE(result.has_value()) << message;
  };
  std::for_each(vlist2.begin(), vlist2.end(), test_query2);
  {
    const auto result = map->findMinKey();
    ASSERT_FALSE(result.has_value()) << message;
  }

  // Clean map data test
  message = "Cleaning map data failed.";
  ASSERT_TRUE(map->isEmpty()) << message;
  std::for_each(vlist.begin(), vlist.end(), test_add);
  ASSERT_FALSE(map->isEmpty()) << message;
  map->clear();
  ASSERT_TRUE(map->isEmpty()) << message;
}

template <typename MapClass> inline
void testTinyCapacityMap(zisc::Map<MapClass, int>* map)
{
  ASSERT_EQ(1, map->capacity()) << "Map capacity isn't 1.";
  constexpr std::size_t n = 65536;
  for (std::size_t i = 0; i < n; ++i) {
    ASSERT_EQ(0, map->size()) << "Map isn't empty.";
    ASSERT_TRUE(map->isEmpty()) << "Map isn't empty.";
    const char* message = "Adding a value failed.";
    const auto value = zisc::cast<int>(i);
    {
      const std::optional<std::size_t> result = map->add(value);
      ASSERT_TRUE(result.has_value()) << message;
    }
    ASSERT_EQ(1, map->size()) << message;
    ASSERT_FALSE(map->isEmpty()) << message;
    message = "Quering a value failed.";
    {
      const std::optional<std::size_t> result = map->contain(value);
      ASSERT_TRUE(result.has_value()) << message;
    }
    message = "Removing a value failed.";
    {
      const std::optional<std::size_t> result = map->remove(value);
      ASSERT_TRUE(result.has_value()) << message;
    }
    ASSERT_EQ(0, map->size()) << message;
    ASSERT_TRUE(map->isEmpty()) << message;
  }
}

} /* namespace test */

#endif /* TEST_MAP_TEST_INL_HPP */
