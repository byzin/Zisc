/*!
  \file map.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MAP_HPP
#define ZISC_MAP_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
template <std::movable> class ContainerOverflowError;

//! Specify a type of a mapped value
template <typename Type>
concept MappedValue = std::is_void_v<Type> || std::movable<Type>;

/*!
  \brief No brief description

  No detailed description.

  \tparam MapClass No description.
  \tparam Key No description.
  \tparam T No description.
  \tparam Compare No description.
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T = void,
          std::invocable<Key, Key> Compare = std::less<Key>>
class Map : private NonCopyable<Map<MapClass, Key, T, Compare>>
{
 public:
  // Type aliases
  using KeyT = std::remove_volatile_t<Key>;
  using ConstKeyT = std::add_const_t<KeyT>;
  using MappedT = std::remove_volatile_t<T>;
  using ValueT = std::conditional_t<std::is_void_v<T>, KeyT,
                                                       std::pair<KeyT, MappedT>>;
  using ConstValueT = std::add_const_t<ValueT>;
  using CompareT = std::remove_volatile_t<Compare>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_const_t<std::add_lvalue_reference_t<ConstValueT>>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_const_t<std::add_pointer_t<ConstValueT>>;

  // Type aliases for STL
  using key_type = KeyT;
  using mapped_type = MappedT;
  using value_type = ValueT;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = CompareT;
  using reference = Reference;
  using const_reference = ConstReference;
  using pointer = Pointer;
  using const_pointer = ConstPointer;

  // Exception
  using OverflowError = ContainerOverflowError<ValueT>;


  //! Return an iterator to the beginning
  auto begin() noexcept;

  //! Return an iterator to the beginning
  auto cbegin() const noexcept;

  //! Return an iterator to the end
  auto end() noexcept;

  //! Return an iterator to the end
  auto cend() const noexcept;


  //! Insert the given value into the map
  template <typename ...Args>
  [[nodiscard]] std::optional<size_type> add(Args&&... args);

  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Check if the given value is contained in the map
  [[nodiscard]] std::optional<size_type> contain(ConstKeyT& key) const noexcept;

  //! Find the minimum key in the map
  [[nodiscard]] std::optional<Pointer> findMinKey() noexcept;

  //! Find the minimum key in the map
  [[nodiscard]] std::optional<ConstPointer> findMinKey() const noexcept;

  //! Retrun the value by the given index
  Reference get(const size_type index) noexcept;

  //! Retrun the value by the given index
  ConstReference get(const size_type index) const noexcept;

  //! Check if the map is bounded
  static constexpr bool isBounded() noexcept;

  //! Check if the map is concurrent
  static constexpr bool isConcurrent() noexcept;

  //! Check whether the map is empty
  bool isEmpty() const noexcept;

  //! Remove the value from the map
  [[nodiscard]] std::optional<size_type> remove(ConstKeyT& key);

  //! Change the maximum possible number of elements. The map will be cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements in the map
  size_type size() const noexcept;

 protected:
  // Type aliases
  using MapT = std::remove_volatile_t<MapClass>;
  using ConstMapT = std::add_const_t<MapT>;
  using MapPtr = std::add_pointer_t<MapT>;
  using ConstMapPtr = std::add_pointer_t<ConstMapT>;
  using MapReference = std::add_lvalue_reference_t<MapT>;
  using ConstMapReference = std::add_lvalue_reference_t<ConstMapT>;


  //! Create a map
  Map() noexcept;

  //! Move a data
  Map(const Map& other) noexcept;


  //! Move a data
  Map& operator=(const Map& other) noexcept;


  //! Get the key from the given value
  static ConstKeyT& getKey(ConstReference value) noexcept;

  //! Return the reference to the map class
  MapReference ref() noexcept;

  //! Return the reference to the map class
  ConstMapReference ref() const noexcept;
};

} // namespace zisc

#include "map-inl.hpp"

#endif // ZISC_MAP_HPP
