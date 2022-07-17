/*!
  \file lock_free_lock_bst-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_LOCK_BST_INL_HPP
#define ZISC_LOCK_FREE_LOCK_BST_INL_HPP

#include "lock_free_lock_bst.hpp"
// Standard C++ library
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory>
#include <numeric>
#include <optional>
#include <span>
#include <thread>
#include <utility>
#include <vector>
// Zisc
#include "container_overflow_error.hpp"
#include "map.hpp"
#include "zisc/boolean.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/internal/flock/epoch.hpp"
#include "zisc/internal/flock/epoch_pool_impl.hpp"
#include "zisc/internal/flock/lock.hpp"
#include "zisc/internal/flock/log.hpp"
#include "zisc/internal/flock/log_array.hpp"
#include "zisc/internal/flock/memory_pool.hpp"
#include "zisc/internal/flock/mutable.hpp"
#include "zisc/internal/flock/write_once.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::LockFreeLockBst(
    const std::span<const std::thread::id> thread_id_list,
    pmr::memory_resource* mem_resource) noexcept :
        LockFreeLockBst(thread_id_list, 1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::LockFreeLockBst(
    const std::span<const std::thread::id> thread_id_list,
    const size_type cap,
    pmr::memory_resource* mem_resource) noexcept :
        timestamp_{
            pmr::allocateUnique(pmr::polymorphic_allocator<TimestampT>{mem_resource})},
        epoch_{
            pmr::allocateUnique(pmr::polymorphic_allocator<flock::Epoch>{mem_resource},
                                thread_id_list,
                                timestamp_.get(),
                                mem_resource)},
        write_announcements_{epoch_->workerInfo(), mem_resource},
        log_list_{
            thread_id_list.size(),
            typename decltype(log_list_)::allocator_type{mem_resource}},
        current_id_list_{
            thread_id_list.size(),
            typename decltype(current_id_list_)::allocator_type{mem_resource}},
        helping_list_{
            thread_id_list.size(),
            typename decltype(helping_list_)::allocator_type{mem_resource}},
        log_array_pool_{epoch_.get(), mem_resource},
        internal_pool_{epoch_.get(), log_list_, mem_resource},
        leaf_pool_{epoch_.get(), log_list_, mem_resource},
        descriptor_pool_{epoch_.get(), log_list_, mem_resource},
        dummy_{
            pmr::allocateUnique(pmr::polymorphic_allocator<DummyNode>{mem_resource},
                                maxKey())},
        root_{
            pmr::allocateUnique(pmr::polymorphic_allocator<InternalNode>{mem_resource},
                                KeyT{0},
                                nullptr,
                                dummy_.get())}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::LockFreeLockBst(LockFreeLockBst&& other) noexcept :
    BaseMapT(std::move(other)),
    timestamp_{std::move(other.timestamp_)},
    epoch_{std::move(other.epoch_)},
    write_announcements_{std::move(other.write_announcements_)},
    log_list_{std::move(other.log_list_)},
    current_id_list_{std::move(other.current_id_list_)},
    helping_list_{std::move(other.helping_list_)},
    log_array_pool_{std::move(other.log_array_pool_)},
    internal_pool_{std::move(other.internal_pool_)},
    leaf_pool_{std::move(other.leaf_pool_)},
    descriptor_pool_{std::move(other.descriptor_pool_)},
    dummy_{std::move(other.dummy_)},
    root_{std::move(other.root_)}
{
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::~LockFreeLockBst() noexcept
{
  clear();
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::operator=(LockFreeLockBst&& other) noexcept -> LockFreeLockBst&
{
  clear();
  BaseMapT::operator=(std::move(other));
  timestamp_ = std::move(other.timestamp_);
  epoch_ = std::move(other.epoch_);
  write_announcements_ = std::move(other.write_announcements_);
  log_list_ = std::move(other.log_list_);
  current_id_list_ = std::move(other.current_id_list_);
  helping_list_ = std::move(other.helping_list_);
  log_array_pool_ = std::move(other.log_array_pool_);
  internal_pool_ = std::move(other.internal_pool_);
  leaf_pool_ = std::move(other.leaf_pool_);
  descriptor_pool_ = std::move(other.descriptor_pool_);
  dummy_ = std::move(other.dummy_);
  root_ = std::move(other.root_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
template <typename ...Args> inline
auto LockFreeLockBst<Key, T, Compare>::add(Args&&... args) -> std::optional<size_type>
    requires std::is_nothrow_constructible_v<ValueT, Args...>
{
  ValueT value{std::forward<Args>(args)...};
  auto thank = [this, &value]() -> std::optional<size_type>
  {
    ConstKeyT& key = BaseMapT::getKey(value);
    std::optional<size_type> result{};
    while (!result.has_value()) {
      FindQueryResult query = findLocation(root(), key);
      if (equal(key, query.c_))
        break;
      auto impl = [this, &value, &query]() -> std::optional<size_type>
      {
        return addImpl(value, query);
      };
      result = tryLock(query.p_, impl);
    }
    return result;
  };
  std::optional<size_type> result = epoch_->with(std::move(thank));
  if (!result.has_value())
    result = invalidId();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::capacity() const noexcept -> size_type
{
  const size_type cap  = leaf_pool_.capacity();
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeLockBst<Key, T, Compare>::capacityMax() noexcept -> size_type
{
  const size_type cap = flock::MemoryPool<LeafNode>::capacityMax();
  return cap;
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
void LockFreeLockBst<Key, T, Compare>::clear() noexcept
{
  // Skip clear operation after moving data to ather 
  if (BaseMapT::isEmpty())
    return;
  removeSubTree(root());
  log_array_pool_.clear();
  internal_pool_.clear();
  leaf_pool_.clear();
  descriptor_pool_.clear();
  initialize();
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::contain(ConstKeyT& key) const noexcept
    -> std::optional<size_type>
{
  auto thunk = [this, &key]() noexcept -> std::optional<size_type>
  {
    flock::Log& log = threadLog();
    ConstNodePtrT c = root()->rightChild(&log);
    while (!c->isLeaf())
      c = compare(key, c) ? c->leftChild(&log) : c->rightChild(&log);
    const std::optional<size_type> index = equal(key, c)
        ? std::make_optional(leaf_pool_.getIndex(*static_cast<const LeafNode*>(c)))
        : std::make_optional(invalidId());
    return index;
  };
  return epoch_->with(std::move(thunk));
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::findMinKey() noexcept
    -> std::optional<Pointer>
{
  auto thunk = [this]() noexcept -> std::optional<size_type>
  {
    flock::Log& log = threadLog();
    ConstNodePtrT c = root()->rightChild(&log);
    while (!c->isLeaf()) {
      ConstNodePtrT l = c->leftChild(&log);
      c = (l != nullptr) ? l : c->rightChild(&log);
    }
    return std::make_optional(leaf_pool_.getIndex(*static_cast<const LeafNode*>(c)));
  };
  const std::optional<size_type> index = epoch_->with(std::move(thunk));
  std::optional<Pointer> result{};
  if (index.has_value() && (*index != invalidId())) {
    const LeafNode& node = leaf_pool_.getObject(*index);
    result = const_cast<Pointer>(&node.value_);
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::findMinKey() const noexcept
    -> std::optional<ConstPointer>
{
  auto thunk = [this]() noexcept -> std::optional<size_type>
  {
    flock::Log& log = threadLog();
    ConstNodePtrT c = root()->rightChild(&log);
    while (!c->isLeaf()) {
      ConstNodePtrT l = c->leftChild(&log);
      c = (l != nullptr) ? l : c->rightChild(&log);
    }
    return std::make_optional(leaf_pool_.getIndex(*static_cast<const LeafNode*>(c)));
  };
  const std::optional<size_type> index = epoch_->with(std::move(thunk));
  std::optional<ConstPointer> result{};
  if (index.has_value() && (*index != invalidId())) {
    const LeafNode& node = leaf_pool_.getObject(*index);
    result = &node.value_;
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto LockFreeLockBst<Key, T, Compare>::get(const size_type index) noexcept
    -> Reference
{
  return leaf_pool_.getObject(index).value_;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto LockFreeLockBst<Key, T, Compare>::get(const size_type index) const noexcept
    -> ConstReference
{
  return leaf_pool_.getObject(index).value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
constexpr bool LockFreeLockBst<Key, T, Compare>::isBounded() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
constexpr bool LockFreeLockBst<Key, T, Compare>::isConcurrent() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::remove(ConstKeyT& key) -> std::optional<size_type>
{
  auto thank = [this, &key]() -> std::optional<size_type>
  {
    NodePtrT prev_leaf = nullptr;
    std::optional<size_type> result{};
    while (!result.has_value()) {
      FindQueryResult query = findLocation(root(), key);
      if (!equal(key, query.c_) || ((prev_leaf != nullptr) && (prev_leaf != query.c_)))
        break;
      prev_leaf = query.c_;
      auto impl = [this, &key, &query]() -> std::optional<size_type>
      {
        auto impl2 = [this, &key, &query]() -> std::optional<size_type>
        {
          return removeImpl(key, query);
        };
        return tryLock(query.p_, impl2);
      };
      result = tryLock(query.gp_, impl);
    }
    return result;
  };
  std::optional<size_type> result = epoch_->with(std::move(thank));
  if (!result.has_value())
    result = invalidId();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
pmr::memory_resource* LockFreeLockBst<Key, T, Compare>::resource() const noexcept
{
  pmr::memory_resource* mem_resource = log_list_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
void LockFreeLockBst<Key, T, Compare>::setCapacity(size_type cap) noexcept
{
  constexpr size_type lowest_size = 1;
  cap = (std::max)(lowest_size, cap);

  clear();
  internal_pool_.reserve(cap);
  leaf_pool_.reserve(cap);
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto LockFreeLockBst<Key, T, Compare>::size() const noexcept -> size_type
{
  size_type s = leaf_pool_.size();
  return s;
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::Node::Node([[maybe_unused]] Node&& other) noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::Node::operator=([[maybe_unused]] Node&& other) noexcept
    -> Node&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::Node::leftChild([[maybe_unused]] flock::Log* log) noexcept
    -> NodePtrT
{
  return nullptr;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::Node::leftChild([[maybe_unused]] flock::Log* log) const noexcept
    -> ConstNodePtrT
{
  return nullptr;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::Node::lock() noexcept -> LockT*
{
  return nullptr;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::Node::lock() const noexcept -> const LockT*
{
  return nullptr;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::Node::rightChild([[maybe_unused]] flock::Log* log) noexcept
    -> NodePtrT
{
  return nullptr;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::Node::rightChild([[maybe_unused]] flock::Log* log) const noexcept
    -> ConstNodePtrT
{
  return nullptr;
}


/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] left No description.
  \param [in] right No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::InternalNode::InternalNode(ConstKeyT& key,
                                                             NodePtrT left,
                                                             NodePtrT right) noexcept :
    left_{left},
    right_{right},
    key_{key},
    is_removed_{false}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::InternalNode::InternalNode(InternalNode&& other) noexcept :
    Node(std::move(other)),
    lock_{std::move(other.lock_)},
    left_{std::move(other.left_)},
    right_{std::move(other.right_)},
    key_{std::move(other.key_)},
    is_removed_{std::move(other.is_removed_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::InternalNode::operator=(InternalNode&& other) noexcept
    -> InternalNode&
{
  Node::operator=(std::move(other));
  lock_ = std::move(other.lock_);
  left_ = std::move(other.left_);
  right_ = std::move(other.right_);
  key_ = std::move(other.key_);
  is_removed_ = std::move(other.is_removed_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockBst<Key, T, Compare>::InternalNode::isLeaf() const noexcept
{
  return false;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::InternalNode::key() const noexcept -> ConstKeyT&
{
  return key_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::InternalNode::leftChild(flock::Log* log) noexcept
    -> NodePtrT
{
  return left_.load(log);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::InternalNode::leftChild(flock::Log* log) const noexcept
    -> ConstNodePtrT
{
  return left_.load(log);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::InternalNode::lock() noexcept
    -> LockT*
{
  return &lock_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::InternalNode::lock() const noexcept
    -> const LockT*
{
  return &lock_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::InternalNode::rightChild(flock::Log* log) noexcept
    -> NodePtrT
{
  return right_.load(log);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::InternalNode::rightChild(flock::Log* log) const noexcept
    -> ConstNodePtrT
{
  return right_.load(log);
}

/*!
  \details No detailed description

  \param [in] key No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::DummyNode::DummyNode(ConstKeyT& key) noexcept :
    key_{key}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockBst<Key, T, Compare>::DummyNode::isLeaf() const noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::DummyNode::key() const noexcept -> ConstKeyT&
{
  return key_;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::LeafNode::LeafNode(RReference value) noexcept :
    value_{std::move(value)}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockBst<Key, T, Compare>::LeafNode::LeafNode(LeafNode&& other) noexcept :
    Node(std::move(other)),
    value_{std::move(other.value_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::LeafNode::operator=(LeafNode&& other) noexcept
    -> LeafNode&
{
  Node::operator=(std::move(other));
  value_ = std::move(other.value_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockBst<Key, T, Compare>::LeafNode::isLeaf() const noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::LeafNode::key() const noexcept -> ConstKeyT&
{
  return BaseMapT::getKey(value_);
}

/*!
  \details No detailed description

  \param [in] value No description.
  \param [in] query No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::addImpl(Reference value, FindQueryResult& query)
    -> std::optional<size_type>
{
  auto* p = static_cast<InternalNode*>(query.p_);
  NodePtrT c = query.c_;
  NodePtrMutableT* ptr = query.is_p_left_ ? &p->left_ : &p->right_;
  flock::Log& log = threadLog();
  NodePtrT l_new = ptr->load(&log);
  std::optional<size_type> result{};
  if (!p->is_removed_.load(&log) && (l_new == c)) {
    ConstKeyT& key = BaseMapT::getKey(value);
    NodePtrT new_l = leaf_pool_.newObj(std::move(value));
    NodePtrT new_p = compare(c->key(), key) ? internal_pool_.newObj(key, c, new_l)
                                            : internal_pool_.newObj(c->key(), new_l, c);
    ptr->store(new_p, &write_announcements_, &log);
    result = leaf_pool_.getIndex(*static_cast<const LeafNode*>(new_l));
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockBst<Key, T, Compare>::compare(ConstKeyT& lhs, ConstKeyT& rhs) noexcept
{
  const bool result = CompareT{}(lhs, rhs);
  return result;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockBst<Key, T, Compare>::compare(ConstKeyT& lhs, ConstNodePtrT rhs) noexcept
{
  return compare(lhs, rhs->key());
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockBst<Key, T, Compare>::equal(ConstKeyT& lhs, ConstKeyT& rhs) noexcept
{
  const bool result = !CompareT{}(lhs, rhs) && !CompareT{}(rhs, lhs);
  return result;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockBst<Key, T, Compare>::equal(ConstKeyT& lhs, ConstNodePtrT rhs) noexcept
{
  return equal(lhs, rhs->key());
}

/*!
  \details No detailed description

  \param [in] root No description.
  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::findLocation(NodePtrT root,
                                                    ConstKeyT& key) const noexcept
    -> FindQueryResult
{
  flock::Log& log = threadLog();
  NodePtrT gp = nullptr;
  NodePtrT p = root;
  NodePtrT c = p->rightChild(&log);
  bool is_gp_left = false;
  bool is_p_left = false;
  while (!c->isLeaf()) {
    gp = p;
    is_gp_left = is_p_left;
    p = c;
    is_p_left = compare(key, p);
    c = is_p_left ? p->leftChild(&log) : p->rightChild(&log);
  }
  return {gp, p, c, is_gp_left, is_p_left, {0}};
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
void LockFreeLockBst<Key, T, Compare>::initialize() noexcept
{
  // Reserve pool
  const std::size_t n = epoch_->workerInfo().numOfWorkers();
  const std::size_t c = n * 1024;
  if (log_array_pool_.capacity() < c)
    log_array_pool_.reserve(c);
  if (descriptor_pool_.capacity() < c)
    descriptor_pool_.reserve(c);

  //
  std::iota(current_id_list_.begin(), current_id_list_.end(), 0);
  std::fill_n(helping_list_.begin(), n, Boolean{});

  //
  *root_ = InternalNode{KeyT{0}, nullptr, dummy_.get()};
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeLockBst<Key, T, Compare>::invalidId() noexcept -> size_type
{
  constexpr size_type invalid = (std::numeric_limits<size_type>::max)();
  return invalid;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeLockBst<Key, T, Compare>::maxKey() noexcept -> KeyT
{
  return (std::numeric_limits<KeyT>::max)();
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] result No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::removeImpl(ConstKeyT& key,
                                                  FindQueryResult& query)
    -> std::optional<size_type>
{
  auto* p = static_cast<InternalNode*>(query.p_);
  auto* gp = static_cast<InternalNode*>(query.gp_);
  NodePtrMutableT* ptr = query.is_gp_left_ ? &gp->left_ : &gp->right_;
  std::optional<size_type> result{};
  flock::Log& log = threadLog();
  if (gp->is_removed_.load(&log) || (ptr->load(&log) != p))
    return result;
  NodePtrT lc = p->leftChild(&log);
  NodePtrT rc = p->rightChild(&log);
  if (query.is_p_left_)
    std::swap(lc, rc);
  if (rc != query.c_)
    return result;
  p->is_removed_ = true;
  ptr->store(lc, &write_announcements_, &log); // shortcut
  internal_pool_.retire(p);
  result = leaf_pool_.getIndex(*static_cast<const LeafNode*>(query.c_));
  leaf_pool_.retire(static_cast<LeafNode*>(query.c_));
  return result;
}

/*!
  \details No detailed description

  \param [in] root No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
void LockFreeLockBst<Key, T, Compare>::removeSubTree(NodePtrT root) noexcept
{
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::root() noexcept -> NodePtrT
{
  return root_.get();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockBst<Key, T, Compare>::root() const noexcept -> ConstNodePtrT
{
  return root_.get();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
flock::Log& LockFreeLockBst<Key, T, Compare>::threadLog() const noexcept
{
  const flock::WorkerInfo& info = epoch_->workerInfo();
  const flock::Log& log = info.takeOut<const flock::Log>(log_list_);
  return *const_cast<flock::Log*>(&log);
}

/*!
  \details No detailed description

  \tparam Thank No description.
  \param [in] node No description.
  \param [in] func No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
template <std::invocable Thank> inline
bool LockFreeLockBst<Key, T, Compare>::tryLock(NodePtrT node, Thank&& func) noexcept
{
  const flock::WorkerInfo& info = epoch_->workerInfo();
  flock::Log& log = info.takeOut<flock::Log>(log_list_);
  std::size_t& current_id = info.takeOut<std::size_t>(current_id_list_);
  Boolean& helping = info.takeOut<Boolean>(helping_list_);
  const bool result = node->lock()->tryLock(std::forward<Thank>(func),
                                            epoch_.get(),
                                            &write_announcements_,
                                            &log,
                                            &current_id,
                                            &helping,
                                            &descriptor_pool_);
  return result;
}

} // namespace zisc

#endif // ZISC_LOCK_FREE_LOCK_BST_INL_HPP
