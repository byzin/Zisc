/*!
  \file mutex_bst-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_BST_INL_HPP
#define ZISC_MUTEX_BST_INL_HPP

#include "mutex_bst.hpp"
// Standard C++ library
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <optional>
#include <shared_mutex>
#include <utility>
#include <vector>
// Zisc
#include "search_tree.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
MutexBst<Key, T, Compare>::MutexBst(pmr::memory_resource* mem_resource) noexcept :
    MutexBst(defaultCapacity(), mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
MutexBst<Key, T, Compare>::MutexBst(const size_type cap,
                                    pmr::memory_resource* mem_resource) noexcept :
    index_stack_{typename decltype(index_stack_)::allocator_type{mem_resource}},
    node_pool_{typename decltype(node_pool_)::allocator_type{mem_resource}},
    node_list_{typename decltype(node_list_)::allocator_type{mem_resource}}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
MutexBst<Key, T, Compare>::MutexBst(MutexBst&& other) noexcept :
    index_stack_{std::move(other.index_stack_)},
    node_pool_{std::move(other.node_pool_)},
    node_list_{std::move(other.node_list_)}
{
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
MutexBst<Key, T, Compare>::~MutexBst() noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::operator=(MutexBst&& other) noexcept -> MutexBst&
{
  index_stack_ = std::move(other.index_stack_);
  node_pool_ = std::move(other.node_pool_);
  node_list_ = std::move(other.node_list_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::add(ConstReference value) -> std::optional<size_type>
{
  ValueT copy = value;
  return add(std::move(copy));
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::add(RReference value) -> std::optional<size_type>
{
  ConstKeyT& key = BaseTreeT::getKey(value);
  size_type node_index = invalidId();
  {
    std::unique_lock lock{mutex_};
    pmr::vector<Pointer>& list = node_list_;
    auto pos = std::lower_bound(list.begin(), list.end(), key, MutexBst::compare);
    if ((pos == list.end()) || !MutexBst::equal(BaseTreeT::getKey(**pos), key)) {
      node_index = index_stack_.back();
      index_stack_.pop_back();
      Pointer ptr = getMemory(node_index);
      ptr = ::new (ptr) ValueT{std::move(value)};
      list.emplace(pos, ptr);
    }
  }
  return (node_index != invalidId())
      ? std::make_optional(node_index)
      : std::optional<size_type>{};
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::capacity() const noexcept -> size_type
{
  size_type cap = 0;
  {
    std::shared_lock lock{mutex_};
    cap = node_pool_.capacity();
  }
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr auto MutexBst<Key, T, Compare>::capacityMax() noexcept -> size_type
{
  const size_type cap = (std::numeric_limits<size_type>::max)() >> 1;
  return cap;
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
void MutexBst<Key, T, Compare>::clear() noexcept
{
  std::for_each(node_list_.begin(), node_list_.end(), std::destroy_at<ValueT>);
  node_list_.clear();
  std::iota(index_stack_.rbegin(), index_stack_.rend(), 0);
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::contain(ConstKeyT& key) const noexcept
    -> std::optional<size_type>
{
  size_type node_index = invalidId();
  {
    std::shared_lock lock{mutex_};
    const pmr::vector<Pointer>& list = node_list_;
    auto pos = std::lower_bound(list.begin(), list.end(), key, MutexBst::compare);
    if ((pos != list.end()) && MutexBst::equal(BaseTreeT::getKey(**pos), key))
      node_index = getIndex(*pos);
  }
  return (node_index != invalidId())
      ? std::make_optional(node_index)
      : std::optional<size_type>();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr auto MutexBst<Key, T, Compare>::defaultCapacity() noexcept -> size_type
{
  const size_type cap = 1024;
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::findMinKey() noexcept -> std::optional<Pointer>
{
  const pmr::vector<Pointer>& list = node_list_;
  {
    std::shared_lock lock{mutex_};
    ZISC_ASSERT(std::is_sorted(list.begin(), list.end(), MutexBst::compareNode),
                "The node list isn't sorted.");
    return !node_list_.empty()
        ? std::make_optional(node_list_.front())
        : std::optional<Pointer>{};
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::findMinKey() const noexcept -> std::optional<ConstPointer>
{
  const pmr::vector<Pointer>& list = node_list_;
  {
    std::shared_lock lock{mutex_};
    ZISC_ASSERT(std::is_sorted(list.begin(), list.end(), MutexBst::compareNode),
                "The node list isn't sorted.");
    return !node_list_.empty()
        ? std::make_optional(node_list_.front())
        : std::optional<ConstPointer>{};
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::get(const size_type index) noexcept
    -> Reference
{
  Reference ref = *getMemory(index);
  return ref;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::get(const size_type index) const noexcept
    -> ConstReference
{
  ConstReference ref = *getMemory(index);
  return ref;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr bool MutexBst<Key, T, Compare>::isBounded() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr bool MutexBst<Key, T, Compare>::isConcurrent() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::remove(ConstKeyT& key) -> std::optional<size_type>
{
  size_type node_index = invalidId();
  {
    std::unique_lock lock{mutex_};
    pmr::vector<Pointer>& list = node_list_;
    auto pos = std::lower_bound(list.begin(), list.end(), key, MutexBst::compare);
    if ((pos != list.end()) && MutexBst::equal(BaseTreeT::getKey(**pos), key)) {
      node_index = getIndex(*pos);
      std::destroy_at<ValueT>(*pos);
      list.erase(pos);
      index_stack_.emplace_back(node_index);
    }
  }
  return (node_index != invalidId())
      ? std::make_optional(node_index)
      : std::optional<size_type>{};
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
void MutexBst<Key, T, Compare>::setCapacity(const size_type cap) noexcept
{
  clear();
  index_stack_.resize(cap);
  node_pool_.resize(index_stack_.size());
  node_list_.reserve(index_stack_.size());
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::size() const noexcept -> size_type
{
  size_type s = 0;
  {
    std::shared_lock lock{mutex_};
    s = node_list_.size();
  }
  return s;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
bool MutexBst<Key, T, Compare>::compare(ConstPointer lhs, ConstKeyT& rhs) noexcept
{
  ConstKeyT& lhs_key = BaseTreeT::getKey(*lhs);
  const bool result = CompareT{}(lhs_key, rhs);
  return result;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
bool MutexBst<Key, T, Compare>::compareNode(ConstPointer lhs, ConstPointer rhs) noexcept
{
  return MutexBst::compare(lhs, BaseTreeT::getKey(*rhs));
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
bool MutexBst<Key, T, Compare>::equal(ConstKeyT& lhs, ConstKeyT& rhs) noexcept
{
  const bool result = !CompareT{}(lhs, rhs) && !CompareT{}(rhs, lhs);
  return result;
}

/*!
  \details No detailed description

  \param [in] node No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::getIndex(ConstPointer node) const noexcept -> size_type
{
  const auto index = cast<size_type>(std::distance(getMemory(0), node));
  return index;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::getMemory(const size_type index) noexcept -> Pointer
{
  static_assert(sizeof(ValueT) == sizeof(Memory),
                "The memory size isn't same as the value type.");
  static_assert(std::alignment_of_v<ValueT> == std::alignment_of_v<Memory>,
                "The memory alignment isn't same as the value type.");
  auto* mem = reinterp<Pointer>(&node_pool_[index]);
  return mem;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::getMemory(const size_type index) const noexcept -> ConstPointer
{
  static_assert(sizeof(ValueT) == sizeof(Memory),
                "The memory size isn't same as the value type.");
  static_assert(std::alignment_of_v<ValueT> == std::alignment_of_v<Memory>,
                "The memory alignment isn't same as the value type.");
  const auto* mem = reinterp<ConstPointer>(&node_pool_[index]);
  return mem;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr auto MutexBst<Key, T, Compare>::invalidId() noexcept -> size_type
{
  constexpr size_type invalid = (std::numeric_limits<size_type>::max)();
  return invalid;
}

} // namespace zisc

#endif // ZISC_MUTEX_BST_INL_HPP
