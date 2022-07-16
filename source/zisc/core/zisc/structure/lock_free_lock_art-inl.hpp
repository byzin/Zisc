/*!
  \file lock_free_lock_art-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_LOCK_ART_INL_HPP
#define ZISC_LOCK_FREE_LOCK_ART_INL_HPP

#include "lock_free_lock_art.hpp"
// Standard C++ library
#include <array>
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
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"
#include "zisc/internal/flock/epoch.hpp"
#include "zisc/internal/flock/lock.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
LockFreeLockArt<Key, T, Compare>::LockFreeLockArt(
    const std::span<std::thread::id> thread_id_list,
    pmr::memory_resource* mem_resource) noexcept :
        LockFreeLockArt(thread_id_list, 1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
LockFreeLockArt<Key, T, Compare>::LockFreeLockArt(
    const std::span<std::thread::id> thread_id_list,
    const size_type cap,
    pmr::memory_resource* mem_resource) noexcept :
        full_pool_{mem_resource},
        indirect_pool_{mem_resource},
        sparse_pool_{mem_resource},
        leaf_pool_{mem_resource}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
LockFreeLockArt<Key, T, Compare>::LockFreeLockArt(LockFreeLockArt&& other) noexcept :
    full_pool_{std::move(other.full_pool_)},
    indirect_pool_{std::move(other.indirect_pool_)},
    sparse_pool_{std::move(other.sparse_pool_)},
    leaf_pool_{std::move(other.leaf_pool_)}
{
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
LockFreeLockArt<Key, T, Compare>::~LockFreeLockArt() noexcept
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
auto LockFreeLockArt<Key, T, Compare>::operator=(LockFreeLockArt&& other) noexcept -> LockFreeLockArt&
{
  clear();
  BaseMapT::operator=(std::move(other));
  full_pool_ = std::move(other.full_pool_);
  indirect_pool_ = std::move(other.indirect_pool_);
  sparse_pool_ = std::move(other.sparse_pool_);
  leaf_pool_ = std::move(other.leaf_pool_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
template <typename ...Args> inline
auto LockFreeLockArt<Key, T, Compare>::add(Args&&... args) -> std::optional<size_type>
    requires std::is_nothrow_constructible_v<ValueT, Args...>
{
  ValueT value{std::forward<Args>(args)...};
  ConstKeyT& key = BaseMapT::getKey(value);

  auto thank = [this, key, v = std::move(value)]() -> bool
  {
    NodePtrT root = nullptr;
    bool result = false;
    while (true) {
      auto [gp, p, cptr, c, byte_pos] = findLocation(root, key);
      if ((c != nullptr) && c->isLeafNode() && (c->byte_num_ == byte_pos)) {
        result = false; // already in the tree
        break;
      }
      if (cptr != nullptr) { // child pointer exists
        auto thank = [this, p, cptr, c, &v]() -> bool
        {
          // exit and retry if state has changed
          if (p->is_removed_.load() || (cptr->load() != c))
            return false;

          // create new leaf
          NodePtrT new_l = leaf_pool_.newObj(std::move(v));

          // Fill a null pointer with the new leaf
          if (c == nullptr) {
            *cptr = new_l;
          }
          else { // split an existing leaf into a sparse node with two children
            *cptr = sparse_pool_.newObj(byte_pos, c, c->key_, new_l, key);
          }
          return true;
        };
        if (p->lock().tryLock(std::move(thank))) {
          result = true;
          break;
        }
      }
      else if (addChild(gp, p, v)) { // no child pointer, need to add
        result = true;
        break;
      }
    }
    return result;
  };
  return epoch_.with(std::move(thank));
}

//template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
//auto LockFreeLockArt<Key, T, Compare>::capacity() const noexcept -> size_type
//{
//}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr auto LockFreeLockArt<Key, T, Compare>::capacityMax() noexcept -> size_type
{
  const size_type cap = (std::numeric_limits<size_type>::max)() >> 1;
  return cap;
}

/*!
  \details No detailed description
  */
