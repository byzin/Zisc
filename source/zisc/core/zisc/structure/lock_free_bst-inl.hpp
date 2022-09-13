/*!
  \file lock_free_bst-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_BST_INL_HPP
#define ZISC_LOCK_FREE_BST_INL_HPP

#include "lock_free_bst.hpp"
// Standard C++ library
#include <array>
#include <atomic>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "container_overflow_error.hpp"
#include "map.hpp"
#include "zisc/algorithm.hpp"
#include "zisc/error.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::LockFreeBst(pmr::memory_resource* mem_resource) noexcept :
    LockFreeBst(1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::LockFreeBst(const size_type cap,
                                          pmr::memory_resource* mem_resource) noexcept :
    node_pool_{typename decltype(node_pool_)::allocator_type{mem_resource}},
    index_pool_{mem_resource}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::LockFreeBst(LockFreeBst&& other) noexcept :
    node_pool_{std::move(other.node_pool_)},
    root_{std::move(other.root_)},
    index_pool_{std::move(other.index_pool_)}
{
}

/*!
  \details No detailed description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::~LockFreeBst() noexcept
{
  clear();
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::operator=(LockFreeBst&& other) noexcept
    -> LockFreeBst&
{
  clear();
  node_pool_ = std::move(other.node_pool_);
  root_ = std::move(other.root_);
  index_pool_ = std::move(other.index_pool_);
  return *this;
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare>
template <typename ...Args> inline
auto LockFreeBst<Key, T, Compare>::add(Args&&... args) -> std::optional<size_type>
{
  NodePtr node = nullptr;
  ValueT value{std::forward<Args>(args)...};

  NodeInfo current{NodeInfo::root0Index()};
  NodeInfo prev{NodeInfo::root1Index()};
  size_type node_index = invalidId();
  while (true) {
    const double k =(node != nullptr)
        ? node->key(isRoot(node))
        : static_cast<double>(BaseMapT::getKey(value));
    const size_type dir = locate(prev, current, k);
    if ((dir == 2) || (dir == invalidId())) [[unlikely]] {
      if (node != nullptr)
        returnNodeIndex(getIndex(node));
      break;
    }

    if (node == nullptr) {
      const size_type index = issueNodeIndex();
      using OverflowErr = typename BaseMapT::OverflowError;
      if (index == RingBuffer::overflowIndex()) {
        const char* message = "Bst overflow happened.";
        throw OverflowErr{message, resource(), std::move(value)};
      }
      node = getNode(index);
      node->setValue(std::move(value));
      node->setLeftChild(NodeInfo{index, false, false, true});
    }

    NodeInfo r = getNode(current)->childP(dir);
    node->setRightChild(NodeInfo{r.index(), false, false, true});
    node->setBackLink(current);

    NodeInfo new_value{r.index(), false, false, true};
    const NodeInfo replacement{getIndex(node), false, false, false};
    const bool result = cas(&getNode(current)->child(dir), new_value, replacement);
    if (result) {
      node_index = getIndex(node);
      break;
    }

    // For helping
    const NodeInfo new_r = getNode(current)->childP(dir);
    if (new_r.index() == r.index()) {
      NodeInfo new_current = prev;
      if (new_r.isMarked())
        cleanMark(current, dir);
      else if (new_r.flag())
        cleanFlag(current, r, prev, true);
      current = new_current;
      prev = getNode(new_current)->backLinkP();
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
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::capacity() const noexcept -> size_type
{
  return node_pool_.capacity();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::capacityMax() noexcept -> size_type
{
  return NodeInfo::indexMax() + 1;
}

/*!
  \details No detailed description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::clear() noexcept
{
  // Skip clear operation after moving data to other
  if (node_pool_.empty())
    return;

  //! \todo destroy elements

  index_pool_.full();
  initialize();
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::contain(ConstKeyT& key) const noexcept
    -> std::optional<size_type>
{
  NodeInfo current{NodeInfo::root0Index()};
  NodeInfo prev{NodeInfo::root1Index()};
  const size_type dir = locate(prev, current, static_cast<double>(key));
  return (dir == static_cast<size_type>(ComparisonResult::kIsEqual))
      ? std::make_optional(current.index())
      : std::optional<size_type>{};
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::findMinKey() noexcept
    -> std::optional<Pointer>
{
  NodeInfo current{NodeInfo::root0Index()};
  NodeInfo prev{NodeInfo::root1Index()};

  size_type node_index = invalidId();
  const size_type dir = findMinKeyImpl(prev, current);
  if (dir != invalidId()) [[likely]] {
    NodeInfo next = getNode(current)->childP(dir);
    if (!isRoot(next))
      node_index = next.index();
  }
  return (node_index != invalidId())
      ? std::make_optional(&getNode(node_index)->value())
      : std::optional<Pointer>{};
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::findMinKey() const noexcept
    -> std::optional<ConstPointer>
{
  NodeInfo current{NodeInfo::root0Index()};
  NodeInfo prev{NodeInfo::root1Index()};

  size_type node_index = invalidId();
  const size_type dir = findMinKeyImpl(prev, current);
  if (dir != invalidId()) [[likely]] {
    NodeInfo next = getNode(current)->childP(dir);
    if (!isRoot(next))
      node_index = next.index();
  }
  return (node_index != invalidId())
      ? std::make_optional(&getNode(node_index)->value())
      : std::optional<ConstPointer>{};
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::get(const size_type index) noexcept
    -> Reference
{
  NodePtr node = getNode(index);
  return node->value();
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::get(const size_type index) const noexcept
    -> ConstReference
{
  ConstNodePtr node = getNode(index);
  return node->value();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr bool LockFreeBst<Key, T, Compare>::isBounded() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr bool LockFreeBst<Key, T, Compare>::isConcurrent() noexcept
{
  return true;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::remove(ConstKeyT& key) -> std::optional<size_type>
{
  NodeInfo current{NodeInfo::root0Index()};
  NodeInfo prev{NodeInfo::root1Index()};

  size_type node_index = invalidId();
  const size_type dir = locate(prev, current, keyMinusUlp(key));
  if (dir != invalidId()) [[likely]] {
    NodeInfo next = getNode(current)->childP(dir);
    if (equal(static_cast<double>(key), getNode(next)->key(isRoot(next)))) {
      const size_type next_index = next.index();
      const bool result = tryFlag(current, next, prev, true);
      if (result)
        node_index = next_index;
      const NodeInfo current_child = getNode(current)->childP(dir);
      if (current_child.index() == next.index())
        cleanFlag(current, next, prev, true);
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
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
pmr::memory_resource* LockFreeBst<Key, T, Compare>::resource() const noexcept
{
  auto alloc = node_pool_.get_allocator();
  return alloc.resource();
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::setCapacity(size_type cap) noexcept
{
  constexpr size_type lowest_size = 1;
  cap = (std::max)(lowest_size, cap);

  const size_type cap_pow2 = std::bit_ceil(cap);
  if ((capacity() < cap_pow2) && (cap_pow2 <= capacityMax())) {
    clear();
    node_pool_.clear();
    node_pool_.resize(cap_pow2);
    index_pool_.setSize(cap_pow2 << 1);
  }
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::size() const noexcept -> size_type
{
  const size_type d = index_pool_.distance();
  const size_type s = index_pool_.size() >> 1;
  return s - d;
}

/*!
  \details No detailed description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::NodeInfo::NodeInfo() noexcept :
    info_{invalidIndex()}
{
}

/*!
  \details No detailed description

  \param [in] index No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::NodeInfo::NodeInfo(const size_type index) noexcept :
    NodeInfo(index, false, false, false)
{
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] flag No description.
  \param [in] is_marked No description.
  \param [in] is_threaded No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::NodeInfo::NodeInfo(const size_type index,
                                                 const bool flag,
                                                 const bool is_marked,
                                                 const bool is_threaded) noexcept :
    info_{create(index, flag, is_marked, is_threaded)}
{
}

/*!
  \details No detailed description

  \param [in] info No description.
  \param [in] flag No description.
  \param [in] is_marked No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::NodeInfo::NodeInfo(const NodeInfo info,
                                                 const bool flag,
                                                 const bool is_marked) noexcept :
    NodeInfo(info.index(), flag, is_marked, info.isThreaded())
{
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::NodeInfo::flag() const noexcept
{
  const size_type v = flagMask() & info_;
  return v != 0;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::NodeInfo::index() const noexcept -> size_type
{
  const size_type v = indexMask() & info_;
  return v;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::NodeInfo::indexMax() noexcept -> size_type
{
  const size_type v = invalidIndex() - 3;
  return v;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::NodeInfo::invalidIndex() noexcept
    -> size_type
{
  size_type v = (std::numeric_limits<size_type>::max)();
  v = v >> 3;
  return v;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::NodeInfo::isInvalid() const noexcept
{
  const size_type i = index();
  return i == invalidIndex();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::NodeInfo::isMarked() const noexcept
{
  const size_type v = markMask() & info_;
  return v != 0;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::NodeInfo::isRoot() const noexcept
{
  const size_type i = index();
  return (i == root0Index()) || (i == root1Index());
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::NodeInfo::isThreaded() const noexcept
{
  const size_type v = threadMask() & info_;
  return v != 0;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::NodeInfo::root0Index() noexcept
    -> size_type
{
  const size_type index = indexMax() + 1;
  return index;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::NodeInfo::root1Index() noexcept
    -> size_type
{
  const size_type index = root0Index() + 1;
  return index;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] flag No description.
  \param [in] is_marked No description.
  \param [in] is_threaded No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::NodeInfo::create(const size_type index,
                                                    const bool flag,
                                                    const bool is_marked,
                                                    const bool is_threaded) noexcept
    -> size_type
{
  [[maybe_unused]] constexpr size_type zero = 0;
  ZISC_ASSERT(isInBounds(index, zero, invalidIndex()+1), "The index is out of range.");
  const size_type p = indexMask() & index;
  constexpr size_type shift = std::bit_width(indexMask());
  const size_type bits = (flag ? 0b001 : 0) |
                         (is_marked ? 0b010 : 0) |
                         (is_threaded ? 0b100 : 0);
  const size_type v = static_cast<size_type>(bits << shift) | p;
  return v;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::NodeInfo::flagMask() noexcept
    -> size_type
{
  const size_type mask = 0b01ull << 61;
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::NodeInfo::markMask() noexcept
    -> size_type
{
  const size_type mask = 0b01ull << 62;
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::NodeInfo::indexMask() noexcept
    -> size_type
{
  const size_type mask = (std::numeric_limits<size_type>::max)() >> 3;
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::NodeInfo::threadMask() noexcept
    -> size_type
{
  const size_type mask = 0b01ull << 63;
  return mask;
}

/*!
  \details No detailed description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::Node::Node() noexcept :
    key_{std::numeric_limits<double>::infinity()}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
LockFreeBst<Key, T, Compare>::Node::Node(Node&& other) noexcept :
    child_{other.leftChildP(), other.rightChildP()},
    back_link_{other.backLinkP()},
    pre_link_{other.preLink()},
    key_{other.key_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::operator=(Node&& other) noexcept -> Node&
{
  child_[0].store(other.leftChildP(), std::memory_order::release);
  child_[1].store(other.rightChildP(), std::memory_order::release);
  back_link_.store(other.backLinkP(), std::memory_order::release);
  pre_link_ = other.preLink();
  key_ = other.key_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::backLink() noexcept
    -> std::atomic<NodeInfo>&
{
  return back_link_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::backLink() const noexcept
    -> const std::atomic<NodeInfo>&
{
  return back_link_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::backLinkP() const noexcept -> NodeInfo
{
  return backLink().load(std::memory_order::acquire);
}

/*!
  \details No detailed description

  \param [in] dir No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::child(const size_type dir) noexcept
    -> std::atomic<NodeInfo>&
{
  ZISC_ASSERT(dir <= 1, "The dir is out of range.");
  return child_[dir];
}

/*!
  \details No detailed description

  \param [in] dir No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::child(const ComparisonResult dir) noexcept
    -> std::atomic<NodeInfo>&
{
  return child(static_cast<size_type>(dir));
}

/*!
  \details No detailed description

  \param [in] dir No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::child(const size_type dir) const noexcept
    -> const std::atomic<NodeInfo>&
{
  ZISC_ASSERT(dir <= 1, "The dir is out of range.");
  return child_[dir];
}

/*!
  \details No detailed description

  \param [in] dir No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::child(const ComparisonResult dir) const noexcept
    -> const std::atomic<NodeInfo>&
{
  return child(static_cast<size_type>(dir));
}

/*!
  \details No detailed description

  \param [in] dir No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::childP(const size_type dir) const noexcept
    -> NodeInfo
{
  ZISC_ASSERT(dir <= 1, "The dir is out of range.");
  const NodeInfo info = child_[dir].load(std::memory_order::acquire);
  return info;
}

/*!
  \details No detailed description

  \param [in] dir No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::childP(const ComparisonResult dir) const noexcept
    -> NodeInfo
{
  return childP(static_cast<size_type>(dir));
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
double LockFreeBst<Key, T, Compare>::Node::key(const bool is_root) const noexcept
{
  const double k = is_root ? key_ : BaseMapT::getKey(*value_);
  return k;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::leftChild() noexcept
    -> std::atomic<NodeInfo>&
{
  return child(0);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::leftChild() const noexcept
    -> const std::atomic<NodeInfo>&
{
  return child(0);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::leftChildP() const noexcept -> NodeInfo
{
  return childP(0);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::preLink() const noexcept -> NodeInfo
{
  return pre_link_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::rightChild() noexcept
    -> std::atomic<NodeInfo>&
{
  return child(1);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::rightChild() const noexcept
    -> const std::atomic<NodeInfo>&
{
  return child(1);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::rightChildP() const noexcept -> NodeInfo
{
  return childP(1);
}

/*!
  \details No detailed description

  \param [in] link No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::Node::setBackLink(const NodeInfo info) noexcept
{
  back_link_.store(info, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] link No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::Node::setChild(const size_type dir,
                                                  const NodeInfo info) noexcept
{
  child(dir).store(info, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] link No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::Node::setChild(const ComparisonResult dir,
                                                  const NodeInfo info) noexcept
{
  setChild(static_cast<size_type>(dir), info);
}

/*!
  \details No detailed description

  \param [in] key No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::Node::setKey(const double key) noexcept
{
  key_ = key;
}

/*!
  \details No detailed description

  \param [in] link No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::Node::setLeftChild(const NodeInfo info) noexcept
{
  leftChild().store(info, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] link No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::Node::setPreLink(const NodeInfo info) noexcept
{
  pre_link_ = info;
}

/*!
  \details No detailed description

  \param [in] link No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::Node::setRightChild(const NodeInfo info) noexcept
{
  rightChild().store(info, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::Node::setValue(RReference value) noexcept
{
  value_.set(std::move(value));
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::value() noexcept -> Reference
{
  return *value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::Node::value() const noexcept -> ConstReference
{
  return *value_;
}

/*!
  \details No detailed description

  \param [in,out] old_value No description.
  \param [in] new_value No description.
  \param [in] replacement No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::cas(std::atomic<NodeInfo>* old_value,
                                       NodeInfo new_value,
                                       const NodeInfo replacement) noexcept
{
  bool result = false;
  for (size_type i = 0; i < numOfCasTrials() && !result; ++i) {
    result = old_value->compare_exchange_strong(new_value,
                                                replacement,
                                                std::memory_order::acq_rel,
                                                std::memory_order::acquire);
  }
  return result;
}

/*!
  \details No detailed description

  \param [in,out] prev No description.
  \param [in,out] current No description.
  \param [in,out] back No description.
  \param [in] is_threaded No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::cleanFlag(NodeInfo& prev,
                                             NodeInfo& current,
                                             NodeInfo& back,
                                             const bool is_threaded) noexcept
{
  if (is_threaded)
    cleanFlagThreaded(prev, current, back);
  else
    cleanFlagNoThreaded(prev, current, back);
}

/*!
  \details No detailed description

  \param [in,out] prev No description.
  \param [in,out] current No description.
  \param [in,out] back No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::cleanFlagNoThreaded(NodeInfo& prev,
                                                       NodeInfo& current,
                                                       [[maybe_unused]] NodeInfo& back) noexcept
{
  NodeInfo right = getNode(current)->rightChildP();
  if (right.isMarked()) {
    const NodeInfo left = getNode(current)->leftChildP();
    NodeInfo pre_node = getNode(current)->preLink();
    if (left.index() != pre_node.index()) { // This is cat 3 node
      tryMark(current, 0);
      cleanMark(current, 0);
    }
    else {
      const ComparisonResult p_dir = compare(getNode(current)->key(isRoot(current)),
                                             getNode(prev)->key(isRoot(prev)));
      if (left.index() == current.index()) { // cat 1 node
        NodeInfo new_value{current.index(), true, false, false};
        const NodeInfo replacement{right, false, false};
        cas(&getNode(prev)->child(p_dir), new_value, replacement);
        if (!right.isThreaded()) {
          NodePtr ref = getNode(right);
          ref->backLink().compare_exchange_weak(current,
                                                prev,
                                                std::memory_order::acq_rel,
                                                std::memory_order::acquire);
        }
      }
      else {
        NodeInfo new_value{current.index(), true, false, true};
        NodeInfo replacement{right, false, false};
        cas(&getNode(pre_node)->rightChild(), new_value, replacement);
        if (!right.isThreaded()) {
          NodePtr ref = getNode(right);
          ref->backLink().compare_exchange_strong(current,
                                                  prev,
                                                  std::memory_order::acq_rel,
                                                  std::memory_order::acquire);
        }
        new_value = NodeInfo{current.index(), true, false, false};
        replacement = NodeInfo{pre_node.index(), false, false, right.isThreaded()};
        cas(&getNode(prev)->child(p_dir), new_value, replacement);
        {
          NodePtr ref = getNode(pre_node);
          ref->backLink().compare_exchange_strong(current,
                                                  prev,
                                                  std::memory_order::acq_rel,
                                                  std::memory_order::acquire);
        }
      }
      // The current is removed
      returnNodeIndex(current.index());
    }
  }
  else if (right.isThreaded() && right.flag()) {
    NodePtr del_node = getNode(right);
    NodeInfo parent = del_node->backLinkP();
    while (true) {
      const ComparisonResult p_dir = compare(del_node->key(isRoot(del_node)),
                                             getNode(parent)->key(isRoot(parent)));
      const NodeInfo temp = getNode(parent)->childP(p_dir);
      if (temp.isMarked()) {
        cleanMark(parent, static_cast<size_type>(p_dir));
      }
      else if (temp.flag()) {
        break;
      }
      else {
        NodeInfo new_value{getIndex(del_node), false, false, false};
        const NodeInfo replacement{getIndex(del_node), true, false, false};
        if (cas(&getNode(parent)->child(p_dir), new_value, replacement))
          break;
      }
    }
    NodeInfo back_node = getNode(parent)->backLinkP();
    cleanFlag(parent, right, back_node, false);
  }
}

/*!
  \details No detailed description

  \param [in,out] prev No description.
  \param [in,out] current No description.
  \param [in,out] back No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::cleanFlagThreaded(NodeInfo& prev,
                                                     NodeInfo& current,
                                                     NodeInfo& back) noexcept
{
  while (true) {
    NodeInfo next = getNode(current)->rightChildP();
    if (next.isMarked())
      break;
    if (next.flag()) {
      if (back.index() == next.index())
        back = getNode(back)->backLinkP();

      NodeInfo back_node = getNode(current)->backLinkP();
      cleanFlag(current, next, back_node, next.isThreaded());

      if (back.index() == next.index()) {
        const auto p_dir = compare(getNode(prev)->key(isRoot(prev)),
                                   getNode(back_node)->key(isRoot(back_node)));
        NodeInfo back_temp = back;
        NodeInfo back_child = getNode(back_temp)->childP(p_dir);
        prev = back_child;
      }
    }
    else {
      NodePtr p = getNode(current);
      if (p->preLink().index() != prev.index()) // step 2: set the prelink
        p->setPreLink(prev);

      // step 3: mark the outgoing right link
      NodeInfo new_value{next, false, false};
      const NodeInfo replacement{next, false, true};
      const bool result = cas(&p->rightChild(), new_value, replacement);
      if (result)
        break;
    }
  }
  cleanMark(current, 1);
}

/*!
  \details No detailed description

  \param [in] current No description.
  \param [in] mark_dir No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::cleanMark(NodeInfo& current,
                                             const size_type mark_dir) noexcept
{
  if (mark_dir == 0)
    cleanMarkLeft(current);
  else
    cleanMarkRight(current);
}

/*!
  \details No detailed description

  \param [in] current No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::cleanMarkLeft(NodeInfo& current) noexcept
{
  const NodeInfo left = getNode(current)->leftChildP();
  NodeInfo right = getNode(current)->rightChildP();
  if (right.isMarked()) {
    NodeInfo pre_node = getNode(current)->preLink();
    tryMark(pre_node, 0);
    cleanMark(pre_node, 0);
  }
  else if (right.isThreaded() && right.flag()) {
    NodeInfo del_node = right;
    NodeInfo del_node_pa = getNode(del_node)->backLinkP();
    NodeInfo pre_parent = getNode(current)->backLinkP();
    const ComparisonResult p_dir = compare(getNode(del_node)->key(isRoot(del_node)),
                                           getNode(del_node_pa)->key(isRoot(del_node_pa)));
    const NodeInfo del_node_l = getNode(del_node)->leftChildP();
    const NodeInfo del_node_r = getNode(del_node)->rightChildP();
    //
    NodeInfo new_value{current.index(), true, false, false};
    NodeInfo replacement{left, left.flag(), false};
    cas(&getNode(pre_parent)->rightChild(), new_value, replacement);
    //
    if (!left.isThreaded()) {
      NodePtr p = getNode(left);
      p->backLink().compare_exchange_weak(current,
                                          pre_parent,
                                          std::memory_order::acq_rel,
                                          std::memory_order::acquire);
    }
    //
    {
      new_value = NodeInfo{left, false, true};
      replacement = NodeInfo{del_node_l.index(), false, false, false};
      cas(&getNode(current)->leftChild(), new_value, replacement);
    }
    //
    {
      NodePtr p = getNode(del_node_l);
      p->backLink().compare_exchange_strong(del_node,
                                            current,
                                            std::memory_order::acq_rel,
                                            std::memory_order::acquire);
    }
    //
    {
      new_value = NodeInfo{right.index(), true, false, true};
      replacement = NodeInfo{del_node_r, false, false};
      cas(&getNode(current)->rightChild(), new_value, replacement);
    }
    //
    if (!del_node_r.isThreaded()) {
      NodePtr p = getNode(del_node_r);
      p->backLink().compare_exchange_strong(del_node,
                                            current,
                                            std::memory_order::acq_rel,
                                            std::memory_order::acquire);
    }
    //
    {
      new_value = NodeInfo{del_node.index(), true, false, false};
      replacement = NodeInfo{current.index(), false, false, false};
      cas(&getNode(del_node_pa)->child(p_dir), new_value, replacement);
    }
    //
    {
      NodePtr p = getNode(current);
      p->backLink().compare_exchange_strong(pre_parent,
                                            del_node_pa,
                                            std::memory_order::acq_rel,
                                            std::memory_order::acquire);
    }
  }
}

/*!
  \details No detailed description

  \param [in] current No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::cleanMarkRight(NodeInfo& current) noexcept
{
  const NodeInfo left = getNode(current)->leftChildP();
  NodePtr del_node = getNode(current);
  while (true) {
    NodeInfo pre_node = del_node->preLink();
    NodeInfo parent = del_node->backLinkP();
    const ComparisonResult p_dir = compare(del_node->key(isRoot(del_node)),
                                           getNode(parent)->key(isRoot(parent)));
    const NodeInfo parent_child = getNode(parent)->childP(p_dir);

    // step 4 for category 1 and 2. Actually step 5: flag the incoming parent link
    if (pre_node.index() == left.index()) { // category 1 or 2 node.
      NodeInfo parent = del_node->backLinkP();
      NodeInfo back = getNode(parent)->backLinkP();
      tryFlag(parent, current, back, false);
      if (parent_child.index() == current.index()) { // If the link still persists
        cleanFlag(parent, current, back, false);
        break;
      }
    }
    else {
      // category 3 node.
      // This step 4 for category 3 node. Step 4: flag the incoming parent link of the predecessor
      NodeInfo pre_parent = getNode(pre_node)->backLinkP();
      const NodeInfo temp = getNode(pre_parent)->rightChildP();
      // because predecessor of cat 3 node is always the right child of it's parent
      NodeInfo back_node = getNode(pre_parent)->backLinkP();

      if (temp.isMarked()) {
        cleanMark(pre_parent, 1);
      }
      else if (temp.flag()) {
        cleanFlag(pre_parent, pre_node, back_node, false);
        break;
      }
      else {
        NodeInfo value{pre_node.index(), false, false, false};
        const NodeInfo rep{pre_node.index(), true, false, false};
        if (cas(&getNode(pre_parent)->child(p_dir), value, rep)) {
          cleanFlag(pre_parent, pre_node, back_node, false);
          break;
        }
      }
    }
  }
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::compare(const double lhs, const double rhs) noexcept
    -> ComparisonResult
{
  using ResultT = ComparisonResult;
  const ResultT dir = (lhs < rhs) ? ResultT::kIsLess :
                      (rhs < lhs) ? ResultT::kIsGreater 
                                  : ResultT::kIsEqual;
  return dir;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::equal(const double lhs, const double rhs) noexcept
{
//  const bool result = !CompareT{}(lhs, rhs) && !CompareT{}(rhs, lhs);
  const bool result = lhs == rhs;
  return result;
}

/*!
  \details No detailed description

  \param [in,out] prev No description.
  \param [in,out] current No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::findMinKeyImpl(NodeInfo& prev,
                                                  NodeInfo& current) const noexcept
    -> size_type
{
  size_type step_count = 0;
  size_type result = invalidId();
  while (true) {
    constexpr size_type max_step_count = 1 << 12;
    if (max_step_count < step_count++)
      break;

    constexpr double inf = -std::numeric_limits<double>::infinity();
    const ComparisonResult dir = equal(inf, getNode(current)->key(isRoot(current)))
        ? ComparisonResult::kIsGreater
        : ComparisonResult::kIsLess;

    NodeInfo r = getNode(current)->childP(dir);
    if (r.isMarked() && (dir == ComparisonResult::kIsGreater)) {
      NodeInfo new_prev = getNode(prev)->backLinkP();
      const_cast<LockFreeBst*>(this)->cleanMark(current, static_cast<size_type>(dir));
      prev = new_prev;
      const ComparisonResult p_dir = equal(inf, getNode(prev)->key(isRoot(prev)))
          ? ComparisonResult::kIsGreater
          : ComparisonResult::kIsLess;
      const NodeInfo temp = getNode(prev)->childP(p_dir);
      current = temp;
      continue;
    }

    if (r.isThreaded()) {
      const ComparisonResult p_dir = equal(inf, getNode(r)->key(isRoot(r)))
          ? ComparisonResult::kIsGreater
          : ComparisonResult::kIsLess;
      if ((dir == ComparisonResult::kIsLess) || (p_dir == ComparisonResult::kIsLess)) {
        result = static_cast<size_type>(dir);
        break;
      }
      else {
        prev = current;
        current = r;
      }
    }
    else {
      prev = current;
      current = r;
    }
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::getIndex(ConstNodePtr node) const noexcept
    -> size_type
{
  const auto d = std::distance(node_pool_.data(), node);
  using DiffT = std::remove_cvref_t<decltype(d)>;
  [[maybe_unused]] constexpr DiffT begin = 0;
  [[maybe_unused]] const auto end = static_cast<DiffT>(node_pool_.size());
  ZISC_ASSERT(isInBounds(d, begin, end), "The node is out of the pool.");

  const auto index = static_cast<size_type>(d);
  return index;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::getNode(const size_type index) noexcept
    -> NodePtr
{
  using I = NodeInfo;
  const bool is_root = (index == I::root0Index()) || (index == I::root1Index());
  [[maybe_unused]] constexpr size_type begin = 0;
  [[maybe_unused]] const size_type end = node_pool_.size();
  ZISC_ASSERT(is_root || isInBounds(index, begin, end), "The index is out of the pool.");

  NodePtr node = is_root
      ? &root_[index - NodeInfo::root0Index()]
      : &node_pool_[index];
  return node;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::getNode(const NodeInfo info) noexcept
    -> NodePtr
{
  return getNode(info.index());
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::getNode(const size_type index) const noexcept
    -> ConstNodePtr
{
  using I = NodeInfo;
  const bool is_root = (index == I::root0Index()) || (index == I::root1Index());
  [[maybe_unused]] constexpr size_type begin = 0;
  [[maybe_unused]] const size_type end = node_pool_.size();
  ZISC_ASSERT(is_root || isInBounds(index, begin, end), "The index is out of the pool.");

  ConstNodePtr node = is_root
      ? &root_[index - NodeInfo::root0Index()]
      : &node_pool_[index];
  return node;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::getNode(const NodeInfo info) const noexcept
    -> ConstNodePtr
{
  return getNode(info.index());
}

/*!
  \details No detailed description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::initialize() noexcept
{
  {
    Node& node = root_[0];
    node.setLeftChild(NodeInfo{NodeInfo::root0Index(), false, false, true});
    node.setRightChild(NodeInfo{NodeInfo::root1Index(), false, false, true});
    node.setBackLink(NodeInfo{NodeInfo::root1Index()});
    node.setPreLink(NodeInfo{});
    node.setKey(-std::numeric_limits<double>::infinity());
  }
  {
    Node& node = root_[1];
    node.setLeftChild(NodeInfo{NodeInfo::root0Index(), false, false, false});
    node.setRightChild(NodeInfo{NodeInfo::invalidIndex(), false, false, true});
    node.setBackLink(NodeInfo{});
    node.setPreLink(NodeInfo{});
    node.setKey(std::numeric_limits<double>::infinity());
  }
}

/*!
  \details No detailed description

  \param [in] info No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::isRoot(const NodeInfo info) const noexcept
{
  return info.isRoot();
}

/*!
  \details No detailed description

  \param [in] info No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::isRoot(ConstNodePtr node) const noexcept
{
  const bool result = (node == &root_[0]) || (node == &root_[1]);
  return result;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
double LockFreeBst<Key, T, Compare>::keyMinusUlp(ConstKeyT& key) noexcept
{
  const auto k = static_cast<double>(key);
  ZISC_ASSERT(std::isfinite(k), "The key isn't finite.");
//  constexpr double eps = std::numeric_limits<double>::epsilon();
  constexpr double eps = 0.5;
  const double result = k - eps;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::issueNodeIndex() noexcept -> size_type
{
  const size_type index = index_pool_.dequeue(true);
  std::cout << "  issue index: " << index << std::endl;
  return index;
}

/*!
  \details No detailed description

  \param [in,out] prev No description.
  \param [in,out] current No description.
  \param [in] key No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
auto LockFreeBst<Key, T, Compare>::locate(NodeInfo& prev,
                                          NodeInfo& current,
                                          const double key) const noexcept -> size_type
{
  size_type step_count = 0;
  size_type result = invalidId();
  while (true) {
    constexpr size_type max_step_count = 1 << 12;
    if (max_step_count < step_count++)
      break;

    const ComparisonResult dir = compare(key, getNode(current)->key(isRoot(current)));
    if (dir == ComparisonResult::kIsEqual) {
      result = static_cast<size_type>(ComparisonResult::kIsEqual);
      break;
    }

    NodeInfo r = getNode(current)->childP(dir);
    if (r.isMarked() && (dir == ComparisonResult::kIsGreater)) {
      NodeInfo new_prev = getNode(prev)->backLinkP();
      const_cast<LockFreeBst*>(this)->cleanMark(current, static_cast<size_type>(dir));
      prev = new_prev;
      const ComparisonResult p_dir = compare(key, getNode(prev)->key(isRoot(prev)));
      const NodeInfo temp = getNode(prev)->childP(p_dir);
      current = temp;
      continue;
    }

    if (r.isThreaded()) {
      if ((dir == ComparisonResult::kIsLess) ||
          (compare(key, getNode(r)->key(isRoot(r))) == ComparisonResult::kIsLess)) {
        result = static_cast<size_type>(dir);
        break;
      }
      else {
        prev = current;
        current = r;
      }
    }
    else {
      prev = current;
      current = r;
    }
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::numOfCasTrials() noexcept -> size_type
{
  return 4;
}

/*!
  \details No detailed description

  \param [in] index No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::returnNodeIndex(const size_type index) noexcept
{
  std::cout << "  return index: " << index << std::endl;
  index_pool_.enqueue(index, true);
}


/*!
  \details No detailed description

  \param [in,out] prev No description.
  \param [in,out] current No description.
  \param [in,out] back No description.
  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
bool LockFreeBst<Key, T, Compare>::tryFlag(NodeInfo& prev,
                                           NodeInfo& current,
                                           NodeInfo& back,
                                           const bool is_threaded) noexcept
{
  bool flag = false;
  while (true) {
    ComparisonResult p_dir = compare(getNode(current)->key(isRoot(current)),
                                     getNode(prev)->key(isRoot(prev)));
    // current->key() and prev->key() will be same when they are pointing to
    // the same node. This is only possible by the threaded left-link
    p_dir = (p_dir == ComparisonResult::kIsEqual) ? ComparisonResult::kIsLess : p_dir;

    NodeInfo test{current.index(), false, false, is_threaded};
    const NodeInfo replace{current.index(), true, false, is_threaded};
    const bool result = cas(&getNode(prev)->child(p_dir), test, replace);
    if (result) {
      flag = true;
      break;
    }

    NodeInfo temp = getNode(prev)->childP(p_dir);
    if (temp.index() == current.index()) {
      if (temp.flag())
        break;
      else if (temp.isMarked())
        cleanMark(prev, static_cast<size_type>(p_dir));
      prev = back; // back is provided as third parameter.
                   // It helps to step back and restart
      const auto new_p_dir = compare(getNode(current)->key(isRoot(current)),
                                     getNode(prev)->key(isRoot(prev)));
      temp = getNode(prev)->childP(new_p_dir);
      NodeInfo new_current{temp.index()};
      locate(prev, new_current, getNode(current)->key(isRoot(current)));
      if (new_current.index() != current.index())
        break;
      prev = getNode(prev)->backLinkP();
    }
  }
  return flag;
}

/*!
  \details No detailed description

  \param [in] current No description.
  \param [in] dir No description.
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
void LockFreeBst<Key, T, Compare>::tryMark(NodeInfo current,
                                           const size_type dir) noexcept
{
  while (true) {
    NodeInfo back = getNode(current)->backLinkP();
    NodeInfo next = getNode(current)->childP(dir);
    if (next.isMarked()) {
      break;
    }
    else if (next.flag()) {
      if (!next.isThreaded()) {
        cleanFlag(current, next, back, false);
        continue;
      }
      else if (next.isThreaded() && (dir != 0)) {
        cleanFlag(current, next, back, true);
        continue;
      }
    }
    NodeInfo new_value{next, false, false};
    const NodeInfo replacement{next, false, true};
    const bool result = cas(&getNode(current)->child(dir), new_value, replacement);
    if (result)
      break;
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <std::convertible_to<double> Key,
          MappedValue T,
          std::invocable<Key, Key> Compare> inline
constexpr auto LockFreeBst<Key, T, Compare>::invalidId() noexcept -> size_type
{
  return NodeInfo::invalidIndex();
}

} /* namespace zisc */

#endif /* ZISC_LOCK_FREE_BST_INL_HPP */
