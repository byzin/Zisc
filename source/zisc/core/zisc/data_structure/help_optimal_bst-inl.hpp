/*!
  \file help_optimal_bst-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_HELP_OPTIMAL_BST_INL_HPP
#define ZISC_HELP_OPTIMAL_BST_INL_HPP

#include "help_optimal_bst.hpp"
// Standard C++ library
#include <array>
#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "help_optimal_bst_node.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
HelpOptimalBst::HelpOptimalBst(pmr::memory_resource* mem_resource) noexcept :
    HelpOptimalBst(defaultCapacity(), mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
inline
HelpOptimalBst::HelpOptimalBst(const size_type cap,
                               pmr::memory_resource* mem_resource) noexcept :
    free_data_nodes_{mem_resource},
    free_non_data_nodes_{mem_resource},
    data_node_list_{typename decltype(data_node_list_)::allocator_type{mem_resource}},
    non_data_node_list_{typename decltype(non_data_node_list_)::allocator_type{mem_resource}}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
HelpOptimalBst::HelpOptimalBst(HelpOptimalBst&& other) noexcept :
    free_data_nodes_{std::move(other.free_data_nodes_)},
    free_non_data_nodes_{std::move(other.free_non_data_nodes_)},
    data_node_list_{std::move(other.data_node_list_)},
    non_data_node_list_{std::move(other.non_data_node_list_)},
    c_root_id_{other.c_root_id_},
    p_root_id_{other.p_root_id_}
{
}

/*!
  \details No detailed description
  */