//template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
//inline
//void LockFreeLockArt<Key, T, Compare>::clear() noexcept
//{
//  // Skip clear operation after moving data to ather 
//  if (node_pool_.empty())
//    return;
//
//  std::for_each(node_list_.begin(), node_list_.end(), [](StoragePtr ptr) noexcept
//  {
//    ptr->destroy();
//  });
//  node_list_.clear();
//  std::iota(index_stack_.rbegin(), index_stack_.rend(), 0);
//}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto LockFreeLockArt<Key, T, Compare>::contain(ConstKeyT& key) const noexcept
    -> std::optional<size_type>
{
  auto thank = [this, key]()
  {
    NodePtrT root = nullptr;
    auto [gp, p, cptr, l, pos] = findLocation(root, key);
    if (cptr != nullptr)
      cptr->validate();
    auto* leaf = static_cast<LeafNode*>(l);
    if (leaf != nullptr)
      ---
  };
  return epoch_.with(std::move(thank));
}

/*!
  \details No detailed description

  \return No description
  */
//template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
//inline
//auto LockFreeLockArt<Key, T, Compare>::findMinKey() noexcept -> std::optional<Pointer>
//{
//  const pmr::vector<StoragePtr>& list = node_list_;
//  std::shared_lock lock{mutex_};
//  ZISC_ASSERT(std::is_sorted(list.begin(), list.end(), LockFreeLockArt::compareNode),
//              "The node list isn't sorted.");
//  return !list.empty()
//      ? std::make_optional(node_list_.front()->memory())
//      : std::optional<Pointer>{};
//}

/*!
  \details No detailed description

  \return No description
  */
//template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
//inline
//auto LockFreeLockArt<Key, T, Compare>::findMinKey() const noexcept -> std::optional<ConstPointer>
//{
//  const pmr::vector<StoragePtr>& list = node_list_;
//  std::shared_lock lock{mutex_};
//  ZISC_ASSERT(std::is_sorted(list.begin(), list.end(), LockFreeLockArt::compareNode),
//              "The node list isn't sorted.");
//  return !list.empty()
//      ? std::make_optional(node_list_.front()->memory())
//      : std::optional<ConstPointer>{};
//}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto LockFreeLockArt<Key, T, Compare>::get(const size_type index) noexcept
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
auto LockFreeLockArt<Key, T, Compare>::get(const size_type index) const noexcept
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
constexpr bool LockFreeLockArt<Key, T, Compare>::isBounded() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
constexpr bool LockFreeLockArt<Key, T, Compare>::isConcurrent() noexcept
{
  return true;
}

