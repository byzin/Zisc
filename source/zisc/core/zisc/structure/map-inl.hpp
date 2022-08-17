/*!
  \file map-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MAP_INL_HPP
#define ZISC_MAP_INL_HPP

#include "map.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
template <typename ...Args> inline
auto Map<MapClass, Key, T, Compare>::add(Args&&... args) -> std::optional<size_type>
{
  auto& map = ref();
  return map.add(std::forward<Args>(args)...);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::capacity() const noexcept -> size_type
{
  auto& map = ref();
  return map.capacity();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto Map<MapClass, Key, T, Compare>::capacityMax() noexcept -> size_type
{
  return MapT::capacityMax();
}

/*!
  \details No detailed description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void Map<MapClass, Key, T, Compare>::clear() noexcept
{
  auto& map = ref();
  map.clear();
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::contain(ConstKeyT& key) const noexcept
    -> std::optional<size_type>
{
  auto& map = ref();
  return map.contain(key);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::findMinKey() noexcept -> std::optional<Pointer>
{
  auto& map = ref();
  return map.findMinKey();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::get(const size_type index) noexcept
    -> Reference
{
  auto& map = ref();
  return map.get(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::get(const size_type index) const noexcept
    -> ConstReference
{
  auto& map = ref();
  return map.get(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::findMinKey() const noexcept
    -> std::optional<ConstPointer>
{
  auto& map = ref();
  return map.findMinKey();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr bool Map<MapClass, Key, T, Compare>::isBounded() noexcept
{
  return MapT::isBounded();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr bool Map<MapClass, Key, T, Compare>::isConcurrent() noexcept
{
  return MapT::isConcurrent();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool Map<MapClass, Key, T, Compare>::isEmpty() const noexcept
{
  const bool result = size() == 0;
  return result;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::remove(ConstKeyT& key) -> std::optional<size_type>
{
  auto& map = ref();
  return map.remove(key);
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void Map<MapClass, Key, T, Compare>::setCapacity(const size_type cap) noexcept
{
  auto& map = ref();
  map.setCapacity(cap);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::size() const noexcept -> size_type
{
  auto& map = ref();
  return map.size();
}

/*!
  \details No detailed description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
Map<MapClass, Key, T, Compare>::Map() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
Map<MapClass, Key, T, Compare>::Map([[maybe_unused]] const Map& other) noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::operator=([[maybe_unused]] const Map& other) noexcept
    -> Map&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::getKey(ConstReference value) noexcept
    -> ConstKeyT&
{
  if constexpr (std::is_void_v<T>)
    return value;
  else
    return std::get<0>(value);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::ref() noexcept -> MapReference
{
  auto* q = static_cast<MapPtr>(this);
  return *q;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename MapClass,
          std::movable Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto Map<MapClass, Key, T, Compare>::ref() const noexcept -> ConstMapReference
{
  const auto* q = static_cast<ConstMapPtr>(this);
  return *q;
}

} // namespace zisc

#endif // ZISC_MAP_INL_HPP
