/*!
  \file help_optimal_bst.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_HELP_OPTIMAL_BST_HPP
#define ZISC_HELP_OPTIMAL_BST_HPP

// Standard C++ library
#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>
// Zisc
#include "bounded_bst.hpp"
#include "help_optimal_bst_node.hpp"
#include "ring_buffer.hpp"
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief Help Optimal Lock-free BST

  For more details, please see the following paper:
  <a href="https://ieeexplore.ieee.org/document/7573837">Help-Optimal and Language-Portable Lock-Free Concurrent Data Structures</a>. <br>
  */
class HelpOptimalBst : public BoundedBst<HelpOptimalBst>
{
 public:
  // Type aliases
  using BaseBstType = BoundedBst<HelpOptimalBst>;

  // Type aliases for STL
  using size_type = typename BaseBstType::size_type;


  //! Create a binary search tree
  HelpOptimalBst(pmr::memory_resource* mem_resource) noexcept;

  //! Create a binary search tree
  HelpOptimalBst(const size_type cap,
                 pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  HelpOptimalBst(HelpOptimalBst&& other) noexcept;

  //! Destroy the bst
  ~HelpOptimalBst() noexcept;


  //! Move a data
  HelpOptimalBst& operator=(HelpOptimalBst&& other) noexcept;


  //! Insert the given value into the bst
  template <ConvertibleTo<double> Type>
  std::tuple<bool, size_type> add(const Type& value);

  //! Return the capacity of the bst nodes
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the bst state
  void clear() noexcept;

  //! Check if the given value is contained in the bst
  template <ConvertibleTo<double> Type>
  bool contain(const Type& value) const noexcept;

  //! Return the default capacity for nodes
  static constexpr size_type defaultCapacity() noexcept;

  //! Print tree
//  void printTree(std::ostream* output) const noexcept;

  //! Remove the value from the bst
  template <ConvertibleTo<double> Type>
  bool remove(const Type& value);

  //! Change the maximum possible number of nodes. The bst will be cleared
  void setCapacity(const size_type cap) noexcept;

 private:
  // Type aliases
  using Node = HelpOptimalBstNode;


  //! Append the given splice node into the child of given node
  size_type appendSpliceNode(const size_type node_id,
                             const double key,
                             const size_type backtrack_id,
                             size_type* splice_id) noexcept;

  //! Perform backtrack
  void backtrack(const double key,
                 size_type* ancestor_id,
                 size_type* successor_id,
                 size_type* parent_id,
                 size_type* child_id) const noexcept;

  //! Calculate a key for an internal node
  static double calcInnerNodeKey(const double key1, const double key2) noexcept;

  //! Atomically compare node's child and cmp_id and replace with new_id
  template <bool kIsLeftReplacedWithSmallerKey = true>
  bool casChild(const size_type node_id,
                const double key,
                size_type cmp_id,
                size_type* new_id) noexcept;

  //! Return the id of c root
  size_type cRootId() const noexcept;

  //! Destroy elements stored in the bst
  void destroy() noexcept;

  //! Return the flag bit
  static constexpr size_type flagBit() noexcept;

  //! Return the initial indices for bst operations
  std::tuple<size_type, size_type, size_type, size_type> getInitialIds() const noexcept;

  //! Return the node by the given id
  Node& getNode(const size_type id) noexcept;

  //! Return the node by the given id
  const Node& getNode(const size_type id) const noexcept;

  //! Return the node index corresponding to the given ID
  static size_type getNodeIndex(const size_type id) noexcept;

  //! Return the id to non-spliced node of the given node
  size_type getRefNodeId(const size_type id) const noexcept;

  //! Check if the given node has backtrack ID
  bool hasBacktrack(const size_type id, size_type* backtrack_id) const noexcept;

  //! Initialize the bst
  void initialize() noexcept;

  //! Return the invalid ID
  static constexpr size_type invalidId() noexcept;

  //! Check if the given node is data node
  bool isDataNode(const size_type id) const noexcept;

  //! Check if the given node is dead
  bool isDeadNode(const size_type id) const noexcept;

  //! Check if the given node is leaf
  bool isLeafNode(const size_type id) const noexcept;

  //! Issue an unique data node ID
  size_type issueDataNodeId() noexcept;

  //! Issue an unique non data node ID
  size_type issueNonDataNodeId() noexcept;

  //! Make a dead leaf node
  void makeDeadNode(const double key,
                    size_type* id,
                    const size_type backtrack_id = invalidId()) noexcept;

  //! Make a node ID
  static size_type makeNodeId(const size_type index, const bool has_data) noexcept;

  //! Make a inner node
  void makeInnerNode(const double key,
                     const size_type node1_id,
                     const size_type node2_id,
                     const bool is_node1_left,
                     size_type* id) noexcept;

  //! Make a leaf node
  void makeLeafNode(const double key, size_type* id) noexcept;

  //! Make a splice node
  void makeSpliceNode(const double key,
                      const size_type node_id,
                      const size_type backtrack_id,
                      size_type* id) noexcept;

  //! Make a special node
  void makeSpecialNode(const double key, size_type* id) noexcept;

  //! Print tree
//  void printTreeNode(const std::size_t id, std::ostream* output) const noexcept;
//
//  //! Print tree
//  void printTreeNode(std::vector<std::size_t>* id_list,
//                     std::ostream* output) const noexcept;

  //! Return the id of p root
  size_type pRootId() const noexcept;

  //! Eelease the given node ID
  void releaseNode(const size_type id) noexcept;

  //! Delete data in the given sub-tree and release node IDs
  void releaseNodes(const size_type id,
                    const size_type excluded_id = invalidId()) noexcept;

  //! Free the give node ID
  void returnNodeId(const size_type id) noexcept;

  //! Find a leaf
  void searchLeaf(const double key,
                  size_type* ancestor_id,
                  size_type* successor_id,
                  size_type* parent_id,
                  size_type* child_id) const noexcept;


  RingBuffer free_data_nodes_;
  RingBuffer free_non_data_nodes_;
  pmr::vector<Node> data_node_list_;
  pmr::vector<Node> non_data_node_list_;
  size_type c_root_id_ = 0;
  size_type p_root_id_ = 0;
};

} // namespace zisc

#include "help_optimal_bst-inl.hpp"

#endif // ZISC_HELP_OPTIMAL_BST_HPP
