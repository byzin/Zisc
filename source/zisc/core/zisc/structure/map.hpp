/*!
  \file map.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
  [[nodiscard]]
  auto add(Args&&... args) -> std::optional<size_type>;

  //! Return the maximum possible number of elements
  [[nodiscard]]
  auto capacity() const noexcept -> size_type;

  //! Return the maximum possible capacity
  static constexpr auto capacityMax() noexcept -> size_type;

  //! Clear the contents
  void clear() noexcept;

  //! Check if the given value is contained in the map
  [[nodiscard]]
  auto contain(ConstKeyT& key) const noexcept -> std::optional<size_type>;

  //! Find the minimum key in the map
  [[nodiscard]]
  auto findMinKey() noexcept -> std::optional<Pointer>;

  //! Find the minimum key in the map
  [[nodiscard]]
  auto findMinKey() const noexcept -> std::optional<ConstPointer>;

  //! Retrun the value by the given index
  auto get(const size_type index) noexcept -> Reference;

  //! Retrun the value by the given index
  auto get(const size_type index) const noexcept -> ConstReference;

  //! Check if the map is bounded
  static constexpr auto isBounded() noexcept -> bool;

  //! Check if the map is concurrent
  static constexpr auto isConcurrent() noexcept -> bool;

  //! Check whether the map is empty
  [[nodiscard]]
  auto isEmpty() const noexcept -> bool;

  //! Remove the value from the map
  [[nodiscard]]
  auto remove(ConstKeyT& key) -> std::optional<size_type>;

  //! Change the maximum possible number of elements. The map will be cleared
  void setCapacity(const size_type cap);

  //! Return the number of elements in the map
  [[nodiscard]]
  auto size() const noexcept -> size_type;

 protected:
  // Type aliases
  using MapT = std::remove_volatile_t<MapClass>;
  using ConstMapT = std::add_const_t<MapT>;
  using MapPtr = std::add_pointer_t<MapT>;
  using ConstMapPtr = std::add_pointer_t<ConstMapT>;
  using MapReference = std::add_lvalue_reference_t<MapT>;
  using ConstMapReference = std::add_lvalue_reference_t<ConstMapT>;


  //! Create a map
  Map() noexcept = default;

  //! Move a data
  Map(const Map& other) noexcept;


  //! Move a data
  auto operator=(const Map& other) noexcept -> Map&;


  //! Get the key from the given value
  static auto getKey(ConstReference value) noexcept -> ConstKeyT&;

  //! Return the reference to the map class
  auto ref() noexcept -> MapReference;

  //! Return the reference to the map class
  auto ref() const noexcept -> ConstMapReference;
};

} // namespace zisc

#include "map-inl.hpp"

#endif // ZISC_MAP_HPP