inline
HelpOptimalBst::~HelpOptimalBst() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
HelpOptimalBst& HelpOptimalBst::operator=(HelpOptimalBst&& other) noexcept
{
  free_data_nodes_ = std::move(other.free_data_nodes_);
  free_non_data_nodes_ = std::move(other.free_non_data_nodes_);
  data_node_list_ = std::move(other.data_node_list_);
  non_data_node_list_ = std::move(other.non_data_node_list_);
  c_root_id_ = other.c_root_id_;
  p_root_id_ = other.p_root_id_;
  return *this;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <ConvertibleTo<double> Type> inline
auto HelpOptimalBst::add(const Type& value) -> std::tuple<bool, size_type>
{
  const double key = cast<double>(value);

  auto [ancestor_id, successor_id, parent_id, child_id] = getInitialIds();
  size_type node_id = invalidId();
  size_type node_id_tmp = invalidId();
  size_type inner_id = invalidId();
  size_type splice_id = invalidId();
  size_type sibling_id = invalidId();
  bool is_added = false;
  while (!is_added) {
    searchLeaf(key, &ancestor_id, &successor_id, &parent_id, &child_id);
    if (!isDeadNode(child_id)) {
      const size_type next_id = getRefNodeId(child_id);
      const Node& next = getNode(next_id);
      if (key == next.key())
        break;

      makeLeafNode(key, &node_id);
      const double inner_key = calcInnerNodeKey(next.key(), key);
      const bool is_node_left = key < next.key();
      makeInnerNode(inner_key, node_id, next_id, is_node_left, &inner_id);
      node_id_tmp = inner_id;
      const Node& child = getNode(child_id);
      if (!child.isSplice() && casChild(parent_id, key, child_id, &node_id_tmp)) {
        is_added = true;
        node_id_tmp = invalidId();
      }
      else if (casChild(ancestor_id, key, successor_id, &node_id_tmp)) {
        is_added = true;
        sibling_id = next_id;
      }
      inner_id = is_added ? invalidId() : inner_id;
    }
    else {
      makeLeafNode(key, &node_id);
      if (!hasBacktrack(child_id, nullptr)) {
        node_id_tmp = node_id;
        is_added = casChild(parent_id, key, child_id, &node_id_tmp);
      }
      else {
        sibling_id = appendSpliceNode(parent_id, key, ancestor_id, &splice_id);
        sibling_id = getRefNodeId(sibling_id);
        if (isDeadNode(sibling_id)) {
          node_id_tmp = node_id;
          is_added = casChild(ancestor_id, key, successor_id, &node_id_tmp);
          if (splice_id == sibling_id)
            releaseNodes(splice_id);
        }
        else {
          const Node& parent = getNode(parent_id);
          const double inner_key = calcInnerNodeKey(parent.key(), key);
          const Node& sibling = getNode(sibling_id);
          const bool is_node_left = key < sibling.key();
          makeInnerNode(inner_key, node_id, sibling_id, is_node_left, &inner_id);
          node_id_tmp = inner_id;
          is_added = casChild(ancestor_id, key, successor_id, &node_id_tmp);
          inner_id = is_added ? invalidId() : inner_id;
        }
        splice_id = (splice_id == sibling_id) ? invalidId() : splice_id;
      }
    }
    if (!is_added) {
      backtrack(key, &ancestor_id, &successor_id, &parent_id, &child_id);
      node_id_tmp = invalidId();
      sibling_id = invalidId();
    }
  }
  releaseNodes(node_id_tmp, sibling_id);
  releaseNode(inner_id);
  releaseNode(splice_id);
  return std::make_tuple(is_added, getNodeIndex(node_id));
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto HelpOptimalBst::capacity() const noexcept -> size_type
{
  const size_type cap = data_node_list_.size();
  ZISC_ASSERT((cap == 0) || has_single_bit(cap),
               "The capacity isn't power of 2. capacity = ", cap);
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto HelpOptimalBst::capacityMax() noexcept -> size_type
{
  const size_type cap = size_type{1} << (std::numeric_limits<size_type>::digits - 4);
  return cap;
}

/*!
  \details No detailed description
  */
inline
void HelpOptimalBst::clear() noexcept
{
  free_data_nodes_.full();
  free_non_data_nodes_.full();
  initialize();
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <ConvertibleTo<double> Type> inline
bool HelpOptimalBst::contain(const Type& value) const noexcept
{
  const double key = cast<double>(value);
  size_type node_id = cRootId();
  while (!isLeafNode(node_id)) {
    const Node& node = getNode(node_id);
    const bool is_left = key < node.key();
    node_id = node.child(is_left).load(std::memory_order::acquire);
  }

  const Node& node = getNode(node_id);
  const bool result = (key == node.key()) && !isDeadNode(node_id);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto HelpOptimalBst::defaultCapacity() noexcept -> size_type
{
  return 1024;
}

///*!
//  \details No detailed description
//
//  \param [out] output No description.
//  */
//inline
//void HelpOptimalBst::printTree(std::ostream* output) const noexcept
//{
////  printTreeNode(cRootId(), output);
//  std::vector<std::size_t> id_list = {cRootId()};
//  bool flag = true;
//  for (std::size_t level = 1; flag; ++level) {
//    flag = false;
//    for (const auto id : id_list) {
//      if (id != invalidId()) {
//        flag = true;
//        break;
//      }
//    }
//    if (flag) {
//      (*output) << std::setw(2) << level;
//      printTreeNode(&id_list, output);
//      (*output) << std::endl;
//    }
//  }
//}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <ConvertibleTo<double> Type> inline
bool HelpOptimalBst::remove(const Type& value)
{
  const double key = cast<double>(value);
  auto [ancestor_id, successor_id, parent_id, child_id] = getInitialIds();
  size_type marked_id = invalidId();
  size_type marked_id_tmp = invalidId();
  size_type dead_id = invalidId();
  size_type sibling_id = invalidId();
  size_type sibling_id_tmp = invalidId();
  size_type splice_id = invalidId();
  bool mode = true;
  bool is_removed = false;
  while (!is_removed) {
    searchLeaf(key, &ancestor_id, &successor_id, &parent_id, &child_id);
    const Node& child = getNode(child_id);
    if (mode) {
      const size_type next_id = getRefNodeId(child_id);
      const Node& next = getNode(next_id);
      if ((key != next.key()) || isDeadNode(child_id))
        break;

      if ((child.rightChild().load(std::memory_order::acquire) == invalidId()) &&
          (parent_id != ancestor_id)) {
        makeDeadNode(key, &marked_id, ancestor_id);
        marked_id_tmp = marked_id;
        if (casChild(parent_id, key, child_id, &marked_id_tmp)) {
          mode = false;
          sibling_id = appendSpliceNode(parent_id, key, ancestor_id, &splice_id);
          if (isDeadNode(sibling_id)) {
            makeDeadNode(key, &dead_id);
            casChild(ancestor_id, key, successor_id, &dead_id);
            is_removed = true;
            if (splice_id == sibling_id)
              releaseNodes(splice_id);
          }
          else if (getNode(sibling_id).isSplice()) {
            is_removed = true;
          }
          else {
            sibling_id_tmp = sibling_id;
            is_removed = casChild(ancestor_id, key, successor_id, &sibling_id_tmp);
          }
          splice_id = (splice_id == sibling_id) ? invalidId() : splice_id;
        }
      }
      else {
        makeDeadNode(key, &dead_id);
        is_removed = casChild(ancestor_id, key, successor_id, &dead_id);
      }
    }
    else {
      is_removed = (child_id != marked_id) || (parent_id == ancestor_id);
      if (!is_removed) {
        sibling_id_tmp = sibling_id;
        is_removed = casChild(ancestor_id, key, successor_id, &sibling_id_tmp);
      }
    }
    if (!is_removed) {
      backtrack(key, &ancestor_id, &successor_id, &parent_id, &child_id);
      sibling_id_tmp = invalidId();
    }
  }
  releaseNodes(marked_id_tmp);
  releaseNodes(dead_id);
  releaseNodes(sibling_id_tmp, sibling_id);
  releaseNode(splice_id);
  return is_removed;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
inline
void HelpOptimalBst::setCapacity(const size_type cap) noexcept
{
  // Resizing
  constexpr size_type min_capacity = 3;
  const size_type c = (std::max)(min_capacity, cap);
  const size_type cap_pow2 = bit_ceil(c);
  if ((capacity() < cap_pow2) && (cap_pow2 <= capacityMax())) {
    data_node_list_.resize(cap_pow2);
    non_data_node_list_.resize(cap_pow2 << 2);
    free_data_nodes_.setSize(cap_pow2 << 1);
    free_non_data_nodes_.setSize(cap_pow2 << 3);
  }
  free_data_nodes_.full();
  free_non_data_nodes_.full();

  //
  initialize();
}

/*!
  \details No detailed description

  \param [in] node_id No description.
  \param [in] key No description.
  \param [in] splice_id No description.
  \return No description
  */
inline
auto HelpOptimalBst::appendSpliceNode(const size_type node_id,
                                      const double key,
                                      const size_type backtrack_id,
                                      size_type* splice_id) noexcept -> size_type
{
  const Node& node = getNode(node_id);
  const bool is_left = !(key < node.key());

  size_type sibling_id = invalidId();
  while (sibling_id == invalidId()) {
    const size_type sib_id = node.child(is_left).load(std::memory_order::acquire);
    if (hasBacktrack(sib_id, nullptr)) {
      sibling_id = sib_id;
    }
    else {
      const Node& sibling = getNode(sib_id);
      makeSpliceNode(sibling.key(), sib_id, backtrack_id, splice_id);
      if (casChild<false>(node_id, key, sib_id, splice_id))
        sibling_id = sib_id;
    }
  }
  return sibling_id;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in,out] ancestor_id No description.
  \param [in,out] successor_id No description.
  \param [in,out] parent_id No description.
  \param [in,out] child_id No description.
  */
inline
void HelpOptimalBst::backtrack(const double key,
                               size_type* ancestor_id,
                               size_type* successor_id,
                               size_type* parent_id,
                               size_type* child_id) const noexcept
{
  do {
    const Node* ancestor = std::addressof(getNode(*ancestor_id));
    const bool is_left = key < ancestor->key();
    *successor_id = ancestor->child(is_left).load(std::memory_order::acquire);
  } while (hasBacktrack(*successor_id, ancestor_id));
  *parent_id = *ancestor_id;
  *child_id = *successor_id;
}

/*!
  \details No detailed description

  \param [in] key1 No description.
  \param [in] key2 No description.
  \return No description
  */
inline
double HelpOptimalBst::calcInnerNodeKey(const double key1, const double key2) noexcept
{
  const double k = 0.5 * key1 + 0.5 * key2;
  return k;
}

/*!
  \details No detailed description

  \param [in] node_id No description.
  \param [in] key No description.
  \param [in] cmp_id No description.
  \param [in,out] new_id No description.
  \return No description
  */
template <bool kIsLeftReplacedWithSmallerKey> inline
bool HelpOptimalBst::casChild(const size_type node_id,
                              const double key,
                              size_type cmp_id,
                              size_type* new_id) noexcept
{
  Node& node = getNode(node_id);
  const bool is_left = kIsLeftReplacedWithSmallerKey
      ? key < node.key()
      : !(key < node.key());
  auto& child = node.child(is_left);
  bool result = child.load(std::memory_order::acquire) == cmp_id;
  if (result) {
    constexpr auto success = std::memory_order::acq_rel;
    constexpr auto failure = std::memory_order::acquire;
    result = child.compare_exchange_strong(cmp_id, *new_id, success, failure);
  }
  if (result)
    *new_id = cmp_id; // replace the given value with old id
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto HelpOptimalBst::cRootId() const noexcept -> size_type
{
  return c_root_id_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto HelpOptimalBst::flagBit() noexcept -> size_type
{
  const size_type bit = size_type{1} << (std::numeric_limits<size_type>::digits - 1);
  return bit;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto HelpOptimalBst::getInitialIds() const noexcept
    -> std::tuple<size_type, size_type, size_type, size_type>
{
  const size_type ancestor_id = pRootId();
  const size_type successor_id = cRootId();
  const size_type parent_id = successor_id;
  const Node& parent = getNode(parent_id);
  const size_type child_id = parent.leftChild().load(std::memory_order::acquire);
  return std::make_tuple(ancestor_id, successor_id, parent_id, child_id);
}

/*!
  \details No detailed description

  \param [in] id No description.
  \return No description
  */
inline
auto HelpOptimalBst::getNode(const size_type id) noexcept -> Node&
{
  const size_type index = getNodeIndex(id);
  Node& node = isDataNode(id) ? data_node_list_[index] : non_data_node_list_[index];
  return node;
}

/*!
  \details No detailed description

  \param [in] id No description.
  \return No description
  */
inline
auto HelpOptimalBst::getNode(const size_type id) const noexcept -> const Node&
{
  const size_type index = getNodeIndex(id);
  const Node& node = isDataNode(id) ? data_node_list_[index] : non_data_node_list_[index];
  return node;
}

/*!
  \details No detailed description

  \param [in] id No description.
  \return No description
  */
inline
auto HelpOptimalBst::getNodeIndex(const size_type id) noexcept -> size_type
{
  constexpr size_type mask = ~flagBit();
  const size_type index = id & mask;
  return index;
}

/*!
  \details No detailed description

  \param [in] id No description.
  \return No description
  */
inline
auto HelpOptimalBst::getRefNodeId(const size_type id) const noexcept -> size_type
{
  const Node& node = getNode(id);
  const size_type next = node.isSplice()
      ? node.rightChild().load(std::memory_order::acquire)
      : id;
  return next;
}

/*!
  \details No detailed description

  \param [in] id No description.
  \param [out] backtrack_id No description.
  \return No description
  */
inline
bool HelpOptimalBst::hasBacktrack(const size_type id,
                                     size_type* backtrack_id) const noexcept
{
  const Node& node = getNode(id);
  const size_type bt_id = node.backtrack().load(std::memory_order::acquire);
  const bool result = bt_id != invalidId();
  if (result && (backtrack_id != nullptr))
    *backtrack_id = bt_id;
  return result;
}

/*!
  \details No detailed description
  */
inline
void HelpOptimalBst::initialize() noexcept
{
  // Initialize c root
  {
    size_type node1_id = invalidId();
    makeSpecialNode(Node::max2Key(), &node1_id);

    size_type node2_id = invalidId();
    makeSpecialNode(Node::max1Key(), &node2_id);

    size_type c_root_id = invalidId();
    makeInnerNode(Node::max1Key(), node1_id, node2_id, true, &c_root_id);
    c_root_id_ = c_root_id;
  }

  // Initialize p root
  {
    size_type node2_id = invalidId();
    makeSpecialNode(Node::max0Key(), &node2_id);

    size_type p_root_id = invalidId();
    makeInnerNode(Node::max0Key(), cRootId(), node2_id, true, &p_root_id);
    p_root_id_ = p_root_id;
  }
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto HelpOptimalBst::invalidId() noexcept -> size_type
{
  return (std::numeric_limits<size_type>::max)();
}

/*!
  \details No detailed description

  \param [in] id No description.
  \return No description
  */
inline
bool HelpOptimalBst::isDataNode(const size_type id) const noexcept
{
  const bool result = (id & flagBit()) == flagBit();
  return result;
}

/*!
  \details No detailed description

  \param [in] id No description.
  \return No description
  */
inline
bool HelpOptimalBst::isDeadNode(const size_type id) const noexcept
{
  const Node& node = getNode(id);
  const bool result = id == node.rightChild().load(std::memory_order::acquire);
  return result;
}

/*!
  \details No detailed description

  \param [in] id No description.
  \return No description
  */
inline
bool HelpOptimalBst::isLeafNode(const size_type id) const noexcept
{
  const Node& node = getNode(id);
  const size_type l_child_id = node.leftChild().load(std::memory_order::acquire);
  const bool result = l_child_id == invalidId();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto HelpOptimalBst::issueDataNodeId() noexcept -> size_type
{
  const size_type index = free_data_nodes_.dequeue(true);
  const size_type id = makeNodeId(index, true);
  return id;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto HelpOptimalBst::issueNonDataNodeId() noexcept -> size_type
{
  const size_type index = free_non_data_nodes_.dequeue(true);
  const size_type id = makeNodeId(index, false);
  return id;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [out] id No description.
  \param [in] backtrack_id No description.
  */
inline
void HelpOptimalBst::makeDeadNode(const double key,
                                  size_type* id,
                                  const size_type backtrack_id) noexcept
{
  Node* node = nullptr;
  if (*id == invalidId()) {
    const size_type node_id = issueNonDataNodeId();
    ZISC_ASSERT(!isDataNode(node_id), "The issued ID isn't for leaf node.");
    node = std::addressof(getNode(node_id));
    node->leftChild().store(invalidId(), std::memory_order::release);
    *id = node_id;
  }
  node = (node == nullptr) ? std::addressof(getNode(*id)) : node;
  node->setKey(key);
  node->rightChild().store(*id, std::memory_order::release);
  node->backtrack().store(backtrack_id, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] has_data No description.
  \return No description
  */
inline
auto HelpOptimalBst::makeNodeId(const size_type index,
                                const bool has_data) noexcept -> size_type
{
  const size_type id = has_data ? (flagBit() | index) : index;
  return id;
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] node1_id No description.
  \param [in] node2_id No description.
  \param [in] is_node1_left No description.
  \param [in,out] id No description.
  */
inline
void HelpOptimalBst::makeInnerNode(const double key,
                                   const size_type node1_id,
                                   const size_type node2_id,
                                   const bool is_node1_left,
                                   size_type* id) noexcept
{
  Node* node = nullptr;
  if (*id == invalidId()) {
    const size_type node_id = issueNonDataNodeId();
    ZISC_ASSERT(!isDataNode(node_id), "The issued ID isn't for internal node.");
    node = std::addressof(getNode(node_id));
    node->backtrack().store(invalidId(), std::memory_order::release);
    *id = node_id;
  }
  node = (node == nullptr) ? std::addressof(getNode(*id)) : node;
  node->setKey(key);
  node->child(is_node1_left).store(node1_id, std::memory_order::release);
  node->child(!is_node1_left).store(node2_id, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in,out] id No description.
  */
inline
void HelpOptimalBst::makeLeafNode(const double key, size_type* id) noexcept
{
  Node* node = nullptr;
  if (*id == invalidId()) {
    const size_type node_id = issueDataNodeId();
    ZISC_ASSERT(isDataNode(node_id), "The issued ID isn't for leaf node.");
    node = std::addressof(getNode(node_id));
    node->leftChild().store(invalidId(), std::memory_order::release);
    node->rightChild().store(invalidId(), std::memory_order::release);
    node->backtrack().store(invalidId(), std::memory_order::release);
    *id = node_id;
  }
  node = (node == nullptr) ? std::addressof(getNode(*id)) : node;
  node->setKey(key);
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] node_id No description.
  \param [in] backtrack_id No description.
  \param [out] id No description.
  */
inline
void HelpOptimalBst::makeSpliceNode(const double key,
                                    const size_type node_id,
                                    const size_type backtrack_id,
                                    size_type* id) noexcept
{
  if (isDeadNode(node_id)) {
    makeDeadNode(key, id, backtrack_id);
    return;
  }

  if (*id == invalidId()) {
    const size_type n_id = issueNonDataNodeId();
    ZISC_ASSERT(!isDataNode(n_id), "The issued ID isn't for internal node.");
    *id = n_id;
  }
  Node* node = std::addressof(getNode(*id));
  node->setKey(Node::min0Key());
  {
    const Node& original = getNode(node_id);
    const size_type child_id = original.leftChild().load(std::memory_order::acquire);
    node->leftChild().store(child_id, std::memory_order::release);
  }
  node->rightChild().store(node_id, std::memory_order::release);
  node->backtrack().store(backtrack_id, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in,out] id No description.
  */
inline
void HelpOptimalBst::makeSpecialNode(const double key, size_type* id) noexcept
{
  Node* node = nullptr;
  if (*id == invalidId()) {
    const size_type node_id = issueNonDataNodeId();
    ZISC_ASSERT(!isDataNode(node_id), "The issued ID isn't for special node.");
    node = std::addressof(getNode(node_id));
    node->leftChild().store(invalidId(), std::memory_order::release);
    node->rightChild().store(invalidId(), std::memory_order::release);
    node->backtrack().store(invalidId(), std::memory_order::release);
    *id = node_id;
  }
  node = (node == nullptr) ? std::addressof(getNode(*id)) : node;
  node->setKey(key);
}

///*!
//  \details No detailed description
//
//  \param [out] output No description.
//  */
//inline
//void HelpOptimalBst::printTreeNode(const std::size_t id,
//                                   std::ostream* output) const noexcept
//{
//  const Node& node = getNode(id);
//  if (isLeafNode(id)) {
//    const std::size_t index = getNodeIndex(id);
//    if (isDeadNode(id))
//      (*output) << "dead(" << index << "(" << node.key() << ")) ";
//    else
//      (*output) << index << "(" << node.key() << ") ";
//  }
//  else if (!isDeadNode(id)) {
//    {
//      const std::size_t child_id = node.leftChild().load(std::memory_order::acquire);
//      if (child_id != invalidId())
//        printTreeNode(child_id, output);
//    }
//    {
//      const std::size_t child_id = node.rightChild().load(std::memory_order::acquire);
//      if (child_id != invalidId())
//        printTreeNode(child_id, output);
//    }
//  }
//}
//
//inline
//void HelpOptimalBst::printTreeNode(std::vector<std::size_t>* id_list,
//                                   std::ostream* output) const noexcept
//{
//  constexpr std::size_t w = 32;
//  for (const std::size_t id : *id_list) {
//    if (id == invalidId()) {
//      (*output) << std::setw(w) << " " << "|";
//      continue;
//    }
//
//    const Node& node = getNode(id);
//    std::stringstream str;
//    if (isLeafNode(id)) {
//      if (isDeadNode(id))
//        str << "deadL(" << node.key() << ", " << std::setbase(16) << id << ") ";
//      else
//        str << "L(" << node.key() << ", " << std::setbase(16) << id << ") ";
//    }
//    else {
//      if (node.isSplice())
//        str << "S(" << node.key() << ", " << std::setbase(16) << id << ") ";
//      else if (isDeadNode(id))
//        str << "deadN(" << node.key() << ", " << std::setbase(16) << id << ") ";
//      else
//        str << "N(" << node.key() << ", " << std::setbase(16) << id << ") ";
//    }
//    (*output) << std::setw(w) << str.str() << "|";
//  }
//
//  std::vector<std::size_t> new_list;
//  for (const std::size_t id : *id_list) {
//    if (id == invalidId()) {
//      new_list.emplace_back(invalidId());
//      new_list.emplace_back(invalidId());
//      continue;
//    }
//
//    const Node& node = getNode(id);
//    if (isLeafNode(id)) {
//      new_list.emplace_back(invalidId());
//      new_list.emplace_back(invalidId());
//    }
//    else {
//      const std::size_t left_id = node.leftChild().load(std::memory_order::acquire);
//      const std::size_t right_id = node.rightChild().load(std::memory_order::acquire);
//      if (node.isSplice()) {
//        if (right_id != invalidId() && !isLeafNode(right_id)) {
//          const Node& child_node = getNode(right_id);
//          const std::size_t left_c_id = child_node.leftChild().load(std::memory_order::acquire);
//          if (left_id == left_c_id)
//            new_list.emplace_back(invalidId());
//          else
//            new_list.emplace_back(left_id);
//        }
//        new_list.emplace_back(right_id);
//      }
//      else if (isDeadNode(id)) {
//        new_list.emplace_back(invalidId());
//        new_list.emplace_back(invalidId());
//      }
//      else {
//        new_list.emplace_back(left_id);
//        new_list.emplace_back(right_id);
//      }
//    }
//  }
//  *id_list = std::move(new_list);
//}

/*!
  \details No detailed description

  \return No description
  */
inline
auto HelpOptimalBst::pRootId() const noexcept -> size_type
{
  return p_root_id_;
}

/*!
  \details No detailed description

  \param [in] id No description.
  */
inline
void HelpOptimalBst::releaseNode(const size_type id) noexcept
{
  returnNodeId(id);
}

/*!
  \details No detailed description

  \param [in] id No description.
  */
inline
void HelpOptimalBst::releaseNodes(const size_type id, const size_type excluded_id) noexcept
{
  std::array<size_type, 8> id_stack;
  size_type n = 0;
  if ((id != invalidId()) && (id != excluded_id))
    id_stack[n++] = id;
  while (0 < n) {
    const size_type node_id = id_stack[--n];
    const Node& node = getNode(node_id);
    // Right child
    {
      const size_type r_child_id = node.rightChild().load(std::memory_order::acquire);
      const bool is_dead_node = node_id == r_child_id;
      if (!is_dead_node && (r_child_id != invalidId()) && (r_child_id != excluded_id))
        id_stack[n++] = r_child_id;
    }
    // Left child
    if (!node.isSplice()) {
      const size_type l_child_id = node.leftChild().load(std::memory_order::acquire);
      if ((l_child_id != invalidId()) && (l_child_id != excluded_id))
        id_stack[n++] = l_child_id;
    }
    ZISC_ASSERT(n <= id_stack.size(), "The id stack overflowed.");
    releaseNode(node_id);
  }
}

/*!
  \details No detailed description

  \param [in] id No description.
  */
inline
void HelpOptimalBst::returnNodeId(const size_type id) noexcept
{
  if (id == invalidId())
    return;

#if defined(Z_DEBUG_MODE)
  {
    Node& node = getNode(id);
    node.setKey(Node::min1Key());
  }
#endif // Z_DEBUG_MODE

  const size_type index = getNodeIndex(id);
  if (isDataNode(id))
    free_data_nodes_.enqueue(index, true);
  else
    free_non_data_nodes_.enqueue(index, true);
}

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in,out] ancestor_id No description.
  \param [in,out] successor_id No description.
  \param [in,out] parent_id No description.
  \param [in,out] child_id No description.
  */
inline
void HelpOptimalBst::searchLeaf(const double key,
                                size_type* ancestor_id,
                                size_type* successor_id,
                                size_type* parent_id,
                                size_type* child_id) const noexcept
{
  while (!isLeafNode(*child_id)) {
    const Node& child = getNode(*child_id);
    if (child.isSplice()) {
      *parent_id = child.rightChild().load(std::memory_order::acquire);
    }
    else {
      *ancestor_id = *parent_id;
      *successor_id = *child_id;
      *parent_id = *child_id;
    }
    const Node& parent = getNode(*parent_id);
    const bool is_left = key < parent.key();
    *child_id = parent.child(is_left).load(std::memory_order::acquire);
  }
  ZISC_ASSERT(isLeafNode(*child_id), "The search operation failed.");
}

} // namespace zisc

#endif // ZISC_HELP_OPTIMAL_BST_INL_HPP
