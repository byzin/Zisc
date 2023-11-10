/*!
  \file mutex_bst-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
#include <memory_resource>
#include <mutex>
#include <numeric>
#include <optional>
#include <shared_mutex>
#include <utility>
#include <vector>
// Zisc
#include "container_overflow_error.hpp"
#include "map.hpp"
#include "mutex_bst_iterator.hpp"
#include "zisc/bit.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
MutexBst<Key, T, Compare>::MutexBst(std::pmr::memory_resource* mem_resource) noexcept :
    MutexBst(1, mem_resource)
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
                                    std::pmr::memory_resource* mem_resource) noexcept :
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
    BaseMapT(std::move(other)),
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
  clear();
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
  clear();
  BaseMapT::operator=(std::move(other));
  index_stack_ = std::move(other.index_stack_);
  node_pool_ = std::move(other.node_pool_);
  node_list_ = std::move(other.node_list_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto MutexBst<Key, T, Compare>::begin() noexcept -> Iterator
{
  using PointerPtr = typename Iterator::PointerPtr;
  auto* ptr = reinterp<PointerPtr>(node_list_.data());
  return Iterator{ptr};
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto MutexBst<Key, T, Compare>::cbegin() const noexcept -> ConstIterator
{
  using PointerPtr = typename ConstIterator::PointerPtr;
  const auto* ptr = reinterp<PointerPtr>(node_list_.data());
  return ConstIterator{ptr};
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto MutexBst<Key, T, Compare>::end() noexcept -> Iterator
{
  using PointerPtr = typename Iterator::PointerPtr;
  auto* ptr = reinterp<PointerPtr>(node_list_.data()) + node_list_.size();
  return Iterator{ptr};
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto MutexBst<Key, T, Compare>::cend() const noexcept -> ConstIterator
{
  using PointerPtr = typename ConstIterator::PointerPtr;
  const auto* ptr = reinterp<PointerPtr>(node_list_.data()) + node_list_.size();
  return ConstIterator{ptr};
}

/*!
  \details No detailed description

  \param [in] args No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
template <typename ...Args> inline
auto MutexBst<Key, T, Compare>::add(Args&&... args) -> std::optional<size_type>
{
  ValueT value{std::forward<Args>(args)...};
  ConstKeyT& key = BaseMapT::getKey(value);
  size_type node_index = invalidId();
  {
    std::unique_lock lock{mutex_};
    std::pmr::vector<StoragePtr>& list = node_list_;
    auto pos = std::lower_bound(list.begin(), list.end(), key, MutexBst::compare);
    if ((pos == list.end()) || !MutexBst::equal(*pos, key)) {
      // 
      using OverflowErr = typename BaseMapT::OverflowError;
      if (index_stack_.empty()) {
        const char* message = "Bst overflow happened.";
        throw OverflowErr{message, resource(), std::move(value)};
      }
      //
      node_index = issueStorageIndex();
      StorageRef storage = getStorage(node_index);
      storage.set(std::move(value));
      list.emplace(pos, std::addressof(storage));
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
  const size_type cap  = node_pool_.size();
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
  // Skip clear operation after moving data to ather 
  if (node_pool_.empty())
    return;

  std::for_each(node_list_.begin(), node_list_.end(), [](StoragePtr ptr) noexcept
  {
    ptr->destroy();
  });
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
    const std::pmr::vector<StoragePtr>& list = node_list_;
    auto pos = std::lower_bound(list.begin(), list.end(), key, MutexBst::compare);
    if ((pos != list.end()) && MutexBst::equal(*pos, key))
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
auto MutexBst<Key, T, Compare>::findMinKey() noexcept -> std::optional<Pointer>
{
  const std::pmr::vector<StoragePtr>& list = node_list_;
  std::shared_lock lock{mutex_};
  ZISC_ASSERT(std::is_sorted(list.begin(), list.end(), MutexBst::compareNode),
              "The node list isn't sorted.");
  return !list.empty()
      ? std::make_optional(node_list_.front()->memory())
      : std::optional<Pointer>{};
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::findMinKey() const noexcept -> std::optional<ConstPointer>
{
  const std::pmr::vector<StoragePtr>& list = node_list_;
  std::shared_lock lock{mutex_};
  ZISC_ASSERT(std::is_sorted(list.begin(), list.end(), MutexBst::compareNode),
              "The node list isn't sorted.");
  return !list.empty()
      ? std::make_optional(node_list_.front()->memory())
      : std::optional<ConstPointer>{};
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
  return *getStorage(index);
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
  return *getStorage(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr auto MutexBst<Key, T, Compare>::isBounded() noexcept -> bool
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr auto MutexBst<Key, T, Compare>::isConcurrent() noexcept -> bool
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
    std::pmr::vector<StoragePtr>& list = node_list_;
    auto pos = std::lower_bound(list.begin(), list.end(), key, MutexBst::compare);
    if ((pos != list.end()) && MutexBst::equal(*pos, key)) {
      node_index = getIndex(*pos);
      (*pos)->destroy();
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

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::resource() const noexcept -> std::pmr::memory_resource*
{
  std::pmr::memory_resource* mem_resource = node_pool_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
void MutexBst<Key, T, Compare>::setCapacity(size_type cap) noexcept
{
  constexpr size_type lowest_size = 1;
  cap = (std::max)(lowest_size, cap);

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
auto MutexBst<Key, T, Compare>::compare(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept -> bool
{
  ConstKeyT& lhs_key = BaseMapT::getKey(lhs->get());
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
auto MutexBst<Key, T, Compare>::compareNode(ConstStoragePtr lhs, ConstStoragePtr rhs) noexcept -> bool
{
  return MutexBst::compare(lhs, BaseMapT::getKey(rhs->get()));
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::equal(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept -> bool
{
  ConstKeyT& lhs_key = BaseMapT::getKey(lhs->get());
  const bool result = !CompareT{}(lhs_key, rhs) && !CompareT{}(rhs, lhs_key);
  return result;
}

/*!
  \details No detailed description

  \param [in] node No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::getIndex(ConstStoragePtr node) const noexcept -> size_type
{
  ConstStoragePtr head = node_pool_.data();
  const auto index = cast<size_type>(std::distance(head, node));
  return index;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::getStorage(const size_type index) noexcept
    -> StorageRef
{
  return node_pool_[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::getStorage(const size_type index) const noexcept
    -> ConstStorageRef
{
  return node_pool_[index];
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

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto MutexBst<Key, T, Compare>::issueStorageIndex() noexcept -> size_type
{
  ZISC_ASSERT(!index_stack_.empty(), "The index stack is empty.");
  const size_type index = index_stack_.back();
  index_stack_.pop_back();
  return index;
}

} // namespace zisc

#endif // ZISC_MUTEX_BST_INL_HPP