/*!
  \details Currently a "lazy" remove that only removes the leaf

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto LockFreeLockArt<Key, T, Compare>::remove(ConstKeyT& key) -> std::optional<size_type>
{
  NodePtrT root = nullptr;

  auto thank = [this, key]()
  {
    bool result = false;
    while (true) {
      auto [gp, p, cptr, c, byte_pos] = findLocation(root, key);
      // If not found return
      if ((c == nullptr) || !(c->isLeafNode() && c->byte_num_ == byte_pos)) {
        result = false;
        break;
      }
      auto thank = [this, p, cptr, c]()
      {
        if (p->is_removed_.load() || (cptr->load() != c))
          return false;
        *cptr = nullptr;
        leaf_pool_.retire(static_cast<LeafNode*>(c));
        return true;
      };
      if (p->lock().tryLock(std::move(thank))) {
        result = true;
        break;
      }
    }
  };
  return epoch_.with(thank);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
pmr::memory_resource* LockFreeLockArt<Key, T, Compare>::resource() const noexcept
{
  pmr::memory_resource* mem_resource = node_pool_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
void LockFreeLockArt<Key, T, Compare>::setCapacity(size_type cap) noexcept
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
auto LockFreeLockArt<Key, T, Compare>::size() const noexcept -> size_type
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

  \param [in] type No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::Node::Node(const NodeType type) noexcept :
    type_{type},
    is_removed_{false}
{
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] type No description.
  \param [in] byte_num No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::Node::Node(ConstKeyT& key,
                                             const NodeType type,
                                             const std::size_t byte_num) noexcept :
    key_{key},
    type_{type},
    is_removed_{false},
    byte_num_{cast<uint16b>(byte_num)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::Node::Node(Node&& other) noexcept :
    key_{std::move(other.key_)},
    type_{other.type_},
    is_removed_{std::move(other.is_removed_)},
    byte_num_{other.byte_num_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::Node::operator=(Node&& other) noexcept -> Node&
{
  key_ = std::move(other.key_);
  type_ = other.type_;
  is_removed_ = std::move(other.is_removed_);
  byte_num_ = other.byte_num_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
std::size_t LockFreeLockArt<Key, T, Compare>::Node::byteNum() const noexcept
{
  return cast<std::size_t>(byte_num_);
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::Node::getChild(ConstKeyT& key) noexcept
    -> NodePtrMutableT*
{
  NodePtrMutableT* child = nullptr;
  switch (type_) {
   case NodeType::kFull: {
    child = static_cast<FullNode*>(this)->getChildImpl(key);
    break;
   }
   case NodeType::kIndirect: {
    child = static_cast<IndirectNode*>(this)->getChildImpl(key);
    break;
   }
   case NodeType::kSparse: {
    child = static_cast<SparseNode*>(this)->getChildImpl(key);
    break;
   }
   default:
    break;
  }
  return child;
}

/*!
  \details No detailed description

  \param [in] node No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::Node::isFull() const noexcept
{
  bool result = false;
  switch (node->type_) {
   case NodeType::kFull: {
    result = static_cast<const FullNode*>(this)->isFullImpl();
    break;
   }
   case NodeType::kIndirect: {
    result = static_cast<const IndirectNode*>(this)->isFullImpl();
    break;
   }
   case NodeType::kSparse: {
    result = static_cast<const SparseNode*>(this)->isFullImpl();
    break;
   }
   case default:
    break;
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::Node::isFullNode() const noexcept
{
  const bool result = type_ == NodeType::kFull;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::Node::isIndirectNode() const noexcept
{
  const bool result = type_ == NodeType::kIndirect;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::Node::isLeafNode() const noexcept
{
  const bool result = type_ == NodeType::kLeaf;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::Node::isSparseNode() const noexcept
{
  const bool result = type_ == NodeType::kSparse;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
flock::Lock& LockFreeLockArt<Key, T, Compare>::Node::lock() noexcept
{
  flock::Lock* lock = nullptr;
  switch (type_) {
   case NodeType::kFull: {
    lock = std::addressof(static_cast<FullNode*>(this)->lock_);
    break;
   }
   case NodeType::kIndirect: {
    lock = std::addressof(static_cast<IndirectNode*>(this)->lock_);
    break;
   }
   case NodeType::kSparse: {
    lock = std::addressof(static_cast<SparseNode*>(this)->lock_);
    break;
   }
   default:
    break;
  }
  return *lock;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
const flock::Lock& LockFreeLockArt<Key, T, Compare>::Node::lock() const noexcept
{
  const flock::Lock* lock = nullptr;
  switch (type_) {
   case NodeType::kFull: {
    lock = std::addressof(static_cast<FullNode*>(this)->lock_);
    break;
   }
   case NodeType::kIndirect: {
    lock = std::addressof(static_cast<IndirectNode*>(this)->lock_);
    break;
   }
   case NodeType::kSparse: {
    lock = std::addressof(static_cast<SparseNode*>(this)->lock_);
    break;
   }
   default:
    break;
  }
  return *lock;
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::FullNode::FullNode() noexcept :
    Node(NodeType::kFull),
    children_{nullptr}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::FullNode::FullNode(FullNode&& other) noexcept :
    Node(std::move(other)),
    lock_{std::move(other.lock_)},
    children_{std::move(other.children_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::FullNode::operator=(FullNode&& other) noexcept
    -> FullNode&
{
  Node::operator=(std::move(other));
  lock_ = std::move(other.lock_);
  children_ = std::move(other.children_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::FullNode::getChildImpl(ConstKeyT& key) noexcept
    -> NodePtrMutableT*
{
  const std::size_t b = getByte(key, byteNum());
  return &children_[b];
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] v No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
void LockFreeLockArt<Key, T, Compare>::FullNode::initChild(ConstKeyT& key,
                                                           NodePtrT v) noexcept
{
  const std::size_t b = getByte(key, byteNum());
  children_[b].init(v);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::FullNode::isFullImpl() const noexcept
{
  return false;
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::IndirectNode::IndirectNode() noexcept :
    Node(NodeType::kIndirect)
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::IndirectNode::IndirectNode(IndirectNode&& other) noexcept :
    Node(std::move(other)),
    lock_{std::move(other.lock_)},
    num_used_{std::move(other.num_used_)},
    index_{std::move(other.index_)},
    ptr_{std::move(other.ptr_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::IndirectNode::operator=(IndirectNode&& other) noexcept
    -> IndirectNode&
{
  Node::operator=(std::move(other));
  lock_ = std::move(other.lock_);
  num_used_ = std::move(other.num_used_);
  index_ = std::move(other.index_);
  ptr_ = std::move(other.ptr_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] v No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::IndirectNode::addChild(ConstKeyT& key,
                                                              NodePtrT v) noexcept
{
  const int i = num_used_.load();
  bool result = false;
  if (i < 64) {
    const std::size_t b = getByte(key, byteNum());
    index_[b] = i;
    ptr_[i] = v;
    num_used_ = i + 1;
    result = true;
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::IndirectNode::getChildImpl(ConstKeyT& key) noexcept
    -> NodePtrMutableT*
{
  const std::size_t b = getByte(key, byteNum());
  const int i = index_[b].load();
  NodePtrMutableT* p = (i == -1) ? nullptr : &ptr_[i];
  return p;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] v No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
void LockFreeLockArt<Key, T, Compare>::IndirectNode::initChild(ConstKeyT& key,
                                                               NodePtrT v) noexcept
{
  const std::size_t b = getByte(key, byteNum());
  const int i = num_used_.load() - 1;
  index_[b] = i;
  ptr_[i].init(v);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::IndirectNode::isFullImpl() const noexcept
{
  const bool result = cast<std::size_t>(num_used_.load()) == ptr_.size();
  return result;
}

/*!
  \details No detailed description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::SparseNode::SparseNode() noexcept :
    Node(NodeType::kSparse)
{
}

template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::SparseNode::SparseNode(const std::size_t byte_num,
                                                         NodePtrT v1,
                                                         ConstKeyT& key1,
                                                         NodePtrT v2,
                                                         ConstKeyT& key2) noexcept :
    Node(key1, NodeType::kSparse, byte_num),
    num_used_{2}
{
  keys_[0] = getByte(key1, byteNum());
  keys_[1] = getByte(key2, byteNum());
  ptr_[0].init(v1);
  ptr_[1].init(v2);
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::SparseNode::SparseNode(SparseNode&& other) noexcept :
    Node(std::move(other)),
    lock_{std::move(other.lock_)},
    num_used_{other.num_used_},
    keys_{std::move(other.keys_)},
    ptr_{std::move(ptr_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::SparseNode::operator=(SparseNode&& other) noexcept
    -> SparseNode&
{
  Node::operator=(std::move(other));
  lock_ = std::move(other.lock_);
  num_used_ = other.num_used_;
  keys_ = std::move(other.keys_);
  ptr_ = std::move(ptr_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::SparseNode::getChildImpl(ConstKeyT& key) noexcept
    -> NodePtrMutableT*
{
  const std::size_t b = getByte(key, byteNum());
  NodePtrMutableT* p = nullptr;
  for (std::size_t i = 0; i < num_used_; ++i) {
    if (keys_[i] == b) {
      p = &ptr_[i];
      break;
    }
  }
  return p;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] v No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
void LockFreeLockArt<Key, T, Compare>::SparseNode::initChild(ConstKeyT& key,
                                                             NodePtrT v) noexcept
{
  const std::size_t b = getByte(key, byteNum());
  keys_[num_used_ - 1] = b;
  ptr_[num_used_ - 1].init(v);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::SparseNode::isFullImpl() const noexcept
{
  const bool result = num_used_ == keys_.size();
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::LeafNode::LeafNode(RReference value) noexcept :
    Node(BaseMapT::getKey(value), NodeType::kLeaf, sizeof(KeyT)),
    value_{std::move(value)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
LockFreeLockArt<Key, T, Compare>::LeafNode::LeafNode(LeafNode&& other) noexcept :
    Node(std::move(other)),
    value_{std::move(other.value_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::LeafNode::operator=(LeafNode&& other) noexcept
    -> LeafNode&
{
  Node::operator=(std::move(other));
  value_ = std::move(other.value_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
template <typename ...Args> inline
auto LockFreeLockArt<Key, T, Compare>::addImpl(Args&&... args) -> std::optional<size_type>
{
}

/*!
  \details This might involve copying p either because p is sparse (can only be copied)
  or because p is indirect but full
  If p is copies, then gp is updated to point to it

  \param [in] gp No description.
  \param [in] p No description.
  \param [in] value No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::addChild(NodePtrT gp,
                                                NodePtrT p,
                                                RReference value) noexcept
{
  bool result = false;
  if (p->isIndirectNode() && !p->isFull()) {
    auto thank = [this, p, &value]() -> bool
    {
      bool result = false;
      if (!p->is_removed_.load()) {
        ConstKeyT key = BaseMapT::getKey(value);
        NodePtrT c = leaf_pool_.newObj(std::move(value));
        auto* parent = static_cast<IndirectNode*>(p);
        result = parent->addChild(key, c);
      }
      return result;
    };
    result = p->lock().tryLock(std::move(thank));
  }
  else {
    auto thank = [this, gp, p, &value]() -> bool
    {
      NodePtrMutableT* x = gp->getChild(p->key_);
      ConstKeyT key = BaseMapT::getKey(value);
      if (gp->is_removed_.load() || (x->load() != p))
        return false;
      return p->lock().tryLock([this, key, p, x, &value]() -> bool
      {
        return addChildImpl(key, p, x, value);
      });
    };
    result = gp->lock().tryLock(std::move(thank));
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] p No description.
  \param [in] x No description.
  \param [in] value No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
bool LockFreeLockArt<Key, T, Compare>::addChildImpl(ConstKeyT& key,
                                                    NodePtrT p,
                                                    NodePtrMutableT* x,
                                                    RReference value) noexcept
{
  NodePtrT c = leaf_pool_.newObj(std::move(v));
  if (p->isIndirectNode()) {
    auto* i_n = static_cast<IndirectNode*>(p);
    i_n->is_removed_ = true;

    // Copy indirect to full
    *x = full_pool_.newInit([key, i_n, c](FullNode* f_n) noexcept
    {
      f_n->key_ = i_n->key_;
      f_n->byte_num_ = i_n->byte_num_;
      for (std::size_t i = 0; i < i_n->index_.size(); ++i) {
        const int j = i_n->index_[i].load();
        if (j != -1)
          f_n->children_[i].init(i_n->ptr_[j].load());
      }
      f_n->initChild(key, c);
    });
    indirect_pool_.retire(i_n);
  }
  else { // p is sparse
    auto* s_n = static_cast<SparseNode*>(p);
    s_n->is_removed_ = true;
    if (p->isFull()) {
      // copy sparse to indirect
      *x = indirect_pool_.newInit([key, s_n, c](IndirectNode* i_n) noexcept
      {
        i_n->key_ = s_n->key_;
        i_n->byte_num_ = s_n->byte_num_;
        i_n->num_used_.init(s_n->keys_.size() + 1);
        for (std::size_t i = 0; i < s_n->keys_.size(); ++i) {
          i_n->index_[s_n->keys_[i]].init(i);
          i_n->ptr_[i].init(s_n->ptr_[i].load());
        }
        i_n->initChild(key, c);
      });
    }
    else {
      // copy sparse to sparse
      *x = sparse_pool_.newInit([key, s_n, c](IndirectNode* s_c) noexcept
      {
        s_c->key_ = s_n->key_;
        s_c->byte_num_ = s_n->byte_num_;
        s_c->num_used_ = s_n->num_used_ + 1;
        for (std::size_t i = 0; i < s_n->num_used_; ++i) {
          s_c->keys_[i] = s_n->keys_[i];
          s_c->ptr_[i].init(s_n->ptr_[i].load());
        }
        s_c->initChild(key, c);
      });
    }
  }
  return true;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
bool LockFreeLockArt<Key, T, Compare>::compare(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept
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
bool LockFreeLockArt<Key, T, Compare>::compareNode(ConstStoragePtr lhs, ConstStoragePtr rhs) noexcept
{
  return LockFreeLockArt::compare(lhs, BaseMapT::getKey(rhs->get()));
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
bool LockFreeLockArt<Key, T, Compare>::equal(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept
{
  ConstKeyT& lhs_key = BaseMapT::getKey(lhs->get());
  const bool result = !CompareT{}(lhs_key, rhs) && !CompareT{}(rhs, lhs_key);
  return result;
}

/*!
  \details No detailed description

  \param [in] root No description.
  \param [in] key No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
auto LockFreeLockArt<Key, T, Compare>::findLocation(NodePtrT root,
                                                    ConstKeyT& key) noexcept
    -> FindQueryResult
{
  std::size_t pos = 0;
  NodePtrT gp = nullptr;
  NodePtrT p = root;
  FindQueryResult result{};
  while (true) {
    NodePtrMutableT* cptr = p->getChild(key);
    if (cptr == nullptr) { // has no child with given key
      result = FindQueryResult{gp, p, nullptr, nullptr, pos};
      break;
    }
    NodePtrT c = cptr->read();
    if (c == nullptr) { // has empty child with given key
      result = FindQueryResult{gp, p, cptr, nullptr, pos};
      break;
    }
    {
      ++pos;
      while ((pos < c->byte_num_) && (getByte(key, pos) == getByte(c->key_, pos)))
        ++pos;
    }
    if ((pos != c->byte_num_) || c->isLeafNode()) { // reached leaf
      result = FindQueryResult{gp, p, cptr, c, pos};
      break;
    }
    gp = p;
    p = c;
  }
  return result;
}

/*!
  \details assumes little endian, and currently specific to integer types
  need to generalize to character or byte strings

  \param [in] key No description.
  \param [in] pos No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare> inline
std::size_t LockFreeLockArt<Key, T, Compare>::getByte(ConstKeyT& key,
                                                      const std::size_t pos) noexcept
{
  constexpr std::size_t mask = 255;
  const std::size_t shift = 8 * sizeof(KeyT) - (1 + pos);
  const std::size_t result = cast<std::size_t>(key >> shift) & mask;
  return result;
}

/*!
  \details No detailed description

  \param [in] node No description.
  \return No description
  */
template <std::movable Key, MappedValue T, std::invocable<Key, Key> Compare>
inline
auto LockFreeLockArt<Key, T, Compare>::getIndex(ConstStoragePtr node) const noexcept -> size_type
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
auto LockFreeLockArt<Key, T, Compare>::getStorage(const size_type index) noexcept
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
auto LockFreeLockArt<Key, T, Compare>::getStorage(const size_type index) const noexcept
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
constexpr auto LockFreeLockArt<Key, T, Compare>::invalidId() noexcept -> size_type
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
auto LockFreeLockArt<Key, T, Compare>::issueStorageIndex() noexcept -> size_type
{
  ZISC_ASSERT(!index_stack_.empty(), "The index stack is empty.");
  const size_type index = index_stack_.back();
  index_stack_.pop_back();
  return index;
}

} // namespace zisc

#endif // ZISC_LOCK_FREE_LOCK_ART_INL_HPP
